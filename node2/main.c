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

# define IR_DETECT_THRESHOLD 3500 // adc is 12 bit: range 0 - 4096


// Enumerated gamestate
enum gameState {
    NODE2_IDLE = 0,
    NODE2_RUNNING,
    NODE2_ERROR,
    NODE2_CALIBRATING,
    NODE2_RESETTING
};

enum gameState current_gamestate = NODE2_IDLE;
static uint8_t calibrated_flag = 0;

// Variables for safe data receive during CAN interrupt
static volatile int8_t latest_joystic_x = 0;
static volatile int8_t latest_joystic_y = 0;
static volatile uint8_t solenoid_fire_request = 0;
static volatile uint8_t game_stopped_flag = 0;



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

    can_init(can_settings, 1); // Interrupt: 1=enable, 0=disable

    servo_init();

    encoder_init();

    ir_init();

    motor_init();

    pid_TC0_timer_init();

}


void delay_by_counting(void) {
    for (volatile int i = 0; i < 2000000; i++);
}

// For debug purposes
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

    current_gamestate = NODE2_RESETTING;

    // Tell node1 the game is over
    CanMsg tx = {
        .id = CAN_ID_GAMESTATE,
        .length = 2,
        .byte = {0, score}
    };
    can_tx(tx);

    printf("Game Over\n\r");

    // Reset motor
    pid_center_motor();

    current_gamestate = NODE2_IDLE;
}

/**
 * Can bus handler
 */
void can_handler(CanMsg *rx) {

    // Check received can id 
    switch (rx->id) {
    case CAN_ID_GAMESTATE:
        // If busy, do nothing
        if (current_gamestate == NODE2_CALIBRATING ||
            current_gamestate == NODE2_RESETTING) {
                break;
            }
        
        // Start game if byte[0] == 1
        else if (rx->byte[0] == 1) {
            current_gamestate = NODE2_RUNNING;
        }
        // Stop game if byte[0] is 0 
        else {
            game_stopped_flag = 1;
        }
        break;

    case CAN_ID_JOYSTICK:
        // For debug
        can_printmsg_int(*rx);

        // Save latest joystick values
        latest_joystic_x = (int8_t)rx->byte[0];
        latest_joystic_y = (int8_t)rx->byte[1];

        break;

    case CAN_ID_IO_BUTTONS:

        // Check Solenoid fire command received 
        if (rx->byte[0] & (1 << 4)) {
            solenoid_fire_request = 1;
        }
        break;

    case CAN_ID_ERROR:
        // To be properly implemented
        current_gamestate = NODE2_RESETTING;
        break;

    case CAN_ID_SCORE:
        break;

    case CAN_ID_SETTINGS:
        if (rx->byte[0] == 1) {
            current_gamestate = NODE2_CALIBRATING;
        }
        break;

    default:
        break;
    }
}

/**
 * Game running function
 */
void update_game(void) {

    switch (current_gamestate){
    case NODE2_IDLE:
        // Wait for start command
        break;

    case NODE2_RUNNING:

        if (!calibrated_flag) {
            printf("Run Motor Calibration please!\n\r");
            current_gamestate = NODE2_IDLE;
            break;
        }

        if (game_stopped_flag) {
            game_stopped_flag = 0;
            printf("Game stopped\n\r");
            game_over(0);
            break;
        }

        pid_set_enable(1);

        // Update racked servo
        servo_set_pos(latest_joystic_y);

        // Update motor target refrence
        pid_set_pos_refrence(latest_joystic_x);

        if (solenoid_fire_request) {
            solenoid_fire_request = 0; // Reset flag
            //solenoid_pulse();
        }

        // TODO goal counter
        // Check IR sensor for goal
        uint16_t ir_level = ir_read();
        if (ir_level < IR_DETECT_THRESHOLD) { 
            printf("----Game over----\n\n\r");

            game_over(0);
            break;
        }

        break;

    case NODE2_ERROR:
        
        break;

    case NODE2_CALIBRATING:

        motor_calibrate();
                //debug
        calibrated_flag = 1;
        current_gamestate = NODE2_IDLE;

        CanMsg tx = {
            .id = CAN_ID_SETTINGS,
            .length = 0,
            .byte = {}
        };
        can_tx(tx);

        break;

    case NODE2_RESETTING:
        printf("Reseting Motor to Center...\n\r");
        pid_center_motor();

        printf("Motor Centering Complete\n\r");
        current_gamestate = NODE2_IDLE;
        
        break;
    
    default:
        current_gamestate = NODE2_IDLE;
        break;
    }


}

/**
 * This function is called by timer interrupt
 * Setup in init function
 * Set to 100 calls a second (Every 10 ms) 
 * */ 
void TC0_Handler(void) {
    // Read and reset status/flag
    uint32_t status = TC0->TC_CHANNEL[0].TC_SR;

    if (status & TC_SR_CPCS) {
        pid_update(); // Run PID regulator at periodic intervals
    }
}

/**
 * Interrupt Service Routine
 * Called on recieved CAN message
 */
void CAN0_Handler(void) {
    CanMsg rx;

    if (can_rx(&rx)) {
        can_handler(&rx);
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

        update_game(); // Main game function

    }

    return 0;
}

/**
 * REMOVE
 * 
 * 
 * 
 * 
 * 
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


    CanMsg rx;
    if (can_rx(&rx)) {
        can_handler(&rx);
    }  
 */