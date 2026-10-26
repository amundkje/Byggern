#ifndef NODE1_STRUCTS_H
#define NODE1_STRUCTS_H


#include <stdint.h> //Used for *int8_t


// Struct representing the data in a CAN-frame
// Using the standard: CAN 2.0A
typedef struct {
    uint16_t id;        // 11-bit CAN ID (0x000 to 0x7FF)
    uint8_t length;     // Number of bytes in payload (0-8)
    uint8_t bytes[8];    // Payload (up to 8 bytes)
} canMsg;

// Enumerated gamestate
enum gameState {
    MENU = 0,
    RUNNING = 1,
    ERROR = 2
};

// Enumerated joystick direction
enum joystick_direction {
    NEUTRAL = 0,
    UP = 1,
    DOWN = 2,
    LEFT = 3,
    RIGHT = 4
};

// Button states struct when read from io-board
typedef struct __attribute__((packed)) {
    union {
        uint8_t right;
        struct {
            uint8_t R1:1;
            uint8_t R2:1;
            uint8_t R3:1;
            uint8_t R4:1;
            uint8_t R5:1;
            uint8_t R6:1;
        };
    };
    union {
        uint8_t left;
        struct {
            uint8_t L1:1;
            uint8_t L2:1;
            uint8_t L3:1;
            uint8_t L4:1;
            uint8_t L5:1;
            uint8_t L6:1;
            uint8_t L7:1;
        };
    };
    union {
        uint8_t nav;
        struct {
            uint8_t NB:1;
            uint8_t NR:1;
            uint8_t ND:1;
            uint8_t NL:1;
            uint8_t NU:1;
        };
    };
} Buttons;

// IO-board led light definition
typedef enum { 
IO_LED1 = 0,
IO_LED2 = 1, 
IO_LED3 = 2, 
IO_LED4 = 3, 
IO_LED5 = 4, 
IO_LED6 = 5 
} io_led_t;

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