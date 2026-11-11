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


// Global Variables
static volatile uint8_t can_message_pending_flag = 0;
static volatile enum gameState current_gamestate = NODE1_MENU; 
static volatile uint8_t last_game_score = 0;
static volatile uint8_t hp_remaining = 0;
static volatile uint8_t hp_max = 3;


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
 * Gamelogic function.
 * Continually called from main loop with a small delay.
 */
void run_game(void) {

    enum joystick_direction dir = joystick_get_direction();
    Buttons io_buttons = io_button_read();

    switch (current_gamestate) {
    case NODE1_MENU:
        enum menuItems menu_choice = menu_update();

        // If start game chosen: Change state to running and initialize game
        if (menu_choice == MENU_START_GAME) {
            printf_P(PSTR("Starting game\n\r"));

            uint8_t data[] = {1};
            can_send(CAN_ID_GAMESTATE, 1, data); // Send start came command to node 2
            
            current_gamestate = NODE1_RUNNING;
            hp_remaining = hp_max;

            char hp_rem_char[2];
            snprintf(hp_rem_char, sizeof(hp_rem_char), "%u", hp_remaining);

            oled_reset();
            oled_print("Good luck");
            oled_goto_line(1);
            oled_goto_column(0);
            oled_print("Lives Remaining: ");
            oled_print(hp_rem_char);

        }

        // Send request to node 2 to autocalibrate the motor.
        else if (menu_choice == MENU_CALIBRATE_MOTOR) {

            uint8_t data[] = {1};
            can_send(CAN_ID_SETTINGS, 1, data); // Send autocalibrate request to node 2

            current_gamestate = NODE1_CALIBRATING;

            printf_P(PSTR("Sending motor calibrating request\n\r"));
            oled_reset();
            oled_print("Waiting for Motor Calibration...");
        }

        // Run joystick calibration sequence
        else if (menu_choice == MENU_CALIBRATE_JOYSTICK) {
            run_autocalibration();
            menu_init();
        }

        // Show credits
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

    // The RUNNING state checks current gamestate and updates node 2
    case NODE1_RUNNING:

        // If SL7 pressed, stop game and return to menu
        if (io_buttons.L7) {
            current_gamestate = NODE1_MENU;

            uint8_t data[] = {0};
            can_send(CAN_ID_GAMESTATE, 1, data); // Send stop game command to node 2

            printf_P(PSTR("Game stopped\n\r"));

            menu_init();
        }

        // If SR5 pressed, send fire solenoid request to node 2
        else if (io_buttons.R5) {
            
            uint8_t data[] = {io_buttons.right};
            can_send(CAN_ID_IO_BUTTONS, 1, data);
            printf_P(PSTR("Fire Solenoid\n\r"));
            
        }

        // Send current joystick postition to node 2
        else {
            update_arm();
        }
    
        break;

    // Wait state while motor is auto-calibrating
    case NODE1_CALIBRATING:

        // Return to menu on joystick left or menu button
        if (dir == LEFT) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        else if (io_buttons.L7) {
            current_gamestate = NODE1_MENU;
            menu_init();
        }
        break;

    // Wait state to display score after game is over
    case NODE1_GAME_OVER:
  
        // Return to menu on joystick left or menu button
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

    // On unknown state, return to menu
    default:
        current_gamestate = NODE1_MENU;
        menu_init();
        break;
    }
}


/**
 * Function to handle incoming can packets.
 * 
 */
void can_handler(canMsg *rx) {
    switch (rx->id) {

    // Node 2 has experienced a game over.
    // Change state to game over and display score
    case CAN_ID_GAMESTATE:
        if (rx->bytes[0] == 0) {
            last_game_score = rx->bytes[1];
            current_gamestate = NODE1_GAME_OVER;
            
            char last_score[4]; // Up to "255" plus '\0'
            snprintf(last_score, sizeof(last_score), "%u", (unsigned)last_game_score);
            oled_reset();
            oled_print("Game Over");
            oled_goto_line(1);
            oled_goto_column(0);
            oled_print("Score: ");
            oled_print(last_score);

            printf_P(PSTR("----Game Over----\n\n\r"));
        }
        break;
    
    // Node 2 inform that the motor auto calibration is complete, return to menu.
    case CAN_ID_SETTINGS:
        printf_P(PSTR("Motor Calibration Complete\n\r"));
        current_gamestate = NODE1_MENU;
        menu_init();
        break; 

    // Node 2 inform that a Hit Point has been lost, update remaining lives.
    case CAN_ID_HEALTH:
        hp_remaining = rx->bytes[0]; // get hp count from node 2

        oled_reset();
        oled_print("Lives Remaining: "); 

        char hp_text[4];
        snprintf(hp_text, sizeof(hp_text), "%u", hp_remaining);
        oled_print(hp_text);
        break;
    
    // On unknown can ID, do nothing
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

    // Loop:
    while(1) {

        // Handle can packet if recieved
        if (can_message_pending_flag) {
            canMsg rx;
            if (can_rx(&rx)) {
                can_handler(&rx);
            }
            can_message_pending_flag = 0;
        }

        // Run main game logic
        run_game();
        _delay_ms(50);
    };

    return 0;
}
