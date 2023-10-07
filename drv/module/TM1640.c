#include <stdio.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <common.h>
#include "stdint.h"
#include <string.h>
#include "TM1640.h"
//引脚定义
#define SET_SCLK()      PIN_SET(GPIOA7)
#define CLK_SCLK()      PIN_CLEAR(GPIOA7)
#define SET_DATA()      PIN_SET(GPIOA9)
#define CLK_DATA()      PIN_CLEAR(GPIOA9)


#define EIGHT_SEGMENT_CODE_LENGTH 7

uint8_t table[] ={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

union EightSegmentCode eightSegmentCode[EIGHT_SEGMENT_CODE_LENGTH]={0};

//显示,地址,命令
#define INC_Addr 0x40
#define Addr     0x44

//亮度
#define ON_1_16  (0X88+0)
#define ON_2_16  (0X88+1)
#define ON_4_16  (0X88+2)
#define ON_10_16  (0X88+3)
#define ON_11_16  (0X88+4)
#define ON_12_16  (0X88+5)
#define ON_13_16  (0X88+6)
#define ON_14_16  (0X88+7)

void gpio_aip16400_init(void);

/************************************************************************************************************

*********************************************IIC总线驱动层***************************************************

*************************************************************************************************************/
void start(void)
{
	SET_SCLK();
	SET_DATA();
	SysTick_DelayUS(2);
	CLK_DATA();
	SysTick_DelayUS(2);
	CLK_SCLK();
	SysTick_DelayUS(2);
}

void stop(void)
{	 	
	CLK_DATA();
	SET_SCLK();
	SysTick_DelayUS(2);
	SET_DATA();
	SysTick_DelayUS(2);
}

void TM1640_WriteData(unsigned char num)   
{
	unsigned char i=0;
	
	CLK_DATA();
	CLK_SCLK();
	
	for(i=0;i<8;i++)
	{
		CLK_SCLK();
		SysTick_DelayUS(2);
		if(num&0x01) 
		{
			SET_DATA(); 
		}
		else 
		{
			CLK_DATA();
		}
		SysTick_DelayUS(2);
		SET_SCLK();
		SysTick_DelayUS(2);
		num=num>>1;
	}
	
	CLK_DATA();
	CLK_SCLK();
}

void aip1640_write(uint8_t data)
{
    start();
    TM1640_WriteData(data);
    stop();
}

/***************************************************************************************************************

*********************************************TM1638应用层*******************************************************

***************************************************************************************************************/

void TM1640_Init(void)
{
	gpio_aip16400_init();
	aip1640_write(INC_Addr);
	start();
	TM1640_WriteData(ON_10_16);
	stop();
	memset(eightSegmentCode,0xff,EIGHT_SEGMENT_CODE_LENGTH);
	TM1640_WriteEightSegmentCode();
}

void TM1640_WriteArrayData(uint8_t startSegment,uint8_t *inputData,uint8_t length)
{
  static uint8_t lastInputData[EIGHT_SEGMENT_CODE_LENGTH],status = 0,i=0;
	
  if(status==0)
	{
    status = 1;
    memcpy(lastInputData,inputData,EIGHT_SEGMENT_CODE_LENGTH);
  }
	else
	{
   if(memcmp(lastInputData,inputData,EIGHT_SEGMENT_CODE_LENGTH)==0)
	 {
		start();
		TM1640_WriteData(ON_1_16);
		stop();
		return;
   }
    memcpy(lastInputData,inputData,EIGHT_SEGMENT_CODE_LENGTH);		
  }	
	
	start();

	TM1640_WriteData(0xc0+startSegment);

	for(i=startSegment;i<length;i++)
	{
		TM1640_WriteData(*inputData);
		inputData++;
	}
	
	stop();	
}
void TM1640_WriteEightSegmentCode(void)
{
	 TM1640_WriteArrayData(SEG1,(uint8_t*)eightSegmentCode,EIGHT_SEGMENT_CODE_LENGTH);
}
