
#include <avr/io.h> 


void spi_init(){


    DDRB &= (1 << PB0); // SET INPUT
    DDRB |= (1 << PB3) | (1 << PB4); //SET OUTPUT

    PORTB |= (1 << PB3) | (1 << PB4); // SET HIGH
    


}