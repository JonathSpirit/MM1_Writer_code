/*
Copyright 2021 Guillaume Guillet
*/

#include "uart.h"
#include "config.h"

xdata unsigned char _uart_receiveData[UART_RECEIVE_SIZE];
unsigned short _uart_receiveSize=0;
bit _uart_receiveFlag=0;

xdata unsigned char _uart_transmitData[UART_TRANSMIT_SIZE];
unsigned short _uart_transmitSize=0;
bit _uart_transmitFlag=0;

unsigned short _receiveCounter = 0;
unsigned short _transmitCounter = 0;

unsigned char _timeoutCounter = 0;

void UART_control() interrupt UART0_INTERRUPT
{
    unsigned char receiveBuff;
    
    if (RI0)
    {//Reception
        RI0=0;
        
        if (!_uart_receiveFlag)
        {
            if (_receiveCounter >= UART_RECEIVE_SIZE)
            {//Overflow
                _receiveCounter = 0;
            }
            else
            {//Receiving data
                receiveBuff = SBUF0;
                
                if (receiveBuff == UART_END_OF_RECEPTION)
                {
                    _uart_receiveData[_receiveCounter] = 0;
                    
                    _uart_receiveSize = _receiveCounter;
                    _receiveCounter = 0;
                    _uart_receiveFlag = 1;
                    
                    //Clear timeout
                    TR0 = 0;
                    _timeoutCounter = 0;
                    TH0 = TIMER0_PRELOAD_HIGH;
                    TL0 = TIMER0_PRELOAD_LOW;
                }
                else
                {
                    _uart_receiveData[_receiveCounter++] = receiveBuff;
                    
                    TR0 = 1; //Start timeout
                    _timeoutCounter = 0;
                }
            }
        }
    }
    if (TI0)
    {//Transmission
        TI0=0;
        if (_transmitCounter < _uart_transmitSize)
        {//Transmitting data
            SBUF0 = _uart_transmitData[_transmitCounter++];
        }
        else
        {//End of transmission
            _transmitCounter = 0;
            _uart_transmitFlag = 1;
        }
    }
}

void UART_timeout() interrupt TIMER0_INTERRUPT
{
    TR0 = 0;
    
    TH0 = TIMER0_PRELOAD_HIGH;
    TL0 = TIMER0_PRELOAD_LOW;
    
    if ((++_timeoutCounter) >= 100)
    {//1 seconde timeout
        _timeoutCounter = 0;
        
        //clear uart0 reception
        RI0 = 0; 
        _receiveCounter = 0;
        _uart_receiveSize = 0;
        _uart_receiveFlag = 0;
    }
    else
    {
        TR0 = 1;
    }
}