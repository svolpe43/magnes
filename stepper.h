/* 
 * File:   stepper.h
 * Author: shawn.volpe
 *
 * Created on April 1, 2017, 2:55 AM
 */

#include <xc.h>

// output pins
#define stepper_a       LATBbits.LATB4
#define stepper_b       LATAbits.LATA4
#define stepper_c       LATBbits.LATB8
#define stepper_d       LATBbits.LATB7

extern int us_stepper_delay(float rpm);
extern void init_stepper();
extern void set_rpm(float rpm);
extern void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt( void );
