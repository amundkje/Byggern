// Includes
#include <stdint.h>
#include <stdio.h>
//#include <delay.h>
#include <avr/io.h> 


// Include self made files
#include "../common/include/uart.h"
#include "../common/include/sram.h"
#include "../common/include/adc.h"
#include "../common/include/joystick.h"

#include "../common/src/sram.c"
#include "../common/src/adc.c"
#include "../sandbox/sram_test.c"


// Defines
#define F_CPU 4915200UL // Clock Speed
#define BUFFER_SIZE 255

#include <util/delay.h> // Must be after define F_CPU 

// Global variables
volatile char* ext_adc = (volatile char*) 0x1000; // ADC address pointer


// Test adc
void adc_test(void) {
    printf("ADC test begun\n\r");

    volatile uint8_t* adc_ch0 = ext_adc;
    volatile uint8_t* adc_ch1 = ext_adc + 0x01;
    volatile uint8_t* adc_ch2 = ext_adc + 0x02;
    volatile uint8_t* adc_ch3 = ext_adc + 0x03;

    while (1) {

        *adc_ch0 = 0x00;
        _delay_us(10);
        uint8_t adc_value_ch0 = *adc_ch0;
        printf("ADC ch0 value: %d \n\r", adc_value_ch0);

        _delay_us(10);
        uint8_t adc_value_ch1 = *adc_ch1;
        printf(" ADC ch1 value: %d \n\r", adc_value_ch1);

        _delay_us(10);
        uint8_t adc_value_ch2 = *adc_ch2;
        printf("   ADC ch2 value: %d \n\r", adc_value_ch2);

        _delay_us(10);
        uint8_t adc_value_ch3 = *adc_ch3;
        printf("    ADC ch3 value: %d \n\r", adc_value_ch3);

        _delay_ms(500);
        
    }
    return;
}


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

