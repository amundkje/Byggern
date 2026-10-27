#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

void servo_init(void);
void servo_set_pos(int8_t position_percentage);

#endif