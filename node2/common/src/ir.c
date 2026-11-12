// ir.c

#include "sam.h"
#include "ir.h"

#define IR_ADC_CHANNEL 7 // input A0 in arduino board

/**
 * Initialize ir reading with adc
 */
void ir_init(void) {
    // 1. Enable peripheral clock for ADC (Peripheral ID 37) in PMC
    PMC->PMC_PCER1 |= (1u << (ID_ADC - 32));

    // 2. Reset the ADC controller
    ADC->ADC_CR = ADC_CR_SWRST;

    // 3. Configure mode register (prescaler, startup, tracking times)
    ADC->ADC_MR = ADC_MR_PRESCAL(4) | ADC_MR_STARTUP_SUT64 | ADC_MR_TRACKTIM(2);

    // 4. Enable desired ADC channel (e.g. channel 0 for AD0)
    ADC->ADC_CHER = (1u << IR_ADC_CHANNEL);
}


/**
 * Read current ir level
 */
uint32_t ir_read(void) {
    // Start conversion via software trigger
    ADC->ADC_CR = ADC_CR_START;

    // Wait until conversion completes for the specific channel
    while (!(ADC->ADC_ISR & (1u << IR_ADC_CHANNEL)));

    // Return the 12-bit conversion value
    return ADC->ADC_CDR[IR_ADC_CHANNEL];
}
