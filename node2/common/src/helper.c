
#include "helper.h"

//TODO: you know it, I know it... replace?
void delay_by_counting(uint32_t count_to_i) {
    for (volatile uint32_t i = 0; i < count_to_i; i++);
}