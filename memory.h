/*
Copyright 2021 Guillaume Guillet
*/

#ifndef _MEMORY_H_INCLUDED_
#define _MEMORY_H_INCLUDED_

unsigned char Read();
void Write(unsigned char mdata);

void SetOutputEnable(bit enable);

void SetAddress(unsigned long address);

#endif //_MEMORY_H_INCLUDED_