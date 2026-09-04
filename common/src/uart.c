// uart.c

void uart_init(unsigned long baud) {

}

void uart_putc(char c) {

}

char uart_getc(void) {

    return;
}

void uart_puts(const char *str) {
    int len_str = len(str);
    for (int i = 0; i <= len_str; i++) {
        uart_putc(str(i));
    }
}
