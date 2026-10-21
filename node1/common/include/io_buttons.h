#ifndef IO_BUTTONS_H
#define IO_BUTTONS_H

// includes
#include <stdio.h>

// Defines
#define IO_BTN_LEFT     (1 << 0)
#define IO_BTN_RIGHT    (1 << 1)
#define IO_BTN_UP       (1 << 2)
#define IO_BTN_DOWN     (1 << 3)
#define IO_BTN_JOYSTICK (1 << 4)
#define IO_BTN_TOUCH    (1 << 5)


// Function declarations
uint8_t io_button_state(void);
uint8_t io_button_pressed(void);

#endif