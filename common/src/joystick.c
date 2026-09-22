#include <stdio.h>
#include <avr/io.h>
#include "../include/joystick.h"
#include "../include/adc.h"

// hva er maks og min motstand gitt til adcen og hva er de digitale verdiene
//burde være mulig å zeroe rundt slik at 0000 0000 er min og 1111 1111 er maks...
// Calibrate current joystick position.
unsigned int max = 200;
unsigned int max_mid_x = 105/max; // +-5 fra deadzone 100
unsigned int min_mid_x = 95/max; // 
unsigned int max_mid_y = 105/max; // 
unsigned int min_mid_y = 95/max; // 
unsigned int new_power_x = 0;
unsigned int new_power_y = 0;
/*
void joystick_calibratex(unsigned int *x) {
    adc_read(&x);
    unsigned int mid_x = *x/x_max;
}
void joystick_calibratey(unsigned int *y) {
    adc_read(&y);
    unsigned int mid_y = *y/y_max;
}
*/
void joystick_updatex(unsigned int *x) {
    adc_read(x);
    unsigned int power_x = ((long)*x*100)/max;
    return power_x;
}
void joystick_updatey(unsigned int *y) {
    adc_read(y);
    unsigned int power_y = ((long)*y*100)/max;
    return power_y;
}
struct Directional_power{
    int Up;
    int Down;
    int Left;
    int Right;
};
void joystick_structxy(struct Directional_power *D, int power_x, int power_y) {
    if (power_x <= min_mid_x) {
        new_power_x = 100-power_x;
        D->Left = new_power_x;        // joystick powers logic Left Right
    } else if (power_x <= max_mid_x){ // gives 0-100 output for motors
        D->Left = 0;                  // Low volt is inversed to correct.. 
        D->Right = 0;                 // powervalues in opposite direction
    } else if (power_x >= max_mid_x){
        D->Right = power_x;
    } 
    
    if (power_y <= min_mid_y){        // The same for y direction Up down 
        new_power_y= 100-power_y;
        D->Down = new_power_y;
    } else if (power_y <= max_mid_y){
        D->Down = 0;
        D->Up = 0;
    } else if (power_y >= max_mid_y){
        D->Up = power_y;
    } 
    return D;
}

// Return a struct containing postition X and Y of the joystick.
//TODO lag en god beskrivelse av funksjonen.
/*
struct joystick_get_position(void) {

};
*/

/*
// Take in position and return a enumerated direction.
// {NUTRAL, RIGHT, LEFT, UP, DOWN}
//TODO lag en god beskrivelse av funksjonen.
enum joystick_get_direction(struct *position) {

};
*/