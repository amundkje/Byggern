// test_node1.c

#include <avr/io.h>       // Registerdefinisjoner for ATmega162
#include <util/delay.h>   // Delay-funksjoner



int main(void) {

    // 1. Konfigurer PB0 som UTGANG ved å sette bit 0 i DDRB (Data Direction Register B)
    DDRB |= (1 << PB0); 

    // 2. Uendelig løkke for å toggle pinnen kontinuerlig
    while (1) {
        PORTB ^= (1 << PB0);  // Toggler tilstanden på PB0 (bruker XOR-operasjon)
        _delay_ms(500);       // Venter i 500 millisekunder (gir 1 Hz blinking)
    }

    return 0;
}
//Eivind er kul
// ^= er xor
// |= er or
// &= er and antar jeg