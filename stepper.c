
#include "stepper.h"

// rounding macro
#define round(x) (int)(x + 0.5)

// stepper global values
float deg_per_step;
float last_rpm = 0.0;
unsigned int us_delay;
unsigned int stepper_timer;
unsigned int cur_stepper_output;

// uncomment for half step
unsigned int stepper_outputs[4][4] = {
    {1, 1, 0, 0},
    //{0, 1, 0, 0},
    {0, 1, 1, 0},
    //{0, 0, 1, 0},
    {0, 0, 1, 1},
    //{0, 0, 0, 1},
    {1, 0, 0, 1},
    //{1, 0, 0, 0}
};
float degrees_moved;
unsigned int move;
unsigned int direction;


// calculate the stepper delay from the desired rpm and the angle per step
// deg/second / deg/step = step/second
// 1000 / step/second = millisecond/step
int us_stepper_delay(float rpm){
    return round(1000.00 / (rpm * 360.00 / 60.00 / deg_per_step));
}

void init_stepper(){

    //pulley ratio
    float pulley_ratio = 30.25 / 66.0;

    stepper_timer = 0;
    cur_stepper_output = 0;
    deg_per_step = 0.70 * pulley_ratio;
    us_delay = us_stepper_delay(5.00);
    stop_motor();
    
    
    //***** Timer 3 *****//
    PR3 = 0x0F9F;       // 1 KHz 1 Interrupt every 1 ms
    T3CON = 0;          // Clear Timer 3 settings
    IFS0bits.T3IF = 0;  // Clear Output Compare interrupt flag
    IEC0bits.T3IE = 1;  // Enable Time 3 interrupts
    T3CONbits.TON = 1;  // Start Timer 3 to capture every rising edge
    TMR3 = 0;           // Reset counter to 0
}

void move_deg(float rpm, float degree){
    move = 1;
    degrees_moved = 0;

    if(degree > 0){
        direction = 1;
    }else{
        direction = 0;
        degree = -degree;
    }

    us_delay = us_stepper_delay(rpm);
    while(degrees_moved < degree){}
    stop_motor();
}

void set_rpm(float rpm){
    if(last_rpm != rpm){
        move = 1;
        us_delay = us_stepper_delay(rpm);
        stepper_timer = 0;
        last_rpm = rpm;
    }
}

void stop_motor(){
    move = 0;
}

void increment(int direction){
    if(direction){
        cur_stepper_output++;
        if (cur_stepper_output == 4){
            cur_stepper_output = 0;
        }
    }else{
        cur_stepper_output--;
        if (cur_stepper_output == -1){
            cur_stepper_output = 3;
        }
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt( void ){
    
    if(move == 1){
        stepper_timer++;
        if(stepper_timer == us_delay){
            stepper_timer = 0;

            increment(direction);

            stepper_a = stepper_outputs[cur_stepper_output][0];
            stepper_b = stepper_outputs[cur_stepper_output][1];
            stepper_c = stepper_outputs[cur_stepper_output][2];
            stepper_d = stepper_outputs[cur_stepper_output][3];
        
            degrees_moved += deg_per_step;
        }
    }
    IFS0bits.T3IF = 0;
}