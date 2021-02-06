/*
Copyright 2021 Guillaume Guillet
*/

#ifndef _UART_H_INCLUDED_
#define _UART_H_INCLUDED_

#define UART_RECEIVE_SIZE 400
#define UART_TRANSMIT_SIZE 400

#define UART_END_OF_RECEPTION '#'

extern xdata unsigned char _uart_receiveData[UART_RECEIVE_SIZE];
extern unsigned short _uart_receiveSize;
extern bit _uart_receiveFlag;

extern xdata unsigned char _uart_transmitData[UART_TRANSMIT_SIZE];
extern unsigned short _uart_transmitSize;
extern bit _uart_transmitFlag;

#endif //_UART_H_INCLUDED_