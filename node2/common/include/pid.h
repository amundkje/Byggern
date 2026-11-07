#ifndef PID_H
#define PID_H

#include <stdint.h>
#include "sam.h"

void pid_init(float kp, float ki, float sample_time_ms);
void pid_TC0_timer_init(void);
void pid_set_enable(uint8_t enable);
void motor_calibrate(void);
void pid_set_pos_refrence(int8_t pos_percent);
void pid_update(void);
void pid_set_refrence(int32_t encoder_value);
void pid_center_motor(void);


#endif