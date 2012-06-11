/**
\brief K20-specific definition of the "board" bsp module.

\author Xavi Vilajosana <xvilajosana@eecs.berkeley.edu>, May 2012.
 */

#include "board.h"
#include "board_info.h"
#include "leds.h"
#include "led.h"
#include "bsp_timer.h"
#include "smc.h"
#include "mcg.h"

//=========================== variables =======================================
extern int mcg_clk_hz;

//=========================== prototypes ======================================
//=========================== public ==========================================

extern int mote_main(void);

int main(void) {
	return mote_main();
}

void board_init() {
	//enable all port clocks.
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
			| SIM_SCGC5_PORTB_MASK
			| SIM_SCGC5_PORTC_MASK
			| SIM_SCGC5_PORTD_MASK
			| SIM_SCGC5_PORTE_MASK );


	//init all pins for the radio
	//SLPTR
	PORTB_PCR3 = PORT_PCR_MUX(1);// -- PTB3 used as gpio for slptr
	GPIOB_PDDR |= RADIO_SLPTR_MASK; //set as output
	//set to low
	PORT_PIN_RADIO_SLP_TR_CNTL_LOW();

	//RADIO RST -- TODO in the TWR change it to another pin! this is one of the leds.
	PORTC_PCR10 = PORT_PCR_MUX(1);// -- PTC10 used as gpio for radio rst
	GPIOC_PDDR |= RADIO_RST_MASK; //set as output
	PORT_PIN_RADIO_RESET_LOW();//activate the radio.
	
	//ptc5 .. ptc5 is pin 62, irq A
	enable_irq(RADIO_EXTERNAL_PORT_IRQ_NUM);//enable the irq. The function is mapped to the vector at position 105 (see manual page 69). The vector is in isr.h
		
	//external port radio_isr.
	PORTC_PCR5 = PORT_PCR_MUX(1);// -- PTC5 used as gpio for radio isr through llwu
	GPIOC_PDDR &= ~1<<RADIO_ISR_PIN; //set as input ==0
	PORTC_PCR5 |= PORT_PCR_IRQC(0x09); //9 interrupt on raising edge. page 249 of the manual.	
	PORTC_PCR5 |= PORT_PCR_ISF_MASK; //clear any pending interrupt.

	llwu_init();//low leakage unit init - to recover from deep sleep
	
	debugpins_init();
	leds_init();
	bsp_timer_init();
	uart_init();
	radiotimer_init();
	spi_init();	
	radio_init();
	leds_all_off();
}


void board_sleep() {
	uint8_t op_mode;
	clk_monitor_0(OFF);//turn off clock monitors so the freq can be changed without causing a reset
	PORTA_PCR2 |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;//JTAG_TDO -- disconnect jtag before entering deep sleep.    
	enter_lls();
	//enter_wait();
	//enter_wait();
	clk_monitor_0(ON);//enable it again.
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================
/*
 * This function is only called when the radio isr is executed and the MCU 
 * is either RUNNING or in WAIT state. 
 * In case that the MCU is in deep sleep (LLS mode), the LLWU is handling the
 * interrupt and consequently this function is not being call. For some reason,
 * the LLWU interrupt, does not trigger this interrupt function. In contrast the
 * LPTMR ISR is invoked right after the LLWU_isr in case the timer wake's up the MCU. 
 * 
 * So if you are working in LLS mode, then check LLWU_isr function in llwu.c file.
 * 
 */
void radio_external_port_c_isr(void) {
	debugpins_isr_set();
	 //reconfigure..	
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	if ((PORTC_ISFR) & (RADIO_ISR_MASK)) {
		
		PORTC_PCR5 |= PORT_PCR_ISF_MASK;    //clear flag
		PORTC_ISFR |= RADIO_ISR_MASK; //clear isr flag
		radio_isr();
	}else{
		while(1);
	}		   
	debugpins_isr_clr();
}


