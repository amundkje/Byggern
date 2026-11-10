#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sam.h>
#include <helper.h>

#include "uart.h" // Not the same uart.c as node1, this was given on canvas
#include "can.h"  // Not the same can.c as node1, this was given on canvas
#include "servo.h"
#include "encoder.h"
#include "ir.h"
#include "motor.h"
#include "pid.h"
#include "solenoid.h"


#define F_CPU       84000000
#define UART_BAUD   9600

// Can IDs
#define CAN_ID_GAMESTATE    1
#define CAN_ID_HEALTH       2
#define CAN_ID_JOYSTICK     3
#define CAN_ID_SETTINGS     15
#define CAN_ID_IO_BUTTONS   20
#define CAN_ID_ERROR        99 //Future :)

#define SOLENOID_BUTTON (1 << 4)

static volatile uint32_t IR_DETECT_THRESHOLD = 4096; // adc is 12 bit: range 0 - 4096


// Enumerated gamestate
enum gameState {
    NODE2_IDLE = 0,
    NODE2_RUNNING,
    NODE2_ERROR,
    NODE2_CALIBRATING,
    NODE2_RESETTING
};

enum gameState current_gamestate = NODE2_IDLE;
static volatile  uint8_t calibrated_flag = 0;



// Variables for safe data receive during CAN interrupt
static volatile int8_t latest_joystic_x = 0;
static volatile int8_t latest_joystic_y = 0;
static volatile uint8_t solenoid_fire_request = 0;
static volatile uint8_t game_stopped_flag = 0;


static volatile uint32_t IR_rest_val = 2000; 
static volatile uint8_t IR_below_threshold_count = 0; 
static volatile uint8_t hp_loss_max = 3;
static volatile uint8_t hp_loss_count = 0;
static volatile uint8_t ball_reset_count = 0;
static volatile uint8_t IR_THRESHOLD_MULTIPLIER = 6;

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
    delay_by_counting(500000);
    IR_rest_val = ir_read();
    IR_DETECT_THRESHOLD = IR_rest_val * 0.6;
    printf("IR_REST_VAL:   %d", IR_rest_val);

    motor_init();

    solenoid_init();

    pid_TC0_timer_init();



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
 * TODO: IMPLEMENT SCORE
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

    current_gamestate = NODE2_RESETTING;
}

/**
 * Can bus handler -CANHANDLER
 */
void can_handler(CanMsg *rx) {

    // Check received can id 
    switch (rx->id) {
    case CAN_ID_GAMESTATE:
        // for debug
        //can_printmsg(*rx);

        // If busy, reject: do nothing
        if (current_gamestate == NODE2_CALIBRATING ||
            current_gamestate == NODE2_RESETTING) {
                break;
            }
        
        // Start game if byte[0] == 1
        else if (rx->byte[0] == 1) {
            current_gamestate = NODE2_RUNNING;
            printf("Game RUNNING");
            pid_set_enable(1);
        }
        // Stop game if byte[0] is 0 
        else {
            current_gamestate = NODE2_RESETTING;
            printf("Stop Signal: Received\n\r");
        }
        break;

    case CAN_ID_JOYSTICK:
        // For debug
        // can_printmsg_int(*rx);

        // Save latest joystick values
        latest_joystic_x = (int8_t)rx->byte[0];
        latest_joystic_y = (int8_t)rx->byte[1];

        break;

    case CAN_ID_IO_BUTTONS:
        printf("Fire signal received\n\r");
        solenoid_fire_request = 1;

        /*
        // Check Solenoid fire command received 
        if (rx->byte[0] & SOLENOID_BUTTON) { 
            printf("Fire signal received\n\r");
            solenoid_fire_request = 1;
        }
        */
        break;
        
    case CAN_ID_ERROR:
        // To be properly implemented
        current_gamestate = NODE2_RESETTING;
        break;

    case CAN_ID_HEALTH:
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

        // Update racked servo
        servo_set_pos(latest_joystic_y);

        // Update motor target refrence
        pid_set_pos_refrence(latest_joystic_x);

        if (solenoid_fire_request) {
            solenoid_fire_request = 0; // Reset flag
            solenoid_trigger();
        }




        //TODO: extract!!
        if (IR_below_threshold_count >= 5) { //Confirmed Trigger: 
            hp_loss_count++;
            uint8_t current_hp = hp_loss_max-hp_loss_count;
            printf("HP:   %d \n\r",current_hp);

             CanMsg tx = { //TODO: extract into a wrapper method?
            .id = CAN_ID_HEALTH ,
            .length = 1,
            .byte = {current_hp}
            }; can_tx(tx);
        

            printf("You failed! -1 HP\n\r");



            IR_below_threshold_count = 0;



            if (hp_loss_count >= hp_loss_max) {
                hp_loss_count = 0; // Reset HP count
                game_over(0);
            } 
            printf("Remove ball \r\n");
               
            while(1){  //DO NOTHING :)
                uint32_t read = ir_read();
                // printf("IR lazy: %d \n\r",read);
                if (read >= IR_DETECT_THRESHOLD) {
                    ball_reset_count++;
                } else ball_reset_count = 0;

                if (ball_reset_count >= 5) {
                    break;
                }
            }
        }

        uint32_t read = ir_read();
        // printf("IR running: %d \n\r",read);
        // printf("IR_VALUE: %d \n\r",read);
        
        if (read <= IR_DETECT_THRESHOLD) {  //TODO: Extract
            IR_below_threshold_count++;
            // printf("IR Threshold:  %d\n\r", IR_below_threshold_count);
        } else IR_below_threshold_count = 0;



        
        
        

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

        printf("Sent calibration OK\n\r");

        break;

    case NODE2_RESETTING:
        /* Need integrator part first
        printf("Reseting Motor to Center...\n\r");
        pid_center_motor();
        printf("Motor Centering Complete\n\r");
        */
       pid_set_enable(0);
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
 * @note Set to 100 calls a second (Every 10 ms) 
 * */ 
void TC0_Handler(void) {
    // Read and reset status/flag
    uint32_t status = TC0->TC_CHANNEL[0].TC_SR;

    if (status & TC_SR_CPCS) {
        pid_update(); // Run PID regulator at periodic intervals
        solenoid_update();
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
        delay_by_counting(300000);

    }

    return 0;
}
