#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sam.h>

#include "uart.h" // Not the same uart.c as node1, this was given on canvas
#include "can.h"  // Not the same can.c as node1, this was given on canvas

#include "servo.h"


#define F_CPU       84000000
#define UART_BAUD   9600

// Can IDs
#define CAN_ID_GAMESTATE    1
#define CAN_ID_JOYSTICK     2
#define CAN_ID_IO_BUTTONS   3
#define CAN_ID_ERROR        4


// Enumerated gamestate
enum gameState {
    NODE2_IDLE = 0,
    NODE2_RUNNING,
    NODE2_ERROR
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

}


void delay_by_counting(void) {
    for (int i = 0; i < 2000000; i++);
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
 * Can bus handler
 */
void can_handler(CanMsg *rx) {

    // Check received can id 
    switch (rx->id) {
        case CAN_ID_GAMESTATE:
            if (rx->byte[0] == 1) {
                current_gamestate = NODE2_RUNNING;
                printf("----Game Running----\n\n\r");
            }
            else {
                current_gamestate = NODE2_IDLE;
                printf("----Game idle----\n\n\r");
            }
            break;

        case CAN_ID_JOYSTICK:
            // For debug
            can_printmsg_int(*rx);

            if (current_gamestate == NODE2_RUNNING) {
                servo_set_pos((int8_t)rx->byte[1]);
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

    if (0) { //TODO: ir goal detected
        printf("----Game over----\n\n\r");

        CanMsg tx = {
            .id = CAN_ID_GAMESTATE,
            .length = 1,
            .byte = {0}
        };

        can_tx(tx);

        current_gamestate = NODE2_IDLE;
    }

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