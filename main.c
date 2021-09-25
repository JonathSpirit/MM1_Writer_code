/*
Copyright 2021 Guillaume Guillet
Licensed under MIT License
Revision 2

Code:  https://github.com/JonathSpirit/MM1_Writer_code
Board: https://github.com/JonathSpirit/MM1_Writer
*/

/* CHANGELOG
Revision 2:
    - add cstring function to help
        - add TransmitCString
        - add CopyCStringToTransmitBuffer
    - add macro BEGIN_TRANSMISSION_AND_WAIT
    - add Uint24ToString
    - read command now return the start address
    - add $I command (info), that will return code information
    - add flash memory compatibility
    - add $M command to set memory model (flash, eeprom, etc...)

Revision 1:
    - Initial release
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
    unsigned char startSector;
    unsigned long startAddress;
    
    unsigned char buffRead;
    
    unsigned char memoryModel = MEMM_EEPROM;
    
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
                    
                    TransmitCString("HELLO\n");
                }
                else if ( !strncmp("$M", _uart_receiveData, 2) )
                {//Memory model command (flash, eeprom etc...), return saved memory model
                    /*
                    [$M] [1 data 8bit] [#]
                    3 size
                    
                    return
                    [1 data 8bit] [\n]
                    1 size
                    */
                    
                    if (_uart_receiveSize == 3)
                    {
                        i = _uart_receiveData[2] - '0';
                        if (i < _MEMM_SIZE)
                        {
                            memoryModel = i;
                        }
                        _uart_transmitSize = 2;
                        _uart_transmitData[0] = memoryModel+'0';
                        _uart_transmitData[1] = '\n';
                        BEGIN_TRANSMISSION_AND_WAIT
                    }
                    else
                    {
                        /*
                        return
                        [BAD_SIZE][\n]
                        9 size
                        */
                        TransmitCString("BAD_SIZE\n");
                    }
                }
                else if ( !strncmp("$FES", _uart_receiveData, 4) )
                {//Flash erase sector command, return start and count
                    /*
                    [$FES] [Start sector 8bit] [Sector count 8bit] [#]
                    $FES   xxx                 xxx                 #
                    4      3                   3
                    10 size
                    
                    return
                    [ERASED] [Start sector 8bit] [Sector count 8bit] [\n]
                    12 size
                    */
                    
                    if (_uart_receiveSize == 10)
                    {
                        numOfData = StringToUint8(_uart_receiveData+7);
                        startSector = StringToUint8(_uart_receiveData+4);
                        
                        if (numOfData > 0)
                        {
                            if ( ((unsigned short)startSector) + numOfData <= 0xFF )
                            {
                                SetOutputEnable(1);
                                
                                for (i=0; i<numOfData; ++i)
                                {
                                    FlashEraseSector(startSector + i);
                                }
                                
                                _uart_transmitSize = 0;
                                CopyCStringToTransmitBuffer("ERASED");
                                Uint8ToString(_uart_transmitData+_uart_transmitSize, startSector);
                                _uart_transmitSize += 3;
                                Uint8ToString(_uart_transmitData+_uart_transmitSize, numOfData);
                                _uart_transmitSize += 3;
                                _uart_transmitData[_uart_transmitSize++] = '\n';
                                BEGIN_TRANSMISSION_AND_WAIT
                            }
                            else
                            {
                                /*
                                return
                                [BAD_SIZE][\n]
                                9 size
                                */
                                TransmitCString("BAD_SIZE\n");
                            }
                        }
                        else
                        {
                            /*
                            return
                            [BAD_SIZE][\n]
                            9 size
                            */
                            TransmitCString("BAD_SIZE\n");
                        }
                    }
                    else
                    {
                        /*
                        return
                        [BAD_SIZE][\n]
                        9 size
                        */
                        TransmitCString("BAD_SIZE\n");
                    }
                }
                else if ( !strncmp("$I", _uart_receiveData, _uart_receiveSize) )
                {//Info command, return code information
                    /*
                    [$I][#]
                    2 size
                    
                    return (every information is separate with ':' and a '=' is used for arguement)
                    ... [\n]
                    n size
                    */
                    
                    _uart_transmitSize = 0;
                    
                    CopyCStringToTransmitBuffer("Name=MM1_Writer_code:REV=");
                    Uint8ToString(_uart_transmitData+_uart_transmitSize, REVISION);
                    _uart_transmitSize += 3;
                    
                    CopyCStringToTransmitBuffer(":CompileDate=");
                    CopyCStringToTransmitBuffer(__DATE__);
                    
                    CopyCStringToTransmitBuffer(":CompileTime=");
                    CopyCStringToTransmitBuffer(__TIME__);
                    
                    BEGIN_TRANSMISSION_AND_WAIT
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
                    
                                    switch (memoryModel)
                                    {
                                    case MEMM_FLASH:
                                        for (i=0; i<numOfData; ++i)
                                        {
                                            FlashByteProgram(startAddress + i, dataArray[i]);
                                        }
                                        break;
                                    default:
                                        for (i=0; i<numOfData; ++i)
                                        {
                                            SetAddress(startAddress + i);
                                            Write(dataArray[i]);
                                            Sleep(20);
                                        }
                                        break;
                                    }
                                    
                                    /*
                                    return
                                    [WRITED][\n]
                                    7 size
                                    */
                                    TransmitCString("WRITED\n");
                                }
                                else
                                {
                                    /*
                                    return
                                    [BAD_CHECKSUM][\n]
                                    13 size
                                    */
                                    TransmitCString("BAD_CHECKSUM\n");
                                }
                            }
                            else
                            {
                                /*
                                return
                                [BAD_SIZE][\n]
                                9 size
                                */
                                TransmitCString("BAD_SIZE\n");
                            }
                        }
                        else
                        {
                            /*
                            return
                            [BAD_SIZE][\n]
                            9 size
                            */
                            TransmitCString("BAD_SIZE\n");
                        }
                    }
                    else
                    {
                        /*
                        return
                        [BAD_SIZE][\n]
                        9 size
                        */
                        TransmitCString("BAD_SIZE\n");
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
                    [READED][Data checksum 8bit][Start address 24bit][n data 8bit] ... [\n]
                    READED  xxx                 xxxxxxxx             xxx           ... \n
                    6       3                   8                    n*3
                    9+8+n*3 size
                    */
                    if (_uart_receiveSize == 18)
                    {
                        numOfData = StringToUint24(_uart_receiveData+10);
                        startAddress = StringToUint24(_uart_receiveData+2);
                        
                        if ( (numOfData<=100) && (numOfData!=0) && (startAddress+numOfData-1 <= 0x00FFFFFF) )
                        {//Check if arguments is good
                            _uart_transmitSize = 0;
                            checksumCalculated = 0;
                            ptr = _uart_transmitData + CopyCStringToTransmitBuffer("READEDxxx");
                            Uint24ToString(ptr, startAddress);
                            ptr+=8;
                            
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
                            
                            _uart_transmitSize = 18+numOfData*3;
                            BEGIN_TRANSMISSION_AND_WAIT
                        }
                        else
                        {
                            /*
                            return
                            [BAD_SIZE][\n]
                            9 size
                            */
                            TransmitCString("BAD_SIZE\n");
                        }
                    }
                    else
                    {
                        /*
                        return
                        [BAD_SIZE][\n]
                        9 size
                        */
                        TransmitCString("BAD_SIZE\n");
                    }
                }
                else
                {
                    /*
                    return
                    [UNKNOWN][\n]
                    8 size
                    */
                    TransmitCString("UNKNOWN\n");
                }
            }
            else
            {
                /*
                return
                [BAD_SIZE][\n]
                9 size
                */
                TransmitCString("BAD_SIZE\n");
            }
            
            _uart_receiveFlag = 0;
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
void Uint24ToString(unsigned char* str, unsigned long uint24)
{
    //xx'xxx'xxx
    unsigned char i;
    
    for (i=0; i<8; ++i)
    {
        str[7-i] = (uint24%10)+'0';
        uint24 /= 10;
    }
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

