#include <stdio.h>
#include <avr/io.h>
#include "../include/sram.h"

/**
 * 
 */
void sram_init(void) {   
     // Activate external memory Interface (XMEM)
    MCUCR |= (1 << SRE);

    // Mask out the four colliding pins used for JTAG, so they still can be used as JTAG.
    // Remaining adress bus is now 12 bits.
    SFIOR |= (1 << XMM2);
}

