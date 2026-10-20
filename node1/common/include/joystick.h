#ifndef JOYSTICK_H
#define JOYSTICK_H


#include "node1_structs.h"



void joystick_init();

void __default_normalisation();

void joystick_update_normalisation(
    uint8_t min_value_x, uint8_t max_value_x,
    uint8_t min_value_y, uint8_t max_value_y,
    uint8_t outer_margin, uint8_t deadzone);


xy_signed joystick_normalise_input(xy_unsigned pos_xy);

directional_pos joystick_get_directional(int8_t x, int8_t y);



#endif
