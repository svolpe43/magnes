
/* 
 * File:   main.c
 * Author: shawn.volpe
 *
 * Created on February 14, 2017, 12:03 AM
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

// configure delay functions
#define FCY   4000000L // Fosc - osc after and prescalars of PLL, Fcy = Focy/2 Fosc is 8Mhz
#define FCY_MS ((unsigned long)(FCY / 1000))        //instruction cycles per millisecond
#define FCY_US ((unsigned long)(FCY / 1000000))     //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(FCY_MS * ((unsigned long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(FCY_US * ((unsigned long) us));   //delay some number of microseconds

// 3 different periods with 50% duty {OCRS, PR}
unsigned int duties[3][3] = {
    {0x80E8, 0xFFFF}, // 60 Hz
    {0x3E80, 0x7FFF}, // 122 Hz
    {0x07FD, 0x0FFF}  // 975 kHz
};

// Initialize ports
/*
    A0 - 
    A1 - 
    A2 - 
    A3 - 
    B0 - 
    B1 - 
    B2 - 
    B3 - 
    B4 - 
    B5 - 
    B6 - 
    B7 - 
    B8 - 
    B9 - 
    B10 - 
    B11 - 
    B12 - 
    B13 - 
    B14 - 
    B15 - 
*/
void InitPorts(void) {
    AD1PCFG = 0xFFFF;
    
    TRISA = 0;
    TRISB = 0;

    RPOR3bits.RP6R = 18; //OC to RP6
}

void InitPwm(int duty_index){
    
    //***** OC1 *****//
    OC1CONbits.OCM      = 0;        // Output compare channel is disabled
    
    OC1R                = duties[duty_index][0];
    OC1RS               = duties[duty_index][0];
    
    OC1CONbits.OCSIDL   = 0;        // Output capture will continue to operate in CPU Idle mode
    OC1CONbits.OCFLT    = 0;        // No PWM Fault condition has occurred (this bit is only used when OCM<2:0> = 111)
    OC1CONbits.OCTSEL   = 0;        // Timer2 is the clock source for output Compare
    OC1CONbits.OCM      = 0x6;      // PWM mode on OC, Fault pin disabled
    
    //***** Timer2 *****//
    PR2                 = duties[duty_index][1];
    T2CON               = 0;        // Configure timer 2 settings
    IFS0bits.T2IF       = 0;        // Clear Output Compare interrupt flag
    IEC0bits.T2IE       = 0;        // Enable Output Compare interrupts
    T2CONbits.TON       = 1;        // Start Timer2 with assumed settings
    TMR2                = 0;        // Reset counter to 0
}

int main(void){
    
    InitPorts();
    InitPwm(1);
    InitStepper();
    
    LATBbits.LATB7 = 0;
    LATBbits.LATB8 = 1;
    LATBbits.LATB9 = 0;
    
    int hold = 0;

    while(1){
        
        if(PORTAbits.RA0 == 0 && PORTAbits.RA1 == 0 && PORTBbits.RB0 == 0){
            hold = 0;
        }
        
        if(hold == 0){
            if (PORTAbits.RA0 == 1) {
                hold = 1;
                LATBbits.LATB7 = 1;
                LATBbits.LATB8 = 0;
                LATBbits.LATB9 = 0;
                InitPwm(0);
                UpdateStepper(20.00); // 86,400 X faster than actual speed
            } else if (PORTAbits.RA1 == 1) {
                hold = 1;
                LATBbits.LATB7 = 0;
                LATBbits.LATB8 = 1;
                LATBbits.LATB9 = 0;
                InitPwm(1);
                UpdateStepper(0.694); // 1,000 X faster than actual speed
            } else if (PORTBbits.RB0 == 1) {
                hold = 1;
                LATBbits.LATB7 = 0;
                LATBbits.LATB8 = 0;
                LATBbits.LATB9 = 1;
                InitPwm(2);
                UpdateStepper(0.000694); // figure out actual speed
            }
        }
    }
}
