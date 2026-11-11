#ifndef ADC_H
#define ADC_H

#include <stdint.h> 
#include "node1_structs.h"


void adc_clk_init(void);
pos_2D_unsigned adc_read(void);
void adc_test(void);

#endif