/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lpower_demo.c
 *
 * @author        sophia.wu
 *
 * @version       1.0.0
 *
 * @date          2017/03/23
 *
 * @brief        Sample code for lpower_demo case.
 *
 * @note
 *    2017/03/23, sophia.wu, V1.0.0
 *        Initial version.
 */
#include <stdio.h>
#include <common.h>
#include <drv/dev/lpower.h>
#include <drv/dev/rtc.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define MODE_SLEEP
//#define MODE_DEEPSLEEP
//#define MODE_STOP

#define SYSTICK_INT_DISABLE     IO_BITS_CLEAR(SysTick->CTRL,(1<<1))
#define SYSTICK_INT_ENABLE      IO_BITS_SET(SysTick->CTRL,(1<<1))
/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Static variables
*******************************************************************************/

/*******************************************************************************
* Reference variables
*******************************************************************************/

/*******************************************************************************
* Local function prototypes
*******************************************************************************/

#ifdef MODE_SLEEP
static void SleepMode(void)
{
    printf("Enter sleep mode \n");
    /* disable systick interrupt*/
    SYSTICK_INT_DISABLE;
    SleepMode_Enter();

    Configure_CPU_WFE();
    Configure_CPU_WFE();

    PR_DEBUG("wakeup cpu 1 \n");
}
#endif

#ifdef MODE_DEEPSLEEP
static void DeepSleepMode(void)
{
    printf("Enter Deep sleep mode \n");
    /* disable systick interrupt*/
    IO_BITS_CLEAR(SysTick->CTRL, (1 << 1));
    DeepSleepMode_Enter();

    Configure_CPU_WFI();

    PR_DEBUG("wakeup cpu 2 \n");
}
#endif

void Callback(void)
{
    PR_DEBUG("rtc case\n");
}
void RTC_Cfg(void)
{
    RTC_Config_T rtcConfig;
    Timer_T timer;
    rtcConfig.timerCallback = NULL;
    rtcConfig.alarmCallback = NULL;
    rtcConfig.counterCallback = NULL;
#if defined(CHIP_GM6621)
    rtcConfig.clock = RTC_CLOCK_IRCLS;
#endif

#if defined(CHIP_F6621B)
    rtcConfig.clock         = RTC_CLOCK_LOSC;
#endif
    RTC_Init(&rtcConfig);
    RTC_Start();

    timer.cycle.tv_sec = 5;
    timer.cycle.tv_msec = 0;
    RTC_StartTimer(&timer);
}


int main(void)
{
    RET_CODE ret = RET_OK;

    SysTick_Init();
    DebugConsoleInit(SYS_UART_BAUD_RATE48M);
    /****For IDE Debug using******/
    SysTick_DelayMS(3000);
#if defined(CHIP_GM6621)||defined(CHIP_F6621B)
    LPower_PeriphClkConfig_T LPower_ClockCfgInit;
    LPower_PeriphClk_T LPower_PeriphClk;

    RTC_Cfg();

    memset(&LPower_ClockCfgInit, 0, sizeof(LPower_PeriphClkConfig_T));
    LPower_ClockCfgInit.rtcOn = Periph_ON;

    memset(&LPower_PeriphClk, 0, sizeof(LPower_PeriphClk_T));
    LPower_PeriphClk.PeriphClk = PERIPHCLOCK_SOURCE_LOSC32K;
    ret = LPower_PeriphClockCfg(&LPower_ClockCfgInit, &LPower_PeriphClk);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

#endif

#ifdef MODE_SLEEP
    SleepMode();
    SYSTICK_INT_ENABLE;
#endif

#ifdef MODE_DEEPSLEEP
    DeepSleepMode();
    SYSTICK_INT_ENABLE;
#endif

#if defined(CHIP_GM6611B)||defined(CHIP_GM6621)||defined(CHIP_F6621B)
#ifdef MODE_STOP
    LPower_AppMode_T AppMode;

    memset(&AppMode, 0, sizeof(LPower_AppMode_T));
    AppMode.LpowerMode = STOP_MODE;
#if defined(CHIP_GM6621)||defined(CHIP_F6621B)
    AppMode.EflashEn = EFLASH_Enable;
    AppMode.LcdAccessEn = LCD_ACCESS_Disable;
#endif
    printf("ready for stop\n");

    SysTick_DelayMS(100);

    LPower_Enter(&AppMode);
    Configure_CPU_WFI();

    /*
     * 5s RTC Timer interrupt, and consequently wakes-up
     * CPU right from Stop mode
     */

    ret = LPower_Exit(&AppMode);
    /*For uart clock switch*/
    SysTick_DelayMS(1000);

    if (ret == RET_STOP_FLAG_INCOMPATIBLE)
    {
        printf("LPower mode flag set is error !\n");
    }
    if (ret == RET_STOP_OK)
    {
        printf("LPower mode test pass !\n");
    }
    SysTick_DelayMS(1000);

#endif
#endif

    while (1);

}

