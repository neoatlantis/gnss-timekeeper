/*
 * File:   isr.c
 * Author: standard
 *
 * Created on September 10, 2023, 9:13 PM
 */


#include <xc.h>
#include "gpsclock.h"
#include "gpsread.h"

void __interrupt(high_priority) h_isr(void){
    gpsclock_interrupt();
}

void __interrupt(low_priority) l_isr(void){
    gpsread_interrupt();
}