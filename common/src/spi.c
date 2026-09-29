
#include <avr/io.h>
#include "spi.h"


void spi_init() {

    // Configure output pins
    DDRB |= (1 << SPI_CAN_CS) | (1 << OLED_DC) | (1 << OLED_CS) |
            (1 << IO_CS) | (1 << SPI_MOSI) | (1 << SPI_SCK);

    // Configure input pins
    DDRB |= (1 << SPI_MISO);

    // Initialize ChipSelect outputs as HIGH
    // They are active low
    PORTB |= (1 << SPI_CAN_CS) | (1 << OLED_CS) | (1 << IO_CS);

    // Initialize Data/!Command output as HIGH
    // This does not matter, but now its a known value
    PORTB |= (1 << OLED_DC);

    // Configure SPI Control Register (SPCR)
    // This activate the MCU's SPI module
    // SPE = 1: Aktiver SPI-modulen 
    // MSTR = 1: Sett ATmega162 som SPI Master 
    // DORD = 0: Send MSB først (standard for SSD1309 og I/O-kort) 
    // CPOL = 0, CPHA = 0: SPI Mode 0 
    // SPR0 = 1, SPR1 = 0: Klokkefrekvens f\_osc / 16 (307,2 kHz ved 4,9152 MHz krystall)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

}


uint8_t spi_transmit(uint8_t data) {

}


uint8_t spi_read(void) {
    
}


