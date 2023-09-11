/*
 * File:   display.c
 * Author: standard
 *
 * Created on 2023年9月10日, 下午3:10
 */


#include <xc.h>

#include "common.h"

#include "display.h"

char UART1_init(const long int baudrate){
    unsigned int x;
    x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);   //SPBRG for Low Baud Rate
    if(x>255){                                   //If High Baud Rage Required{
        x = (_XTAL_FREQ - baudrate*16)/(baudrate*16); //SPBRG for High Baud Rate
        SPBRGH1 = (x >> 8) & 0xFF;
        SPBRG1  = x & 0xFF;
        BAUDCON1bits.BRG16 = 1;//Setting High Baud Rate
    } else {
        BAUDCON1bits.BRG16 = 0;
        SPBRG1 = x & 0xFF;
    }

    TXSTA1bits.SYNC = 0;                                     //Setting Asynchronous Mode, ie UART
    RCSTA1bits.SPEN = 1;                                     //Enables Serial Port
    TRISCbits.TRISC7 = 1;                                   //As Prescribed in Datasheet
    TRISCbits.TRISC6 = 1;                                   //As Prescribed in Datasheet
    RCSTA1bits.CREN = 1;                                     //Enables Continuous Reception
    TXSTA1bits.TXEN = 1;                                     //Enables Transmission
    return 1;                                                //Returns 0 to indicate UART initialization failed
}

void display_putchar(char data){
  while(!TXSTA1bits.TRMT);
  TXREG1 = data;
}

void display_output_line(char line){
    for(char i=0; i<20; i++){
        display_putchar(display_framebuffer[line][i]);
    }
}

void display_refresh(){
    display_putchar(0xFE);
    display_putchar(0x46); // clear screen
    
    display_output_line(0);
    display_output_line(2);
    display_output_line(1);
    display_output_line(3);
}

void display_set_line(char line, char data[]){
    line = line % 4;
    for(char i=0; i<20; i++){
        if(data[i] == '\0') break;
        display_framebuffer[line][i] = data[i];
    }
}


void display_init(void) {
    UART1_init(9600);
}
