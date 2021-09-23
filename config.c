/*
Copyright 2021 Guillaume Guillet
Licensed under MIT License
*/

#include "config.h"

// Peripheral specific initialization functions,
// Called from the Init_Device() function
void PCA_Init()
{
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x00;
}

void Timer_Init()
{
    TCON      = 0x40;
    TMOD      = 0x21;
    TL0       = 0xC0;
    TH0       = 0x63;
    TH1       = 0x30;
    SFRPAGE   = CONFIG_PAGE;
    CKCON1    = 0x04;
    TMR5CN    = 0x04;
    TMR5RLL   = 0x80;
    TMR5RLH   = 0x44;
    TMR5L     = 0x80;
    TMR5H     = 0x44;
    SFRPAGE   = LEGACY_PAGE;
}

void UART_Init()
{
    SCON0     = 0x10;
}

void Voltage_Reference_Init()
{
    REF0CN    = 0x08;
}

void Port_IO_Init()
{
    // P0.0  -  Unassigned,  Open-Drain, Digital
    // P0.1  -  Unassigned,  Open-Drain, Digital
    // P0.2  -  Unassigned,  Open-Drain, Digital
    // P0.3  -  Unassigned,  Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Push-Pull,  Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Open-Drain, Digital

    // P1.0  -  Skipped,     Open-Drain, Digital
    // P1.1  -  Skipped,     Open-Drain, Digital
    // P1.2  -  Skipped,     Open-Drain, Digital
    // P1.3  -  Skipped,     Open-Drain, Digital
    // P1.4  -  Skipped,     Open-Drain, Digital
    // P1.5  -  Skipped,     Open-Drain, Digital
    // P1.6  -  Skipped,     Open-Drain, Digital
    // P1.7  -  Skipped,     Open-Drain, Digital

    // P2.0  -  Skipped,     Push-Pull,  Digital
    // P2.1  -  Skipped,     Push-Pull,  Digital
    // P2.2  -  Skipped,     Push-Pull,  Digital
    // P2.3  -  Skipped,     Push-Pull,  Digital
    // P2.4  -  Skipped,     Push-Pull,  Digital
    // P2.5  -  Skipped,     Push-Pull,  Digital
    // P2.6  -  Unassigned,  Open-Drain, Digital
    // P2.7  -  Unassigned,  Open-Drain, Digital

    // P3.0  -  Unassigned,  Open-Drain, Digital

    P0MDOUT   = 0x10;
    P2MDOUT   = 0x3F;
    P1SKIP    = 0xFF;
    P2SKIP    = 0x3F;
    XBR0      = 0x01;
    XBR1      = 0x40;
}

void Oscillator_Init()
{
    FLSCL     = 0x90;
    CLKSEL    = 0x03;
}

void Interrupts_Init()
{
    IE        = 0x12;
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device(void)
{
    PCA_Init();
    Timer_Init();
    UART_Init();
    Voltage_Reference_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
