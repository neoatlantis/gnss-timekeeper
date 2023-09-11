/*
 * File:   main.c
 * Author: standard
 *
 * Created on 2023年9月10日, 下午2:57
 */


#include "config_bits.h"
#include <xc.h>

#include "common.h"


#include "display.h"
#include "gpsclock.h"
#include "isr.h"


void main(void) {
    
    
    TRISDbits.TRISD4 = 0;
    
    // initialize global interrupt settings
    RCONbits.IPEN = 1;
    
    gpsclock_init();
    
    // enable high priority interrupts
    INTCONbits.GIEH = 1;
    
    display_init();
    __delay_ms(100);
    
    display_set_line(0, "hello world                 x ");
    
    while(1){
        display_sprintf(1, "t= %x", gpsclock_pll_freq());
        display_refresh();
        __delay_ms(20);
    }
}
