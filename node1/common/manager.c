// Lib. Includes
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <util/delay.h>
#include <oled.h>

// Custom Drivers
#include "joystick.h"
#include "uart.h"
#include "sram.h"
#include "adc.h"
#include "spi.h"
#include "node1_structs.h"
#include "can.h"
#include "mcp2515_registers.h"
#include "mcp2515.h"

// Can IDs
#define CAN_ID_GAMESTATE    1
#define CAN_ID_JOYSTICK     2
#define CAN_ID_ERROR        3



/**
 * Reads new ADC input and returns a normalised joystick input.
 * enable_debug_mode is not in use yet.
 * @see xy_signed struct for return type.
 */
xy_signed read_joystick(uint8_t enable_debug_mode) {
    pos_2D_unsigned analog_pos = adc_read();
    
    // For debug
    //printf("X = %d    Y = %d\n\r", analog_pos.joystick.x, analog_pos.joystick.y);

    return joystick_normalise_input(analog_pos.joystick);
}


enum joystick_direction joystick_get_direction() {
    return joystick_get_directional_basic(read_joystick(0));
}

/**
 * Read joystick postition and send to node2 via CAN bus
 */
void update_arm(void) {
    xy_signed new_pos = read_joystick(0);

    canMsg msg = {
        .id = CAN_ID_JOYSTICK,
        .length = 2,
        .bytes = {new_pos.x, new_pos.y}
    };

    can_tx(msg);

}











uint8_t calibration_retrieve(const char *direction, uint8_t on_x_axis, uint8_t is_max_val){
    uint8_t new_value = is_max_val ? 0 : UINT8_MAX;
    uint8_t read_value = 0;
    oled_reset();
    oled_print("JOYSTICK:   ");
    oled_print(direction);
    oled_goto_line(1);
    oled_goto_column(0);
    oled_print("Hold: 2 seconds..");
    _delay_ms(3000);
    oled_reset();
    oled_home();
    
   


    uint8_t i = 0;
    while (i < 15)
    {

        // Check analog axis wanted:
        if (on_x_axis) {
            read_value = adc_read().joystick.x; 
        } else {
            read_value = adc_read().joystick.y; 
        }


        
        if (is_max_val&& read_value > new_value)
        {
            new_value = read_value;
        }else if (!is_max_val && read_value < new_value)
        {
            new_value = read_value;
        }
        
        _delay_ms(200);
        i++;
    }
    oled_reset();
    oled_home();
    oled_print("Well done 'Applause..'");

    return new_value;
}
 
    
    
   
    
    



/**
 * TODO: Apply dummyproofing, feedback system, outer_margin and deadzone support.
 */
void run_autocalibration() {
    char value_text[4];

    // PreCalib-info.
    oled_reset();
    oled_home();
    oled_print("Running autocalibration...");
    oled_goto_line(1);
    oled_goto_column(0);
    oled_print("Ready up gimbo");
    _delay_ms(5000); //Apply fitting delay.
    //Gather new profile
    uint8_t y_up = calibration_retrieve("UPWARDS",0,1);
    oled_reset();
    oled_home();
    snprintf(value_text, sizeof(value_text), "%u", y_up);
    oled_print(value_text);
    _delay_ms(500); //Apply fitting delay.



    uint8_t x_r = calibration_retrieve("RIGHT",1,1);
    snprintf(value_text, sizeof(value_text), "%u", x_r);
    oled_print(value_text);
    _delay_ms(500); //Apply fitting delay.



    uint8_t y_down = calibration_retrieve("DOWN",0,0);
    snprintf(value_text, sizeof(value_text), "%u", y_down);
    oled_print(value_text);
    _delay_ms(500); //Apply fitting delay.



    uint8_t x_l = calibration_retrieve("LEFT",1,0);
    snprintf(value_text, sizeof(value_text), "%u", x_l);
    oled_print(value_text);
    _delay_ms(500); //Apply fitting delay.
        




    //Apply new profile
    joystick_update_normalisation(x_l,x_r,y_down,y_up,1,30);
    //Give confirmation to user
    oled_reset();
    oled_home();
    oled_print("You can now stop fiddeling with my joystick..");
    _delay_ms(5000);
    oled_reset();
    oled_home();
}
