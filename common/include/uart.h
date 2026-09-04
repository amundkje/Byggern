// uart.h
#ifndef UART_H
#define UART_H

void uart_init(void);
void uart_transmit(unsigned char data);
unsigned char uart_receive(void);

#endif