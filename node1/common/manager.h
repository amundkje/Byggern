#ifndef MANAGER_H
#define MANAGER_H

#include "node1_structs.h"
#include <stdint.h>


xy_signed read_joystick(uint8_t enable_debug_mode);

void update_arm(void);


#endif