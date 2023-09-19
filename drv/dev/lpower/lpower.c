/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          LPower.c
 *
 * @author        sophia.wu
 *
 * @version       1.0.0
 *
 * @date          2019/04/16
 *
 * @brief         Implementation of LPower Driver.
 *
 * @note
 *    2019/04/16, sophia.wu, V1.0.0
 *        Support 6621.
 */

#include <drv/dev/lpower.h>
#include "lpower_dev.h"
#include <drv/dev/clock.h>

#define LPower_DEBUG_PRINTF

#ifndef LPower_DEBUG_PRINTF
    #define LPower_PR_DEBUG(...)        do {} while (0)
#else
    #define UART_BAUD_RATE          19200
    #define LPower_PR_DEBUG         printf
#endif


void SleepMode_Enter(void)
{
    /*Set PMU enable*/
    LPOWER_PMC_PMU_ENABLE;
}

void DeepSleepMode_Enter(void)
{
    /*Set PMU enable*/
    LPOWER_PMC_PMU_ENABLE;
    /*Enable Sleep Deep for Cortex-M0*/
    SCB->SCR |= (1UL << 2);
}

#if defined(CHIP_GM6611B)||defined(CHIP_GM6621)||defined(CHIP_F6621B)
/**
 * @brief Proctect ldo,CHIP_GM6621 SYS Clock decrease to 8Mhz,CHIP_F6621B SYS Clock decrease to 2.1Mhz
 * @return None.
 */
static void LPower_SysClockDec(void)
{
    OSC_Config_T s_OscConfig;
    CLOCK_Config_T s_ClkConfig;

#if (defined(OSC_EXTERNAL_CLOCK))
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    /* PLL reference clock is direved from external high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;
#else
    /*Use internal oscillator */
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock = PLL_IRC_HSCLK;
#endif
    OSC_SetConfig(&s_OscConfig);

#if defined(CHIP_F6621B)
    /*Use MIRC as clock source*/
    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_MIRC;
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
    s_ClkConfig.flashLatency = FLASH_LATENCY_0;
#endif

#if defined(CHIP_GM6621) || defined(CHIP_GM6611B)
    /*Use HIRC as clock source*/
    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HIRC8M;
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
    s_ClkConfig.flashLatency = FLASH_LATENCY_1;
#endif
    CLOCK_SetConfig(&s_ClkConfig);

    /*Disable HOSC and PLL clock to save power*/
    SysClock_Disable(SYSCLOCK_SOURCE_HOSC8M);
    SysClock_Disable(SYSCLOCK_SOURCE_PLL48M);


#if defined(CHIP_F6621B)
    SysClock_Disable(SYSCLOCK_SOURCE_HIRC);
#endif

}

/**
 * @brief SYS Clock increase F6621B need to inc
 * @ GM6621 sysclock is 8Mhz,didn't need to increase,sysclock is 48Mhz,need to increase
 * @ F6621B need to increase
 * @return None.
 */
static void LPower_SysClockInc(void)
{
    OSC_Config_T s_OscConfig;
    CLOCK_Config_T s_ClkConfig;

#if (defined(OSC_EXTERNAL_CLOCK))
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    /* PLL reference clock is direved from external high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;
#else
    /*Use internal oscillator */
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock = PLL_IRC_HSCLK;
#endif
    OSC_SetConfig(&s_OscConfig);
    /*Use HIRC as clock source*/
    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    if (SYS_CLOCK == 48000000)
    {
        /* Use PLL clock as CPU clock source */
        s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
    }
#if defined(OSC_EXTERNAL_CLOCK)
    if (SYS_CLOCK == 8000000)
    {
        /* Use HOSC clock as CPU clock source */
        s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HOSC8M;
    }
#else
    if (SYS_CLOCK == 8000000)
    {
        /* Use HISC clock as CPU clock source */
#if defined(CHIP_GM6621)
        s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HIRC8M;

#elif defined(CHIP_F6621B)
        s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HIRC;
#endif
    }
#endif
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
    s_ClkConfig.flashLatency = FLASH_LATENCY_2;
    CLOCK_SetConfig(&s_ClkConfig);
}

