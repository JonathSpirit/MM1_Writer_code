/*
Copyright 2021 Guillaume Guillet
*/

#include "main.h"
#include "config.h"
#include "uart.h"
#include "memory.h"
#include <string.h>

void main()
{
    unsigned char i;
    unsigned char* ptr;
    
    unsigned char checksum;
    unsigned char checksumCalculated;
    
    unsigned char numOfData;
    unsigned long startAddress;
    
    unsigned char buffRead;
    
    xdata unsigned char dataArray[100];
    
    //Init
    Init_Device();
    
    MEM_CE = 0;
    MEM_OE = 0;
    MEM_WE = 1;

    SERIAL_IN = 0;
    SRCLK = 0;
    RCLK = 0;
    
    SetOutputEnable(0);
    SetAddress(0x000000);
    
    //Enable all interrupt
    EA = 1;
    
    //Main loop
    while (1)
    {
        if (_uart_receiveFlag)
        {
            if (_uart_receiveSize >= 2)
            {
                if ( !strncmp("$H", _uart_receiveData, _uart_receiveSize) )
                {//Hello command, return hello
                    /*
                    [$H][#]
                    2 size
                    
                    return
                    [HELLO][\n]
                    6 size
                    */
                    _uart_transmitData[0] = 'H';
                    _uart_transmitData[1] = 'E';
                    _uart_transmitData[2] = 'L';
                    _uart_transmitData[3] = 'L';
                    _uart_transmitData[4] = 'O';
                    _uart_transmitData[5] = '\n';
                    _uart_transmitSize = 6;
                }
                else if ( !strncmp("$W", _uart_receiveData, 2) )
                {//Write command, write data on the memory
                    /*
                    [$W][Data checksum 8bit][Start address 24bit][n data 8bit] ... [#]
                    $W  xxx                 xxxxxxxx             xxx           ... #
                    2   3                   8                    n*3
                    13+n*3 size
                    */
                    if (_uart_receiveSize >= 16)
                    {
                        if ( (_uart_receiveSize-13) % 3 == 0 )
                        {//Check if all the data is in 3char format
                            numOfData = (_uart_receiveSize-13) / 3;
                            checksum = StringToUint8(_uart_receiveData+2);
                            startAddress = StringToUint24(_uart_receiveData+5);
                            
                            if (numOfData <= 100)
                            {
                                checksumCalculated = 0;
                                ptr = _uart_receiveData+13;
                                for (i=0; i<numOfData; ++i)
                                {
                                    dataArray[i] = StringToUint8(ptr);
                                    checksumCalculated += dataArray[i];
                                    ptr += 3;
                                }
                                
                                if (checksumCalculated == checksum)
                                {
                                    //Everything is ok ! Begin the writing !
                                    SetOutputEnable(1);
                    
                                    for (i=0; i<numOfData; ++i)
                                    {
                                        SetAddress(startAddress + i);
                                        Write(dataArray[i]);
                                        Sleep(20);
                                    }
                                    
                                    /*
                                    return
                                    [WRITED][\n]
                                    7 size
                                    */
                                    strcpy(_uart_transmitData, "WRITED\n");
                                    _uart_transmitSize = 7;
                                }
                                else
                                {
                                    /*
                                    return
                                    [BAD_CHECKSUM][\n]
                                    13 size
                                    */
                                    strcpy(_uart_transmitData, "BAD_CHECKSUM\n");
                                    _uart_transmitSize = 13;
                                }
                            }
                            else
                            {
                                /*
                                return
                                [BAD_SIZE][\n]
                                9 size
                                */
                                strcpy(_uart_transmitData, "BAD_SIZE\n");
                                _uart_transmitSize = 9;
                            }
                        }
                        else
                        {
                            /*
                            return
                            [BAD_SIZE][\n]
                            9 size
                            */
                            strcpy(_uart_transmitData, "BAD_SIZE\n");
                            _uart_transmitSize = 9;
                        }
                    }
                    else
                    {
                        /*
                        return
                        [BAD_SIZE][\n]
                        9 size
                        */
                        strcpy(_uart_transmitData, "BAD_SIZE\n");
                        _uart_transmitSize = 9;
                    }
                }
                else if ( !strncmp("$R", _uart_receiveData, 2) )
                {//Read command, read data on the memory
                    /*
                    [$R][Start address 24bit][Num of data 24bit][#]
                    $R  xxxxxxxx             xxxxxxxx           #
                    2   8                    8
                    18 size
                    
                    return
                    [READED][Data checksum 8bit][n data 8bit] ... [\n]
                    READED  xxx                 xxx           ... \n
                    6       3                   n*3
                    9+n*3 size
                    */
                    if (_uart_receiveSize == 18)
                    {
                        numOfData = StringToUint24(_uart_receiveData+10);
                        startAddress = StringToUint24(_uart_receiveData+2);
                        
                        if ( (numOfData<=100) && (numOfData!=0) && (startAddress+numOfData-1 <= 0x00FFFFFF) )
                        {//Check if arguments is good
                            checksumCalculated = 0;
                            strcpy(_uart_transmitData, "READEDxxx");
                            ptr = _uart_transmitData+9;
                            
                            SetOutputEnable(0);
                            for (i=0; i<numOfData; ++i)
                            {
                                SetAddress(startAddress+i);
                                Sleep(1);
                                
                                buffRead = Read();
                                checksumCalculated += buffRead;
                                Uint8ToString(ptr, buffRead);
                                ptr+=3;
                            }
                            //Set checksum
                            Uint8ToString(_uart_transmitData+6, checksumCalculated);
                            *ptr = '\n'; //lastChar;
                            
                            _uart_transmitSize = 10+numOfData*3;
                        }
                        else
                        {
                            /*
                            return
                            [BAD_SIZE][\n]
                            9 size
                            */
                            strcpy(_uart_transmitData, "BAD_SIZE\n");
                            _uart_transmitSize = 9;
                        }
                    }
                    else
                    {
                        /*
                        return
                        [BAD_SIZE][\n]
                        9 size
                        */
                        strcpy(_uart_transmitData, "BAD_SIZE\n");
                        _uart_transmitSize = 9;
                    }
                }
                else
                {
                    /*
                    return
                    [UNKNOWN][\n]
                    8 size
                    */
                    strcpy(_uart_transmitData, "UNKNOWN\n");
                    _uart_transmitSize = 8;
                }
            }
            else
            {
                /*
                return
                [BAD_SIZE][\n]
                9 size
                */
                strcpy(_uart_transmitData, "BAD_SIZE\n");
                _uart_transmitSize = 9;
            }
            
            _uart_receiveFlag = 0;
            
            TI0 = 1;
            _uart_transmitFlag = 0;
            while (!_uart_transmitFlag);
        }
    }
}

void Sleep(unsigned int ms)
{
    EA = 0; //Desactivate interrupt
    SFRPAGE = CONFIG_PAGE;
    
    TR5 = 1; //start timer5
    while (ms)
    {
        while (!TF5H);
        --ms;
        TF5H = 0;
    }
    TR5 = 0; //stop timer 5
    
    SFRPAGE = LEGACY_PAGE;
    EA = 1; //Reactivate interrupt
}

void Uint8ToString(unsigned char* str, unsigned char uint8)
{
    //xxx
    str[0] = (uint8/100)+'0';
    str[1] = (uint8%100)/10+'0';
    str[2] = (uint8%10)+'0';
}

unsigned char StringToUint8(unsigned char* str)
{
    //xxx
    return (str[0]-'0')*100 + (str[1]-'0')*10 + (str[2]-'0');
}

unsigned long StringToUint24(unsigned char* str)
{
    //xx'xxx'xxx
    return (str[0]-'0')*10000000 + (str[1]-'0')*1000000 + (str[2]-'0')*100000 + (str[3]-'0')*10000 + (str[4]-'0')*1000 + (str[5]-'0')*100 + (str[6]-'0')*10 + (str[7]-'0');
}

