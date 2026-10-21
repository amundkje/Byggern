#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

#include "spi.h"

/**
 * Return state of buttons in a byte. Use bitmask to read button state
 * 
 */
uint8_t io_button_state(void) {

    spi_select(SPI_SLAVE_IO);

    // Send dummy byte to receive button state.
    uint8_t button_state = spi_transceive(0xFF);
    spi_deselect();

    return button_state;
}

/**
 * Function to detect button pressed edge. 
 * Button bit is only 1 when button had a rising edge.
 */
uint8_t io_button_pressed(void) {

    static uint8_t prev_state = 0;

    spi_select(SPI_SLAVE_IO);

    // Send dummy byte to receive button state.
    uint8_t current_state = spi_transceive(0xFF);
    spi_deselect();

    uint8_t button_pressed = current_state & ~prev_state;

    prev_state = current_state;
    
    return button_pressed;
}

