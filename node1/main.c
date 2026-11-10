

// Lib. Includes
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

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
#include "oled.h"
#include "menu.h"
#include <common/manager.h>
#include "io_buttons.h"


// User Definitions
//..

// Can IDs
#define CAN_ID_GAMESTATE    1
#define CAN_ID_JOYSTICK     2
#define CAN_ID_IO_BUTTONS   3
#define CAN_ID_HEALTH        4
#define CAN_ID_SETTINGS     5
#define CAN_ID_ERROR        99

// Global Variables
static volatile uint8_t can_message_pending_flag = 0;
static volatile enum gameState current_gamestate = NODE1_MENU; 
static volatile uint8_t last_game_score = 0;


// Remember: Please do not break the code :-)

// Initialize stdout
void init() {
    // Initialise low-level peripherals before drivers that depend on them.
    uart_init();
    stdout = fdevopen(uart_putchar, uart_getchar);  //Allows printf to use UART
    _delay_ms(5);
    
    adc_clk_init();
    _delay_ms(5);

    sram_init();
    _delay_ms(5);

    spi_init();
    _delay_ms(5);

    can_init(MODE_NORMAL, 1); // Interrupt: 1=enable, 0=disable
    _delay_ms(5);

    oled_init();
    _delay_ms(5);    

    joystick_init();
    _delay_ms(5);

    menu_init();
    _delay_ms(5);
}


/**
 * Interrupt Service Routine
 * Called when INT0 interrupt happen when PD2 is pulled low
 */
ISR(INT0_vect) {
    // Set flag indicating a CAN-packet is received
    can_message_pending_flag = 1;
}

/**
 * Wrapper to send can packets
 */
void can_send(uint32_t id, uint8_t length, const uint8_t *data)
{
    canMsg tx = {
        .id = id,
        .length = length
    };

    for (uint8_t i = 0; i < length && i < 8; i++) {
        tx.bytes[i] = data[i];
    }

    can_tx(tx);
}

/**
 * Gamelogic
 */
void run_game(void) {

    enum joystick_direction dir = joystick_get_direction();
    Buttons io_buttons = io_button_read();

    switch (current_gamestate) {
    case NODE1_MENU:
        enum menuItems menu_choice = menu_update();

        if (menu_choice == MENU_START_GAME) {

            uint8_t data[] = {1};
            can_send(CAN_ID_GAMESTATE, 1, data);
            
            current_gamestate = NODE1_RUNNING;
            oled_reset();
            oled_print("Game starting...");
            printf_P(PSTR("Starting game\n\r"));

        }

        else if (menu_choice == MENU_CALIBRATE_MOTOR) {

            uint8_t data[] = {1};
            can_send(CAN_ID_SETTINGS, 1, data);


            current_gamestate = NODE1_CALIBRATING;
            printf_P(PSTR("Sending motor calibrating request\n\r"));
            oled_reset();
            oled_print("Waiting for Motor Calibration...");
        }

        else if (menu_choice == MENU_CALIBRATE_JOYSTICK) {
            run_autocalibration();
            menu_init();
        }

        else if (menu_choice == MENU_CREDITS) {
            current_gamestate = NODE1_CREDITS;
            oled_reset();
            oled_print("Made by:");
            oled_goto_line(1);
            oled_goto_column(0);
            oled_print("   Scooby Doo");
            oled_goto_line(2);
            oled_goto_column(0);
            oled_print("   Donkey");
            oled_goto_line(3);
            oled_goto_column(0);
            oled_print("   Tuxedo Mask");
        }
        break;

    case NODE1_RUNNING:

        // If SL7 pressed, stop game.
        if (io_buttons.L7) {
            current_gamestate = NODE1_MENU;

            uint8_t data[] = {0};
            can_send(CAN_ID_GAMESTATE, 1, data);

            printf_P(PSTR("Game stopped\n\r"));

            menu_init();
        }

        else if (io_buttons.R5) {
            
            uint8_t data[] = {io_buttons.right};
            can_send(CAN_ID_IO_BUTTONS, 1, data);
            printf_P(PSTR("Fire Solenoid\n\r"));
            
        }

        // Send joystick and button info to node 1
        else {
            update_arm();
        }
    
        break;

    case NODE1_CALIBRATING:

        if (dir == LEFT) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        else if (io_buttons.L7) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        break;

    case NODE1_GAME_OVER:
  
        if (dir == LEFT) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        
        else if (io_buttons.L7) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        break;

    case NODE1_CREDITS:
        
        if (dir == LEFT) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        
        else if (io_buttons.L7) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        break;

    default:
        current_gamestate = NODE1_MENU;
        menu_init();
        break;
    }
}

void can_handler(canMsg *rx) {
    switch (rx->id) {
    case CAN_ID_GAMESTATE:
        if (rx->bytes[0] == 0) {
            last_game_score = rx->bytes[1];
            current_gamestate = NODE1_GAME_OVER;
            
            /* char last_score[4]; // Up to "255" plus '\0'
            // snprintf(last_score, sizeof(last_score), "%u", (unsigned)last_game_score);
            oled_reset();
            oled_print("Game Over");
            oled_goto_line(1);
            oled_goto_column(0);
            oled_print("Score: ");
            oled_print(last_score);

            printf_P(PSTR("----Game Over----\n\n\r"));
        }
        break;
    
    case CAN_ID_SETTINGS:
        printf_P(PSTR("Motor Calibration Complete\n\r"));
        current_gamestate = NODE1_MENU;
        menu_init();
        break; 

    default:
        
        break;
    }
}

/**
 * Main function for the Node1 application.
 *
 * Initializes the UART and transmits a test string.
 *
 * @return 0 on successful execution.
 */
int main(void) {

    // Init --Please dont put anything above :)
    init();
    _delay_ms(2000); // FIX TO ADC PROBLEMS ¯\_(ツ)_/¯

    // One time code, to see system start.:
    printf_P(PSTR("\n\r"));
    printf_P(PSTR("Hello There\n\r"));
    //printf("GENERAL KENOBI!!\n\n\r");

    // Run test functions here.
    // Please delete test functions when no longer needed.
    // Test functions to be used later can have a dedicated file in sandbox.

   
    // Loop:
    while(1) {

        if (can_message_pending_flag) {
            canMsg rx;
            if (can_rx(&rx)) {
                can_handler(&rx);
            }
            can_message_pending_flag = 0;
        }

        run_game();
        _delay_ms(50);
    };

    return 0;
}


/*
        canMsg rx;
        if (can_rx(&rx)) {
            can_handler(&rx);
        }



    uint32_t counter = 0;
    while (1) {

        counter +=1;
        printf("TESTING DIRECTIONAL TRANSLATION. %d \n\r", counter);
        
        xy_signed pos = read_joystick(0);
        printf("X: %d -- Y: %d \n\r",pos.x,pos.y);

        printf("DIRECTION ENUM:   %d \n\r", get_directional_basic());
    
        _delay_ms(500);
    }
*/
