<<<<<<< HEAD
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
const char *joystick_directional_struct(struct Directional_power *P, enum select, unsigned int x, unsigned int y, int Deadzone)
//struct joystick_get_position(void);
//enum joystick_get_direction(struct *position);

#endif
=======
// Temp. stashed, will be recommited in the future.
>>>>>>> 380b85f0d2ef62a2a75fc1e1cab35bb2d650ac59
