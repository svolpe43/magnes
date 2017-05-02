
/* 
 * File:   main.c
 * Author: shawn.volpe
 *
 * Created on February 14, 2117, 12:13 AM
 */

// CONFIG2
#pragma config POSCMOD = HS             // Primary Oscillator Select (Primary oscillator disabled)
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

#define LED_1           LATBbits.LATB11
#define LED_2           LATBbits.LATB10
#define LED_3           LATBbits.LATB9

int ain;
int last_mode;
int cur_mode;

void init_peripherals() {

    TRISA = 0;
    TRISB = 0;
    
    // analog - pin 2 - AN0
    TRISBbits.TRISB2 = 1;
    AD1PCFG = 0xffef; //0b0000000000010000;
    
    // output capture - RP6 - OC1
    RPOR3bits.RP6R = 18;

    // pot input
    init_ad();
    init_stepper();
    init_oled();
    update_pwm(0);
}

void write_speed(char *value) {
    clear();
    set_cursor(5, 0);
    print(value);
    set_cursor(0, 1);
    print("Times Real Speed");
}

void update_if_needed(float rpm, int mode, char *value){
    cur_mode = mode;
    if (last_mode != cur_mode) {
        write_speed(value);
        update_pwm(mode);
        set_rpm(rpm);
        last_mode = cur_mode;
    }
}

int main(void){
    
    init_peripherals();
    
    LED_1 = 1;
    LED_2 = 1;
    LED_3 = 1;
    
    last_mode = 0;
    cur_mode = 0;

    while(1){
 
        ain = get_ad_value();
        
        if (ain > 666) {
            LED_1 = 1;
            LED_2 = 0;
            LED_3 = 0;
            update_if_needed(60.00, 0, "86,000");
            
        } else if (ain > 333) {
            LED_1 = 0;
            LED_2 = 1;
            LED_3 = 0;
            update_if_needed(20.00, 1, "1,111");
            
        } else {
            LED_1 = 0;
            LED_2 = 0;
            LED_3 = 1;
            update_if_needed(1.00, 2, "1");
            
        }
    }
}
