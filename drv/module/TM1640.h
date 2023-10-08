#ifndef __TM1640_H
#define __TM1640_H

#include <stdint.h>
//#define OFF 0;
//#define ON 1;

//数码管相关定义
#define SEG1 (uint8_t)0x00
#define SEG2 (uint8_t)0x01
#define SEG3 (uint8_t)0x02
#define SEG4 (uint8_t)0x03
#define SEG5 (uint8_t)0x04
#define SEG6 (uint8_t)0x05
#define SEG7 (uint8_t)0x06
#define SEG8 (uint8_t)0x07
#define SEG9 (uint8_t)0x08
#define SEG10 (uint8_t)0x09
#define SEG11 (uint8_t)0x0A
#define SEG12 (uint8_t)0x0B
#define SEG13 (uint8_t)0x0C
#define SEG14 (uint8_t)0x0D
#define SEG15 (uint8_t)0x0E
#define SEG16 (uint8_t)0x0F

union EightSegmentCode
{
  unsigned char uint8_LED;
	
  struct
	{
    unsigned char A		:1;
    unsigned char B		:1;
    unsigned char C		:1;
    unsigned char D		:1;
    unsigned char E		:1;     
    unsigned char F   :1;     
    unsigned char G   :1;      
    unsigned char DP  :1;    
  } Bits;
	
  struct
	{
    unsigned char ABCDEFG :7;    
    unsigned char DP :1;    
  } Bits1;
	
};

extern unsigned char table[];

extern union EightSegmentCode eightSegmentCode[];

void aip1640_Init(void);

void aip1640_WriteEightSegmentCode(void);

void aip1640_WriteArrayData(uint8_t startSegment, uint8_t *inputData, uint8_t length);


#endif
