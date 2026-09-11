// uart.h
#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_transmit(unsigned char data);
void uart_transmit_string(const char *str);

unsigned char uart_receive_string(char *buffer, unsigned char max_length);
void uart_receive_string_blocking(char *buffer, unsigned char max_length);
int uart_receive_nonblocking(unsigned char *buffer);
char uart_receive(void); //Old function, blocking receive.

#endif