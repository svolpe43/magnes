/* 
 * File:   oled.h
 * Author: shawn.volpe
 *
 * Created on April 9, 2017, 8:07 PM
 */

#include <xc.h>

#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x28
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

#define LCD_RS          LATBbits.LATB0
#define LCD_RW          LATBbits.LATB1
#define LCD_EN          LATBbits.LATB3

#define LCD_TRIS_RS     TRISBbits.TRISB0
#define LCD_TRIS_RW     TRISBbits.TRISB1
#define LCD_TRIS_EN     TRISBbits.TRISB3

#define LCD_BUSY        PORTBbits.RB15;
#define LCD_PORT        PORTB
#define LCD_TRIS        TRISB
#define LCD_LAT         LATB

#define FCY   4000000L // Fosc - osc after and prescalars of PLL, Fcy = Focy/2 Fosc is 8Mhz
#define FCY_MS ((unsigned long)(FCY / 1000))        //instruction cycles per millisecond
#define FCY_US ((unsigned long)(FCY / 1000000))     //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(FCY_MS * ((unsigned long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(FCY_US * ((unsigned long) us));   //delay some number of microseconds

extern void pulse_enable(void);
extern void wait_busy( void );
extern void lcd_write_4(unsigned char value);
extern void clear();
extern void home();
extern void set_cursor(int col, int row);
extern void send(unsigned char value, int mode);
extern void write(unsigned char value);
extern void command(unsigned char value);
extern void init_oled(void);
extern void print(char *text);
