/* 
 * File:   stepper.h
 * Author: shawn.volpe
 *
 * Created on April 1, 2017, 2:55 AM
 */

#include "pot.h"

void init_ad(){
    AD1CON1 = 0x00E0;   // auto convert after end of sampling
    AD1CON2 = 0;        // use MUXA, AVss and AVdd are used as Vref+/- 
    AD1CON3 = 0x1F02;   // 0001 1111 0000 0001 max sample time = 31 * Tad, Tad = 2 x Tcy must be at least 75 ns
    AD1CSSL = 0;        // no scanning required
    AD1CHS = 0b0000000000000100;      // select analog input channel AN4
    
    AD1CON1bits.ADON = 1; // turn on the ADC;
}

int get_pot_value(){
    AD1CON1bits.SAMP = 1;       // start sampling, automatic conversion will follow;
    while (!AD1CON1bits.DONE);  // wait to complete the conversion
    return ADC1BUF0;            // read the conversion result
}

