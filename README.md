# MM1_Writer_code

Copyright (C) 2021 Guillaume Guillet\
Licensed under MIT License

<table border="0px">
<tr>
<td>
MIT License.
</td>
</tr>
<tr>
<td>
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
</td>
</tr>
</table>

Made with [Keil uVision](https://www2.keil.com/mdk5/uvision/)

## Description
This is the uc code for the project [MM1_Writer](https://github.com/JonathSpirit/MM1_Writer).

<img src="images/TeraTerm.png" alt="TeraTerm" width="400"/> <img src="images/board.jpeg" alt="board" width="400"/>

## Stats

![version](https://img.shields.io/badge/revision-MM1_Writer_code_R2-blue)

![bug](https://img.shields.io/github/issues/JonathSpirit/MM1_Writer_code/bug)\
![fixed](https://img.shields.io/github/issues/JonathSpirit/MM1_Writer_code/fixed)

## Communication
To communicate with the board you have to use a usb to uart bridge and connect every cable to the required pins (RX, TX, GND).

The board is configured like so : 9600baud, no parity, 1 stop bit.

Every command/response is in ascii. A command begin with '$' and must end with '#' (no line feed or carriage return).
A response end with a line feed.

### Commands
Here is the commands list (you can find more detail in the source code):

Hello command, return hello
| Command  | Response     |
| -------- | ------------ |
| [$H] [#] | [HELLO] [\n] |

Info command, return code information\
(every information in the response is separate with ':' and a '=' is used for arguement)
| Command  | Response |
| -------- | -------- |
| [$I] [#] | ... [\n] |

Write command, write data on the memory\
(response can be WRITED, BAD_CHECKSUM etc...)
| Command                                                               | Response |
| --------------------------------------------------------------------- | -------- |
| [$W] [Data checksum 8bit] [Start address 24bit] [n data 8bit] ... [#] | ... [\n] |

Read command, read data on the memory
| Command                                            | Response                                                                   |
| -------------------------------------------------- | -------------------------------------------------------------------------- |
| [$R] [Start address 24bit] [Num of data 24bit] [#] | [READED] [Data checksum 8bit] [Start address 24bit] [n data 8bit] ... [\n] |

