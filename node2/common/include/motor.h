#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

void motor_init(void);
void motor_set_speed(int8_t speed_percent);
void motor_set_pos(int8_t joystick_x_percent);

#endif