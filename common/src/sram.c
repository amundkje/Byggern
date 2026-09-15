#include <stdio.h>
#include <avr/io.h>
#include "../include/sram.h"




void latch_switch(uint8_t  on_or_off){
    //turn on C as output:
    if (on_or_off) 
        PORTE |= (1 << PE1); 
    else
        PORTE &= ~(1 << PE1);
};

/**
 * 
 */
void sram_init(void) {




    //SE PÅ HVA MCUCR GJØR!!!!


    /*
    DDRA = (0xFF); //Activate whole registry.
    DDRC = (1<<PC0) && (1<<PC1) && (1<<PC2) && (1<<PC3); //Activate remaning (output) address ports
    PORTA = (0x00); //Set all ports in A to zero.
    PORTC = (0<<PC0) && (0<<PC1) && (0<<PC2) && (0<<PC3); //Set PORTC 0->3 to zero.

    
    */

 
     // Activate external memory Interface (XMEM)
    MCUCR |= (1 << SRE);

    // Mask out the four colliding pins used for JTAG, so they still can be used as JTAG.
    // Remaining adress bus is now 12 bits.
    SFIOR |= (1 << XMM2);
    
}