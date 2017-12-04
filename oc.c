
/* 
 * File:   pot.h
 * Author: shawn.volpe
 *
 * Created on April 4, 2017, 11:02 AM
 */

#include <xc.h>
#include "oc.h"

unsigned int duties[3][3] = {
    {0x80E8, 0xFFFF}, // 60 Hz
    {0x3E80, 0x7FFF}, // 122 Hz
    {0x07FD, 0x0FFF}  // 975 kHz
};

void update_pwm(int duty_index){
    
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
