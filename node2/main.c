#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sam.h>

#include "uart.h" // Not the same uart.c as node1, this was given on canvas
#include "can.h"  // Not the same can.c as node1, this was given on canvas
#include "servo.h"
#include "encoder.h"
#include "ir.h"
#include "motor.h"
#include "pid.h"


#define F_CPU       84000000
#define UART_BAUD   9600

// Can IDs
#define CAN_ID_GAMESTATE    1
#define CAN_ID_JOYSTICK     2
#define CAN_ID_IO_BUTTONS   3
#define CAN_ID_SCORE        4
#define CAN_ID_SETTINGS     5
#define CAN_ID_ERROR        99

# define IR_DETECT_THRESHOLD 220 // uint8. When adc detect level bellow


// Enumerated gamestate
enum gameState {
    NODE2_IDLE = 0,
    NODE2_RUNNING,
    NODE2_ERROR,
    NODE2_CALIBRATING,
    NODE2_RESETTING
};

enum gameState current_gamestate = NODE2_IDLE;



void init(void) {
    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    uart_init(F_CPU, UART_BAUD); // (cpu_freq, baud)

    /**
     * CAN INIT
     * 
     * Set bit timing to match setup for node1 mcp2515
     * Baudrate 250 KHz
     * Amount of TQ = 16
     * TQ = 250 ns
     * 
     * // Prescaler clock:
     * BRP_sam = baud * TQ - 1 = 20
     * 
     * TQ1 = Sync (As described by the standard)
     * TQ2-3 = PropSeg (To acount for delay on cable)
     * TQ4-10 = PS1
     * Sample is taken here at 62.5% of the frame
     * TQ11-16 = PS2
    */
    CanInit can_settings = {
        .phase1 = 6, 
        .phase2 = 5,
        .propag = 1,
        .sjw = 0,
        .brp = 20,
        .smp = 0
    };

    can_init(can_settings, 0); // Interrupt: 1=enable, 0=disable

    servo_init();

    encoder_init();

    ir_init();

    motor_init();

    // Set timer interrupt every 10 ms.
    // Parameter is amount of ticks between interrupts.
    SysTick_Config(SystemCoreClock / 100); 

}


void delay_by_counting(void) {
    for (volatile int i = 0; i < 2000000; i++);
}
void can_printmsg_int(CanMsg m){
    printf("CanMsg(id:%d, length:%d, data:{", m.id, m.length);
    if(m.length){
        printf("%d", (int8_t)m.byte[0]);
    }
    for(uint8_t i = 1; i < m.length; i++){
        printf(", %d", (int8_t)m.byte[i]);
    }
    printf("})\n\r");
}


/**
 * Handle Game over
 * TODO handle score
 */
void game_over(uint8_t score) {

    // Tell node1 the game is over
    CanMsg tx = {
        .id = CAN_ID_GAMESTATE,
        .length = 2,
        .byte = {0, score}
    };
    can_tx(tx);

    // Reset motor
    pid_center_motor();
}

/**
 * Can bus handler
 */
void can_handler(CanMsg *rx) {

    // Check received can id 
    switch (rx->id) {
        case CAN_ID_GAMESTATE:
            // If busy, send node1 back to menu
            if (current_gamestate == NODE2_CALIBRATING ||
                current_gamestate == NODE2_RESETTING) {
                    game_over(0);
                }
            
            // Start game if byte[0] == 1
            else if (rx->byte[0] == 1) {
                current_gamestate = NODE2_RUNNING;
                pid_set_enable(1);
                printf("----Game Running----\n\n\r");
            }
            else {
                current_gamestate = NODE2_RESETTING;
                printf("Game stopped. Resetting motor... \n\r");
                pid_center_motor();
                current_gamestate = NODE2_IDLE;
                printf("----Game idle----\n\n\r");
            }
            break;

        case CAN_ID_JOYSTICK:
            // For debug
            can_printmsg_int(*rx);

            int8_t speed;
            if (current_gamestate == NODE2_RUNNING) {
                // Set servo (racket rotation)
                servo_set_pos((int8_t)rx->byte[1]);

                // Set motor

                
            }
            break;

        case CAN_ID_IO_BUTTONS:
            if (current_gamestate == NODE2_RUNNING) {
                uint8_t left_btns = rx->byte[0];
                uint8_t right_btns = rx->byte[1];
                uint8_t nav = rx->byte[2];

                // Check button SR5 (Solenoid pulse)
                if (left_btns & (1 << 4)) {
                    // SOlenoid fire at will
                }               
            }
            break;

        case CAN_ID_ERROR:
            break;

        case CAN_ID_SCORE:
            break;

        case CAN_ID_SETTINGS:
            current_gamestate = NODE2_CALIBRATING;
            motor_calibrate();
            current_gamestate = NODE2_RESETTING;
            pid_center_motor();
            current_gamestate = NODE2_IDLE;
            break;

        default:
            break;
        }
}

/**
 * Game running function
 */
void update_game(void) {

    if (current_gamestate != NODE2_RUNNING) {
        return;
    }

    // Check for game over when ball crosses ir line
    uint8_t ir_level = ir_read();
    if (ir_level < IR_DETECT_THRESHOLD) { 
        printf("----Game over----\n\n\r");

        game_over(0);

        current_gamestate = NODE2_IDLE;
    }

}

/**
 * This function is called by timer interrupt
 * Setup in init function
 * Set to 100 calls a second (Every 10 ms) 
 * */ 
void SysTick_Handler(void) {
    pid_update();
}


int main()
{
    // Please dont place code before init
    init();

    // Testprint
    printf("\n\r");
    printf("GENERAL KENOBI\n\r");
    printf("*COUGH*\n\n\r");

    // Testfunctions here:




    // Main loop
    while (1)
    {
        
        CanMsg rx;

        if (can_rx(&rx)) {
            can_handler(&rx);
        }  


        update_game(); // Main game function
    }

    return 0;
}

/**
 * REMOVE
 * 
 *     while (1) {
        int16_t pos = encoder_read();
        printf("Encoder pos: %d\n\r", pos);

        delay_by_counting();
    }


    adc_init(7);
    while (1) {

    
    uint8_t read = adc_read(7);
    printf("ADC value: %d\n\r", read);
    
    delay_by_counting();
    }
 */