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

// Can IDs
#define CAN_ID_GAMESTATE    1
#define CAN_ID_JOYSTICK     2
#define CAN_ID_ERROR        3



/**
 * Reads new ADC input and returns a normalised joystick input.
 * enable_debug_mode is not in use yet.
 * @see xy_signed struct for return type.
 */
xy_signed read_joystick(uint8_t enable_debug_mode) {
    pos_2D_unsigned analog_pos = adc_read();
    
    // For debug
    //printf("X = %d    Y = %d\n\r", analog_pos.joystick.x, analog_pos.joystick.y);

    return joystick_normalise_input(analog_pos.joystick);
}


enum joystick_direction joystick_get_direction() {
    return joystick_get_directional_basic(read_joystick(0));
}

/**
 * Read joystick postition and send to node2 via CAN bus
 */
void update_arm(void) {
    xy_signed new_pos = read_joystick(0);

    canMsg msg = {
        .id = CAN_ID_JOYSTICK,
        .length = 2,
        .bytes = {new_pos.x, new_pos.y}
    };

    cantx(msg);

}




