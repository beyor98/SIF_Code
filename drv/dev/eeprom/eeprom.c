/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            eeprom.c
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date           2016/06/29 
 *    
 * @brief           This file emulates EEPROM with the flash in chip, it uses two pages, once a page
 *                     is full, it will transfer the valid data to another page. The data stored to emulated
 *                     EEPROM composes of 16bits value and 16bits virtual address. The first four byte
 *                     of each page indicates the page's status : valid, erase, receive.
 * 
 * @note
 *    2016/06/29, terry.wu, V1.0.0
 *        Initial version.
 */

#include <stdio.h>
#include <drv/dev/eeprom.h>
#include <drv/dev/flash.h>


//#define EEPROM_PRINT            1
#ifndef EEPROM_PRINT
    #define eeprom_print(...)   do{}while(0)
#else
    #define eeprom_print        printf
#endif

#define VALUE_PER_BYTE          2
#define VALUE_ADDR_PER_BYTE     4

/* Actual number of variable that need to store to EEPROM */
static uint8_t s_varNum = 0;
/* Store virtual address of variable */
static uint16_t s_varVirtualAddr[VAR_MAX_NUM] = {0};
/* Record the address of next write, it's the offset to the page start address */
static uint16_t s_curWriteAddr = 0;
/* Mapping of virtual address and page offset */
static uint16_t s_virtualAddrMap[VAR_MAX_NUM][2] = {{0,0}};


static int16_t FindValidPage(EEPROM_Operation_T operation)
{
	int16_t ret = -1;
	uint32_t page0Status = PAGE_VALID;
	uint32_t page1Status = PAGE_VALID;

	page0Status = (*(__IO uint32_t *)EEPROM_PAGE0_START_ADDR);
	page1Status = (*(__IO uint32_t *)EEPROM_PAGE1_START_ADDR);

	switch (operation)
	{
		case READ_FROM_VALID_PAGE:
			if (page0Status == PAGE_VALID)
			{
				ret = EEPROM_PAGE0;
			}
			else if (page1Status == PAGE_VALID)
			{
				ret = EEPROM_PAGE1;
			}
			else
			{
				ret = -1;
			}
			break;
		case WRITE_TO_VALID_PAGE:
			if (page0Status == PAGE_VALID)
			{
				ret = (page1Status == PAGE_RECEIVE) ? EEPROM_PAGE1 : EEPROM_PAGE0;
			}
			else if (page1Status == PAGE_VALID)
			{
				ret = (page0Status == PAGE_RECEIVE) ? EEPROM_PAGE0 : EEPROM_PAGE1;
			}
			else
			{
				ret = -1;
			}
			break;
		default:
		    eeprom_print("unsupport EEPROM operation.\n");
			ret = -1;
			break;
	}

	return ret;
}

static int8_t InitCurWriteAddr(void)
{
	int8_t ret = -1;
	int16_t validPage = 0;
	uint32_t startAddr = 0;
	uint32_t endAddr = 0;
	uint32_t addr = 0;

	validPage = FindValidPage(WRITE_TO_VALID_PAGE);
	if (validPage < 0)
	{
		eeprom_print("Can not find valid page.\n");
		return -1;
	}

	startAddr = (uint32_t)(FLASH_LOGICAL_BASE_ADDRESS + FLASH_PAGE_SIZE * validPage);
	endAddr = (uint32_t)(startAddr + FLASH_PAGE_SIZE - 1);
	
	addr = startAddr + 4;  /* the first four byte store the page status */
	/* find the available address to write */
	while (addr < endAddr)
	{
		if ((*(__IO uint32_t *)addr) == 0xFFFFFFFF)
		{
			s_curWriteAddr = (uint16_t)(addr - startAddr);
			ret = 0;
			break;
		}
		else
		{
			addr += VALUE_ADDR_PER_BYTE;
		}
	}

	return ret;
}

static void ResetVirtualAddrMap(void)
{
	uint8_t i = 0;

	for (i = 0; i < s_varNum; i++)
	{
		s_virtualAddrMap[i][0] = s_varVirtualAddr[i];
		s_virtualAddrMap[i][1] = 0xFFFF;
	}
}

