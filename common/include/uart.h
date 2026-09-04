// uart.h
#ifndef UART_H
#define UART_H

void uart_init(unsigned long baud);
void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char *str);

#endif