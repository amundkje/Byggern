#ifndef IR_H
#define IR_H

#include <stdint.h>
#include "sam.h"

void ir_init(void);
uint32_t ir_read(void);

#endif