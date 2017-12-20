/* 
 * File:   rot.h
 * Author: shawn.volpe
 *
 * Created on April 1, 2017, 2:55 AM
 */

#include "rot.h"
#include <xc.h>

unsigned char last_state = 0b0011;

void init_rot(){
    TRISBbits.TRISB9 = 1;
    TRISBbits.TRISB10 = 1;
    TRISBbits.TRISB11 = 1;
}

/*
 * returns 3 values
 * 0x0 - no change
 * 0x1 - turn clockwise
 * 0x2 - turn counterclockwise
 *
 */
unsigned char get_rot_value(){
        unsigned char state = PORTBbits.RB10 << 1 | PORTBbits.RB9;
        unsigned char return_val = 0x0;

        if(last_state != state){
            if(last_state == 0x0){
                if(state == 0x1){
                    return_val = 0x1;
                }else if(state == 0x2){
                    return_val = 0x2;
                }
            }
            last_state = state;
        }

        return return_val;
}

unsigned char get_rot_but_value(){
    return PORTBbits.RB11;
}

