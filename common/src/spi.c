
#include <avr/io.h>
#include "spi.h"


/*
Initialize the SPI communication.
*/
void spi_init() {

    // Configure output pins
    DDRB |= (1 << CAN_CS) | (1 << OLED_CS) |
            (1 << IO_CS) | (1 << MOSI) | (1 << SCK);

    // Configure input pins
    DDRB &= ~(1 << MISO);

    // Initialize ChipSelect outputs as HIGH
    // They are active low
    PORTB |= (1 << CAN_CS) | (1 << OLED_CS) | (1 << IO_CS);

    // Configure SPI Control Register (SPCR)
    // This activate the MCU's SPI module
    // SPE = 1: Aktiver SPI-modulen 
    // MSTR = 1: Sett ATmega162 som SPI Master 
    // DORD = 0: Send MSB først (standard for SSD1309 og I/O-kort) 
    // CPOL = 0, CPHA = 0: SPI Mode 0 
    // SPR0 = 1, SPR1 = 0: Klokkefrekvens f\_osc / 16 (307,2 kHz ved 4,9152 MHz krystall)
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

}


/*
This function set the target slave select pin LOW (active) 
and all other slave select HIGH (deactivated)
*/
void spi_select(spi_slave_t slave) {

    // Activate selected slave.
    // Deactivate the others as a precaution.
    switch (slave) {
        case SPI_SLAVE_CAN:
            PORTB |= (1 << OLED_CS) | (1 << IO_CS);
            PORTB &= ~(1 << CAN_CS);
            break;
        case SPI_SLAVE_OLED:
            PORTB |= (1 << CAN_CS) | (1 << IO_CS);
            PORTB &= ~(1 << OLED_CS);
            break;
        case SPI_SLAVE_IO:
            PORTB |= (1 << CAN_CS) | (1 << OLED_CS);
            PORTB &= ~(1 << IO_CS);
            break;
    }
}


/*
This function set all slave select pins HIGH (deactivated)
*/
void spi_deselect(void) {

    // Set all ChipSelect signals HIGH (deactivated)
    PORTB |= (1 << CAN_CS) | (1 << OLED_CS) | (1 << IO_CS);
}


/*
Send one byte over SPI and read received byte.
This function is blocking until byte shift is complete.
*/
uint8_t spi_transceive(uint8_t data) {

    // Write data to SPI data register
    // SPDR = SPI Data Registry
    SPDR = data;

    // Wait until shift is complete (SPIF bit goes high)
    // SPSR = SPI Status Registry
    while (!(SPSR & (1 << SPIF)));

    // Return received data from SPI Data Registry
    return SPDR;

}


/*
Read data from slave by sending a dummy byte
*/
uint8_t spi_read(void) {

    // Send dummy byte
    return spi_transceive(0xFF);
}


/*
Send data to slave without reading recieved data
*/
void spi_write(uint8_t data) {

    // Send byte without reading recieved data
    spi_transceive(data);
}


/**
* This function sends and receives n bytes.
* @param uint8_t If tx_data = NULL this sends dummy-bytes.
*                Needs to be same length as length param.
* @param uint8_t If rx_data = NULL this does not store the recived bytes.
* @param uint8_t length = n
*
*/
void spi_transceive_bytes(spi_slave_t slave, 
                        const uint8_t* tx_data, 
                        uint8_t* rx_data, 
                        uint8_t length)
{

    // Activate selected slave
    spi_select(slave);

    // Loop through every byte
    for (uint8_t i = 0; i < length; i++) {

        // Send data if it exist, else send dummy-bytes
        uint8_t byte_to_send = (tx_data != NULL) ? tx_data[i] : 0xFF;
        uint8_t received_byte = spi_transceive(byte_to_send);

        // Store data if rx_data buffer is given
        if (rx_data != 0) {
            rx_data[i] = received_byte;
        }

    }

    // Deselect slave
    spi_deselect();
}


