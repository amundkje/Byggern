#ifndef ADC_H
#define ADC_H

#include <stdint.h> //Used for *int8_t
#include "node1_structs.h"


void adc_clk_init(void);
/**
 * Reads the analog input from both joystick and touchpad.
 * @return an unsigned struct.
 */
pos_2D_unsigned adc_read(void);
void adc_test(void);

#endif