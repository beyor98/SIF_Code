/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file Uart_Test.c
 *
 * @version 1.0.0
 * 
 * @date 2015/08/21 
 *    
 * @contents Verification and example code of Flash. 
 * 
 * @history
 *	   Date              By              Comments       Ver.
 *	============    ===============     =========     =========
 *	 2016/02/16      Wizephen Wang       Create        1.0.0
 **********************************************************************/

//#include <common.h>
//#include <drv/dev/flash.h>

/******************************************************************************
* Constants and macros
******************************************************************************/
//#define MAX_WRRD_BYTE    7

/******************************************************************************
* Global variables
******************************************************************************/

//uint8_t g_wrdata[MAX_WRRD_BYTE];
//uint8_t g_rddata[MAX_WRRD_BYTE];
   
/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
/******************************************************************************
* Local functions
******************************************************************************/
int main (void);
/******************************************************************************
* Global functions
******************************************************************************/

#define WRITE_DATA_1 0x5AA5A55A
#define WRITE_DATA_2 0x00000000
#define FLASH_PAGE_SIZE 0x400
#define INFO_PAGE_SIZE  0x200

#define TIMEOUT_MASSERASE 0x30000
#define TIMEOUT_PAGEERASE 0x10000
#define TIMEOUT_PROGRAM   0x400
#define ERROR_FLAGS       (1 << 16 | 1 << 20 | 1 << 24 | 1 < 28)

#define FLASH_START_ADDRESS 0x00000000
#define FLASH_TOTAL_SIZE    0x10000
#define FLASH_END_ADDRESS   (FLASH_START_ADDRESS + FLASH_TOTAL_SIZE)

#define INFOPAGE_START_ADDRESS 0x1FFFF000
#define INFOPAGE_TOTAL_SIZE    0x800
#define INFOPAGE_END_ADDRESS   (INFOPAGE_START_ADDRESS + INFOPAGE_TOTAL_SIZE)

#define FT_OK_INDICATOR   (*((volatile uint32_t*)(0x40012008)) = 0x0a) //gpio_indicator(0x0a)
#define FT_ERR_INDICATOR  (*((volatile uint32_t*)(0x40012008)) = 0x05) //gpio_indicator(0x05) 
#define FT_GPIO_CONFIGURE do{\
                                *((volatile uint32_t*)(0x40012010)) = 0xf; \
                                *((volatile uint32_t*)(0x40012018)) = 0xf; \
                            }while(0)
                                                       
typedef unsigned int  uint32_t;
typedef unsigned char uint8_t;                            

/*                            
void gpio_indicator(uint8_t indicator)                            
{
    *(volatile uint32_t*)(0x40012008) = indicator;
}
*/
                            
void mass_erase_flash(void)                            
{
    uint32_t timeout = 0;
    uint32_t addr = 0;
    /*First issue a mass erase*/
    *(volatile uint32_t *)(0x40001004) = 0x21;
    /*Wait till mass erase done*/
    while((!(*(volatile uint32_t *)(0x40001008) & (1 << 12))) && (timeout < TIMEOUT_MASSERASE));    
    if (timeout > TIMEOUT_MASSERASE)
    {
        /*drive a gpio to indicate error*/
        FT_ERR_INDICATOR;
        goto Ever_Loop;     
    }
    /*clear mass erase done flag*/
    *(volatile uint32_t *)(0x40001008) = 1 << 12;
    
    /*check write error*/
    if ((*(volatile uint32_t *)(0x40001008)) & ERROR_FLAGS)
    {        
        /*drive a gpio to indicate error*/
        FT_ERR_INDICATOR;
        goto Ever_Loop;    
    }
    
    for (addr = 0;addr < 0x10000; addr += 4)
    {
        if (*(volatile uint32_t *)(addr) != 0xFFFFFFFF)
        {
            /*drive a gpio to indicate error*/
            FT_ERR_INDICATOR;
            goto Ever_Loop;
        }                
    }
    
    return ;
    
Ever_Loop:
     while(1);    
}

