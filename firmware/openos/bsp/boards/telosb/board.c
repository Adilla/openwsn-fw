/**
\brief TelosB-specific definition of the "board" bsp module.

\author Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012.
*/

#include "msp430f1611.h"
#include "board.h"
// bsp modules
#include "leds.h"
#include "uart.h"
#include "spi.h"
#include "timers.h"
#include "radio.h"
#include "radiotimer.h"
#include "debugpins.h"

//=========================== variables =======================================

//=========================== prototypes ======================================

//=========================== public ==========================================

void board_init() {
   // disable watchdog timer
   WDTCTL     =  WDTPW + WDTHOLD;
   
   // setup clock speed
   DCOCTL     =  DCO0 | DCO1 | DCO2;             // MCLK at ~8MHz
   BCSCTL1    =  RSEL0 | RSEL1 | RSEL2;          // MCLK at ~8MHz
                                                 // by default, ACLK from 32kHz XTAL which is running
   
   // initialize pins
   P4DIR     |=  0x20;                           // [P4.5] radio VREG:  output
   P4DIR     |=  0x40;                           // [P4.6] radio reset: output
   //-- debug pins
   DEBUG_PIN_FRAME_INIT();
   DEBUG_PIN_SLOT_INIT();
   DEBUG_PIN_FSM_INIT();
   DEBUG_PIN_TASK_INIT();
   DEBUG_PIN_ISR_INIT();
   DEBUG_PIN_RADIO_INIT();
   
   // initialize bsp modules
   leds_init();
   uart_init();
   spi_init();
   timers_init();
   radio_init();
   radiotimer_init();
   
   // enable interrupts
   __bis_SR_register(GIE);
}

void board_sleep() {
   __bis_SR_register(GIE+LPM0_bits);             // sleep, but leave ACLK on
}

//=========================== private =========================================

//=========================== interrupt handlers ==============================

// DACDMA_VECTOR

// PORT2_VECTOR

#pragma vector = USART1TX_VECTOR
__interrupt void USART1TX_ISR (void) {
   DEBUG_PIN_ISR_SET();
   if (uart_isr_tx()==1) {                       // UART; TX
      __bic_SR_register_on_exit(CPUOFF);
   }
   DEBUG_PIN_ISR_CLR();
}

#pragma vector = USART1RX_VECTOR
__interrupt void USART1RX_ISR (void) {
   DEBUG_PIN_ISR_SET();
   if (uart_isr_rx()==1) {                       // UART: RX
      __bic_SR_register_on_exit(CPUOFF);
   }
   DEBUG_PIN_ISR_CLR();
}

// PORT1_VECTOR

#pragma vector = TIMERA1_VECTOR
__interrupt void TIMERA1_ISR (void) {
   DEBUG_PIN_ISR_SET();
   if (timer_isr_1()==1) {                       // timer: 1
      __bic_SR_register_on_exit(CPUOFF);
   }
   DEBUG_PIN_ISR_CLR();
}

#pragma vector = TIMERA0_VECTOR
__interrupt void TIMERA0_ISR (void) {
   DEBUG_PIN_ISR_SET();
   if (timer_isr_0()==1) {                       // timer: 0
      __bic_SR_register_on_exit(CPUOFF);
   }
   DEBUG_PIN_ISR_CLR();
}

// ADC12_VECTOR

// USART0TX_VECTOR

#pragma vector = USART0RX_VECTOR
__interrupt void USART0RX_ISR (void) {
   DEBUG_PIN_ISR_SET();
   if (spi_isr()==1) {                           // SPI
      __bic_SR_register_on_exit(CPUOFF);
   }
   DEBUG_PIN_ISR_CLR();
}

// WDT_VECTOR

#pragma vector = COMPARATORA_VECTOR
__interrupt void COMPARATORA_ISR (void) {
   DEBUG_PIN_ISR_SET();
   __bic_SR_register_on_exit(CPUOFF);            // restart CPU
   DEBUG_PIN_ISR_CLR();
}

#pragma vector = TIMERB1_VECTOR
__interrupt void TIMERB1_ISR (void) {
   DEBUG_PIN_ISR_SET();
   if (radiotimer_isr()==1) {                    // radiotimer
      __bic_SR_register_on_exit(CPUOFF);
   }
   DEBUG_PIN_ISR_CLR();
}

// TIMERB0_VECTOR

// NMI_VECTOR







