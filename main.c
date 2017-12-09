
/* 
 * File:   main.c
 * Author: shawn.volpe
 *
 * Created on February 14, 2117, 12:13 AM
 */

// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = SEC            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = ON             // IOLOCK Protection (Once IOLOCK is set, cannot be changed)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator (FRC))
#pragma config SOSCSEL = LPSOSC         // Sec Oscillator Select (Low Power Secondary Oscillator (LPSOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Windowed Watchdog Timer enabled; FWDTEN must be 1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is enabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "stepper.h"
#include "pot.h"
#include "oled.h"
#include "oc.h"

// configure delay functions
#define FCY   4000000L // Fosc - osc after and prescalars of PLL, Fcy = Focy/2 Fosc is 8Mhz
#define FCY_MS ((unsigned long)(FCY / 1000))        //instruction cycles per millisecond
#define FCY_US ((unsigned long)(FCY / 1000000))     //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(FCY_MS * ((unsigned long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(FCY_US * ((unsigned long) us));   //delay some number of microseconds

// tris
// 1 - Input
// 0 - Output

#define LED           LATBbits.LATB5

int last_mode;
int cur_mode;

char* times[24] = {
    "1200 AM", "100 AM", "200 AM", "300 AM", "400 AM", "500 AM",
    "600 AM", "700 AM", "800 AM", "900 AM", "1000 AM", "1100 AM",
    "1200 PM", "100 PM", "200 PM", "300 PM", "400 PM", "500 PM",
    "600 PM", "700 PM", "800 PM", "900 PM", "1000 PM", "1100 PM"
};

void init_peripherals() {

    TRISA = 0;
    TRISB = 0;
    
    // pot input - analog / Pin AN4
    TRISBbits.TRISB2 = 1;
    AD1PCFG = 0xffef; //0b0000000000010000;
    init_ad();
    
    // output capture - RP3 / pin 6 / OC1
    RPOR1bits.RP3R = 18;
    
    init_oled();
    init_stepper();
}

void screen_write(char *value) {
    clear();
    set_cursor(0, 0);
    print(value);
}

void update(float rpm, int mode, char *value){
    cur_mode = mode;
    if (last_mode != cur_mode) {
        screen_write(value);
        set_rpm(rpm);
        last_mode = cur_mode;
    }
}

int main(void){
    
    init_peripherals();
    
    LATBbits.LATB3 = 1;

    last_mode = 0;
    cur_mode = 0;

    screen_write("Moving 90 degrees");
    //move_deg(5, 180);

    while(1){
        int index = get_ad_value() / 43;
        update(5, index, times[index]);
    }
}
