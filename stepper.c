
#include "stepper.h"

// rounding macro
#define round(x) (int)(x + 0.5)

// stepper global values
float angle_per_step;
float cur_rpm = 0.0;
float last_rpm = 0.0;
unsigned int us_delay;
unsigned int stepper_timer;
unsigned int cur_stepper_output;
unsigned int stepper_outputs[8][4] = {
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 0}
};

// calculate the stepper delay from the desired rpm and the angle per step
int us_stepper_delay(float rpm){
    return round(1000.00 / (rpm * 360.00 / 60.00 / angle_per_step));
}

void init_stepper(){
    
    stepper_timer = 0;
    cur_stepper_output = 0;
    angle_per_step = 0.35;
    //us_delay = us_stepper_delay(0.694); // 1,000 X faster than actual speed
    us_delay = us_stepper_delay(60.00);
    
    
    //***** Timer 3 *****//
    PR3 = 0x0F9F;       // 1 KHz 1 Interrupt every 1 ms
    T3CON = 0;          // Clear Timer 3 settings
    IFS0bits.T3IF = 0;  // Clear Output Compare interrupt flag
    IEC0bits.T3IE = 1;  // Enable Time 3 interrupts
    T3CONbits.TON = 1;  // Start Timer 3 to capture every rising edge
    TMR3 = 0;           // Reset counter to 0
}

void set_rpm(float rpm){
        
    cur_rpm = rpm;
        
    // update the stepper delay
    if(last_rpm != cur_rpm){
        us_delay = us_stepper_delay(rpm);
        stepper_timer = 0;
        last_rpm = cur_rpm;
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt( void ){
    
    stepper_timer++;
    if(stepper_timer == us_delay){
        stepper_timer = 0;
        
        // increment stepper and wrap around
        cur_stepper_output++;
        if (cur_stepper_output == 8) {
            cur_stepper_output = 0;
        } else if (cur_stepper_output == 0) {
            cur_stepper_output = 7;
        }

        stepper_a = stepper_outputs[cur_stepper_output][0];
        stepper_b = stepper_outputs[cur_stepper_output][1];
        stepper_c = stepper_outputs[cur_stepper_output][2];
        stepper_d = stepper_outputs[cur_stepper_output][3];
    }
    
    IFS0bits.T3IF = 0;
}