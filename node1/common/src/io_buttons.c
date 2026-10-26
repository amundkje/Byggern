#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>

#include "spi.h"
#include "io_buttons.h"
#include "node1_structs.h"

#define IO_CMD_BUTTONS 0x04
#define IO_CMD_LED_ONOFF 0X05
#define IO_CMD_LED_PWM 0X06



/**
 * Return state of buttons in a byte. Use bitmask to read button state
 * 
 */
Buttons io_button_read(void) {

    Buttons buttons;

    spi_select(SPI_SLAVE_IO);

    // Send cmd byte to start button read
    spi_write(IO_CMD_BUTTONS);
    _delay_us(40); // Specified delay before read

    // Send dummy bytes to read the three registers
    // containing button states.
    // Read right buttons
    buttons.right = spi_transceive(0xFF);
    _delay_us(2); // Specified delay before next read

    // Read left buttons
    buttons.left = spi_transceive(0xFF);
    _delay_us(2); // Specified delay before next read

    // Read nav "joystick"
    buttons.nav = spi_transceive(0xFF);
    _delay_us(2); // Specified delay before next read

    spi_deselect();

    return buttons;
}


/**
 * Turn on/off led on the io-board
 */
void io_led_set(io_led_t led, uint8_t state) {
    if (led > IO_LED6) return;

    spi_select(SPI_SLAVE_IO);

    // Send cmd for led activation
    spi_write(IO_CMD_LED_ONOFF);
    _delay_us(40); // Specified delay before read 
    
    // Send which led to be toggled
    spi_write((uint8_t)led);
    _delay_us(2);

    // Send let state
    spi_write(state ? 1 : 0);

    spi_deselect();
}

/**
 * Choose brightness for a led on the io-board
 */
void io_led_pwm(io_led_t led, uint8_t brightness) {
    if (led > IO_LED6) return;

    spi_select(SPI_SLAVE_IO);

    // Send cmd for led activation
    spi_write(IO_CMD_LED_PWM);
    _delay_us(40); // Specified delay before read 
    
    // Send which led to be toggled
    spi_write((uint8_t)led);
    _delay_us(2);

    // Send let brightness
    spi_write(brightness);

    spi_deselect();
}



/** OLD. PROB REMOVE
 * 
 * Function to detect button pressed edge. 
 * Button bit is only 1 when button had a rising edge.
 
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

*/