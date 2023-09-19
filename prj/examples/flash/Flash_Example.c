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
 *     Date              By              Comments       Ver.
 *  ============    ===============     =========     =========
 *   2016/02/16      Wizephen Wang       Create        1.0.0
 **********************************************************************/

#include <common.h>
#include <drv/dev/flash.h>

/******************************************************************************
* Constants and macros
******************************************************************************/
#define MAX_WRRD_BYTE    7

/******************************************************************************
* Global variables
******************************************************************************/

uint8_t g_wrdata[MAX_WRRD_BYTE];
uint8_t g_rddata[MAX_WRRD_BYTE];

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
int main(void);
/******************************************************************************
* Global functions
******************************************************************************/


int main(void)
{
    int i = 0;

    FLASH_Param_T param;
    Page_LockType_T locked;
    uint32_t pageAddress = 0;

    SysTick_Init();

    for (i = 0; i < 16; i ++)
    {
        g_wrdata[i] = i + 1;
    }

    Flash_GetParam(&param);

#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_GM6621) || defined(CHIP_F6601C)
    if (param.sizePerPage   != FLASH_PAGE_SIZE   || \
            param.pagesPerFlash != FLASH_TOTAL_PAGES || \
            param.sizePerFlash  != FLASH_TOTAL_SIZE)
    {
        return RET_ERROR;
    }
#endif

    /* Flash page address */
    pageAddress = FLASH_PAGE_SIZE * 32;

    locked = Flash_PageIsLocked(FLASH_PAGE_SIZE * 32);

    /* Flash page-erase works only when the target page is unprotected */
    if (locked != PAGE_IS_LOCKED)
    {
        /* Issue a page-earse */
        Flash_Erase(ERASE_TYPE_PAGE, pageAddress);
        /* Write a sequence of data */
        Flash_Write(pageAddress, g_wrdata, MAX_WRRD_BYTE);
        Flash_Read(pageAddress, g_rddata, MAX_WRRD_BYTE);

        for (i = 0; i < MAX_WRRD_BYTE; i ++)
        {
            if (g_wrdata[i] != g_rddata[i])
            {
                return RET_ERROR;
            }
        }
    }

    while (1) {};
}
