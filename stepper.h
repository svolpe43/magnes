/* 
 * File:   stepper.h
 * Author: shawn.volpe
 *
 * Created on April 1, 2017, 2:55 AM
 */

#include <xc.h>

// Stepper output pins
#define stepper_a    LATBbits.LATB15
#define stepper_b    LATBbits.LATB14
#define stepper_c    LATBbits.LATB13
#define stepper_d    LATBbits.LATB12

extern int us_stepper_delay(float rpm);
extern void init_stepper();
extern void update_stepper(float rpm);
extern void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt( void );
