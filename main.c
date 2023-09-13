/*
 * File:   main.c
 * Author: standard
 *
 * Created on 2023年9月10日, 下午2:57
 */


#include "config_bits.h"
#include <xc.h>
#include <string.h>

#include "common.h"

#include "display.h"
#include "gpsclock.h"
#include "gpsread.h"
#include "isr.h"


void main(void) {
    
    OSCTUNEbits.PLLEN = 1;
    __delay_ms(1000);
    
    display_init();
    gpsread_init();
    
    display_set_line(0, "NeoAtlantis GPSClock");
    display_set_line(1, "Status: starting... ");
    display_set_line(2, "                    ");
    display_set_line(3, "                    ");
    

    
    
    
    TRISDbits.TRISD4 = 0;
    
    // initialize global interrupt settings
    RCONbits.IPEN = 1;
    
    gpsclock_init();
    
    // enable high priority interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    display_set_line(3, "                    ");;
    
    while(1){
        CLRWDT();
        
        if(gpsread_has_new_message()){
            gpsread_mark_as_read();
            
            if(0 == strncmp(gpsread_message, "$GNGGA", 6)){
                char substr[20];
                strncpy(substr, gpsread_message, 20);
                display_set_line(3, substr);
            }
        }
        
        display_sprintf(2, "t= %x p=%x", gpsclock_pll_freq(), PLL_PRELOAD);
        display_refresh();
        __delay_ms(20);
    }
}
