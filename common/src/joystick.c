#include <stdio.h>
#include <avr/io.h>
#include "../include/joystick.h"
#include "../include/adc.h"

// hva er maks og min motstand gitt til adcen og hva er de digitale verdiene
//burde være mulig å zeroe rundt slik at 0000 0000 er min og 1111 1111 er maks...
// Calibrate current joystick position.
unsigned int x_max = 2000;
unsigned int y_max = 2000;
void joystick_calibrate(unsigned int *x, unsigned int *y) {
    adc_read(&x, &y);
    unsigned int mid_x = *x/x_max;
    unsigned int mid_y = *y/y_max;
}:

void joystick_update(unsigned int *x, unsigned int *y) {
    adc_read(&x, &y);
    unsigned int power_x = *x/x_max;
    unsigned int power_y = *y/y_max; 
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