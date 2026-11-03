#ifndef MANAGER_H
#define MANAGER_H

#include "node1_structs.h"
#include <stdint.h>


xy_signed read_joystick(uint8_t enable_debug_mode);

enum joystick_direction joystick_get_direction();

void update_arm(void);

void run_autocalibration();
uint8_t calibration_retrieve(const char *direction, uint8_t on_x_axis, uint8_t is_max_val);

#endif