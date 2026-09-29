#ifndef SPI_H
#define SPI_H

#include <stdio.h>


// Pin definitions
#define SPI_CAN_CS  PB1 // CAN slave !ChipSelect
#define OLED_DC     PB2 // Data / !Command
#define OLED_CS     PB3 // OLED slave !ChipSelect
#define IO_CS       PB4 // IO board slave !ChipSelect
#define SPI_MOSI    PB5
#define SPI_MISO    PB6
#define SPI_SCK     PB7


// Function declaration
void spi_init();
uint8_t spi_transmit(uint8_t data);
uint8_t spi_read(void);


#endif