/**
 * @brief Configure LPower PERIPH Clock
 * @param LPower_PeriphClkConfig_T     PERIPH Clock Switch:LIRC or LOSC
 * @param LPower_PeriphClk_T           config Periph:RTC,ACMP,WDG,LCD on or off
 * @return None.
 */
#if defined(CHIP_GM6621) || defined(CHIP_F6621B)
RET_CODE LPower_PeriphClockCfg(LPower_PeriphClkConfig_T *pLPower, LPower_PeriphClk_T *pCLK)
{
    RET_CODE ret = RET_OK;
    PeriphClk_Config_T PeriphClkCfg;
    if ((pCLK) == NULL)
    {
        LPower_PR_DEBUG("%s:input paramer error\n", __FUNCTION__);
        ret = RET_INVPARAM;
        goto out;
    }
#if defined(CHIP_GM6621) || defined(CHIP_F6621B)
    if (pCLK->PeriphClk == PERIPHCLOCK_SOURCE_LIRC32K)
    {
        /*LIRC stable*/
#if defined(CHIP_GM6621)
        SysClock_Enable(SYSCLOCK_SOURCE_LIRC32K);
#endif
        while (!Clock_IsNowStable(IRC32K_SOURCE_STABLE_MASK));
    }

    if (pCLK->PeriphClk == PERIPHCLOCK_SOURCE_LOSC32K)
    {
        /*LOSC stable*/
#if defined(CHIP_GM6621)
        SysClock_Enable(SYSCLOCK_SOURCE_LOSC32K);
#endif
#if defined(CHIP_F6621B)
        LOSC_PIN_MODE_INIT;
        LOSC_PIN_AAR_INIT;
        AOPDEV->LOSC |= SYS_AOP_CRY32K;
#endif
        while (!Clock_IsNowStable(OSC32K_SOURCE_STABLE_MASK));
    }

    /*Gate RTC, WDT, LCD, ACMP*/
    memset(&PeriphClkCfg, 0, sizeof(PeriphClk_Config_T));
    /*Clk switch LIRC:32K*/
#if defined(CHIP_GM6621)
    if (pCLK->PeriphClk == PERIPHCLOCK_SOURCE_LIRC32K)
    {
        if (pLPower->wdtOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_WDT_MASK;
            PeriphClkCfg.wdtClock = WDT_CLOCK_LIRC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->acmpOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_ACMP_MASK;
            PeriphClkCfg.acmpClock = ACMP_CLOCK_LIRC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->rtcOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_RTC0_MASK;
            PeriphClkCfg.rtc0Clock = RTC0_CLOCK_LIRC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->lcdOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_LCD_MASK;
            PeriphClkCfg.lcd_config_t.lcdClock = LCD_CLOCK_LIRC;
            PeriphClkCfg.lcd_config_t.cpClock = LCD_CLOCK_CP_LIRC;
            PeriphClk_SetConfig(&PeriphClkCfg);

        }

    }

    /*Clk switch LOSC:32.768K*/
    if (pCLK->PeriphClk == PERIPHCLOCK_SOURCE_LOSC32K)
    {
        if (pLPower->wdtOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_WDT_MASK;
            PeriphClkCfg.wdtClock = WDT_CLOCK_LOSC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->acmpOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_ACMP_MASK;
            PeriphClkCfg.acmpClock = ACMP_CLOCK_LOSC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->rtcOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_RTC0_MASK;
            PeriphClkCfg.rtc0Clock = RTC0_CLOCK_LOSC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->lcdOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_LCD_MASK;
            PeriphClkCfg.lcd_config_t.lcdClock = LCD_CLOCK_LOSC;
            PeriphClkCfg.lcd_config_t.cpClock = LCD_CLOCK_CP_LOSC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
    }

#elif defined(CHIP_F6621B)
    if (pCLK->PeriphClk == PERIPHCLOCK_SOURCE_LIRC32K)
    {
        if (pLPower->rtcOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_RTC_LCD_MASK;
            PeriphClkCfg.rtclcdClock = RTCLCD_CLOCK_LIRC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->lcdOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_RTC_LCD_MASK;
            PeriphClkCfg.rtclcdClock = RTCLCD_CLOCK_LIRC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->acmpOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_ACMP_MASK;
            PeriphClkCfg.acmpClock = ACMP_CLOCK_RTCCLK;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
    }

    /*Clk switch LOSC:32.768K*/
    if (pCLK->PeriphClk == PERIPHCLOCK_SOURCE_LOSC32K)
    {
        if (pLPower->rtcOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_RTC_LCD_MASK;
            PeriphClkCfg.rtclcdClock = RTCLCD_CLOCK_LOSC;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
        if (pLPower->lcdOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_RTC_LCD_MASK;
            PeriphClkCfg.rtclcdClock = RTCLCD_CLOCK_LOSC;
            PeriphClk_SetConfig(&PeriphClkCfg);


        }
        if (pLPower->acmpOn == Periph_ON)
        {
            PeriphClkCfg.periphClkMask = CLOCK_PERIPH_ACMP_MASK;
            PeriphClkCfg.acmpClock = ACMP_CLOCK_RTCCLK;
            PeriphClk_SetConfig(&PeriphClkCfg);
        }
    }
#endif
#endif

out:
    return ret;
}


#endif
/**
 * @brief LPower_Enter
 * @param LPower_AppMode_T     config AppMode
 * @return None.
 */
#if defined(CHIP_GM6611B)
void LPower_Enter(LPower_AppMode_T *pMode)
{
    if (pMode->LpowerMode == STOP_MODE)
    {
        LPower_SysClockDec();
        LPOWER_STOP_PENDING_CLEAR;
        LPOWER_STOPMODE_SET;
    }
    else if (pMode->LpowerMode == STOPDEEP_MODE)
    {
        LPower_SysClockDec();
        SysClock_Disable(SYSCLOCK_SOURCE_LOSC32K);
        LPOWER_STOPDEEP_PENDING_CLEAR;
        LPOWER_GPIOWKUP_PENDING_CLEAR;
        LPOWER_GPIOWKUP_CFG;
        LPOWER_STOPDEEPMODE_SET;
    }

}
#elif defined(CHIP_GM6621)||defined(CHIP_F6621B)
void LPower_Enter(LPower_AppMode_T *pMode)
{
    LPower_SysClockDec();
    if (pMode->LpowerMode == STOP_MODE)
    {
        if (pMode->EflashEn == EFLASH_Enable)
        {
            /*Enable eFlash deep-standby*/
            LPOWER_EFLASH_ENABLE;
        }
        else
        {
            /*Disable eFlash deep-standby*/
            LPOWER_EFLASH_DISABLE;
        }

        if (pMode->LcdAccessEn == LCD_ACCESS_Enable)
        {
            /*Enable LCD Access*/
            LPOWER_LCD_ENABLE;
        }
        else
        {
            /*Disable LCD Access*/
            LPOWER_LCD_DISABLE;
        }
        StopMode_Enter();
    }
#if defined(CHIP_F6621B)
    if (pMode->LpowerMode == STANDBY_MODE)
    {
        StandbyMode_Enter();
    }
#endif
}


#endif

/**
 * @brief LPower Exit
 * @param LPower_SysClk_T     recover from lpower mode Sys Clock config
 * @param LPower_AppMode_T     config AppMode
 * @return LPower exit result.
 */
#if defined(CHIP_GM6611B) || defined(CHIP_GM6621) || defined(CHIP_F6621B)
RET_CODE LPower_Exit(LPower_AppMode_T *pMode)
{
    RET_CODE ret = RET_OK;

#if defined(CHIP_GM6621)
    /*Clear D_LOAD_EN*/
    LPOWER_D_LOAD_CLEAR;
#endif
    if (pMode->LpowerMode == STOP_MODE)
    {
        SysTick_DelayUS(200);
        LPower_SysClockInc();

#if defined(CHIP_GM6621) || defined(CHIP_F6621B)
        if (LPOWER_STOP_FLAG)
        {
            ret = RET_STOP_OK;
            /*Wait interrupt happens*/
            LPOWER_STOP_FLAG_CLEAR;
            LPower_PR_DEBUG("Recover from Stop mode !\n");
        }
        else
        {
            ret = RET_STOP_FLAG_INCOMPATIBLE;

            LPower_PR_DEBUG("Recover by unknown reason !\n");
        }
#endif
    }
#if defined(CHIP_GM6611B)
    if (pMode->LpowerMode == STOPDEEP_MODE)
    {
        LPower_SysClockInc();
    }
#endif
    return ret;

}
#endif
#endif

