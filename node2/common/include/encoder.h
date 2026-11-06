#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>

void encoder_init(void);
int16_t encoder_read(void);
void encoder_set_zero_point(void);

#endif