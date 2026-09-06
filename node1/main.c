#include "../common/include/uart.h"


/**
 * Main function for the Node1 application.
 *
 * Initializes the UART and transmits a test string.
 *
 * @return 0 on successful execution.
 */
int main(void) {

    uart_init();

    const char *test_string = "Hello from the other side!";
    uart_transmit_string(test_string);

    return 0;
}