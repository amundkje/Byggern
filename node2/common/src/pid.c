#include <stdint.h>
#include "sam.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"

// PI controller weigths
static float Kp = 1.0f;
static float Ki = 0.05f;
static float dt = 0.01f; // 10 ms sampletime

#define INTEGRATE_MAX_SUM 500.0f

// Position variables
static int32_t refrence_pos = 0; // Motor Controller target encoder value
static int32_t encoder_max = 2800; // Changed with calibration
static float sum_error = 0.0f; // Error sum for integrating part

// PID enable flag
static uint8_t pid_enabled = 0;


void pid_init(float kp, float ki, float sample_time_sec) {
    Kp = kp;
    Ki = ki;
    dt  = sample_time_sec;
    sum_error = 0.0f;

}


/**
 * Set pid controller enable flag
 * @note also resets integrator sum on disable
 */
void pid_set_enable(uint8_t enable) {
    pid_enabled = enable;
    if (!pid_enabled) {
        motor_set_speed(0);
        sum_error = 0.0f;
    }
}


/**
 * Manualy set target position of the controller
 * in encoder ticks
 * @note motor calibration required before use
 */
void pid_set_refrence(int32_t encoder_value) {
    if (encoder_value < 0) encoder_value = 0;
    if (encoder_value > encoder_max) encoder_value = encoder_max;
    refrence_pos = encoder_value;
}

/**
 * Automatically reset position of motor to center
 */
void pid_center_motor(void) {
    pid_set_refrence(encoder_max / 2);

    pid_set_enable(1);
    while (abs(refrence_pos - encoder_read()) > 20);
    pid_set_enable(0);
}


/**
 * Automatic calibration sequence.
 * Motor is moved to the left wall, and the
 * encoder value at that location is set to 0
 */
void motor_calibrate(void) {
    printf("Starting Motor Auto-Calibration... \n\r");

    // Move motor Left to set encoder 0 value
    motor_set_speed(-30);

    int16_t prev_pos = encoder_read();
    int8_t stable_count = 0;

    // Wait untill ecoder position stop changing.
    while (stable_count < 10) {

        // Simple delay
        for (volatile int i = 0; i < 100000; i++);

        int16_t current_pos = encoder_read();
        if (abs(current_pos - prev_pos) < 2) {
            stable_count++;
        }
        else {
            stable_count = 0;
        }
        prev_pos = current_pos;
    }

    // Stop motor and set encoder value to zero
    motor_set_speed(0);
    encoder_set_zero_point(); // Set encoder value to 0
    printf("Left limit set to 0 \n\r");

    // Move motor Right to find max encoder value
    motor_set_speed(30);
    stable_count = 0;
    prev_pos = encoder_read();

    // Wait untill ecoder position stop changing.
    while (stable_count < 10) {

        // Simple delay
        for (volatile int i = 0; i < 100000; i++);

        int16_t current_pos = encoder_read();
        if (abs(current_pos - prev_pos) < 2) {
            stable_count++;
        }
        else {
            stable_count = 0;
        }
        prev_pos = current_pos;
    }

    motor_set_speed(0);
    encoder_max = encoder_read();
    printf("Right limit set to encoder value %ld \n\r", encoder_max);

    // Calculate midpoint
    refrence_pos = encoder_max / 2;

    // Return motor to center pos
    pid_center_motor();

    printf("Encoder Calibration Complete!\n\r");

}


/**
 * Calculate joystick percentage (-100%, 100%) to encoder tick values
 */
void pid_set_pos_refrence(int8_t joystick_x_percent) {
    // Prevent access to encoder values outside range
    if (joystick_x_percent < -100) joystick_x_percent = -100;
    if (joystick_x_percent > 100) joystick_x_percent = 100;
    
    refrence_pos = ((int32_t)(joystick_x_percent + 100) * encoder_max) / 2;
}

/**
 * PI-REGULATOR
 * @note This function is called at a set intervall (10 ms)
 * This ensures the same dt for the integration calculation
 * 
 */
void pid_update(void){

    // Return if PID controller is diabled
    if (!pid_enabled) return;

    int32_t actual_pos = encoder_read();

    // Calculate error (e = y_ref - y_actual)
    int32_t error = refrence_pos - actual_pos;

    // Integrate past error
    // Using past sum instead of saving every error
    sum_error += (float)error * dt;
    // Anti-Windup cutoff to prevent insane integrate buildup
    if (sum_error > INTEGRATE_MAX_SUM) sum_error = INTEGRATE_MAX_SUM;
    if (sum_error < -INTEGRATE_MAX_SUM) sum_error = -INTEGRATE_MAX_SUM;

    // Calculate control signal u (motor speed)
    float u = (Kp * (float)error) + (Ki * sum_error);

    // Make sure control signal is within legal range (-100%, 100%)
    if (u > 100.0f) u = 100.0f;
    if (u < -100.0f) u = -100.0f;

    // Update motor speed
    motor_set_speed((int8_t)u);
}   