static int8_t InitVirtualAddrMap(void)
{
	int8_t ret = -1;
	uint8_t i = 0;
	int16_t validPage = 0;
	uint32_t startAddr = 0;
	uint32_t addr = 0;
	uint16_t virtualAddr = 0xFFFF;
	uint8_t matchNum = 0;

	ResetVirtualAddrMap();

	validPage = FindValidPage(READ_FROM_VALID_PAGE);
	if (validPage < 0)
	{
		eeprom_print("Can not find valid page.\n");
		return ret;
	}

	startAddr = (uint32_t)(FLASH_LOGICAL_BASE_ADDRESS + FLASH_PAGE_SIZE * validPage);		
	addr = (uint32_t)(startAddr + FLASH_PAGE_SIZE - 2); /* search from the end of page */
	while (addr > (startAddr + 2))
	{
		virtualAddr = (*(__IO uint16_t *)addr);
		for (i = 0; i < s_varNum; i++)
		{
			if ((virtualAddr == s_virtualAddrMap[i][0]) && (s_virtualAddrMap[i][1] == 0xFFFF))
			{
				/* store the offset of the virtual address */
				s_virtualAddrMap[i][1] = (uint16_t)(addr - startAddr);
				matchNum++;
				break;
			}
		}

		if (matchNum >= s_varNum)
		{
			break;
		}
		else
		{
			addr -= VALUE_ADDR_PER_BYTE;
		}
	}

	return 0;
}


RET_CODE EEPROM_SetVirtualAddrTable(uint16_t *pVirtualAddr, uint8_t num)
{
	RET_CODE ret = RET_ERROR;
	uint16_t i = 0;

	if (num > VAR_MAX_NUM)
	{
		eeprom_print("Can not support so many variables, num = %d.\n", num);
		return ret;
	}
	
	for (i = 0; i < num; i++)
	{
		if (pVirtualAddr[i] == INVALID_VIRTUAL_ADDR)
		{
			eeprom_print("Invalid address for variable, pVirtualAddr[%d] = 0x%x.\n",
				i, pVirtualAddr[i]);
			return ret;
		}
	}

	for (i = 0; i < num; i++)
	{
		s_varVirtualAddr[i] = pVirtualAddr[i];
	}
	s_varNum = num;

	return RET_OK;
}

RET_CODE EEPROM_Format(void)
{
	RET_CODE ret = RET_OK;
	uint32_t pageStatus = PAGE_VALID;

	ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE0_START_ADDR);
	if (ret != RET_OK)
	{
		return ret;
	}
	pageStatus = PAGE_VALID;
	ret = Flash_Write(EEPROM_PAGE0_START_ADDR, (uint8_t *)&pageStatus, 4);
	if (ret != RET_OK)
	{
		return ret;
	}

	ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE1_START_ADDR);
	if (ret != RET_OK)
	{
		return ret;
	}

	return ret;
}

static uint16_t GetReadOffset(uint16_t virtualAddr)
{
	uint8_t i = 0;
	uint16_t offset = 0xFFFF;

	for (i = 0; i < s_varNum; i++)
	{
		if (s_virtualAddrMap[i][0] == virtualAddr)
		{
			offset = s_virtualAddrMap[i][1];
			break;
		}
	}
	if (i >= s_varNum)
	{
		eeprom_print("Invalid virtual address, virtualAddr=0x%x.\n", virtualAddr);
	}

	return offset;
}

static int8_t SetReadOffset(uint16_t virtualAddr, uint16_t offset)
{
	uint8_t i = 0;
	int8_t ret = 0;

	for (i = 0; i < s_varNum; i++)
	{
		if (s_virtualAddrMap[i][0] == virtualAddr)
		{
			s_virtualAddrMap[i][1] = offset;
			break;
		}
	}

	if (i >= s_varNum)
	{
		ret = -1;
		eeprom_print("Invalid virtual address, virtualAddr=0x%x.\n", virtualAddr);
	}

	return ret;
}

RET_CODE EEPROM_Read(uint16_t virtualAddr, uint16_t *pValue)
{
	int16_t validPage = 0;
	uint32_t startAddr = 0;
	uint16_t offset = 0;

	if (virtualAddr == INVALID_VIRTUAL_ADDR)
	{
		eeprom_print("Invalid virtual address.\n");
		return RET_ERROR;
	}
	
	validPage = FindValidPage(READ_FROM_VALID_PAGE);
	if (validPage < 0)
	{
		eeprom_print("Can not find valid page.\n");
		return RET_ERROR;
	}

	startAddr = (uint32_t)(FLASH_LOGICAL_BASE_ADDRESS + FLASH_PAGE_SIZE * validPage);
	offset = GetReadOffset(virtualAddr); /* get the virtual address offset */
	if (offset == 0xFFFF)
	{
		return RET_ERROR;
	}

	*pValue = (*(__IO uint16_t *)(startAddr + offset - 2)); /* value is 2 byte ahead of address */

	return RET_OK;
}

