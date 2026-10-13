#include <stdio.h>
#include <avr/io.h>
#include "../include/joystick.h"

#include "adc.h"
#include <stdlib.h>




// Algorithm for normalsation:
//e.g 40 : 170
// outer_margin = 1.05 
// OFFSETT_VALUE = -(max + min)*outer_margin/2
// 40-OFFSET_VALUE = -65
// 170-OFFSET_VALUE = 65
//    -65 : 65
// normalised = norm_input * (100/offsetted_max_value)

// ----

uint8_t OFFSET_VALUE_X;
uint8_t MAX_ANALOG_VAL_X;
uint8_t OFFSET_VALUE_Y;
uint8_t MAX_ANALOG_VAL_Y;
uint8_t DEADZONE;

void joystick_init() {
    //Currently init. with predetermined values, automate in future.
    __default_normalisation();
}

/**
 * A predetermined normalisation setup, for quick startup.
 */
void __default_normalisation() {
    joystick_update_normalisation(
        36,175,36,175,
        1,10
    );
}

/**
 * Updates the normalisation constants based on given x,y values.
 * 
 * @param outer_margin is added/substracted from both x and y to extend max reach.
 * NOTE: If incorrect values, check if the |min_value| equals |max_value|, they should be almost the same value.
 */
void joystick_update_normalisation(
    uint8_t min_value_x, uint8_t max_value_x,
    uint8_t min_value_y, uint8_t max_value_y,
    uint8_t outer_margin, uint8_t deadzone){

    // Calculate OFFSET
    OFFSET_VALUE_X = (max_value_x + min_value_x)/2;
    OFFSET_VALUE_Y = (max_value_y + min_value_y)/2;
    //Store offsett, max value.
    MAX_ANALOG_VAL_X = max_value_x-OFFSET_VALUE_X+outer_margin;
    MAX_ANALOG_VAL_Y = max_value_y-OFFSET_VALUE_Y+outer_margin;

    DEADZONE = deadzone; // percentage.
    

    //Just for debug:
    printf("OFF_X: %d | OFF_Y: %d | MAX_X: %d | MAX_Y: %d | DZ: %d\r\n", 
       OFFSET_VALUE_X, OFFSET_VALUE_Y, 
       MAX_ANALOG_VAL_X, MAX_ANALOG_VAL_Y, 
       DEADZONE);

}


xy_signed joystick_normalise_input(xy_unsigned pos_xy) {
    xy_signed new_pos_xy;
    // printf("Got values x,y: %d : %d \n\r",pos_xy.x,pos_xy.y);
    // printf("Offset xy is: %d  -- %d ..\n\r",OFFSET_VALUE_X,OFFSET_VALUE_Y);
    new_pos_xy.x = (((pos_xy.x-OFFSET_VALUE_X) *100) /MAX_ANALOG_VAL_X);
    new_pos_xy.y = (((pos_xy.y-OFFSET_VALUE_Y)* 100)/MAX_ANALOG_VAL_Y);
    return new_pos_xy;
}


#if 0
//Currently not in use.
const char *Direction[]={
    "Up",
    "Down",
    "Left",
    "Right",
    "Neutral"
};
#endif 


/**
 * Changes the two-axis value, into four seperate directions, plus neutral. 
 * Checks if the two-axises is inside deadzone, if so neutral = 1, and all other values 
 * are zero.
 * 
 * @return a directional struct, see "directional_pos".
 * @important: Values for left and down will be additively inverted (-1*), to positive.
 */
directional_pos joystick_get_directional(int8_t x, int8_t y) {
    directional_pos new_pos = {0}; //Init to all zero values.
    uint8_t neutral_x = 0; //0 or 1 only.
    uint8_t neutral_y = 0; // .--.
    //Check if neutral on either axis:
    if(abs(x) < DEADZONE) {
        neutral_x = 1;
        // printf("DEADZONE X \n\r");
    }
    if(abs(y) < DEADZONE) {
        neutral_y = 1;
        // printf("DEADZONE Y \n\r");
    }
    if (neutral_x & neutral_y) {
        //Joystick is neutral on both axis.
        new_pos.neutral = 1;   // up/down/left/right = 0, neutral = 1.
        return new_pos;
    }
    if(!neutral_x){
        if (x > 0) { //Positive, RIGHT.
            new_pos.right = x;
        } else { // Negative, LEFT.
            new_pos.left = -x; //Minus to revert to positive.
        }
    }
    if(!neutral_y){
        if (y > 0) { //Positive, UP.
            new_pos.up = y;
        } else { // Negative, DOWN.
            new_pos.down = -y; //Minus to revert to positive.
        }
    }
    return new_pos;
}
// Min touchpadvalues? and what type is it?
/*
void joystick_touchpad(unsigned int x, unsigned int y){
}
*/
