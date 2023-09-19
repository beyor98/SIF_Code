/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file cpu_clock.c
 *
 * @version 1.0.0
 *
 * @date 2015/08/21
 *
 * @contents Verification and example code of CPU clock.
 *
 * @history
 *	Date			   By			Comments		Ver.
 *	=========		============	=========	 =========
 *	2016/07/25		Wizephen.Wang	 Create		   1.0.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/clock.h>

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/

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

int main (void)
{
    uint8_t i = 0;
    OSC_Config_T s_OscConfig;
    CLOCK_Config_T s_ClkConfig;
#if defined(CHIP_GM6621)
	PeriphClk_Config_T pConfig;
#endif

    SysTick_Init();
    SysTick_DelayMS(1000);
    DebugConsoleInit(115200);

    /*
     * CPU clock switches to PLL 48MHz Output,
     * with OSC 8MHz as reference clock,
     * without prescaler
     **************************************/
    /* Use external oscillator */
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_HIGH;
    s_OscConfig.oscState = OSC_EXTERNAL_HIGH_ON;

#if defined(CHIP_GM6601)
    /* Disable PLL bypass mode */
    s_OscConfig.pllConfig.pllBypassEnable = 0;
#endif
    /* PLL reference clock is direved from external high oscillator */
    s_OscConfig.pllConfig.referenceClock  = PLL_OSC_HSCLK;

    OSC_SetConfig(&s_OscConfig);

    s_ClkConfig.clkMask = CLOCK_MASK_SYSCLK;
#if defined(CHIP_GM6601)
    /* Use PLL clock as CPU clock source */
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL;
#endif

#if defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_GM6621) || defined(CHIP_F6601C)
    s_ClkConfig.sysClkSource = SYSCLOCK_SOURCE_PLL48M;
#endif
    /* No prescaler for CPU clock */
    s_ClkConfig.sysClkPrescaler = CLOCK_DIVIDED_1;

    /*
     * Set Flash lantency to 2 to ensure correct CPU instruction fetching
     * in case that CPU clock is higher than 35MHz
     */
    s_ClkConfig.flashLatency = FLASH_LATENCY_2;

    CLOCK_SetConfig(&s_ClkConfig);
    while(i < 10)
    {
        printf("\r CPU works at a frequency of 48MHz \n");
        SysTick_DelayMS(500);
        i ++;
    }
	/* Enable LOSC */ 
#if defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_GM6621) || defined(CHIP_F6601C)
	SysClock_Enable(SYSCLOCK_SOURCE_LOSC32K);
#endif

#if defined(CHIP_GM6621)
	/* COnfigure peripheral clock */
	pConfig.periphClkMask = CLOCK_PERIPH_ACMP_MASK | CLOCK_PERIPH_LCD_MASK | CLOCK_PERIPH_LVD_MASK;
	pConfig.lvdClock = LVD_CLOCK_LOSC;
	pConfig.acmpClock = ACMP_CLOCK_LIRC;
	pConfig.lcd_config_t.cpClock = LCD_CLOCK_CP_LOSC;
	pConfig.lcd_config_t.lcdClock = LCD_CLOCK_LOSC;
	PeriphClk_SetConfig(&pConfig);
#endif

    while (1)
    {
        __asm ("nop");
    }
}
