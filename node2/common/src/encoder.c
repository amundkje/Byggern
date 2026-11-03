#include <stdint.h>

#include "sam.h"
#include "encoder.h"




void encoder_init(void) {
    // Encoder is read on TC2 (Timer Counter)

    // Activate power/clk for TC2 channel 0
    // TC2 has 3 channels: TC6, TC7, TC9 (confusing i know) 
    PMC->PMC_PCER1 |= (1 << (ID_TC6 - 32));

    // Set PC25 and PC26 as peripheral B
    // Theese are the phase inputs from the encoder (PHA, PHB)
    // They are used as input for TC2 in the periphery
    PIOC->PIO_PDR |= PIO_PC25 | PIO_PC26; // Deactivate normal GPIO
    PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26; // Set peripheral B

    // Configure TC2 Block Mode Register
    // QDEN = activate decoder, POSEN = activate position tracking.
    // EDGPHA = count both channels (phases)
    TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN | TC_BMR_EDGPHA;

    // Configure Channel Mode Register
    // TCCLKS = choose clk source
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;

    // Activate and trigger counter on ch0
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

}


int16_t encoder_read(void) {
    // Read counter value from the TC2_ch0 counter register
    return (int16_t)TC2->TC_CHANNEL[0].TC_CV;
}