// can.c

#include <avr/io.h>
#include <avr/interrupt.h>
#include "can.h"
#include "mcp2515.h"
#include "mcp2515_registers.h"


/**
 * This function initializes the mcp2515 chip
 */
void can_init(uint8_t mode) {

    // Configure the mcp2515 chip
    mcp2515_init(mode);

    // Deactivate mask filtering on RXB0 to receive all messages.
    // Set bit 5-6 in RXB0CTRL HIGH
    mcp2515_bit_modify(MCP_RXB0CTRL, 0x60, 0x60);

    // Activate RB0 interrupt signal on msg recieve to MCU
    mcp2515_bit_modify(MCP_CANINTE, MCP_RX0IF, MCP_RX0IF);

}

/**
 * Configure mcp2515 to send interrupt signal to mcu
 * when a can msg is received
 */
void can_interrupt_init(void) {

    // Set PD2 as input
    DDRD &= ~(1 << PD2);

    // Configure INT0 to trigger on falling edge
    MCUCR |= (1 << ISC01);
    MCUCR &= ~(1 << ISC00);

    // Activate external interrupt INT0
    GICR |= (1 << INT0);

    // Activate global interrupts
    sei();
}


/**
 * Send can message using TXB0 in mcp2515
 * @param msg is a can_message_t struct containing frame data
 */
void can_send(const can_message_t* msg) {

    // First, Load can-packet content to registers in mcp2515

    // Write the 11 bit ID to SIDH and SIDL registers
    // Write ID bit 10-3 to SIDH
    mcp2515_write(MCP_TXB0SIDH, (msg->id >> 3) & 0xFF);
    // Write ID bit 2-0 to bit 7-5 in SIDL
    mcp2515_write(MCP_TXB0SIDL, (msg->id & 0x07) << 5);

    // Write Data Length Code to bit 3-0 in DLC register
    mcp2515_write(MCP_TXB0DLC, msg->length & 0x0F);

    // Write databytes to payload registers (TXB0D0 - TXB0D7)
    mcp2515_write_bytes(MCP_TXB0D0, msg->data, msg->length);

    // With everything loaded in the mcp2515,
    // send can-packet with RTS
    mcp2515_request_to_send(MCP_RTS_TX0);

}


/**
 * Receive can message via RXB0 in the mcp2515
 * @param msg is a can_message_t struct to recieve frame data
 * @return 1 for message received, else 0 for no new msg in rx-register
 */
uint8_t can_receive(can_message_t* msg) {

    // Check if msg-received flag in mcp2515 is set (RX0IF)
    uint8_t interrupt_flags = mcp2515_read(MCP_CANINTF);

    // If msg received, read packet content i RXB0 registers
    if (interrupt_flags & MCP_RX0IF) {

        // Extract ID from SIDH and SIDL
        uint8_t sidh = mcp2515_read(MCP_RXB0SIDH);
        uint8_t sidl = mcp2515_read(MCP_RXB0SIDL);
        msg->id = (sidh << 3) | (sidl >> 5);

        // Extract data payload length
        msg->length = mcp2515_read(MCP_RXB0DLC);

        // Extract data payload
        mcp2515_read_bytes(MCP_RXB0D0, msg->data, msg->length);

        // Reset interrupt flag so new messages can be received
        mcp2515_bit_modify(MCP_CANINTF, MCP_RX0IF, 0x00);

        return 1; // Message received
    }

    return 0; // No new message in RXB0
}

