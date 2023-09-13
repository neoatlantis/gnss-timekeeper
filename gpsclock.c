/*
 * File:   gpsclock.c
 * Author: standard
 *
 * Created on 2023年9月10日, 下午3:01
 * 
 * GPS Clock accepts the 1pps signal provided by external module, and generates
 * an internal signal on 2kHz that matches the 1pps signal in latter's each
 * period.
 * 
 * This is done by enabling the ECCP1 module in compare mode and watches on
 * timer 1 which is increased using Fosc/4 = 48MHz/4 = 12MHz. The CCP1 module
 * should count 2000 times when the next 1PPS signal is in, and at this moment
 * the timer 1 register should have a value as low as possible.
 * 
 * The value loaded in CCP1 is adjusted to achieve above objective. At 12 MHz
 * this value should be around 6000 to give 2000Hz output, and each increase
 * or decrease should give 1/3 (2000/6000) adjustments.
 */


#include <xc.h>
#include "gpsclock.h"
unsigned long long SYSTEM_CLOCK;
unsigned long long LAST_1PPS_SYSTEM_CLOCK;
unsigned long      COUNT_1PPS_RECEIVED;


uint16_t PLL_LAST_PULSES = 0;
uint16_t PLL_PULSES  = 0;
uint32_t PLL_RESIDUAL = 0;
#define PLL_PRELOAD_LOWER_BOUND 6050
#define PLL_PRELOAD_UPPER_BOUND 5950
#define PLL_DESIRED_PULSES      2000

#define LOAD_CCPR1(x) CCPR1H=(x>>8); CCPR1L=x & 0xFF;

void gpsclock_init(void){
    /*
     * GPS PPS reception is the most important job. We set it as high priority
     * interrupt on external interrupt 3, which corresponds to the MikroE socket
     * No. 1 (left) on board.
     * 
     * What the interrupt handler does, is:
     * 1. It records the system clock time at this moment.
     * 2. It toggles any following pin.
     * 3. It increases 1pps received counter.
     */
    
    // Interrupt of 1 PPS external input
    INTCON2bits.INT3IP = 1; // int 3 -> high priority
    INTCON3bits.INT3IE = 1; // enable interrupt 3
    
    // set up timer 3 as 16 bit timer, no prescalar, use internal Fosc/4 as
    // source, and no interrupt on overflow.
    T3CONbits.RD16 = 1;
    T3CONbits.T3CCP2 = T3CONbits.T3CCP1 = 1; // timer 3 for all CCP modules
    T3CONbits.T3CKPS = 0b00;
    T3CONbits.TMR3CS = 0;
    T3CONbits.TMR3ON = 0; // timer 3 is started together with 1PPS interrupt
                          // for synchronicity.
    
    // set up CCP 1 to use timer 1 as source, in comparator mode, and generates
    // software interrupt in high priority on match.
    CCPR1 = PLL_PRELOAD;
    CCP1CONbits.CCP1M = 0b1011;
    IPR1bits.CCP1IP = 1; // high priority
    PIE1bits.CCP1IE = 1; // enable interrupt
    
}

char gpsclock_interrupt(void){
    
    if(INTCON3bits.INT3IF){
        T3CONbits.TMR3ON = 0;
        uint16_t timer3_residual = TMR3L | (TMR3H << 8);
        TMR3H = 0;
        TMR3L = 0;
        T3CONbits.TMR3ON = 1;
        
        
        PLL_RESIDUAL = timer3_residual;
        
        //LAST_1PPS_SYSTEM_CLOCK = SYSTEM_CLOCK;
        //COUNT_1PPS_RECEIVED += 1;
        
        PLL_LAST_PULSES = PLL_PULSES;
        PLL_PULSES = 0;
        
        
        if (PLL_LAST_PULSES < PLL_DESIRED_PULSES){
            PLL_PRELOAD -= 1;
        } else if(PLL_LAST_PULSES > PLL_DESIRED_PULSES) {
            PLL_PRELOAD += 1;
        } else {
            if(PLL_RESIDUAL > 0x0F){
                PLL_PRELOAD -= 1;
            }
        }
        
        CCPR1 = PLL_PRELOAD;
        INTCON3bits.INT3IF = 0;
    }
    
    if(PIR1bits.CCP1IF){ // on timer 0 interrupt
        PIR1bits.CCP1IF = 0;
        LATDbits.LATD4 ^= 1;
        PLL_PULSES += 1;
    }
    return 0;
}

uint16_t gpsclock_pll_freq(void){
    return PLL_LAST_PULSES;
}

uint16_t gpsclock_pll_residual(void){
    return PLL_RESIDUAL & 0xFFFF;
}
