


// Lib. Includes
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <util/delay.h> // Must be after define F_CPU 


// Custom Drivers
#include "joystick.h"
#include "uart.h"
#include "sram.h"
#include "adc.h"
#include "spi.h"


// User Definitions
#define BUFFER_SIZE 255 

// Please do not break the code :)






// Simple test to set a pin high
void test_micro_with_pin(void) {
    // Set PC0 as output
    DDRC |= (1 << PC0);
    PORTC |= (1 << PC0);  
}


// Initialize stdout
void init() {
    stdout = fdevopen(uart_putchar, uart_getchar);  

    // Initialise other modules:
    adc_clk_init();
    sram_init();
    uart_init();
    spi_init();
}


/**
 * Main function for the Node1 application.
 *
 * Initializes the UART and transmits a test string.
 *
 * @return 0 on successful execution.
 */
int main(void) {

    // Init --Please dont but anything above :)
    init();    

    // One time code:
    printf("\n\r");
    printf("Hello There\n\r");
    printf("GENERAL KENOBI!!\n\n\r");



    // Loop:
    while(1) {
        // Loop code:
        
    };

    return 0;
}

