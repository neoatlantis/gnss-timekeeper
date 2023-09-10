/*
 * File:   isr.c
 * Author: standard
 *
 * Created on September 10, 2023, 9:13 PM
 */


#include <xc.h>
#include "gpsclock.h"

void __interrupt(high_priority) h_isr(void){
    gpsclock_interrupt();
}
