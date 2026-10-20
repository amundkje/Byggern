#ifndef CAN_H
#define CAN_H

#include <stdio.h>
#include "node1_structs.h"


// Function declaration
void can_init(uint8_t mode, uint8_t rxInterrupt);
void can_interrupt_init(void);
void cantx(canMsg msg);
uint8_t canrx(canMsg* msg);


#endif