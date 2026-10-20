

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


#include <common/manager.h>

// User Definitions
#define BUFFER_SIZE 255

// Global Variables
volatile uint8_t can_message_pending_flag = 0;

// Remember: Please do not break the code :-)


// Place Test functions here.
// --

// Initialize stdout
void init() {
    stdout = fdevopen(uart_putchar, uart_getchar);  //Allows printf to use UART

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
    

    while (1)
    {

    }
    



    canMsg test_msg = {
        .id = 1,
        .length = 4,
        .bytes = {'P','i','n','g'}
    };

    can_send(&test_msg);
    printf("Sendt Ping\n\r");

    while (1) {
        if (can_receive(&test_msg)) {
            printf("Received answer %c%c%c%c\n\r", test_msg.bytes[0], test_msg.bytes[1], test_msg.bytes[2], test_msg.bytes[3]);
        }
    }




    // Loop:
    while(1) {
        // Hold code;
    };

    return 0;
}
