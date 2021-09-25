/*
Copyright 2021 Guillaume Guillet
Licensed under MIT License
*/

#ifndef _CONFIG_H_INCLUDED_
#define _CONFIG_H_INCLUDED_

#include <REG51F380.H>

#define UART0_INTERRUPT 4
#define TIMER0_INTERRUPT 1

#define TIMER0_PRELOAD_HIGH 0x63 //10ms timer
#define TIMER0_PRELOAD_LOW 0xC0

#define CONFIG_PAGE 0x0F
#define LEGACY_PAGE 0x00

void Init_Device(void);

#endif //_CONFIG_H_INCLUDED_