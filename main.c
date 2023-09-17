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
#include "ui.h"
#include "gpsclock.h"
#include "gpsread.h"
#include "isr.h"


#define read2digits(str, i) ((str[i]-'0')*10+(str[i+1]-'0'))

void main(void) {
    
    char ui_redraw_counter = 0;
    
    OSCTUNEbits.PLLEN = 1;
    __delay_ms(1000);
    
    display_init();
    gpsread_init();
    
    ui_init();
    

    
    
    
    TRISDbits.TRISD4 = 0;
    
    // initialize global interrupt settings
    RCONbits.IPEN = 1;
    
    gpsclock_init();
    
    // enable high priority interrupts
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
    
    
    while(1){
        CLRWDT();
        
        if(gpsread_has_new_message() && !gpsclock_override_on_next_1pps){
            gpsread_mark_as_read();
            
            if(0 == strncmp(gpsread_message, "$GNGGA", 6)){
                SYSTEM_CLOCK_NEXT_OVERRIDE.microsecond = 0;
                SYSTEM_CLOCK_NEXT_OVERRIDE.second = read2digits(gpsread_message, 11);
                SYSTEM_CLOCK_NEXT_OVERRIDE.minute = read2digits(gpsread_message, 9);
                SYSTEM_CLOCK_NEXT_OVERRIDE.hour = read2digits(gpsread_message, 7);
                
                gpsclock_override_on_next_1pps = 1;
            }
        }
        
        ui_refresh();
        
        if(ui_redraw_counter++ > 10){
            ui_redraw();
            ui_redraw_counter = 0;
        }
        
    }
}
