/*
Copyright 2021 Guillaume Guillet
*/

#include "memory.h"
#include "main.h"

unsigned char Read()
{
    return MEMDATA;
}
void Write(unsigned char mdata)
{
    MEMDATA = mdata;
    
    MEM_WE = 0;
    MEM_WE = 1;
}

void SetOutputEnable(bit enable)
{
    if (enable)
    {//Set to push-pull
        MEM_OE = 1;
        MEMDATA_PORT = 0xFF;
        MEMDATA = 0x00;
    }
    else
    {//Set to open-drain
        MEM_OE = 0;
        MEMDATA_PORT = 0x00;
        MEMDATA = 0xFF;
    }
}

void SetAddress(unsigned long address)
{
    unsigned char i;

    for (i=0; i<24; ++i)
    {
        SERIAL_IN = address&(0x00800000>>i);
        SRCLK = 1;
        SRCLK = 0;
    }

    RCLK = 1;
    RCLK = 0;
}