static RET_CODE TransferPage(uint16_t virtualAddr, uint16_t value)
{
	uint8_t i = 0;
	int16_t validPage = 0;
	uint16_t valueTmp = 0;
	RET_CODE ret = RET_OK;
	uint32_t oldPageAddr = 0;
	uint32_t newPageAddr = 0;
	uint32_t pageStatus = PAGE_VALID;

	validPage = FindValidPage(READ_FROM_VALID_PAGE);
	if (validPage == EEPROM_PAGE0)
	{
		newPageAddr = EEPROM_PAGE1_START_ADDR;
		oldPageAddr = EEPROM_PAGE0_START_ADDR;
	}
	else if (validPage == EEPROM_PAGE1)
	{
		newPageAddr = EEPROM_PAGE0_START_ADDR;
		oldPageAddr = EEPROM_PAGE1_START_ADDR;
	}
	else
	{
		eeprom_print("Can not find valid page.\n");
		return RET_ERROR;
	}

	/* set the new page in receive status */
	pageStatus = PAGE_RECEIVE;
	ret = Flash_Write((uint32_t)newPageAddr, (uint8_t *)&pageStatus, 4);
	if (ret != RET_OK)
	{
		eeprom_print("Write page status to EEPROM fail.\n");
		return ret;
	}
	s_curWriteAddr += 4;

	/* write the new passed value to EEPROM */
	ret = EEPROM_Write(virtualAddr, value);
	if (ret != RET_OK)
	{
		eeprom_print("Write value to EEPROM fail.\n");
		return ret;
	}

	/* transfer data from old page to new page */
	for (i = 0; i < s_varNum; i++)
	{
		if (s_varVirtualAddr[i] != virtualAddr)
		{
			ret = EEPROM_Read(s_varVirtualAddr[i], (uint16_t *)&valueTmp);
			if (ret == RET_OK)
			{
				EEPROM_Write(s_varVirtualAddr[i], valueTmp);
			}
		}
	}

	ret = Flash_Erase(ERASE_TYPE_PAGE, oldPageAddr);
	if (ret != RET_OK)
	{
		return ret;
	}

	pageStatus = PAGE_VALID;
	ret = Flash_Write((uint32_t)newPageAddr, (uint8_t *)&pageStatus, 2);
	//ret = Flash_Program2Address(newPageAddr, pageStatus, FLASH_PROGRAM_DATA_WIDTH_16BIT);
	if (ret != RET_OK)
	{
		eeprom_print("Write page status to EEPROM fail.\n");
		return ret;
	}

	return ret;
}

RET_CODE EEPROM_Write(uint16_t virtualAddr, uint16_t value)
{
	int16_t validPage = 0;
	uint32_t startAddr = 0;
	uint32_t dataWrite = 0;
	RET_CODE ret = RET_OK;

	if (virtualAddr == INVALID_VIRTUAL_ADDR)
	{
		eeprom_print("Invalid virtual address.\n");
		return RET_ERROR;
	}

	if ((s_curWriteAddr != 0) && ((s_curWriteAddr % FLASH_PAGE_SIZE) == 0))
	{
		s_curWriteAddr = 0;
		/* page full, transfer the valid data to another page */
		ret = TransferPage(virtualAddr, value);
		return ret;
	}
	
	validPage = FindValidPage(WRITE_TO_VALID_PAGE);
	if (validPage < 0)
	{
		eeprom_print("Can not find valid page.\n");
		return RET_ERROR;
	}

	startAddr = (uint32_t)(FLASH_LOGICAL_BASE_ADDRESS + FLASH_PAGE_SIZE * validPage);
/*
	ret = Flash_Write((uint32_t)(startAddr + s_curWriteAddr), (uint8_t *)&value, 2);
	if (ret != RET_OK)
	{
		eeprom_print("Write value to EEPROM fail.\n");
		return ret;
	}

	ret = Flash_Write((uint32_t)(startAddr + s_curWriteAddr + 2), (uint8_t *)&virtualAddr, 2);
	if (ret != RET_OK)
	{
		eeprom_print("Write address to EEPROM fail.\n");
		return ret;
	}
*/
	dataWrite = ((virtualAddr << 16) | (value)); /* write together to avoid issue caused by power off */
	ret = Flash_Write((uint32_t)(startAddr + s_curWriteAddr), (uint8_t *)&dataWrite, 4);
	if (ret != RET_OK)
	{
		eeprom_print("Write value to EEPROM fail.\n");
		return ret;
	}
	
	SetReadOffset(virtualAddr, (uint16_t)(s_curWriteAddr + 2)); /* update virtual address to table */
	s_curWriteAddr += VALUE_ADDR_PER_BYTE;  /* the next write address */
	
	return RET_OK;
}

