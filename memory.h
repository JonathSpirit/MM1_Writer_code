/*
Copyright 2021 Guillaume Guillet
Licensed under MIT License
*/

#ifndef _MEMORY_H_INCLUDED_
#define _MEMORY_H_INCLUDED_

enum MEMORY_MODEL
{
    MEMM_EEPROM = 0,
    MEMM_FLASH  = 1,
    
    _MEMM_SIZE  = 2
};

//Flash command tested with SST39LF010

//Erase a flash memory sector (4kB sector)
void FlashEraseSector(unsigned char sector);

//Byte-Program a flash memory (64kB max)
void FlashByteProgram(unsigned short address, unsigned char mdata);

unsigned char Read();
void Write(unsigned char mdata);

void SetOutputEnable(bit enable);

void SetAddress(unsigned long address);

#endif //_MEMORY_H_INCLUDED_