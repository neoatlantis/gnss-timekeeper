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
}

char gpsclock_interrupt(void){
    if(!INTCON3bits.INT3IF) return 0;
    LAST_1PPS_SYSTEM_CLOCK = SYSTEM_CLOCK;
    COUNT_1PPS_RECEIVED += 1;
    LATDbits.LATD4 = COUNT_1PPS_RECEIVED & 0x01;
    INTCON3bits.INT3IF = 0;
    
    return 0;
}

