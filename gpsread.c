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
#include "gpsclock.h"

char gpsread_buffer[255];
char gpsread_buffer_index = 0;
char gpsread_buffering = 0;


char gpsread_unread = 0;

#define read16LE(buf, i) (buf[i] | (buf[i+1]<<8))

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
        
        if(rc_byte == '$' || rc_byte == 0xb5){
            gpsread_buffering = 1;
            gpsread_buffer_index = 0;
        }
        
        if(gpsread_buffering){
            gpsread_buffer[gpsread_buffer_index] = rc_byte;
            gpsread_buffer_index += 1;
            if(gpsread_buffer_index == 0xFF){
                // buffer overflow
                gpsread_buffering = 0;
            }
            
            if (rc_byte == '\n' || rc_byte == '\r'){
                // end of NMEA message
                strcpy(gpsread_message, gpsread_buffer);
                gpsread_unread = 1;
                gpsread_buffering = 0;
            } else if (
                gpsread_buffer[0] == 0xb5 && 
                gpsread_buffer[1] == 0x62 &&
                gpsread_buffer_index > 6 // after more than 4 bytes read
            ){
                // gps buffered is UBX data
                uint16_t buf_n = 8 + (gpsread_buffer[4] | (gpsread_buffer[5] << 8));
                if(buf_n > 250){
                    gpsread_buffering = 0;
                } else {
                    if(gpsread_buffer_index >= buf_n){
                        memcpy(gpsread_message, gpsread_buffer, buf_n);
                        gpsread_unread = 1;
                        gpsread_buffering = 0;
                        for(uint8_t i=0; i<0xFF; i++) gpsread_buffer[i] = 0;
                    }
                }
            }
        }
        
        if(!RCSTA2bits.CREN) RCSTA2bits.CREN = 1;
        PIR3bits.RC2IF = 0;
    }
}

void gpsread_process_new_message(){
    if(0 == gpsread_unread) return;
    
    if(gpsread_message[0] == 0xb5 && gpsread_message[1] == 0x62){
        gpsread_process_ubx();
    }
    
    
    gpsread_unread = 0;
}

#define upoffset(i) i+6
void gpsread_process_ubx(void){
    char CLS = gpsread_message[2], ID = gpsread_message[3];
    uint16_t LEN = read16LE(gpsread_message, 4);
    if(LEN >= 0x250) return;
    
    uint8_t CK_A=0, CK_B=0, i=0;
    for(i=2; i<LEN+6; i++){
        CK_A = CK_A + gpsread_message[i];
        CK_B = CK_B + CK_A;
    }
    
    if(gpsread_message[LEN+6]!=CK_A || gpsread_message[LEN+7] != CK_B){
        return;
    }
    
    if(0x01==CLS && 0x21==ID){
        // UBX-NAV-TIMEUTC
        gpsclock_override_on_next_1pps = 1;
        SYSTEM_CLOCK_NEXT_OVERRIDE.year = read16LE(gpsread_message, upoffset(12));
        SYSTEM_CLOCK_NEXT_OVERRIDE.month = gpsread_message[upoffset(14)];
        SYSTEM_CLOCK_NEXT_OVERRIDE.day = gpsread_message[upoffset(15)];
        SYSTEM_CLOCK_NEXT_OVERRIDE.hour = gpsread_message[upoffset(16)];
        SYSTEM_CLOCK_NEXT_OVERRIDE.minute = gpsread_message[upoffset(17)];
        SYSTEM_CLOCK_NEXT_OVERRIDE.second = gpsread_message[upoffset(18)];
        return;
    }
    
    if(0x0D==CLS && 0x01==ID){
        uint32_t towMS = read16LE(gpsread_message, upoffset(0)) |
                (read16LE(gpsread_message, upoffset(2)) << 16);
        NOP();
    }
    
    return;
}