/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            eeprom_usage.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date           2016/07/4 
 *    
 * @brief           This file shows the procedure of using emulated EEPROM.
 *                     The emulated EEPROM uses two pages of flash in chip, once a page
 *                     is full, it will transfer the valid data to another page. The data stored to emulated
 *                     EEPROM composes of 16bits value and 16bits virtual address. The first four byte
 *                     of each page indicates the page's status : valid, erase, receive.
 * 
 * @note
 *    2016/06/29, terry.wu, V1.0.0
 *        Initial version.
 */
#include "stdio.h"
#include <common.h>
#include "config.h"
#include <drv/dev/eeprom.h>
#include <drv/dev/uart.h>

#define VAR_CNT    3

static uint16_t s_varAddr[VAR_CNT] = {0x1111, 0x2222, 0x3333};
static uint16_t s_varValue[VAR_CNT] = {0, 0 ,0};


int main(void)
{
	uint16_t value = 0;
	uint32_t baudrate = 19200;
	
	DebugConsoleInit(baudrate);

	EEPROM_SetVirtualAddrTable(s_varAddr, VAR_CNT);
	EEPROM_Init();
	
	for (value = 0; value < 256; value++)
	{
		EEPROM_Write(s_varAddr[0], value);
	}
	EEPROM_Read(s_varAddr[0], &s_varValue[0]);
	printf("data0 : %d.\n", s_varValue[0]);
	
	for (value = 0; value < 256; value++)
	{
		EEPROM_Write(s_varAddr[0], value);
		EEPROM_Write(s_varAddr[1], (value+1));
	}
	EEPROM_Read(s_varAddr[0], &s_varValue[0]);
	EEPROM_Read(s_varAddr[1], &s_varValue[1]);
	printf("data0 : %d.\n", s_varValue[0]);
	printf("data1 : %d.\n", s_varValue[1]);

	for (value = 0; value < 256; value++)
	{
		EEPROM_Write(s_varAddr[0], value);
		EEPROM_Write(s_varAddr[1], (value+1));
		EEPROM_Write(s_varAddr[2], (value+2));
	}
	EEPROM_Read(s_varAddr[0], &s_varValue[0]);
	EEPROM_Read(s_varAddr[1], &s_varValue[1]);
	EEPROM_Read(s_varAddr[2], &s_varValue[2]);
	printf("data0 : %d.\n", s_varValue[0]);
	printf("data1 : %d.\n", s_varValue[1]);
	printf("data2 : %d.\n", s_varValue[2]);
	
	while (1)
	{
		;
	}
}


