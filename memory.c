/*
Copyright 2021 Guillaume Guillet
Licensed under MIT License
*/

#include "memory.h"
#include "main.h"

#define FLASH_ERASE_SECTOR_SEQUENCE 6
#define FLASH_BYTE_PROGRAM_SEQUENCE 4

void FlashEraseSector(unsigned char sector)
{
    unsigned char i;
    xdata unsigned long sequenceAddress[FLASH_ERASE_SECTOR_SEQUENCE]=
    {
        0x5555, 0x2AAA, 0x5555, 0x5555, 0x2AAA, 0
    };
    unsigned char sequenceData[FLASH_ERASE_SECTOR_SEQUENCE]=
    {
        0xAA, 0x55, 0x80, 0xAA, 0x55, 0x30
    };
    
    sequenceAddress[FLASH_ERASE_SECTOR_SEQUENCE-1] = ((unsigned long)sector)<<12;
    
    MEM_WE = 1;
    
    for (i=0; i<FLASH_ERASE_SECTOR_SEQUENCE; ++i)
    {
        SetAddress( sequenceAddress[i] );
        MEMDATA = sequenceData[i];
    
        
        MEM_WE = 0;
        MEM_WE = 1;
    }
    
    Sleep(25);
}

void FlashByteProgram(unsigned short address, unsigned char mdata)
{
    unsigned char i;
    xdata unsigned long sequenceAddress[FLASH_BYTE_PROGRAM_SEQUENCE]=
    {
        0x5555, 0x2AAA, 0x5555, 0
    };
    unsigned char sequenceData[FLASH_BYTE_PROGRAM_SEQUENCE]=
    {
        0xAA, 0x55, 0xA0, 0
    };
    
    sequenceAddress[FLASH_BYTE_PROGRAM_SEQUENCE-1] = address;
    sequenceData[FLASH_BYTE_PROGRAM_SEQUENCE-1] = mdata;
    
    MEM_WE = 1;
    
    for (i=0; i<FLASH_BYTE_PROGRAM_SEQUENCE; ++i)
    {
        SetAddress( sequenceAddress[i] );
        MEMDATA = sequenceData[i];
    
        MEM_WE = 0;
        MEM_WE = 1;
    }
    
    Sleep(1); //Should be 20us not 1ms TODO
}

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