RET_CODE EEPROM_Init(void)
{
	uint8_t i = 0;
	uint16_t value = 0;
	RET_CODE ret = RET_OK;
	uint32_t pageStatus = PAGE_VALID;
	uint32_t page0Status = PAGE_VALID;
	uint32_t page1Status = PAGE_VALID;

	page0Status = (*(__IO uint32_t *)EEPROM_PAGE0_START_ADDR);
	page1Status = (*(__IO uint32_t *)EEPROM_PAGE1_START_ADDR);

	switch (page0Status)
	{
		case PAGE_ERASED:
			if (page1Status == PAGE_VALID)
			{
				ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE0_START_ADDR);
				if (ret != RET_OK)
				{
					return ret;
				}
			}
			else if (page1Status == PAGE_RECEIVE)
			{
				ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE0_START_ADDR);
				if (ret != RET_OK)
				{
					return ret;
				}

				pageStatus = PAGE_VALID;
				ret = Flash_Write(EEPROM_PAGE1_START_ADDR, (uint8_t *)&pageStatus, 4);
				//ret = Flash_Program2Address(EEPROM_PAGE1_START_ADDR, pageStatus, FLASH_PROGRAM_DATA_WIDTH_16BIT);
				if (ret != RET_OK)
				{
					return ret;
				}
			}
			else
			{
				ret = EEPROM_Format();
				if (ret != RET_OK)
				{
					return ret;
				}
			}
			break;
		case PAGE_RECEIVE:
			if (page1Status == PAGE_VALID)
			{
				/* transfer data from page1 to page0 */
				InitCurWriteAddr();
				InitVirtualAddrMap();
				for (i = 0; i < s_varNum; i++)
				{
					if ((*(uint16_t *)(EEPROM_PAGE0_START_ADDR + 6)) != s_varVirtualAddr[i])
					{
						ret = EEPROM_Read(s_varVirtualAddr[i], &value);
						if (ret == RET_OK)
						{
							EEPROM_Write(s_varVirtualAddr[i], value);
						}
					}
				}
				
				ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE1_START_ADDR);
				if (ret != RET_OK)
				{
					return ret;
				}

				pageStatus = PAGE_VALID;
				ret = Flash_Write(EEPROM_PAGE0_START_ADDR, (uint8_t *)&pageStatus, 4);
				//ret = Flash_Program2Address(EEPROM_PAGE0_START_ADDR, pageStatus, FLASH_PROGRAM_DATA_WIDTH_16BIT);
				if (ret != RET_OK)
				{
					eeprom_print("Write page status to EEPROM fail.\n");
					return ret;
				}
			}
			else if (page1Status == PAGE_ERASED)
			{
				ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE1_START_ADDR);
				if (ret != RET_OK)
				{
					return ret;
				}

				pageStatus = PAGE_VALID;
				ret = Flash_Write(EEPROM_PAGE0_START_ADDR, (uint8_t *)&pageStatus, 4);
				//ret = Flash_Program2Address(EEPROM_PAGE0_START_ADDR, pageStatus, FLASH_PROGRAM_DATA_WIDTH_16BIT);
				if (ret != RET_OK)
				{
					return ret;
				}
			}
			else
			{
				ret = EEPROM_Format();
				if (ret != RET_OK)
				{
					return ret;
				}
			}
			break;
		case PAGE_VALID:
			if (page1Status == PAGE_VALID)
			{
				ret = EEPROM_Format();
				if (ret != RET_OK)
				{
					return ret;
				}
			}
			else if (page1Status == PAGE_ERASED)
			{
				ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE1_START_ADDR);
				if (ret != RET_OK)
				{
					return ret;
				}
			}
			else
			{
				/* transfer data from page0 to page1 */
				InitCurWriteAddr();
				InitVirtualAddrMap();
				for (i = 0; i < s_varNum; i++)
				{
					if ((*(uint16_t *)(EEPROM_PAGE1_START_ADDR + 6)) != s_varVirtualAddr[i])
					{
						ret = EEPROM_Read(s_varVirtualAddr[i], &value);
						if (ret == RET_OK)
						{
							EEPROM_Write(s_varVirtualAddr[i], value);
						}
					}
				}
				
				ret = Flash_Erase(ERASE_TYPE_PAGE, EEPROM_PAGE0_START_ADDR);
				if (ret != RET_OK)
				{
					return ret;
				}

				pageStatus = PAGE_VALID;
				ret = Flash_Write(EEPROM_PAGE1_START_ADDR, (uint8_t *)&pageStatus, 4);
				//ret = Flash_Program2Address(EEPROM_PAGE1_START_ADDR, pageStatus, FLASH_PROGRAM_DATA_WIDTH_16BIT);
				if (ret != RET_OK)
				{
					eeprom_print("Write page status to EEPROM fail.\n");
					return ret;
				}
			}
			break;
		default:
			ret = EEPROM_Format();
			if (ret != RET_OK)
			{
				return ret;
			}
			break;
	}

	InitCurWriteAddr();
	InitVirtualAddrMap();

	return RET_OK;
}




