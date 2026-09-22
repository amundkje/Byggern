#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <stdio.h>

// Uncomment when used:

/*
void joystick_calibratex(unsigned int x);
void joystick_calibratey(unsigned int y);
*/
void joystick_updatex(unsigned int *x);
void joystick_updatey(unsigned int *y);
void joystick_structxy(struct Directional_power *D, int power_x, int power_y)
//struct joystick_get_position(void);
//enum joystick_get_direction(struct *position);

#endif