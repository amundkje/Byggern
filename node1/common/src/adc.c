// adc.c

#include <stdio.h>
#include <avr/io.h>
#include "adc.h"
#include <util/delay.h>

// Macro definition "the byte stored at, treated as volatile"
#define ADC_JOYSTICK_X (*(volatile uint8_t *)0x1000) // AIN0 
#define ADC_JOYSTICK_Y (*(volatile uint8_t *)0x1001) // AIN1
#define ADC_TOUCH_X    (*(volatile uint8_t *)0x1002) // AIN2
#define ADC_TOUCH_Y    (*(volatile uint8_t *)0x1003) // AIN3


// Initialize the clock signal to the adc.
// Clock frequency generated is half of the MCU clock.
// F_clk_adc = 2.46 MHz
// This is achieved by using no prescaler and counting to 0, making the signal toggle on every clock cycle.
void adc_clk_init(void) {

    // Activate PD4 (OC3A) as output.
    DDRD |= (1 << PD4);

    // Configure Timer 3 as CTC 
    // TCCR3A (Timer/Counter3 Control Register A)
    TCCR3A = (1 << COM3A0);

    // WGM32 = 1 (for CTC-modus)
    // CS32:0 = 001 (Prescaler N = 1, No prescaling)
    TCCR3B = (1 << WGM32) | (1 << CS30);

    // Sett top value for counter.
    // By setting it to 0, the counter trigger a reset every time its incremented.
    OCR3A = 0;

};


/**
 * Reads the analog input from both joystick and touchpad.
 * @return an unsigned struct.
 */
void adc_reset_read(void){
    ADC_JOYSTICK_X = 0x00; //INITIATE NEW READ.
}


/**
 * Reads current ADC values into a positional matrix
 * Supports debug printing.
 * @return filled positional matrix.
 * 
 */
pos_2D_unsigned adc_read(void) {   
    pos_2D_unsigned input_state_t;

    adc_reset_read(); //Read new values.
    _delay_us(10); //µs is needed for ADC to process.

    input_state_t.joystick.x = ADC_JOYSTICK_X;
    input_state_t.joystick.y = ADC_JOYSTICK_Y;
    input_state_t.touch.x = ADC_TOUCH_X;
    input_state_t.touch.y = ADC_TOUCH_Y;
    return input_state_t;
};

