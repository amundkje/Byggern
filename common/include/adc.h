#ifndef ADC_H
#define ADC_H

#include <stdint.h> //Used for uint8_t

typedef struct {
    uint8_t x;
    uint8_t y;
}joystick_xy;

typedef struct {
    uint8_t x;
    uint8_t y;
}touch_xy;

typedef struct {
    joystick_xy joystick;
    touch_xy touch;
} pos_2D;

void adc_clk_init(void);
pos_2D adc_read(void);
void adc_test(void);



#endif