
#include <stdint.h>

#include "sam.h"
#include "servo.h"

// Counter values.
// Using 1 MHz clk, giving 1 tick = 1 us
#define PWM_PERIOD_TICKS    20000   // 20 ms (50 Hz)
#define SERVO_MIN_TICKS     900     // 0.9 ms (-100%) 
#define SERVO_CENTER_TICKS  1500    // 1.5 ms (0%)
#define SERVO_MAX_TICKS     2100    // 2.1 ms (+100%) 


void servo_init(void) {

    // Activate PWM peripheral (ID_PWN=36)
    PMC->PMC_PCER1 |= (1 << (ID_PWM - 32));

    // Activate pin PB13, and set it to output PWM
    PIOB->PIO_PDR |= PIO_PB13; // Deactivate normal GPIO
    PIOB->PIO_ABSR |= PIO_PB13; // Set pin to peripheral B

    // Set PWM clock to 1 MHz.
    // System clk is 84 MHz -> devide by 84
    PWM->PWM_CLK = PWM_CLK_DIVA(84);

    // Setup PWM channel 1 on PB13
    // Choose clk A and start polarity high
    PWM->PWM_CH_NUM[1].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    // Set period to 20000 ticks
    PWM->PWM_CH_NUM[1].PWM_CPRD = PWM_PERIOD_TICKS;
    
    // Set initial duty-cycle to senter pos 
    PWM->PWM_CH_NUM[1].PWM_CDTY = SERVO_CENTER_TICKS;

    // Activate channel 1
    PWM->PWM_ENA = PWM_ENA_CHID1;
}


void servo_set_pos(int8_t position_percentage) {

    // Make sure in data is inside range
    if (position_percentage < -100) position_percentage = -100;
    if (position_percentage > 100) position_percentage = 100;

    /**
     * Calculate from percentage to microseconds.
     * Formula 1500 us + (percentage * 600 us) / 100
     */
    uint32_t duty_ticks = SERVO_CENTER_TICKS +  
            ((int32_t)position_percentage * (SERVO_MAX_TICKS - SERVO_CENTER_TICKS)) / 100; 
    
    // Safety check: values must be within specified range
    // 0.9 ms to 2.1 ms
    if (duty_ticks < SERVO_MIN_TICKS) duty_ticks = SERVO_MIN_TICKS;
    if (duty_ticks > SERVO_MAX_TICKS) duty_ticks = SERVO_MAX_TICKS;

    // Update PWM duty-cycle
    static uint32_t prev_duty_ticks = 0; 
    if (duty_ticks != prev_duty_ticks) { 
        PWM->PWM_CH_NUM[1].PWM_CDTYUPD = duty_ticks; 
        prev_duty_ticks = duty_ticks; 
    }
}
