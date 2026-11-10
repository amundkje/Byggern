#include "sam.h"
#include "solenoid.h"


#define SOLENOID_PIN        PIO_PC12 // Pin D51 on Arduino board 
#define SOLENOID_PORT       PIOC
#define SOLENOID_ID         ID_PIOC

static volatile uint8_t solenoid_timer = 0;


void solenoid_init(void) {
    // Activate clk for PIOC
    PMC->PMC_PCER0 |= (1 << SOLENOID_ID);

    // Give pin control to PIO
    SOLENOID_PORT->PIO_PER = SOLENOID_PIN;

    // Set HIGH as solenoid trigger is active low
    SOLENOID_PORT->PIO_SODR = SOLENOID_PIN;

    // Activate pin
    SOLENOID_PORT->PIO_OER = SOLENOID_PIN;
}


void solenoid_trigger(void) {
    // Pull pin low to fire
    SOLENOID_PORT->PIO_CODR = SOLENOID_PIN;

    // Set pulse duration (5 = 50 ms)
    solenoid_timer = 5;
}

/**
 * @note this function is called at a set timer (10 ms)
 */
void solenoid_update(void) {
    // Deactivate solenoid after set time
    if (solenoid_timer > 0) {
        solenoid_timer--;
        if (solenoid_timer <= 0) {
            // Set solenoid pin high to deactivate
            SOLENOID_PORT->PIO_SODR = SOLENOID_PIN;
        }
    }
}
