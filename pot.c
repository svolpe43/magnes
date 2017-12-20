/* 
 * File:   stepper.h
 * Author: shawn.volpe
 *
 * Created on April 1, 2017, 2:55 AM
 */

#include "pot.h"

// globals smoothing algorithm
float total;
unsigned int readIndex;
float average;
float last_average;
unsigned int total_smoothing_readings = 100;
float readings[100];

void init_ad(){
    AD1CON1 = 0x00E0;   // auto convert after end of sampling
    AD1CON2 = 0;        // use MUXA, AVss and AVdd are used as Vref+/- 
    AD1CON3 = 0x1F02;   // 0001 1111 0000 0001 max sample time = 31 * Tad, Tad = 2 x Tcy must be at least 75 ns
    AD1CSSL = 0;        // no scanning required
    AD1CHS = 0b000000000000100;      // select analog input channel AN4
    
    AD1CON1bits.ADON = 1; // turn on the ADC;

    total = 0;
    readIndex = 0;
    average = 0;
    last_average = 0;

    int i = 0;
    while(i < total_smoothing_readings){
        readings[i] = 0;
        i++;
    }
}

int get_ad_value(){
    AD1CON1bits.SAMP = 1;       // start sampling, automatic conversion will follow;
    while (!AD1CON1bits.DONE);  // wait to complete the conversion
    return smooth(ADC1BUF0);            // read the conversion result
}

int smooth(int reading){

    // subtract the reading that we are removing
    total = total - readings[readIndex];

    // get newest value
    readings[readIndex] = reading;

    // add the new reading to the total
    total = total + readings[readIndex];

    // increment position in array
    readIndex = readIndex + 1;

    // when we have a full array wrap around and return an average
    if (readIndex >= total_smoothing_readings) {

        readIndex = 0;

        // only start calculating an average when we have 10 values
        average = total / total_smoothing_readings;

        last_average = average;

        return average;
    }

    return last_average;
}

