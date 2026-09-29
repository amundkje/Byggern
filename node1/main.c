


// Lib. Includes
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <util/delay.h> // Must be after define F_CPU 


// Custom Drivers
#include "../common/include/uart.h"
#include "../common/include/sram.h"
#include "../common/include/adc.h"
#include "../common/include/joystick.h"


// User Definitions
#define BUFFER_SIZE 255 

// Please do not break the code :)






// Simple test to set a pin high
void test_micro_with_pin(void) {
    // Set PC0 as output
    DDRC |= (1 << PC0);
    PORTC |= (1 << PC0);  

}


/*
* Can be used to test the UART's receive and transmit functions.
* @NOTE: This is a blocking method, so will run until manually stopped.
*/
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

// Initialize stdout
void init() {
    stdout = fdevopen(uart_putchar, uart_getchar);  
}


/**
 * Main function for the Node1 application.
 *
 * Initializes the UART and transmits a test string.
 *
 * @return 0 on successful execution.
 */
int main(void) {

    // Initialization:
    init();
    adc_clk_init();
    sram_init();
    uart_init();
    

    // One time code:
    printf("\n\r");
    printf("Hello There\n\r");
    printf("GENERAL KENOBI!!\n\n\r");

    adc_test();

    // Loop:
    while(1) {
        // Loop code:
        
    };

    return 0;
}

