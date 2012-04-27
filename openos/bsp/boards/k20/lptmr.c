
/*
 * File:        lptmr.c
 * Purpose:     Provide common low power timer functions
 *
 * Notes:       Right now only function provided is used
 *              to generate a delay in ms. This driver
 *              could be expanded to include more functions
 *              in the future.
 *
 */

#include "lptmr.h"
#include "derivative.h"
#include "arm_cm4.h"


low_timer_hook timer_callback;

void lptmr_isr(void)
{
  GPIOB_PSOR = 0x00080000;           // clear Port B 19 indicate wakeup

  //printf("\n****LPT ISR entered*****\r\n");

  LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;   // write 1 to TCF to clear the LPT timer compare flag
  LPTMR0_CSR = ( LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK | LPTMR_CSR_TCF_MASK  );
      // enable timer
      // enable interrupts
      // clear the flag

  //call the hook;
  timer_callback();
  
}


void lptmr_set_isr_compare_hook(low_timer_hook cbt){
	timer_callback=cbt;
}
/*******************************************************************************
*
*   PROCEDURE NAME:
*       lptmr_init -
*
*******************************************************************************/

void lptmr_init(uint8_t clock_source)
{  
	    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;//power the timer
	
        SIM_SCGC6|=SIM_SCGC6_RTC_MASK; //Enable RTC registers
	    RTC_CR|=RTC_CR_OSCE_MASK;      //Turn on RTC oscillator
	    //SIM_SOPT1|=SIM_SOPT1_OSC32KSEL_MASK;  //Select RTC OSC as source for ERCLK32K
	    SIM_SOPT1 &= ~(3<<18);//clear osc32ksel
	    SIM_SOPT1 |=SIM_SOPT1_OSC32KSEL(2); //select rtc 32khz
	   //printf("SIM_SOPT1 %x\n",SIM_SOPT1);

      
   
        
        // SIM_SCGC6 |= SIM_SCGC6_RTC_MASK;
    //SIM_SOPT1 |= SIM_SOPT1_OSC32KSEL(2); // ERCLK32 is RTC OSC CLOCK 32khz
   //OSC_CR |= OSC_CR_ERCLKEN_MASK|OSC_CR_EREFSTEN_MASK;//select the correct timer
//    RTC_CR |= RTC_CR_OSCE_MASK |
//                 RTC_CR_CLKO_MASK |
//                 RTC_CR_SC8P_MASK  ;
    
    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK  // external osc feeds directly to LPT
                 | LPTMR_PSR_PCS(clock_source)) ; // use the choice of clock          
    
    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK   // Clear any pending interrupt
                 //| LPTMR_CSR_TIE_MASK   // LPT interrupt enabled
                 | LPTMR_CSR_TPS(0)     //TMR pin select
                 |!LPTMR_CSR_TPP_MASK   //TMR Pin polarity
                 |!LPTMR_CSR_TFC_MASK   // Timer Free running counter is reset whenever TMR counter equals compare
                 |!LPTMR_CSR_TMS_MASK   //LPTMR as Timer
                );
    //register interrupt
    //done hardcoded in the vector table in kinetis_sysinit.c
    
    enable_irq(LPTMR_IRQ_NUM);
    
    //LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   //Turn on LPT and start counting
}

void lptmr_enable(){
	  LPTMR0_CSR |=LPTMR_CSR_TIE_MASK|LPTMR_CSR_TEN_MASK;//enable interrupts + timer enable
}

void lptmr_set_compare(PORT_TIMER_WIDTH count){
	LPTMR0_CMR = LPTMR_CMR_COMPARE(count);  //Set compare value
	LPTMR0_CSR |=LPTMR_CSR_TIE_MASK;//enable interrupts
}

PORT_TIMER_WIDTH lptmr_get_current_value(){
	return LPTMR0_CNR;//return the counter value
}

void lptmr_reset_compare(){
	LPTMR0_CMR = LPTMR_CMR_COMPARE(0);  //Set compare value to 0
	LPTMR0_CSR &=~LPTMR_CSR_TIE_MASK;//disable interrupts..
}


void lptmr_disable(){
	 LPTMR0_CSR &=~LPTMR_CSR_TIE_MASK;
	 LPTMR0_CSR &=~LPTMR_CSR_TEN_MASK;//disable interrupts + timer disable
}

