/*
 * File:   gpsclock.c
 * Author: standard
 *
 * Created on 2023年9月10日, 下午3:01
 */


#include <xc.h>

unsigned long long SYSTEM_CLOCK;
unsigned long long LAST_1PPS_SYSTEM_CLOCK;
unsigned long      COUNT_1PPS_RECEIVED;

uint16_t PLL_PRELOAD = 500;
uint16_t PLL_LAST_PULSES = 0;
uint16_t PLL_PULSES  = 0;
#define PLL_PRELOAD_LOWER_BOUND 550
#define PLL_PRELOAD_UPPER_BOUND 450
#define PLL_DESIRED_PULSES      2000

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
    
    INTCON2bits.INT3IP = 1; // int 3 -> high priority
    INTCON3bits.INT3IE = 1; // enable interrupt 3
    
    // set up timer 0 for timing the internal clock freq respective to 1pps
    T0CONbits.T0PS = 0b000; // prescaler = 2
    T0CONbits.PSA = 0;      // enable prescaler
    T0CONbits.T0CS = 0;     // use internal instruction cycle = Fosc/4
    T0CONbits.T08BIT = 0;   // timer 0 is 16 bit
    T0CONbits.TMR0ON = 0;   // off
    
    INTCON2bits.TMR0IP = 1;  // timer 0 overflow interrupt is high priority
    
}

char gpsclock_interrupt(void){
    char pll_reset = 0;
    
    if(INTCON3bits.INT3IF){
        T0CONbits.TMR0ON = 0;
        //LAST_1PPS_SYSTEM_CLOCK = SYSTEM_CLOCK;
        //COUNT_1PPS_RECEIVED += 1;
        
        PLL_LAST_PULSES = PLL_PULSES;
        PLL_PULSES = 0;
        
        if (PLL_LAST_PULSES < PLL_DESIRED_PULSES){
            PLL_PRELOAD -= 1;
        } else if(PLL_LAST_PULSES > PLL_DESIRED_PULSES) {
            PLL_PRELOAD += 1;
        }
        
        INTCON3bits.INT3IF = 0;
        pll_reset = 1;
        T0CONbits.TMR0ON = 1;
    }
    
    if(INTCONbits.TMR0IF || pll_reset){ // on timer 0 interrupt
        INTCONbits.TMR0IF = 0;
        TMR0H = ((0xFFFF-PLL_PRELOAD) >> 8);
        TMR0L = ((0xFFFF-PLL_PRELOAD) & 0xFF);
        INTCONbits.TMR0IE = 1;
        LATDbits.LATD4 ^= 1;
        PLL_PULSES += 1;
        //TIMING_1PPS += 0xFFFF;
    }
    return 0;
}

uint16_t gpsclock_pll_freq(void){
    return PLL_LAST_PULSES;
}
