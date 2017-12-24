
/* 
 * File:   main.c
 * Author: shawn.volpe
 *
 * Created on February 14, 2117, 12:13 AM
 */

// CONFIG2
#pragma config POSCMOD = NONE           // Primary Oscillator Select (Primary oscillator disabled)
#pragma config I2C1SEL = SEC            // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = ON             // IOLOCK Protection (Once IOLOCK is set, cannot be changed)
#pragma config OSCIOFNC = ON            // Primary Oscillator Output Function (OSC2/CLKO/RC15 functions as port I/O (RC15))
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Clock switching and Fail-Safe Clock Monitor are disabled)
#pragma config FNOSC = FRC              // Oscillator Select (Fast RC Oscillator (FRC))
#pragma config SOSCSEL = LPSOSC         // Sec Oscillator Select (Low Power Secondary Oscillator (LPSOSC))
#pragma config WUTSEL = LEG             // Wake-up timer Select (Legacy Wake-up Timer)
#pragma config IESO = OFF               // Internal External Switch Over Mode (IESO mode (Two-Speed Start-up) disabled)

// CONFIG1
#pragma config WDTPS = PS32768          // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128            // WDT Prescaler (Prescaler ratio of 1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Windowed Watchdog Timer enabled; FWDTEN must be 1)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog Timer is enabled)
#pragma config ICS = PGx1               // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config GWRP = OFF               // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF                // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG port is disabled)

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "stepper.h"
#include "pot.h"
#include "oled.h"
#include "rot.h"

// configure delay functions
#define FCY   4000000L // Fosc - osc after and prescalars of PLL, Fcy = Focy/2 Fosc is 8Mhz
#define FCY_MS ((unsigned long)(FCY / 1000))        //instruction cycles per millisecond
#define FCY_US ((unsigned long)(FCY / 1000000))     //instruction cycles per microsecond
#define DELAY_MS(ms)  __delay32(FCY_MS * ((unsigned long) ms));   //__delay32 is provided by the compiler, delay some # of milliseconds
#define DELAY_US(us)  __delay32(FCY_US * ((unsigned long) us));   //delay some number of microseconds

/*
    Tris
    1 - Input
    0 - Output

    Pin Out
    7  - yellow - pot
    18 - yellow - rot A
    21 - green - rot B
    22 - blue - rot button
*/

char* times[24] = {
    "12:00 AM", " 1:00 AM", " 2:00 AM", " 3:00 AM", " 4:00 AM", " 5:00 AM",
    " 6:00 AM", " 7:00 AM", " 8:00 AM", " 9:00 AM", "10:00 AM", "11:00 AM",
    "12:00 PM", " 1:00 PM", " 2:00 PM", " 3:00 PM", " 4:00 PM", " 5:00 PM",
    " 6:00 PM", " 7:00 PM", " 8:00 PM", " 9:00 PM", "10:00 PM", "11:00 PM"
};

int degrees[24] = {
    0, 14.4, 28.8, 43.2, 57.6, 72,
    86.4, 100.8, 115.2, 129.6, 144, 158.4,
    172.8, 187.2, 201.6, 216, 230.4, 244.8,
    259.2, 273.6, 288, 302.4, 316.8, 331.2, 345.6
};

void init() {

    TRISA = 0;
    TRISB = 0;
    
    // pot input - analog / Pin AN4
    TRISBbits.TRISB2 = 1;
    AD1PCFG = 0xffef; //0b0000000000010000;
    init_ad();
    
    // output capture - RP3 / pin 6 / OC1
    RPOR1bits.RP3R = 18;
    
    init_oled();
    init_stepper();
    init_rot();
}

void splash_screen(){
    clear();
    set_cursor(5, 0);
    print("Welcome");
    DELAY_MS(1000);
}

void screen_write(char *value) {
    clear();
    set_cursor(0, 0);
    print(value);
}

void write_state(char *time, char *msg){
    clear();
    set_cursor(4, 0);
    print(time);
    set_cursor(3, 1);
    print(msg);
}

int get_time(int time){
    unsigned char rot_direction = get_rot_value();
    if(rot_direction == 0x1){
        return (time == 23) ? 0 : time + 1;
    }else if(rot_direction == 0x2){
        return (time == 0) ? 23 : time - 1;
    }else{
        return time;
    }
}

float movement(float current, float next){
    float diff = (current > next) ? -(current - next) : next - current;
    return (diff >= 180) ? -(360 - diff) : diff;
}

char* itoa(int num){
    static char  buf[20] = {};
    sprintf(buf, "%d", num);
    return buf;
}

char* deg_msg(float deg){
    int i = deg;
    static char  buf[20] = {};
    sprintf(buf, "%s Degrees", itoa(i));
    return buf;
}

void run(){

    int time = 0;
    float current_degree = 0.0;

    while(1){

        int new_time = get_time(time);

        if(time != new_time){
            time = new_time;
            write_state(times[time], "");
        }

        if(get_rot_but_value()){

            float delta_deg = movement(current_degree, degrees[time]);

            write_state(times[time], deg_msg(delta_deg));

            move_deg(2, delta_deg);

            write_state(times[time], "    Done");

            current_degree = degrees[time];
        }
    }
}

int main(void){
    init();
    splash_screen();
    run();
}
