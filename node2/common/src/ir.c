// ir.c

#include "sam.h"
#include "ir.h"

#define IR_ADC_CHANNEL 7 // input A0 in arduino board

/**
 * Initialize ir reading with adc
 */
void ir_init(void) {
    // 1. Enable peripheral clock for ADC (Peripheral ID 37) in PMC
    PMC->PMC_PCER1 |= (1u << (ID_ADC - 32));

    // 2. Reset the ADC controller
    ADC->ADC_CR = ADC_CR_SWRST;

    // 3. Configure mode register (prescaler, startup, tracking times)
    ADC->ADC_MR = ADC_MR_PRESCAL(4) | ADC_MR_STARTUP_SUT64 | ADC_MR_TRACKTIM(2);

    // 4. Enable desired ADC channel (e.g. channel 0 for AD0)
    ADC->ADC_CHER = (1u << IR_ADC_CHANNEL);
}


/**
 * Read current ir level
 */
uint32_t ir_read(void) {
    // Start conversion via software trigger
    ADC->ADC_CR = ADC_CR_START;

    // Wait until conversion completes for the specific channel
    while (!(ADC->ADC_ISR & (1u << IR_ADC_CHANNEL)));

    // Return the 12-bit conversion value
    return ADC->ADC_CDR[IR_ADC_CHANNEL];
}

/*
To access and use the **ATSAM3X8E 12-bit Analog-to-Digital Converter (ADC)**, you must configure its power rails, enable its peripheral clock in the Power Management Controller (PMC), set up the ADC Mode Register, and read converted values from the channel data registers.

---

### Hardware Requirements & Signal Mapping
* **Voltage Range & Reference**: Analog input signals must stay strictly within the **0 V to 3.3 V** range, defined by the analog reference pin **`ADVREF`**.
* **Power Supply**: The analog supply pin **`VDDANA`** must be supplied with a voltage greater than **2.0 V** for the ADC module to be operational.
* **Channel Assignments**: The ATSAM3X8E provides analog channels **`AD0` through `AD14`**. Note that **channel 15 is unavailable** for external use because it is connected internally to the ATSAM3X8E temperature sensor.

---

### Step-by-Step Initialization & Access Sequence

#### 1. Enable Peripheral Clock in PMC
Before reading or writing to any ADC peripheral registers, you must enable the ADC clock in the **Power Management Controller (PMC)**. The ADC peripheral ID on the ATSAM3X8E is **ID 37**. Because ID 37 is greater than 31, it is configured via register `PMC_PCER1`.

#### 2. Reset and Configure the ADC Mode Register (`ADC_MR`)
* Reset the ADC controller by writing `SWRST` in the Control Register (`ADC_CR`).
* Configure **`ADC_MR`** to establish clock timing:
  * **`PRESCAL`**: Sets the clock prescaler relative to the Master Clock (`MCK`).
  * **`STARTUP`**: Defines the startup delay time for the ADC circuitry.
  * **`TRACKTIM`**: Sets the tracking time for input sampling.
  * Leave trigger bits cleared to use software-triggered conversion mode.

#### 3. Enable Target Analog Channel (`ADC_CHER`)
Enable individual input channels by writing a logic `1` to the corresponding bit in the **Channel Enable Register (`ADC_CHER`)**. For example, writing `(1u << 0)` enables channel `AD0`.

#### 4. Trigger Conversion & Read Data
* **Start Conversion**: Write `ADC_CR_START` to the Control Register (`ADC_CR`) to initiate a conversion sweep.
* **Poll Status**: Wait until the End-of-Conversion bit (`EOCx`) for your channel goes **HIGH** in the Interrupt Status Register (`ADC_ISR`).
* **Read Result**: Retrieve the **12-bit digital value** (0 to 4095) from the channel data register **`ADC_CDR[x]`** or the Last Converted Data Register (`ADC_LCDR`).

*/