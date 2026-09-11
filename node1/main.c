#include <stdint.h>
// #include <delay.h>
#include <stdio.h>

#include "../common/include/uart.h"
#include "../sandbox/sram_test.c"
#include <avr/io.h> 


#define F_CPU 4915200UL // Clock Speed
#define BUFFER_SIZE 255



void test_micro_with_pin(void) {
    // Set PA0 as output
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
    init();
    uart_init();
    
    SRAM_test(); // sram testfile from assignment
    
    // test_transmit_receive();
    printf("\n\r");
    printf("Hello There\n\r");
    printf("GENERAL KENOBI!!");
   



    //const char *test_string = "Hello from the other side!\n";
    //uart_transmit_string(test_string);

    return 0;
}

