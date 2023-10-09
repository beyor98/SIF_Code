#include <stdio.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <common.h>
#include "stdint.h"
#include <string.h>
#include "TM1640.h"
// 引脚定义
#define SET_SCLK() PIN_SET(GPIOA7)
#define CLK_SCLK() PIN_CLEAR(GPIOA7)
#define SET_DATA() PIN_SET(GPIOA9)
#define CLK_DATA() PIN_CLEAR(GPIOA9)

#define EIGHT_SEGMENT_CODE_LENGTH 16

uint8_t table[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

union EightSegmentCode eightSegmentCode[EIGHT_SEGMENT_CODE_LENGTH] = {0};

// 显示,地址,命令
#define INC_Addr 	0x40
#define Addr 			0x44

// 亮度
#define ON_1_16 (0X88 + 0)
#define ON_2_16 (0X88 + 1)
#define ON_4_16 (0X88 + 2)
#define ON_10_16 (0X88 + 3)
#define ON_11_16 (0X88 + 4)
#define ON_12_16 (0X88 + 5)
#define ON_13_16 (0X88 + 6)
#define ON_14_16 (0X88 + 7)

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

void aip1640_WriteData(unsigned char num)
{
	unsigned char i = 0;

	CLK_DATA();
	CLK_SCLK();

	for (i = 0; i < 8; i++)
	{
		CLK_SCLK();
		SysTick_DelayUS(2);
		if (num & 0x01)
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
		num = num >> 1;
	}

	CLK_DATA();
	CLK_SCLK();
}

void aip1640_write(uint8_t data)
{
	start();
	aip1640_WriteData(data);
	stop();
}

/***************************************************************************************************************
*********************************************TM1638应用层*******************************************************
***************************************************************************************************************/
void aip1640_Init(void)
{
	gpio_aip16400_init();
	aip1640_write(INC_Addr);
	aip1640_write(ON_1_16);
	memset(eightSegmentCode, 0x0, EIGHT_SEGMENT_CODE_LENGTH);
	aip1640_WriteArrayData(SEG1, (uint8_t *)eightSegmentCode, EIGHT_SEGMENT_CODE_LENGTH);
	led_start_check();
}

void aip1640_WriteArrayData(uint8_t startSegment, uint8_t *inputData, uint8_t length)
{
	static uint8_t lastInputData[EIGHT_SEGMENT_CODE_LENGTH]; 
	static uint8_t status = 0; 
	static uint8_t i = 0;

	if (status == 0)
	{
		status = 1;
		memcpy(lastInputData, inputData, EIGHT_SEGMENT_CODE_LENGTH);
	}
	else
	{
		if (memcmp(lastInputData, inputData, EIGHT_SEGMENT_CODE_LENGTH) == 0)
		{
			return;
		}
		memcpy(lastInputData, inputData, EIGHT_SEGMENT_CODE_LENGTH);
	}

	start();
	aip1640_WriteData(startSegment);
	for (i=startSegment; i<length; i++)
	{
		aip1640_WriteData(*inputData);
		inputData++;
	}
	stop();
}

void aip1640_WriteEightSegmentCode(void)
{
	aip1640_WriteArrayData(SEG1, (uint8_t *)eightSegmentCode, EIGHT_SEGMENT_CODE_LENGTH);
}

void led_start_check(void)
{
	int i=0;
	for(i=1; i<10; i++)
	{
		eightSegmentCode[0].uint8_LED = table[i];	//Mileage
		eightSegmentCode[1].uint8_LED = table[i];
		eightSegmentCode[2].uint8_LED = table[i];
		eightSegmentCode[3].uint8_LED = table[i];
		eightSegmentCode[4].uint8_LED = table[i];
		
		eightSegmentCode[5].uint8_LED = table[i];	//speed
		eightSegmentCode[6].uint8_LED = table[i];
		eightSegmentCode[7].uint8_LED = table[i];
		eightSegmentCode[8].uint8_LED = table[i];
		
		eightSegmentCode[9].uint8_LED = 0xff;			//Battery frame
		eightSegmentCode[10].Bits.A = 1;
		eightSegmentCode[10].Bits.B = 1;
		eightSegmentCode[10].Bits.C = 1;					//logo
		eightSegmentCode[10].Bits.D = 1;
		
		switch(i){
        case 4: eightSegmentCode[11].Bits.A = 1;	////Battery
								eightSegmentCode[11].Bits.B = 1;
        case 3: eightSegmentCode[11].Bits.C = 1;
								eightSegmentCode[11].Bits.D = 1;
        case 2: eightSegmentCode[11].Bits.E = 1;
								eightSegmentCode[11].Bits.F = 1;
				case 1: eightSegmentCode[11].Bits.G = 1;
								eightSegmentCode[11].Bits.DP = 1;
								break;
    }
		
		aip1640_WriteArrayData(SEG1, (uint8_t *)eightSegmentCode, EIGHT_SEGMENT_CODE_LENGTH);
		SysTick_DelayMS(200);
	}
	
}

void lcd_display_process(uint8_t *sif_data_buf)
{
	static uint8_t lastInputData[EIGHT_SEGMENT_CODE_LENGTH]; 
	static uint8_t status = 0; 

	if (status == 0)
	{
		status = 1;
		memcpy(lastInputData, sif_data_buf, EIGHT_SEGMENT_CODE_LENGTH);
	}
	else
	{
		if (memcmp(lastInputData, sif_data_buf, EIGHT_SEGMENT_CODE_LENGTH) == 0)
		{
			printf("Same as previous received data\n");
			return;
		}
		memcpy(lastInputData, sif_data_buf, EIGHT_SEGMENT_CODE_LENGTH);
	}
	
	if(sif_data_buf[2]>>1 & 0x01)		//P
	{
		eightSegmentCode[7].uint8_LED = 0x73;
		eightSegmentCode[8].uint8_LED = 0x73;
	}
	
	if(sif_data_buf[3]>>4 & 0x01)		//控制器故障
	{
		eightSegmentCode[14].Bits.C = 1;
	}
	
	if(sif_data_buf[3]>>5 & 0x01)		//转把故障
	{
		eightSegmentCode[14].Bits.C = 1;
	}
	
	if(sif_data_buf[3]>>6 & 0x01)		//电机故障
	{
		eightSegmentCode[14].Bits.C = 1;
	}
}
