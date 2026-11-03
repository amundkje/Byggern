#include <stdint.h>
#include "sam.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"

// PI controller weigths
static float Kp = 1.0f;
static float Ki = 0.05f;
static float dt = 0.01f; // 10 ms sampletime

// Position variables
static int32_t refrence_pos = 0;
static int32_t encoder_max = 2800; // Changed with calibration
static float sum_error = 0.0f; // Error sum for integrating part


void pid_init(float kp, float ki, float sample_time_sec) {
    Kp = kp;
    Ki = ki;
    dt  = sample_time_sec;
    sum_error = 0.0f;

}


void motor_calibrate(void) {

}


void pid_set_pos_refrence(int8_t pos_percent) {

}


void pid_update(void){

}
