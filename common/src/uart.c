// uart.c

#include <avr/io.h>
#include "../include/uart.h"

#define clock_frequency 4915200// Clock Speed
#define BAUD 9600
#define UBRR_VALUE ((clock_frequency / 16UL / BAUD) - 1) //UL = Unsigned Long: Unsigned meaning n >= 0, and Long meaning 32bit


// U
void uart_init(void) {

    /* Set baud rate registers*/
    UBRR0H = (unsigned char)(UBRR_VALUE>>8);   // HIGH BYTE
    UBRR0L = (unsigned char)UBRR_VALUE;        //LOW BYTE

    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);

    /* Set frame format: 8data, 2stop bit */
    UCSR1C = (1<<URSEL0)|(1<<USBS0)|(3<<UCSZ00);
}



void uart_transmit(unsigned char data) {

    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) )
    ; //HOLD

    /* Put data into buffer, sends the data */
    UDR0 = data;
}

unsigned char uart_receive(void) {

    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) )
        ;
    /* Get and return received data from buffer */
    return UDR0;
}
