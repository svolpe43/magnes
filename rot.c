/* 
 * File:   rot.h
 * Author: shawn.volpe
 *
 * Created on April 1, 2017, 2:55 AM
 */

#include "rot.h"

void init_rot(){
    AD1PCFG = 0xFFFF;
    RPINR7bits.IC2R = 6;
}

void InitInputCapture(){
    
    last_state = 0b00;
    
    //***** Timer 2 *****//
    T2CON               = 0;         // Clear Timer 2 settings
    IFS0bits.T2IF       = 0;         // Clear Output Compare interrupt flag
    IEC0bits.T2IE       = 0;         // Disable Time 2 interrupts
    T2CONbits.TON       = 1;         // $$$$$ Start Timer 2 to capture every rising edge
    TMR2                = 0;         // Reset counter to 0
    
    //***** Input Capture 2 *****//
    IC2CON              = 0;         // Reset input capture configuration
    IC2CONbits.ICTMR    = 0;         // $$$$$ use timer 2 as clock source
	IC2CONbits.ICSIDL   = 0;         // continue at idle CPU	
	IC2CONbits.ICI      = 0;         // interrupt every capture event
	IC2CONbits.ICM      = 0b001;     // WHAT DOES THIS DO IF T2 has that setting $$$$$ capture on every rising edge
    IEC0bits.IC2IE      = 1;         // Enable Interrupt
}

// triggers on rising edge
void __attribute__ ((__interrupt__, no_auto_psv)) _IC2Interrupt(void){
    
    rising_edge_2 = IC2BUF;                 // Get the new IC value
    T = rising_edge_2 - rising_edge_1;      // Calculate period
    rising_edge_1 = rising_edge_2;			// Shift values
    
    /*
     * Smoothing algorithm for the Period Value
     */
    
    // subtract the reading that we are removing
    total = total - readings[readIndex];
    
    // get newest value
    readings[readIndex] = T;
    
    // add the new reading to the total
    total = total + readings[readIndex];
    
    // increment position in array
    readIndex = readIndex + 1;

    // wrap around to when at the end of array
    if (readIndex >= 10) {
        readIndex = 0;
        start_smoothing = 1;
    }

    // only start calculating an average when we have 10 values
    if(start_smoothing == 1){
        average = total / 10;
    }

    IFS0bits.IC2IF = 0;
}