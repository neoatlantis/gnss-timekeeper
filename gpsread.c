/*
 * File:   gpsread.c
 * Author: standard
 *
 * Created on 2023年9月12日, 下午10:13
 */


#include <xc.h>
#include <string.h>
#include "common.h"
#include "gpsread.h"

char gpsread_buffer[255];
char gpsread_buffer_index = 0;
char gpsread_buffering = 0;


char gpsread_unread = 0;

char UART2_init(const long int baudrate){
    unsigned int x;
    x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);   //SPBRG for Low Baud Rate
    if(x>255){                                   //If High Baud Rage Required{
        x = (_XTAL_FREQ - baudrate*16)/(baudrate*16); //SPBRG for High Baud Rate
        SPBRGH2 = (x >> 8) & 0xFF;
        SPBRG2  = x & 0xFF;
        BAUDCON2bits.BRG16 = 1;//Setting High Baud Rate
    } else {
        BAUDCON2bits.BRG16 = 0;
        SPBRG2 = x & 0xFF;
    }

    TXSTA2bits.SYNC = 0;                                     //Setting Asynchronous Mode, ie UART
    RCSTA2bits.SPEN = 1;                                     //Enables Serial Port
    TRISGbits.TRISG1 = 0;                                   //As Prescribed in Datasheet
    TRISGbits.TRISG2 = 1;                                   //As Prescribed in Datasheet
    RCSTA2bits.CREN = 1;                                     //Enables Continuous Reception
    //TXSTA2bits.TXEN = 1;                                     //Enables Transmission
    
    // set up interrupts
    IPR3bits.RC2IP = 0;
    PIE3bits.RC2IE = 1;
    
    
    return 1;                                                //Returns 0 to indicate UART initialization failed
}

void gpsread_init(){
    UART2_init(38400);
    
}

void gpsread_interrupt(){
    if(PIR3bits.RC2IF){
        char rc_byte = RCREG2;
        
        if(rc_byte == '$'){
            gpsread_buffering = 1;
            gpsread_buffer_index = 0;
        } else if (rc_byte == '\n' || rc_byte == '\r'){
            strcpy(gpsread_message, gpsread_buffer);
            gpsread_unread = 1;
            gpsread_buffering = 0;
        }
        
        if(gpsread_buffering){
            gpsread_buffer[gpsread_buffer_index] = rc_byte;
            gpsread_buffer_index += 1;
            if(gpsread_buffer_index == 0xFF){
                // buffer overflow
                gpsread_buffering = 0;
            }
        }
        
        if(!RCSTA2bits.CREN) RCSTA2bits.CREN = 1;
        PIR3bits.RC2IF = 0;
    }
}

char gpsread_has_new_message(){
    return gpsread_unread != 0;
}

void gpsread_mark_as_read(){
    gpsread_unread = 0;
}