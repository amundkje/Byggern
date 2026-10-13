#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "uart.h" // Not the same uart.c as node1, this was given on canvas
#include "can.h"  // Not the same can.c as node1, this was given on canvas


#define F_CPU       84000000
#define UART_BAUD   9600


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

}

int main()
{
    // Please dont place code before init
    init();

    // Testprint
    printf("\n\r");
    printf("GENERAL KENOBI\n\r");
    printf("*COUGH*\n\r");

    // Testfunctions here:

    CanMsg test_msg = {
        .id = 2,
        .length = 4,
        .byte = "Pong"
    };

    CanMsg rx;

    while (1) {
        if (can_rx(&rx)) {
            printf("Received %c%c%c%c\n\r", rx.byte[0], rx.byte[1], rx.byte[2], rx.byte[3]);

            can_tx(test_msg);
            printf("Sendt Pong in return\n\r");
        }
    }





    while (1)
    {


    }
    
}