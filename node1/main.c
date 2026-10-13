


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
#include "global_structs.h"
#include "can.h"
#include "mcp2515_registers.h"
#include "mcp2515.h"

// User Definitions
#define BUFFER_SIZE 255

// Global Variables
volatile uint8_t can_message_pending_flag = 0;


// Remember: Please do not break the code :-)


// Place Test functions here.

// --

// Initialize stdout
void init() {
    stdout = fdevopen(uart_putchar, uart_getchar);  

    // Initialise other modules:
    adc_clk_init();
    sram_init();
    uart_init();
    spi_init();
    can_init(MODE_NORMAL, 0); // Interrupt: 1=enable, 0=disable

}


/**
 * Interrupt Service Routine
 * Called when INT0 interrupt happen when PD2 is pulled low
 */
ISR(INT0_vect) {
    // Set flag indicating a CAN-packet is received
    can_message_pending_flag = 1;
}


/**
 * Main function for the Node1 application.
 *
 * Initializes the UART and transmits a test string.
 *
 * @return 0 on successful execution.
 */
int main(void) {

    // Init --Please dont put anything above :)
    init();

    // One time code, to see system start.:
    printf("\n\r");
    printf("Hello There\n\r");
    //printf("GENERAL KENOBI!!\n\n\r");

    // Run test functions here.
    // Please delete test functions when no longer needed.
    // Test functions to be used later can have a dedicated file in sandbox.
    




    // Loop:
    while(1) {
        // Hold code;
    };

    return 0;
}


/*
*** OLD TEST FUNCTIONS ***
*** DELETE WHEN NO LONGER IN USE ***
*/


/*
* Can be used to test the UART's receive and transmit functions.
* @NOTE: This is a blocking method, so will run until manually stopped.

void test_transmit_receive() {

    char buffer[BUFFER_SIZE];
    unsigned char length_of_received = 0;

    const char *demo = "Jeg ser deg!\n\r\0";
        uart_transmit_string(demo);
    

    //char <--- A WILD CHAR APPEARD! --Yes I got it as an error....

    while (1)
    {   
        // uart_transmit(uart_receive());

        uart_receive_string(buffer, BUFFER_SIZE);
        uart_transmit_string(buffer);

        
    }
}
    // Simple test to set a pin high
void test_micro_with_pin(void) {
    // Set PC0 as output
    DDRC |= (1 << PC0);
    PORTC |= (1 << PC0);  
}

void test_joystick_normalisation() {
    // stick is already initialised.
    
    pos_2D_unsigned analog_pos;
    xy_signed norm_pos;
    directional_pos scaled_pos;
    printf("Initiating analog test....");
    while (1)
    {   
        analog_pos = adc_read();
        // printf("\n\r");
        // printf("READ ANALOG: \n\r");
        // printf("X: %d \n\r",analog_pos.joystick.x);
        // printf("Y: %d \n\r",analog_pos.joystick.y);
        // printf("------------A END--------------\n\r");
        _delay_ms(100);
    

        norm_pos = joystick_normalise_input(analog_pos.joystick);
        // printf("NORMALISED: \n\r");
        // printf("X: %d \n\r",norm_pos.x);
        // printf("Y: %d \n\r",norm_pos.y);
        // printf("------------NORM END---------------\n\r");
        // _delay_ms(100);

        scaled_pos = joystick_get_directional(norm_pos.x,norm_pos.y);

         printf("up=%d down=%d left=%d right=%d neutral=%d \n\r",
           scaled_pos.up, scaled_pos.down,
            scaled_pos.left, scaled_pos.right, scaled_pos.neutral);
        
    }
}
*/
