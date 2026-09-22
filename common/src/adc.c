#include <stdio.h>
#include <avr/io.h>
#include "../include/adc.h"


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


void adc_read(void) {
//x,y return takk ;)
};
