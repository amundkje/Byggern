// pid.c

#include <stdint.h>
#include "sam.h"
#include "pid.h"
#include "motor.h"
#include "encoder.h"

// PI controller weigths
// These varied a lot based on the hardware. 
// Some motors had more resistance to movment
static float Kp = 0.04f; // Experimetally found
static float Ki = 0.05f; // Experimetally found
static float dt = 0.01f; // 10 ms sampletime

#define INTEGRATE_MAX_SUM 500.0f

// Position variables
static int32_t refrence_pos = 0; // Motor Controller target encoder value
static int32_t encoder_max = 2800; // Changed with calibration
static float sum_error = 0.0f; // Error sum for integrating part

// PID enable flag
static uint8_t pid_enabled = 0;

// Calibration
#define MOTOR_CALIBRATE_STABLE_COUNT    3
#define MOTOR_CALIBRATE_SPEED           40


/**
 * Set PI controller weights
 */
void pid_init(float kp, float ki, float sample_time_sec) {
    Kp = kp;
    Ki = ki;
    dt  = sample_time_sec;
    sum_error = 0.0f;

}

/**
 * Initialize timer with capture/compare interrrupt
 *  
 * Used to generate periodic interrupt to run pid controller
 */
void pid_TC0_timer_init(void) {

    // Activate TC0 peripheral clock
    PMC->PMC_PCER0 |= (1 << ID_TC0);

    // Configure TC0 ch0 to waveform mode with reset on compare match.
    // clk3 = 2 625 000 Hz
    TC0->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_TIMER_CLOCK3 |
                                TC_CMR_WAVE |
                                TC_CMR_WAVSEL_UP_RC;
    
    // Set compare value to achieve 100 Hz (10 ms)
    TC0->TC_CHANNEL[0].TC_RC = 26250;

    // Activate RC match interrupt
    TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;

    // Activate TC0 interrupt in NVIC
    NVIC_EnableIRQ(TC0_IRQn);

    // Start clock and trigger timer
    TC0->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

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
void pid_center_motor(void) { // TODO: Extract
    pid_set_refrence(encoder_max / 2);

    pid_set_enable(1);
    while (abs(refrence_pos - encoder_read()) > 20); // This is not good enough
    pid_set_enable(0);
}


/**
 * Automatic calibration sequence.
 * Motor is moved to the left wall, and the
 * encoder value at that location is set to 0
 */
void motor_calibrate(void) {
    printf("Starting Motor Auto-Calibration... \n\r");

    pid_set_enable(0);
    
    // Move motor Left to set encoder 0 value
    motor_set_speed(-MOTOR_CALIBRATE_SPEED);

    int16_t prev_pos = encoder_read();
    int8_t stable_count = 0;

    // Wait untill ecoder position stop changing.
    while (stable_count < MOTOR_CALIBRATE_STABLE_COUNT) {

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
    motor_set_speed(MOTOR_CALIBRATE_SPEED);
    stable_count = 0;
    prev_pos = encoder_read();

    // Wait untill ecoder position stop changing.
    while (stable_count < MOTOR_CALIBRATE_STABLE_COUNT) {

        // Simple delay
        for (volatile int i = 0; i < 100000; i++);

        int16_t current_pos = encoder_read();
        if (abs(current_pos - prev_pos) < 4) {
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
    printf("Encoder Calibration Complete!\n\r");

    // Return motor to center pos
    //pid_center_motor();
}


/**
 * Calculate joystick percentage (-100%, 100%) to encoder tick values
 */
void pid_set_pos_refrence(int8_t joystick_x_percent) {
    // Prevent access to encoder values outside range
    if (joystick_x_percent < -100) joystick_x_percent = -100;
    if (joystick_x_percent > 100) joystick_x_percent = 100;
    
    refrence_pos = ((int32_t)(joystick_x_percent + 100) * encoder_max) / 200;
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

    // Update motor speed.
    // Negative signed because of direction set 
    // as left/rigth positive/negative encoder direction.
    motor_set_speed((int8_t)u);
}   
