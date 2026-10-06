#ifndef CAN_H
#define CAN_H

#include <stdio.h>
#include "global_structs.h"


// Function declaration
void can_init(uint8_t mode);
void can_interrupt_init(void);
void can_send(const can_message_t* msg);
uint8_t can_receive(can_message_t* msg);


#endif