/* 
 * File:   oled.h
 * Author: shawn.volpe
 *
 * Created on April 9, 2017, 8:07 PM
 */

#include "oled.h"

void pulse_enable(void){
    LCD_EN = 1;
    DELAY_US(50);
    LCD_EN = 0;
}

void wait_busy( void ){
    
    unsigned char busy = 1;
    
    // set data pins to input
    LCD_TRIS |= 0xF000;
    
    // set bits for IR Read
    LCD_RS = 0;
    LCD_RW = 1;
    do {
        LCD_EN = 0;
        LCD_EN = 1;
        DELAY_US(10);
        busy = LCD_BUSY;
        LCD_EN = 0;
        pulse_enable();
        
    } while (busy);
    
    // reset data pins to output
    LCD_TRIS &= 0x0FFF;
    LCD_RW = 0;
}

void lcd_write_4(unsigned char value){
 
    // clear first 4 bits
    LCD_LAT &= 0x0FFF;
 
    // better?
    //LCD_LAT_0 = value & 0b1000;
    //LCD_LAT_1 = value & 0b0100;
    //LCD_LAT_2 = value & 0b1010;
    //LCD_LAT_3 = value & 0b1001;
    
    // clear second 4 bits of value and write it to first 4 bits of LAT
    LCD_LAT |= ( value << 12 );
 
    DELAY_US(50);
    pulse_enable();
}

void clear(){
    command(LCD_CLEARDISPLAY);
    DELAY_US(2000);
}

void home(){
  command(LCD_RETURNHOME);
  DELAY_US(2000);
}

void set_cursor(int col, int row){
    int row_offsets[] = { 0x00, 0x40};
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void send(unsigned char value, int mode) {
    
    LCD_RS = mode;
    LCD_TRIS_RW = 0;
    LCD_RW = 0;
  
    lcd_write_4(value >> 4);
    lcd_write_4(value);
}

void write(unsigned char value){
  send(value, 1);
  wait_busy();
}

void command(unsigned char value){
  send(value, 0);
  wait_busy();
}

void init_oled(void){
    
    // initialize control pins
    LCD_TRIS_RW = 0;
    LCD_TRIS_RS = 0;
    LCD_TRIS_EN = 0;
    LCD_RW = 0;
    LCD_RS = 0;
    LCD_EN = 0;
    
    // let it power on
    DELAY_US(50000);
    
    // initialize all data pins
    LCD_TRIS &= 0x0FFF;
    LCD_LAT &= 0x0FFF;
    
    // Put back into 8-bit mode
    lcd_write_4(0x3);
    DELAY_US(5000);
  
    // try with and without this, probably with tho
    lcd_write_4(0x8);
    DELAY_US(5000);
    
    // Put into 4-bit mode
    lcd_write_4(0x2);
    DELAY_US(5000);
    lcd_write_4(0x2);
    DELAY_US(5000);
    lcd_write_4(0x8);
    DELAY_US(5000);
    
    // Turn Off
    command(0x08);
    DELAY_US(5000);
    
    // Clear Display
    command(0x01);
    DELAY_US(5000);
    
    // Set Entry Mode
    command(0x06);
    DELAY_US(5000);
    
    // Home Cursor
    command(0x02);
    DELAY_US(5000);
    
    // Turn On - enable cursor & blink
    command(0x0C);
    DELAY_US(5000);
}

void print(char *text){
    
    char *letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 ,";
    int datas[] = {
        0b01000001, // A
        0b01000010, // B
        0b01000011, // C
        0b01000100, // D
        0b01000101, // E
        0b01000110, // F
        0b01000111, // G
        0b01001000, // H
        0b01001001, // I
        0b01001010, // J
        0b01001011, // K
        0b01001100, // L
        0b01001101, // M
        0b01001110, // N
        0b01001111, // O
        0b01010000, // P
        0b01010001, // Q
        0b01010010, // R
        0b01010011, // S
        0b01010100, // T
        0b01010101, // U
        0b01010110, // V
        0b01010111, // W
        0b01011000, // X
        0b01011001, // Y
        0b01011010, // Z
        0b01100001, // a
        0b01100010, // b
        0b01100011, // c
        0b01100100, // d
        0b01100101, // e
        0b01100110, // f
        0b01100111, // g
        0b01101000, // h
        0b01101001, // i
        0b01101010, // j
        0b01101011, // k
        0b01101100, // l
        0b01101101, // m
        0b01101110, // n
        0b01101111, // o
        0b01110000, // p
        0b01110001, // q
        0b01110010, // r
        0b01110011, // s
        0b01110100, // t
        0b01110101, // u
        0b01110110, // v
        0b01110111, // w
        0b01111000, // x
        0b01111001, // y
        0b01111010, // z
        0b00110000, // 0
        0b00110001, // 1
        0b00110010, // 2
        0b00110011, // 3
        0b00110100, // 4
        0b00110101, // 5
        0b00110110, // 6
        0b00110111, // 7
        0b00111000, // 8
        0b00111001, // 9
        0b00000010, //' '
        0b00101100, //','
    };
    
    int i = 0;
    while(text[i] != '\0'){
        int j = 0;
        while(text[i] != letters[j]){
            j++;
        }
        write(datas[j]);
        i++;
    }
}
