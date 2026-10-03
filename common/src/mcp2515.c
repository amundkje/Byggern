// mcp2515.c

#include <avr/io.h>
#include "mcp2515.h"
#include "mcp2515_registers.h"
#include "spi.h"


/**
* Send a sequence of instruction over spi to initialize the mcp2515
* @note Requires spi_init(); to be run first
* @return 0 on succsesful init, else return 1
*/
uint8_t mcp2515_init(uint8_t mode) {

    // Force reset to enter known state
    mcp2515_reset();

    // Verify the mcp2515 is in CONFIG MODE
    uint8_t status = mcp2515_read(MCP_CANSTAT);
    if ((status & MODE_MASK) != MODE_CONFIG) {
        return 1; // IC not entered Config Mode correctly
    }

    // Set bit-timing (Baud-Rate)
    // 250 kbps at 16 MHz
    /**
     * See chapter 5 in MCP2515 Datasheet
     * 
     * One bit is devided into 16, giving 16 Time Quanta (TQ)
     * The TQ are devided as follow:
     * TQ1 = Sync (As described by the standard)
     * TQ2-3 = PropSeg (To acount for delay on cable)
     * TQ4-10 = PS1
     * Sample is taken here at 62.5% of the frame
     * TQ11-16 = PS2
     * 
     * Oscillator frequency = 16 MHz
     * Desired CAN Baud-Rate = 250 KHz
     * Desired amount of TQ is 16
     * Length of one TQ = (1/CAN Baud Rate) / 16 = 250 ns
     * Calculate Baud Rate Prescaler (BRP)
     *  TQ = 2*(BRP+1) / F_osc
     *  -> BRP = TQ*F_osc/2 - 1 = 1
     */
    // bit 7-6 = 00 gir SJW = 1*TQ (How many TQ PS1/2 can be extended during sync)
    // bit 5-0 = 00001 gir BRP = 1
    uint8_t cnf1_config = 0x01;
    mcp2515_write(MCP_CNF1, cnf1_config);

    // bit 7 = 1, length of PS2 is determined by CNF3
    // bit 6 = 0, sample once
    // bit 5-3 = 110, PS1 is 6+1 = 7 TQ
    // bit 2-0 = 001, PRSEG length is 1+1 = 2 TQ
    uint8_t cnf2_config = 0xB1;
    mcp2515_write(MCP_CNF2, cnf2_config);

    // bit 2-0 = 101, PS2 is 5+1 = 6 TQ
    uint8_t cnf3_config = 0x05;
    mcp2515_write(MCP_CNF3, cnf3_config);


    // Set mode
    mcp2515_bit_modify(MCP_CANCTRL, MODE_MASK, mode);

    // Verify mode
    status = mcp2515_read(MCP_CANSTAT);
    if ((status & MODE_MASK) != mode) {
        return 1; // IC not entered correct Mode
    }
    
    return 0; // Successful init

}


/*
Send reset instruction over SPI to the mcp2515
*/
void mcp2515_reset(void) {
    spi_select(SPI_SLAVE_CAN);
    spi_write(MCP_RESET);
    spi_deselect();
}


/**
* Read a single byte from a MCP2515 register
* @param address is the register in the msp2515 to be accessed
* @returns the byte from said address
*/
uint8_t mcp2515_read(uint8_t address) {

    spi_select(SPI_SLAVE_CAN);  
    spi_write(MCP_READ);            // Send read instruction to mcp
    spi_write(address);             // Send address to read from

    uint8_t rx_data = spi_read();   // Read data by sending a dummy-byte
    spi_deselect();

    return rx_data;
}


/**
* Write a single byte to a MCP2515 register
* @param address is the register in the msp2515 to be accessed
*/
void mcp2515_write(uint8_t address, uint8_t data) {

    spi_select(SPI_SLAVE_CAN);  
    spi_write(MCP_WRITE);           // Send write instruction to mcp
    spi_write(address);             // Send address to read from
    spi_write(data);                // Send data
    spi_deselect();

}


/**
 * 
 */
void mcp2515_read_bytes(uint8_t start_address,
                        uint8_t* buffer,
                        uint8_t length)
{
    spi_select(SPI_SLAVE_CAN);
    spi_write(MCP_READ);
    spi_write(start_address);

    // mcp2515 automatically increment address pointer when CS is kept low 
    // and next databyte is requested.
    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = spi_read();
    }

    spi_deselect();
}


/**
 * 
 */
void mcp2515_write_bytes(uint8_t start_address,
                        const uint8_t* data,
                        uint8_t length)
{
    spi_select(SPI_SLAVE_CAN);
    spi_write(MCP_WRITE);
    spi_write(start_address);

    // mcp2515 automatically increment address pointer when CS is kept low 
    // and next databyte is sent.
    for (uint8_t i = 0; i < length; i++) {
        spi_write(data[i]);
    }

    spi_deselect();
}


/**
 * This reads the Status Register of the mcp2515
 */
uint8_t mcp2515_read_status(void) {

    spi_select(SPI_SLAVE_CAN);
    spi_write(MCP_READ_STATUS);
    uint8_t status = spi_read();
    spi_deselect();

}


/**
* Send Request-To-Send (RTS) instruction to send data in chose TX buffer on the CAN-bus
* @param rts_instruction is the TX buffer to be sent. (Example: MCP_RTS_TX0)
* @note TX buffer needs to be preloaded with CAN-packet content first 
*/
void mcp2515_request_to_send(uint8_t rts_instruction) {

    spi_select(SPI_SLAVE_CAN);
    spi_write(rts_instruction);
    spi_deselect();

}


/**
* This function modifies 1 or more bit i a register without changing the rest
* Works the same way as mcp2515_write(), but a mask is given aswell
*/
void mcp2515_bit_modify(uint8_t address,
                        uint8_t mask,
                        uint8_t data)
{

    spi_select(SPI_SLAVE_CAN);
    spi_write(MCP_BITMOD);      // Bit-modification Instruction
    spi_write(address);
    spi_write(mask);
    spi_write(data);
    spi_deselect();
}
