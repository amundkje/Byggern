#include <stdio.h>
#include <avr/io.h>
#include "../include/joystick.h"
#include "../include/adc.h"

// hva er maks og min motstand gitt til adcen og hva er de digitale verdiene
//burde være mulig å zeroe rundt slik at 0000 0000 er min og 1111 1111 er maks...
// Calibrate current joystick position.
unsigned int x_max = 255;
unsigned int y_max = 255;
void joystick_calibratex(unsigned int *x) {
    adc_read(&x);
    unsigned int mid_x = *x/x_max;
}:
void joystick_calibratey(unsigned int *y) {
    adc_read(&y);
    unsigned int mid_y = *y/y_max;
}:

void joystick_updatex(unsigned int *x) {
    adc_read(&x);
    
    unsigned int power_x = *x/x_max;

    return power_x;
}:
void joystick_updatey(unsigned int *y) {
    adc_read(&y);
    
    unsigned int power_y = *y/y_max;

    return power_y;
}:
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