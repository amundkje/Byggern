#ifndef IO_BUTTONS_H
#define IO_BUTTONS_H

// includes
#include <stdio.h>
#include "node1_structs.h"

// Defines


// Function declarations
Buttons io_button_read(void);
void io_led_set(io_led_t led, uint8_t state);
void io_led_pwm(io_led_t led, uint8_t brightness);

//uint8_t io_button_pressed(void);

#endif