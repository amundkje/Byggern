// Lib. Includes
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>


// Custom Drivers
#include "joystick.h"
#include "uart.h"
#include "sram.h"
#include "adc.h"
#include "spi.h"
#include "node1_structs.h"
#include "can.h"
#include "mcp2515_registers.h"
#include "mcp2515.h"


/**
 * Reads new ADC input and returns a normalised joystick input.
 *
 * @see xy_signed struct for return type.
 */
xy_signed read_joystick(enable_debug_mode) {
    pos_2D_unsigned analog_pos = adc_read();
    return joystick_normalise_input(analog_pos.joystick);
}


void get_directional_basic() {
    return joystick_get_directional(read_joystick(0));
}

void update_arm(xy_signed new_joystick_pos) {
    xy_signed new_pos = new_joystick_pos;

    int8_t byte_pck[2] = {new_pos.x, new_pos.y};
    printf(byte_pck);

}