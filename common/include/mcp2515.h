#ifndef MCP2515_H
#define MCP2515_H

#include <stdio.h>


// Function declarations
uint8_t mcp2515_init(uint8_t mode);
void mcp2515_reset(void);

uint8_t mcp2515_read(uint8_t address);
void mcp2515_write(uint8_t address, uint8_t data);
void mcp2515_read_bytes(uint8_t start_address,
                        uint8_t* buffer,
                        uint8_t length);
void mcp2515_write_bytes(uint8_t start_address,
                        const uint8_t* data,
                        uint8_t length);
uint8_t mcp2515_read_status(void);

void mcp2515_request_to_send(uint8_t rts_instruction);
void mcp2515_bit_modify(uint8_t address,
                        uint8_t mask,
                        uint8_t data);



#endif