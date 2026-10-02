#ifndef CAN_H
#define CAN_H

#include <stdio.h>


// Struct representing the data in a CAN-frame
// Using the standard: CAN 2.0A
typedef struct {
    uint16_t id;        // 11-bit CAN ID (0x000 to 0x7FF)
    uint8_t length;     // Number of bytes in payload (0-8)
    uint8_t data[8];    // Payload (up to 8 bytes)
} can_message_t;


// Function declaration
void can_init(uint8_t mode);
void can_send(const can_message_t* msg);
uint8_t can_receive(can_message_t* msg);


#endif