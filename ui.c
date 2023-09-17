/*
 * File:   ui.c
 * Author: standard
 *
 * Created on 2023年9月17日, 上午2:08
 */


#include <xc.h>
#include <string.h>

#include "display.h"
#include "gpsclock.h"

void ui_init(void){
    display_set_line(0, "NeoAtlantis GPSClock");
    display_set_line(2, "                    ");
    display_set_line(3, "                    ");
}

void ui_refresh(void) {
    struct gpsclock_status_t gps_status = gpsclock_status();
    
    char status_line[20]   = "                    ";
    
    if(gps_status.tracking){
        strncpy(status_line, "FixOK", 5);
    } else {
        strncpy(status_line, "NoFix", 5);
    }
    
    if(gps_status.verified_2kpps){
        strncpy(status_line+6, "PPS2k", 5);
    } else {
        strncpy(status_line+6, "-----", 5);
    }
    
    display_set_line(1, status_line);
    
    display_sprintf(
        2,
        "%04u-%02u-%02u %02u:%02u:%02u",
        SYSTEM_CLOCK.year,
        SYSTEM_CLOCK.month,
        SYSTEM_CLOCK.day,
        SYSTEM_CLOCK.hour,
        SYSTEM_CLOCK.minute,
        SYSTEM_CLOCK.second
    );
    
    return;
}

void ui_redraw(void){
    display_refresh();
}
