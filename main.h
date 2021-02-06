/*
Copyright 2021 Guillaume Guillet
*/

#ifndef _MAIN_H_INCLUDED_
#define _MAIN_H_INCLUDED_

#include "config.h"

#define MEMDATA P1
#define MEMDATA_PORT P1MDOUT

sbit MEM_CE = P2^0;
sbit MEM_OE = P2^1;
sbit MEM_WE = P2^2;

sbit SERIAL_IN = P2^3;
sbit SRCLK = P2^4;
sbit RCLK = P2^5;

void Sleep(unsigned int ms);

unsigned char StringToUint8(unsigned char* str);
unsigned long StringToUint24(unsigned char* str);

void Uint8ToString(unsigned char* str, unsigned char uint8);

#endif //_MAIN_H_INCLUDED_