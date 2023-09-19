/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file        sysinit.c
 *
 * @author      wen.liu
 *
 * @version     1.0.0
 *
 * @date        2015/08/21
 *
 * @brief       System init routine.
 *
 * @note
 *    2016/05/17, wen.liu, V1.0.0
 *        Initial version.
 *
 *
 */
#include "common.h"

extern int main(void);

/**
 * @brief System initialization function
 *
 * @retval None
 *
 * This is a system initialization function which dedicated in Keil
 * others complier don't use it.
 * it is similar to start function
 */
void SystemInit(void)
{
    OSC_Config_T s_OscConfig;
    CLOCK_Config_T s_ClkConfig;

#if defined(CHIP_GM6611B)
    /* Disable the observed clock feature */
    SYSDEV->PINMUX[2] &= ~(1 << 8);
    /* Disable WDT counting in debug mode*/
    SYSDEV->DBGCTRL = 1 << 0;

#if defined(DISABLE_WDOG)
    WDOG_UNLOCK(WDOG_DEV);
    IO_BIT_CLEAR(WDOG_DEV->CTRL, WDOG_CTRL_IE);
    WDOG_LOCK(WDOG_DEV);
#endif

#endif

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))
#if (!defined(OSC_EXTERNAL_CLOCK))
#if (SYS_CLOCK == 8000000)
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock  = PLL_IRC_HSCLK;
    /* Enable PLL bypass mode */
    s_OscConfig.pllConfig.pllBypassEnable = 1;
    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use HIRC clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HIRC8M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
    /*
     * Set Flash lantency to 0 to ensure correct CPU instruction fetching
     * in case that CPU clock less than 25MHz
     */
    s_ClkConfig.flashLatency = FLASH_LATENCY_0;
    CLOCK_SetConfig(&s_ClkConfig);
#endif

#if (SYS_CLOCK == 48000000)
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    /* Disable PLL bypass mode */
    s_OscConfig.pllConfig.pllBypassEnable = 0;
    /* PLL reference clock is direved from internal high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_IRC_HSCLK;

    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use PLL clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
    /*
     * Set Flash lantency to 2 to ensure correct CPU instruction fetching
     * in case that CPU clock higher than 35MHz
     */
    s_ClkConfig.flashLatency = FLASH_LATENCY_2;
    CLOCK_SetConfig(&s_ClkConfig);
#endif

#else
#if (SYS_CLOCK == 8000000)
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;
    /* Enable PLL bypass mode */
    s_OscConfig.pllConfig.pllBypassEnable = 1;
    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use HOSC clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HOSC8M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
    /*
     * Set Flash lantency to 0 to ensure correct CPU instruction fetching
     * in case that CPU clock less than 25MHz
     */
    s_ClkConfig.flashLatency = FLASH_LATENCY_0;
    CLOCK_SetConfig(&s_ClkConfig);
#endif

#if (SYS_CLOCK == 48000000)
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    /* Disable PLL bypass mode */
    s_OscConfig.pllConfig.pllBypassEnable = 0;
    /* PLL reference clock is direved from external high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;

    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use PLL clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;
    /*
     * Set Flash lantency to 2 to ensure correct CPU instruction fetching
     * in case that CPU clock higher than 35MHz
     */
    s_ClkConfig.flashLatency = FLASH_LATENCY_2;
    CLOCK_SetConfig(&s_ClkConfig);
#endif
#endif
    /* Disable all pinmux setting by default */
    IO_BITS_CLEAR(SYSDEV->PINMUX[0], ((1 << 27) | (1 << 16)));
    IO_BITS_CLEAR(SYSDEV->PINMUX[1], ((1 << 7) | (1 << 6)));

    /* Enable flash acceleration for better CPU performance */
    IO_BIT_SET(FLASH->CTRL, FLASH_CTRL_AEN);
#endif

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#if (!defined(OSC_EXTERNAL_CLOCK))
#if (SYS_CLOCK == 8000000)
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock  = PLL_IRC_HSCLK;
    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use HIRC clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HIRC8M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
#endif

#if (SYS_CLOCK == 48000000)
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    /* PLL reference clock is direved from internal high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_IRC_HSCLK;

    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use PLL clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
#endif
#else
#if (SYS_CLOCK == 8000000)
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;
    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use HOSC clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HOSC8M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
#endif

#if (SYS_CLOCK == 48000000)
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    /* PLL reference clock is direved from external high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;

    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use PLL clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
#endif
#endif
    /* Disable all pinmux setting by default */
    IO_BITS_CLEAR(SYSDEV->PINMUX[0], ((1 << 27) | (1 << 16)));
    IO_BITS_CLEAR(SYSDEV->PINMUX[1], ((1 << 7) | (1 << 6)));

    /* Enable flash acceleration for better CPU performance */
    IO_BIT_SET(FLASH->CTRL, FLASH_CTRL_AEN);
#endif

    /* use gpio to simulate OSCLS 32K, and as such, RTC_Init should
       be called before any gpio operation */
#if defined (CHIP_GM6611)
#if defined (RTC_CLK_SWITCH)
    *(volatile uint32_t *)(0x40000048) |= (0x2 << 16);
    *(volatile uint32_t *)(0x40000094) |= (1 << 8);
    *(volatile uint32_t *)(0x400000A0) |= 0xF;
    *(volatile uint32_t *)(0x40000044) |= (0x1 << 29);
#endif
#endif

#if defined(CHIP_GM6621)
#if (!defined(OSC_EXTERNAL_CLOCK))
#if (SYS_CLOCK == 8000000)
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock  = PLL_IRC_HSCLK;
    OSC_SetConfig(&s_OscConfig);

	s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use HIRC clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HIRC8M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_0;

    CLOCK_SetConfig(&s_ClkConfig);
#endif

#if (SYS_CLOCK == 48000000)
    s_OscConfig.oscMask  = OSC_MASK_INTERNAL_HIGH;
    s_OscConfig.oscState = OSC_INTERNAL_HIGH_ON;
    /* PLL reference clock is direved from internal high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_IRC_HSCLK;

    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use PLL clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
#endif
#else
#if (SYS_CLOCK == 8000000)
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;
    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use HOSC clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_HOSC8M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
#endif
#if (SYS_CLOCK == 48000000)
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;
    /* PLL reference clock is direved from internal high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;

    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
    /* Use PLL clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
#endif
#endif

    /* Enable flash acceleration for better CPU performance */
    IO_BIT_SET(FLASH->CTRL, FLASH_CTRL_AEN);
#endif
    return;
}



