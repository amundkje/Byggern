// motor.c

#include <stdint.h>

#include "sam.h"
#include "motor.h"

#define PWM_PERIOD_TICKS 20000 // 20 ms, same as servo


/**
 * Initialize pwm for motor control
 */
void motor_init(void) {
    // Configer GPIO pin PC23 as output
    // This controls the direction of the motor
    PIOC->PIO_PER |= PIO_PC23; // Activate PIO control
    PIOC->PIO_OER |= PIO_PC23; // Set as output
    PIOC->PIO_CODR |= PIO_PC23; // Initialize with DIR = 0

    // Set PB12 as peripheral B (PWMH0)
    PIOB->PIO_PDR |= PIO_PB12; // Deactivate normal GPIO
    PIOB->PIO_ABSR |= PIO_PB12; // Activate peripheral B

    // Configure PWM channel 0
    // Set clk to CLKA and start polarity high
    PWM->PWM_CH_NUM[0].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;
    PWM->PWM_CH_NUM[0].PWM_CPRD = PWM_PERIOD_TICKS; // Set period
    PWM->PWM_CH_NUM[0].PWM_CDTY = 0; // Start with 0 duty-cycle (motor still)

    // Activate PWM ch0
    PWM->PWM_ENA = PWM_ENA_CHID0;

}


/**
 * Set the motor to a given speed
 * 
 * @param speed_percent has a valid range [-100%, 100%]
 * Negative percetage values makes motor go oposite way.
 */
void motor_set_speed(int8_t speed_percent) {
    // Keep input within legal range
    if (speed_percent < -100) speed_percent = -100;
    if (speed_percent > 100) speed_percent = 100;

    // Choose direction based on percentage
    if (speed_percent < 0) {
        PIOC->PIO_CODR |= PIO_PC23; // Set DIR = 0
        speed_percent = -speed_percent; // Make speed positive for duty-cycle
    }
    else {
        PIOC->PIO_SODR |= PIO_PC23; // Set DIR = 1
    }

    // Calculate duty-cycle
    uint32_t duty_ticks = ((uint32_t)speed_percent * PWM_PERIOD_TICKS) / 100;

    // Update duty-cycle on PWM ch0
    PWM->PWM_CH_NUM[0].PWM_CDTYUPD = duty_ticks;

}


