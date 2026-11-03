#ifndef PID_H
#define PID_H

#include <stdint.h>
#include "sam.h"

void pid_init(float kp, float ki, float sample_time_ms);
void motor_calibrate(void);
void pid_set_pos_refrence(int8_t pos_percent);
void pid_update(void);



#endif