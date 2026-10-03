#ifndef MCP2515_REGISTERS_H
#define MCP2515_REGISTERS_H

/*
This header file contain definitions of registers
and bitmasks required to operate the mcp2515.

Instructions as sent to the mcp2515 over the SPI.
Bitmasks is required to modify single bits without
having to read->modify->write. 
*/


/*
==================================================
    MCP2515 SPI Instructions
==================================================
*/
#define MCP_RESET       0XC0 // Reset internal registers, set Config Mode
#define MCP_READ        0x03 // Read data from chosen register address
#define MCP_WRITE       0X02 // Write data to chosen register address
#define MCP_RTS_TX0     0X81 // Request-to-Send for transmit buffer 0
#define MCP_RTS_TX1     0X82 // Request-to-Send for transmit buffer 1
#define MCP_RTS_TX2     0X84 // Request-to-Send for transmit buffer 2
#define MCP_RTS_ALL     0X87 // Request-to-Send for all transmit buffers
#define MCP_READ_STATUS 0XA0 // Read status bits (rx/tx flags)
#define MCP_RX_STATUS   0xB0 // Read rx buffer status
#define MCP_BITMOD      0x05 // Change some bits in chosen register


/*
==================================================
    MCP2515 Register Addresses
==================================================
*/
// Control and status registers:
#define MCP_CANSTAT     0x0E // CAN Status Register
#define MCP_CANCTRL     0x0F // CAN Control Register

// Bit timing configuration (Baud rate):
#define MCP_CNF3        0x28 // Phase segment 2 
#define MCP_CNF2        0x29 // Propagation / Phase segment 1
#define MCP_CNF1        0x2A // Baud rate Prescaler and Sync Jump Width

// Interrupt and Error flags:
#define MCP_CANINTE     0x2B // Interrupt Enable Register
#define MCP_CANINTF     0x2C // Interrupt Flag Register
#define MCP_EFLAG       0x2D // Error Flag Register
#define MCP_TEC         0x1C // Transmit Error Counter
#define MCP_REC         0x1D // Receive Error Counter

// Transmit Buffer 0 (TXB0)
#define MCP_TXB0CTRL    0x30 // TX Buffer 0 Control Register
#define MCP_TXB0SIDH    0x31 // Standard Identifier High (ID bit 10-3)
#define MCP_TXB0SIDL    0x32 // Standard Identifier Low (ID bit 2-0 + IDE/SSR)
#define MCP_TXB0DLC     0x35 // Data length code (DLC 0-8)
#define MCP_TXB0D0      0x36 // Data byte 0 (start of payload)

// Receive Buffer 0 (RXB0)
#define MCP_RXB0CTRL    0x60 // RX Buffer 0 Control Register
#define MCP_RXB0SIDH    0x61 // Standard Identifier High (ID bit 10-3)
#define MCP_RXB0SIDL    0x62 // Standard Identifier Low (ID bit 2-0 + IDE/SSR)
#define MCP_RXB0DLC     0x65 // Data length code (DLC 0-8)
#define MCP_RXB0D0      0x66 // Data byte 0 (start of payload)


/*
==================================================
    MCP2515 Bitmask and Operating Modes (CANCTRL / CANSTAT)
==================================================
*/
#define MODE_NORMAL     0X00 // Normal mode
#define MODE_SLEEP      0X20 // Sleep mode
#define MODE_LOOPBACK   0X40 // For testing. Internal loop to read own tx
#define MODE_LISTENONLY 0X60 // Listen only, send no ACK
#define MODE_CONFIG     0X70 // Configuration mode to change settings
#define MODE_MASK       0XE0 // Mask for the 3 top bits in CANCTRL/CANSTAT


/*
==================================================
    MCP2515 Interrupt Flag bits (CANINTF)
==================================================
*/
#define MCP_RX0IF       (1 << 0) // RX Buffer 0 full
#define MCP_RX1IF       (1 << 1) // RX Buffer 1 full
#define MCP_TX0IF       (1 << 2) // TX Buffer 0 empty
#define MCP_TX1IF       (1 << 3) // TX Buffer 1 empty
#define MCP_TX2IF       (1 << 4) // TX Buffer 2 empty
#define MCP_MERRF       (1 << 7) // Message Error Flag


/*
==================================================
    Thats all folks ¯\_(ツ)_/¯
==================================================
*/


#endif