void program_word_toflash(uint32_t startaddr, uint32_t endaddr, uint32_t data)
{
    uint32_t timeout = 0;
    uint32_t addr = 0;  
    
    /*Write word to pages*/
    for (addr = startaddr;addr < endaddr; addr += 4)
    {   /*32bit write*/
        *(volatile uint32_t *)(0x40001000) = 0x00;
        /*write address*/
        *(volatile uint32_t *)(0x4000100C) = addr;
        /*data value*/
        *(volatile uint32_t *)(0x40001010) = data;
        /*write data*/
        *(volatile uint32_t *)(0x40001004) = 0x01;  
        timeout = 0;
        /*wait till write done*/
        while((!(*(volatile uint32_t *)(0x40001008) & (1 << 4))) && (timeout < TIMEOUT_PROGRAM));
        if (timeout >= TIMEOUT_PROGRAM)
        {
            /*drive a gpio to indicate error*/
            FT_ERR_INDICATOR;
            goto Ever_Loop;     
        }
        /*clear write done flag*/
        *(volatile uint32_t *)(0x40001008) = 1 << 4;
        /*check write error*/
        if ((*(volatile uint32_t *)(0x40001008)) & ERROR_FLAGS)
        {        
            /*drive a gpio to indicate error*/
            FT_ERR_INDICATOR;
            goto Ever_Loop;        
        }
    }
    
    for (addr = startaddr; addr < endaddr; addr += 4)
    {
        if (*(volatile uint32_t *)(addr) != data)
        {
            /*drive a gpio to indicate error*/
            FT_ERR_INDICATOR;
            goto Ever_Loop;
        }                
    }
    
    return ;
    
Ever_Loop:
     while(1);
}

void page_erase(uint32_t startaddr, uint32_t endaddr, uint32_t page_size)
{
    uint32_t addr = 0;
    uint32_t timeout = 0;
    
    for (addr = startaddr;addr < endaddr; addr += page_size)
    {
        /*write address*/
        *(volatile uint32_t *)(0x4000100C) = addr;
        /*page erase*/
        *(volatile uint32_t *)(0x40001004) = 0x11;
        timeout = 0;
        /*wait till page-erase done*/
        while((!(*(volatile uint32_t *)(0x40001008) & (1 << 8))) && (timeout < TIMEOUT_PAGEERASE));
        if (timeout >= TIMEOUT_PAGEERASE)
        {
            /*drive a gpio to indicate error*/
            FT_ERR_INDICATOR;
            goto Ever_Loop;
        }
        /*clear write page-erase flag*/
        *(volatile uint32_t *)(0x40001008) = 1 << 8;
        /*check write error*/
        if ((*(volatile uint32_t *)(0x40001008)) & ERROR_FLAGS)
        {        
            /*drive a gpio to indicate error*/
            FT_ERR_INDICATOR;
            goto Ever_Loop;
        }
    }
    
    for (addr = startaddr;addr < endaddr; addr += 4)
    {
        if (*(volatile uint32_t *)(addr) != 0xFFFFFFFF)
        {
            /*drive a gpio to indicate error*/
            FT_ERR_INDICATOR;
            goto Ever_Loop;
        }                
    }
    
    return ;
    
Ever_Loop:
     while(1);    
}

int main (void)
{     
    /*Set gpio function*/
    FT_GPIO_CONFIGURE;
    /*===================mass erase========================*/  
    mass_erase_flash();    
    program_word_toflash(FLASH_START_ADDRESS, FLASH_END_ADDRESS, WRITE_DATA_1);            
    
    /*=================page erase============================*/
    page_erase(FLASH_START_ADDRESS, FLASH_END_ADDRESS, FLASH_PAGE_SIZE);        
    program_word_toflash(FLASH_START_ADDRESS, FLASH_END_ADDRESS, WRITE_DATA_2);    
    /*Finally issue a mass erase*/    
    mass_erase_flash();
    
    /*add test for infomation page*/ 
    page_erase(INFOPAGE_START_ADDRESS, INFOPAGE_END_ADDRESS, INFO_PAGE_SIZE); 
    program_word_toflash(INFOPAGE_START_ADDRESS, INFOPAGE_END_ADDRESS, WRITE_DATA_1);            
    page_erase(INFOPAGE_START_ADDRESS, INFOPAGE_END_ADDRESS, INFO_PAGE_SIZE); 
    program_word_toflash(INFOPAGE_START_ADDRESS, INFOPAGE_END_ADDRESS, WRITE_DATA_2);            
    page_erase(INFOPAGE_START_ADDRESS, INFOPAGE_END_ADDRESS, INFO_PAGE_SIZE);                
    
    /*drive a gpio to indicate complete done*/       
    FT_OK_INDICATOR;
    while(1);    
}
