#ifndef SPI_H
#define SPI_H

#include <stdio.h>


// Pin definitions
#define CAN_CS      PB1 // CAN slave !ChipSelect
#define OLED_CS     PB3 // OLED slave !ChipSelect
#define IO_CS       PB4 // IO board slave !ChipSelect
#define MOSI        PB5
#define MISO        PB6
#define SCK         PB7


// Enum to define SPI slaves
typedef enum {
    SPI_SLAVE_CAN,
    SPI_SLAVE_OLED,
    SPI_SLAVE_IO
} spi_slave_t;


// Function declaration
void spi_init();
void spi_select(spi_slave_t slave);
void spi_deselect(void);
uint8_t spi_transcive(uint8_t data);
uint8_t spi_read(void);
void spi_write(uint8_t data);
void spi_transcive_bytes(spi_slave_t slave, 
                        const uint8_t* tx_data, 
                        uint8_t* rx_data, 
                        uint8_t length);


#endif