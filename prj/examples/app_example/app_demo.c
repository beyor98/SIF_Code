/*====================================================================
 *
 * Copyright (C) 2020 Gree Microelectronics.  All Rights Reserved.
 *
 * @file app_example.c
 *
 * @version 1.0.0
 *
 * @date 2020/12/24
 *
 * @contents Verification and example code of Typical applications.
 *
 * @history
 *  Date                 By           Comments        Ver.
 *  =========       ============     =========    =========
 *  2020/12/24      software group    Create        1.0.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/flash.h>
/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#define DBG_PR_EN              1
#if DBG_PR_EN
    #define DBG_PR       printf
#else
    #define DBG_PR(...)  do{}while(0)
#endif

#define MAX_WRRD_BYTE    7
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
#define CHECK_SIZE 50
static int check_idx = 0;
static volatile uint64_t systime_val[CHECK_SIZE];

void Systick_Callback(void)
{
    if (check_idx < CHECK_SIZE)
    {
        systime_val[check_idx]  = SysTick_GetSysTime();
        check_idx++;
    }
    //OS_Schedule();
}

void systick_example(void)
{
    WaitTimer_T waitTimer;
    uint64_t startTime;
    uint32_t startTick;

    SysTick_Init();

    SysTick_DelayTicks(200);                        /* Delay 200 ticks */
    SysTick_DelayUS(200);                           /* Delay 200 microseconds */
    SysTick_DelayMS(200);                           /* Delay 200 milliseconds */

    SysTick_Disable();                              /* Stop systick */
    SysTick_Init_Ex(SYSTICK_1MS, 0, 0);             /* Default configuration is all 0 priority for all devices, system start time is 0. */
    SysTick_DelayTicks(200);                        /* Delay 200 ticks */
    SysTick_DelayUS(200);                           /* Delay 200 microseconds */
    SysTick_DelayMS(200);                           /* Delay 200 milliseconds */

    SysTick_Disable();                              /* Stop systick */
    SysTick_Init_Ex(SYSTICK_1MS, 0, 0);
    WaitTimerInit(&waitTimer, 200, 1);
    while (1)
    {
        /* Wait for a while if it is called in blocking function */
        if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
        {
            break;
        }
    }


    startTime = SysTick_GetSysTime();

    while (1)
    {

        if ((SysTick_GetSysTime() - startTime) >= 200)        /* Wait 200 milliseconds */
        {
            break;
        }
    }

    startTick = SysTick_Get();
    while (1)
    {
        //Polling_Event();
        if (SysTick_Interval(startTick, SysTick_Get()) >= 200 * SYSTICK_US_TICKS)  /* Wait 200 microseconds for specified event */
        {
            break;
        }
    }

}

/******************************************************************************
* flash_example
******************************************************************************/
RET_CODE flash_example(void)
{
    uint32_t i = 0;
    uint32_t pageAddress = 0;
    FLASH_Param_T param;
    Page_LockType_T locked;
    uint8_t wrdata[MAX_WRRD_BYTE];
    uint8_t rddata[MAX_WRRD_BYTE];

    for (i = 0; i < MAX_WRRD_BYTE; i ++)
    {
        wrdata[i] = i + 1;
    }

    Flash_GetParam(&param);
    if (param.sizePerPage   != FLASH_PAGE_SIZE   || \
            param.pagesPerFlash != FLASH_TOTAL_PAGES || \
            param.sizePerFlash  != FLASH_TOTAL_SIZE)
    {
        return RET_ERROR;
    }
    /* Flash page address */
    pageAddress = FLASH_PAGE_SIZE * 32;

    /*
     *  Flash page can locked by option byte,
     *  then erase/program t page will be aborted
     *  and generate an operation error in FLASH_ STATUS register.
     *  This function can protect code segment from being modified.
     */
    locked = Flash_PageIsLocked(FLASH_PAGE_SIZE * 32);
    /* Flash page-erase works only when the target page is unprotected */
    if (locked != PAGE_IS_LOCKED)
    {
        /* Issue a page-earse
         * Don't erase flash frequently,
         * flash has erasure times limit!!
         */
        Flash_Erase(ERASE_TYPE_PAGE, pageAddress);
        /* Write a sequence of data */
        Flash_Write(pageAddress, wrdata, MAX_WRRD_BYTE);
        Flash_Read(pageAddress, rddata, MAX_WRRD_BYTE);

        for (i = 0; i < MAX_WRRD_BYTE; i ++)
        {
            if (wrdata[i] != rddata[i])
            {
                return RET_ERROR;
            }
        }
    }
    return RET_OK;
}

int main(void)
{

    SysTick_Init();
    SysTick_DelayMS(1000);
    DebugConsoleInit(115200);

    systick_example();

    flash_example();

}
