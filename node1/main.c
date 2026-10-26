

// Lib. Includes
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
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
#define BUFFER_SIZE 255

// Can IDs
#define CAN_ID_GAMESTATE    1
#define CAN_ID_JOYSTICK     2
#define CAN_ID_ERROR        3

// Global Variables
volatile uint8_t can_message_pending_flag = 0;
enum gameState current_gamestate = MENU; 

// Remember: Please do not break the code :-)


// Place Test functions here.
// --

// Initialize stdout
void init() {
    stdout = fdevopen(uart_putchar, uart_getchar);  //Allows printf to use UART
    _delay_ms(500);

    // Initialise other modules:
    adc_clk_init();
    _delay_ms(5);

    sram_init();
    _delay_ms(5);

    uart_init();
    _delay_ms(5);

    joystick_init();
    _delay_ms(5);

    spi_init();
    _delay_ms(5);

    can_init(MODE_NORMAL, 0); // Interrupt: 1=enable, 0=disable
    _delay_ms(5);

    oled_init();
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
 * Gamelogic
 */
void run_game(void) {
    while (current_gamestate == MENU) 
    {
        current_gamestate = menu_update();
        _delay_ms(20);
    }


    while (current_gamestate == RUNNING)
    {
        update_arm();
        _delay_ms(10);

        // Buttonpress to return to menu
        Buttons io_buttons = io_button_read();
        if (io_buttons.L7) {
            current_gamestate = MENU;
            menu_init();
        }
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
    printf("\n\r");
    printf("Hello There\n\r");
    //printf("GENERAL KENOBI!!\n\n\r");

    // Run test functions here.
    // Please delete test functions when no longer needed.
    // Test functions to be used later can have a dedicated file in sandbox.

    //while(1){ read_joystick(0); _delay_ms(500); }

    // Loop:
    while(1) {
        run_game();
    };

    return 0;
}


/*
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