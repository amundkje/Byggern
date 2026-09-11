// uart.c


#define F_CPU 4915200UL // Clock Speed

#include <stdio.h>
#include <avr/io.h>
#include "../include/uart.h"

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16UL / BAUD) - 1) //UL = Unsigned Long: Unsigned meaning n >= 0, and Long meaning 32bit


// 
void uart_init(void) {

    /* Set baud rate registers*/
    UBRR0H = (unsigned char)(UBRR_VALUE>>8);   // HIGH BYTE
    UBRR0L = (unsigned char)UBRR_VALUE;        //LOW BYTE

    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    /* Set frame format: 8data, 2stop bit */
    UCSR1C = (1<<URSEL0)|(1<<USBS0)|(3<<UCSZ00);
}










/**
 * Transmit function for sending data over UART.
 * 
 * @param data The value to be transmitted, not the pointer/address.
 * @return None
 * 
 * @note
 * UDRE0: USART Data Register Empty 
 * This flag is set when the transmit buffer is empty and can accept new data.
 * Should be checked BEFORE the uart_transmit() function is called to ensure 
 * that the buffer is ready to accept new data. 
 * -- Otherwise it will hold the program until the buffer is ready.
 */
void uart_transmit(unsigned char value) {

    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) )
    ; //HOLD 

    //TODO: add interupt.

    // ------------ 

    /* Put data into buffer, which sends the data */
    UDR0 = value; //Buffer becomes empty when data is sendt, repeats..
}

/**
 * Uses the uart_transmit() function to send a string of characters over UART.
 *
 * @param str The null-terminated string to be transmitted.
 * @return None
 * @note This function will transmit each character in the string one by one
 * 
 * @ uart_transmit() for details on how the transmission is handled.
 */
void uart_transmit_string(const char *str) {

    while (*str) { 
        uart_transmit(*str++);
    }
}


/**
 * Receive function for receiving data over UART.
 * 
 * @note This function will hold the program until data is received, and then return the value.
 */
char uart_receive(void) {


    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
   ;
    /* Get and return received data from buffer */
    return UDR0;
    }

    /*
    * Receive function for receiving data over UART without blocking the program.
    * If no data is available for reception, the function will return 0.
    */
   
int uart_receive_nonblocking(unsigned char *buffer) {
    if (UCSR0A & (1<<RXC0)) {
        buffer[0] = UDR0;
        return 1; // UDR0; // Return received data
    } else {
        return 0; // No data received
    }
}




/**
 * Receive function for receiving a string of characters over UART.
 * Will hold the program until 1 or more criterias are met:
 * 1. A newline character is received.
 * 2. A carriage return character is received.
 * 3. The maximum length of the string is reached.
 * 4. The null terminator is received.
 * 5. The uart_receive_nonblocking() function returns false, indicating no more data is available.
 * 
 * @param buffer The buffer where the received string will be stored.
 * @param max_length The maximum length of the buffer array, including the null terminator. 
 * A value greater than 1 is required for retrieval to work.
 * @return The number of characters received,
 * 
 * @note If the maximum length is less than 2, the function will return 0, indicating that there is not enough space for a null terminator.
 * This is to ensure that the received string can be properly null-terminated.
 * 
 * This function uses uart_receive_nonblocking() to receive characters one by one.
 * 
 */
unsigned char uart_receive_string(

    char *buffer,
     unsigned char max_length
    ) {
    unsigned char current_length = 0;

    unsigned char received_char;
    int received_status;


    if (max_length < 2) { // See docs!!
        buffer[0] = '\0'; // Null-terminate the string to avoid undefined behavior
        return 0; // Not enough space for a null terminator
    } 


    while (current_length < max_length - 1) { // -1 for null terminator
        received_status = uart_receive_nonblocking(&received_char);
        if (!received_status) {
            break; // No data received, exit the loop!
        }

        
        
        // Check for newline or carriage return:
        if (received_char == '\n' || received_char == '\r' || received_char == '\0') { 
            break; // Exit the loop if newline, carriage return.
        }

        // INCREMENTING!!
        buffer[current_length++] = received_char; 
    }

    buffer[current_length] = '\0'; // Null-terminate the string before returning..
    return current_length; // Return the number of characters received
}


//FOR FDEVOPEN:

// Can include validation with int return in later version.
int uart_putchar(char c, FILE *stream) {
    uart_transmit(c);
    return 0;
}

// stream is for later usage.
int uart_getchar(FILE *stream) {
    return uart_receive();

}