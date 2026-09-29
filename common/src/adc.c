#include <stdio.h>
#include <avr/io.h>
#include "../include/adc.h"


typedef struct {
    uint8_t x;
    uint8_t y;
}joystick_xy;

typedef struct {
    uint8_t x;
    uint8_t y;
}touch_xy;
/**
 * Can be both translated, and direct analog values. 
 * 
 * TODO: Move to own file?
 */
typedef struct {
    joystick_xy joystick;
    touch_xy touch;
} pos_2D;


// Initialize the clock signal to the adc.
// Clock frequency generated is half of the MCU clock.
// F_clk_adc = 2.46 MHz
// This is achieved by using no prescaler and counting to 0, making the signal toggle on every clock cycle.
void adc_clk_init(void) {

    // Activate PD4 (OC3A) as output.
    DDRD |= (1 << PD4);

    // Configure Timer 3 as CTC 
    // TCCR3A (Timer/Counter3 Control Register A)
    TCCR3A = (1 << COM3A0);

    // WGM32 = 1 (for CTC-modus)
    // CS32:0 = 001 (Prescaler N = 1, No prescaling)
    TCCR3B = (1 << WGM32) | (1 << CS30);

    // Sett top value for counter.
    // By setting it to 0, the counter trigger a reset every time its incremented.
    OCR3A = 0;

};


// Global variables
volatile uint8_t* adc_joystick_x_pos = (volatile char*) 0x1000; // ADC AIN0
volatile uint8_t* adc_joystick_y_pos = (volatile char*) 0x1001; // ADC AIN1
volatile uint8_t* adc_touch_x_pos = (volatile char*) 0x1002; // ADC AIN2
volatile uint8_t* adc_touch_y_pos = (volatile char*) 0x1003; // ADC AIN3



/**
 * Overwrites 0x1000 addr of adc, to initiate new read.
 */
void adc_reset_read(void){
    adc_joystick_x_pos = 0x00; //INITIATE NEW READ.
}


 
/**
 * Reads current ADC values into a positional matrix
 * @return filled positional matrix.
 */
pos_2D adc_read(void) {   
    pos_2D input_state_t;

    adc_reset_read(); //Read new values.
    _delay_us(10); //µs is needed for ADC to process.

    input_state_t.joystick.x = adc_joystick_x_pos;
    input_state_t.joystick.y = adc_joystick_y_pos;
    input_state_t.touch.x = adc_touch_x_pos;
    input_state_t.touch.y = adc_touch_y_pos;


    
    return input_state_t;
};


// Test adc
void adc_test(void) {
    printf("ADC test begun\n\r");

    while (1) {

        pos_2D pos;
        pos = adc_read();
        
        
        printf("Joystick: \n\r");
        printf("X: %d \n\r",pos.joystick.x);
        printf("Y: %d \n\r",pos.joystick.y);
        printf("-------------------------------");
        _delay_ms(500);
        
    }
    return;
}

//.....                           
// ------ DRAFT: -------



//Outer max
typedef struct {
    uint8_t  u_y;// Upper 
    uint8_t  l_y;// Lower
    uint8_t  r_x;// RIGHT
    uint8_t  l_x;// LEFT
} cal_max;

 //DEADZONE
typedef struct {
    uint8_t  d_x; //Deadzone x (used for both upper and lower)
    uint8_t  d_y; //Deadzone x (used for both upper and lower)
} cal_dzone;

//Calibration profile
typedef struct {
   cal_max cal_max; //Max outer limits.
   cal_dzone cal_dzone; //calculated deadzone.
} cal_profile; 







/**
 * DRAFT:
 */
void run_calibration() {
    cal_profile new_cal;
    
    

    printf("Starting calibration! \n\r");
    printf("Slowly move the joystick in a circle 3 times, in the outermost position... \n\r");


    
    new_cal.cal_max = calibrate_joystick_max();


    printf("Step 1: COMPLETE \n\r");
    printf("Please do not touch the controller \n\r");

    
    new_cal.cal_dzone = calibrate_joystick_deadzone();

    printf("Deadzone calculated \n\r");
    printf("Step 2: COMPLETE \n\r");
    printf("Complete, Thanks \n\r");
}



/**
 * DRAFT:
 * todo:Calibrate so that the joystick range is -100 to 100 on both x and y. Based on new max dzone.
 */
 cal_max calibrate_joystick_max() {
    cal_max new_max;
    pos_2D pos_raw;

    // Read new values.
    int calibration_done = 0;
    while (!calibration_done)
    {
       
    }
    
    pos_raw = adc_read(); //Reads joystick and touch, can be modularised later to only read what is needed.



    // Map raw values into
    
    return new_max;
 }

/**
 * DRAFT:
 */
cal_dzone calibrate_joystick_deadzone() {
    cal_dzone deadzone_pos;
    
    return deadzone_pos;
}


pos_2D adc_normalise_outer(pos_2D) {
    cal_profile profile; // Either static variable or param inject.
    pos_2D norm_pos;

    //Funny code :)
    
    return norm_pos;
}

// ---