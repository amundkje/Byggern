#ifndef GLOBAL_STRUCTS_H
#define GLOBAL_STRUCTS_H


#include <stdint.h> //Used for *int8_t

// Use to hold both negative and positive values, to +- 100.
typedef struct {
    int8_t x;
    int8_t y;
}xy_signed;


// Use to hold ONLY positive values, 0-255.
typedef struct {
    uint8_t x;
    uint8_t y;
}xy_unsigned;


typedef struct {
    uint8_t x;
    uint8_t y;
}touch_xy_unsigned;

typedef struct {
    xy_unsigned joystick;
    xy_unsigned touch;
} pos_2D_unsigned;

typedef struct {
    xy_signed joystick;
    xy_signed touch;
} pos_2D_signed;

/**
 * Signed Int Directional Struct.
 * Contains:
 * Up,Down,Left,Right,Neutral.
 */
typedef struct{
    int8_t up;
    int8_t down;
    int8_t left;
    int8_t right;
    int8_t neutral;
}directional_pos;

#endif