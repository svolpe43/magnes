/* 
 * File:   stepper.h
 * Author: shawn.volpe
 *
 * Created on April 1, 2017, 2:55 AM
 */

#include <xc.h>

// output pins
#define stepper_a       LATAbits.LATA0
#define stepper_b       LATAbits.LATA1
#define stepper_c       LATBbits.LATB0
#define stepper_d       LATBbits.LATB1

#define stepper_aa       LATAbits.LATA2
#define stepper_bb       LATAbits.LATA3
#define stepper_cc       LATBbits.LATB4
#define stepper_dd       LATAbits.LATA4

extern int us_stepper_delay(float rpm);
extern void init_stepper();
extern void move_deg(float rpm, float degree);
extern void set_rpm(float rpm);
extern void stop_motor();
extern void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt( void );
