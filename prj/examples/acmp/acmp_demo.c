/*====================================================================
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file Acmp_demo.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/05
 *
 * @contents Verification and example code of ACMP Output
 *
 * @history
 *     Date             By            Comments              Ver
 *  =========       =========        =========           =========
 *  2016/07/05      Mike Zheng        Create                     1.0.0
 *    2016/11/22         Mike.Zheng             Update For Release       1.1.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/acmp.h>
#include <string.h>
#if defined(CHIP_GM6621)
    #include <drv/dev/rtc.h>
    #include <drv/dev/pes.h>
#endif

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do {} while (0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{} while (0)
#else
    #define PR_WARNINR         printf
#endif

#define DAC_STEP_VALUE         30

#define RTC_TRIGER_ACMP_STOP_MODE

#if  defined(CHIP_GM6621)
#define HIRC_CLOCK_DISABLE  (*(volatile uint32_t *)(0x40039090) |= 1 << 0)
#define HOSC_CLOCK_DISABLE  (*(volatile uint32_t *)(0x40000094) &= ~(1 << 0))
#define LIRC_CLOCK_DISABLE  (*(volatile uint32_t *)(0x40039090) |= 1 << 16)
#define LOSC_CLOCK_DISABLE  (*(volatile uint32_t *)(0x400390A0) &= ~(1 << 0))
#define PLL_CLOCK_DISABLE   (*(volatile uint32_t *)(0x40000098) &= ~(1 << 0))

#define HIRC_CLOCK_ENABLE   (*(volatile uint32_t *)(0x40039090) &= ~(1 << 0))
#define HOSC_CLOCK_ENABLE   (*(volatile uint32_t *)(0x40000094) |= 1 << 0)
#define LIRC_CLOCK_ENABLE   (*(volatile uint32_t *)(0x40039090) &= ~(1 << 16))
#define LOSC_CLOCK_ENABLE   (*(volatile uint32_t *)(0x400390A0) |= 1 << 0)
#define PLL_CLOCK_ENABLE    (*(volatile uint32_t *)(0x40000098) |= 1 << 0)

#define HOSC_CLOCK_XINOUT_ENABLE (*(volatile uint32_t *)(0x40000094) |= 1 << 16)
#define LOSC_CLOCK_XINOUT_ENABLE (*(volatile uint32_t *)(0x400390A0) |= 1 << 16)

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))
const uint32_t backvar[6] = {0x12345678, 0x87654321, 0x5aa5a55a, 0xa55a5aa5, 0x56808724, 0x9ef0dbca};
uint32_t rdvar[6] = {0x0};
#endif
/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
static ACMP_Handle_T s_Handle;

/******************************************************************************
* Local functions
******************************************************************************/
static void ACMP_Callback(ACMP_Handle_T *pHandle, ACMP_Event_T event, unsigned long Param);
void *memcpy(void *dest, const void *src, unsigned n);
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B))
    static RET_CODE ACMP_ConfigPinmux(void);
    static RET_CODE ACMP_ConfigOutputPinmux(void);
#endif
int main(void);

#if defined(CHIP_GM6621)
    static ACMP_Device_T *gDevice = ACMP_DEV;
#endif

/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/
static void ACMP_Callback(ACMP_Handle_T *pHandle, ACMP_Event_T event, unsigned long Param)
{
    if (event == ACMP_MATCH_EVENT)
    {
        PR_DEBUG("\rnegativePin and positivePin voltage match the trigger mode\n");
    }
}

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) ||defined(CHIP_F6601C))
static RET_CODE ACMP_ConfigPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))

    /*set digtal pin as input , inorde to not affect analog input*/
    pGPIO = GPIOD;
    GPIO_InitStruct.pin  = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
#elif defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    pGPIO = GPIOB;
    GPIO_InitStruct.pin  = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
#endif
    ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        PR_ERR("Err:ACMP_ConfigPinmux\n");
    }

    /*seletc analog pin as input*/
    ACMP_AMA0_PIN_ENABLE();

    return ret;
}

static RET_CODE ACMP_ConfigOutputPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /* config pinmux as acmp output pin */

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))

    pGPIO = GPIOD;
    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD8_AF0_ACMP;
#elif defined(CHIP_GM6611) || defined(CHIP_GM6611B)
    pGPIO = GPIOA;
    GPIO_InitStruct.pin = GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA15_AF0_ACMP;
#endif
    ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        goto out;
    }

out:
    return ret;
}
#endif

#if defined(CHIP_GM6621)
static RET_CODE  ACMP_ConfigOutputPinmuxExt(GPIO_Device_T *pGPIO, GPIO_PinConfig_T GPIO_InitStructOut)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /* Configure GPIOA10 as output pin */
    GPIO_InitStruct.pin  = GPIO_InitStructOut.pin ;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA10_AF6_ACMP0;
    ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
}

static RET_CODE  ACMP_GPIOPinMuxConfigExt(GPIO_Device_T *pGPIO, GPIO_PinConfig_T GPIO_InitStructPins)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /* Configure GPIOA as input analog  pin */
    GPIO_InitStruct.pin  = GPIO_InitStructPins.pin;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
}

void rtc_callback(int i)
{
    printf("rtc_callback i=[%d] \n", i);

    return ;
}
#if defined(CHIP_GM6621)
void Configure_CPU_CLKSource(uint8_t clkSrc)
{
    MODIFY_REG(*(volatile uint32_t *)(0x40000040), 0x07, clkSrc);
}

void Configure_PLL_ReferClk(uint8_t ReferClk)
{
    MODIFY_REG(*(volatile uint32_t *)(0x40000098), 0x3 << 1, ReferClk << 1);
}

static void delay_ms(void)
{
    uint32_t i, j;
    for (i = 0; i < 100; i ++)
    {
        for (j = 0; j < 5000; j ++);
    }
}
#endif

void EnterStopMode(void)
{
    Timer_T timer;
    RTC_Config_T rtcConfig;
    timer.cycle.tv_sec  = 1;
    timer.cycle.tv_msec = 0;
    rtcConfig.clock         = RTC_CLOCK_IRCLS; /*RTC_CLOCK_OSCLS*/
    rtcConfig.alarmCallback = NULL;
    rtcConfig.timerCallback = rtc_callback;
    rtcConfig.counterCallback = NULL;
    MODIFY_REG(*(volatile uint32_t *)(0x40039044), ((unsigned int)0x07 << 29), (unsigned int)2 << 29U);
    LOSC_CLOCK_ENABLE;
    *(volatile uint32_t *)(0x40039020) &= ~(1 << 16);
    *(volatile uint32_t *)(0x40039020) |= (1 << 16);

    /*a. Switch clock to HIRC*/
    Configure_CPU_CLKSource(SYSCLOCK_SOURCE_HIRC8M);
    /*Power down PLL & HOSC*/
    HOSC_CLOCK_DISABLE;
    PLL_CLOCK_DISABLE;

    RTC_Init(&rtcConfig);
    /* RTC works now */
    RTC_Start();

    /*Now CPU works under 8MHz*/
    IO_BITMASK_SET(UART0->TC, UART_TC_SCLK, UART_TC_SCLK_FIELD(8000000 / (19200 << 4)));

    /*LVD3 disabled*/
    *(volatile uint32_t *)(0x400390B0) &= ~(1 << 0); // LVD3

    /*b. Optional, Gate RTC, WDT, LCD, ACMP*/
    AOPDEV->CSR2 |= ((unsigned int)1 << 31u);        /*ACMP*/

    AOPDEV->CSR1 |= (1 << 30u); /*RTC*/

    /* AOPDEV->CSR2 |= (1 << 7u); LCD*/
    AOPDEV->CSR2 &= ~(1 << 7u);
    AOPDEV->CSR2 |= (0 << 7u);
    /*c. Optional, Enable eFlash deep-standby*/
    AOPDEV->PMC |= 1 << 3;
    /*d. Optional, Enable LCD Access*/
    AOPDEV->PMC |= 1 << 6;
    /*e. Set Stop mode & reset Standby mode*/
    MODIFY_REG(AOPDEV->PMC, 0x7, 0x3);
    /*f. Enable Sleep Deep for Cortex-M0*/
    SCB->SCR |= (1UL << 2);
    /*g. Set D_LDO_SEL to normal LDO voltage range */

    /*h. Set LDO_HW_TRIM*/
    AOPDEV->PMC |= 1 << 5;

    /*i. Set D_LOAD_EN*/
    AOPDEV->PMU |= 1 << 0;
    /*Wait at least 10us*/
    SysTick_DelayMS(1);

    printf("reset addr[0x40039020]  = 0x%x \n", *(volatile uint32_t *)(0x40039020));
    printf("be ready for enter stop\n");
    /*Test backup register*/
    /*memcpy(AOPDEV->BAK, backvar, 24);*/
    memcpy((unsigned int *)0x40039170, backvar, 24);
    RTC_TriggerACMPEnable();
    RTC_DisableWakeupIrq(4);
    RTC_StartTimer(&timer);
    RTC_DisableWakeupIrq(4);
    /*j. wfi*/
    __WFI();
    delay_ms();
    if (AOPDEV->SRS & 1 << 30)
    {
        AOPDEV->SRS |= 1 << 30;
        /*Wait interrupt happens*/
        printf("Recover from Stop mode \n");
    }
    else
    {
        printf("Recover by unknown reason \n");
    }

    /*Check backup register*/
    /*memcpy(rdvar, AOPDEV->BAK, 24);*/
    memcpy(rdvar, (unsigned int *)0x40039170, 24);

    if (memcmp(rdvar, backvar, 24) != 0)
    {
        printf("Compare error \n");
    }
    else
    {
        printf("Compare OK \n");
    }
    /*clear D_LOAD_EN*/
    AOPDEV->PMU &= ~(1 << 0);
    /*FCLK/SCLK/HCLK will be gated*/

    printf("Test pass ... \n");
    delay_ms();
    SysTick_DelayMS(1000);
}
#endif

int main(void)
{
    RET_CODE ret = RET_OK;
    ACMP_Config_T config;
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    ACMP_CompareResult_T CompareResult;
#endif
#if defined(CHIP_GM6621)
    GPIO_PinConfig_T GPIO_InitStructPins;
    GPIO_PinConfig_T GPIO_InitStructOut;
    static unsigned int outputVal = 0;
    RTC_Config_T rtcConfig;
    Timer_T timer;
#endif

    SysTick_Init();
    /* init for uart printf log */

    DebugConsoleInit(19200);
#if defined(CHIP_GM6621)
    AOPDEV->PMU &= ~(1 << 0);
#endif
    PR_DEBUG("ACMP Demo Test Start\n");
    SysTick_DelayMS(2000);

    memset(&config, 0, sizeof(ACMP_Config_T));
    config.callback = ACMP_Callback;

#if defined(CHIP_GM6621)
    /*ACMP battery mode,voltage detect only support ACMP_DEV_0 */
    config.devIndex = ACMP_DEV_0;
    config.batteryEn = ENABLE;
    config.standby_flag = 0;
    config.trigerSource = ACMP_TRI_SEL_RTC;
    rtcConfig.clock = RTC_CLOCK_IRCLS;/*RTC_CLOCK_IRCHS*/
    rtcConfig.alarmCallback = NULL;
    rtcConfig.timerCallback = rtc_callback;
    rtcConfig.counterCallback = NULL;
    RTC_Init(&rtcConfig);
    RTC_Start();
    timer.cycle.tv_sec  = 1;
    timer.cycle.tv_msec = 0;
    RTC_StartTimer(&timer);

#endif

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
#if defined(CHIP_GM6601B)
    config.outputInversionEnable = DISABLE;
#endif

#if (!defined(CHIP_GM6601))
    config.trigerAdcEnable = DISABLE;
    config.filterThr = 0x10;
#endif
    config.dacConfig.dacEnbale = ENABLE;
    config.dacConfig.dacValue = DAC_STEP_VALUE;
		#if !defined(CHIP_F6601C)
		    config.dacConfig.referVoltage = DAC_VDD50;
		#endif
    config.negativePin = ANALOG_COMPARE_DAC;//ANALOG_COMPARE_DAC  ANALOG_COMPARE_PIN0
    config.positivePin = ANALOG_COMPARE_PIN1;
    config.outputPinEnable = ENABLE;
    config.triggerMode = ANALOG_MATCH_FALLINGRISING;

    ret = ACMP_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:%d ACMP_ConfigPinmux\n", __FUNCTION__, ret);
        goto out;
    }

    /* if use outputpin as output , must config pinmux first */
    if (config.outputPinEnable == ENABLE)
    {
        ret = ACMP_ConfigOutputPinmux();
        if (ret != RET_OK)
        {
            PR_ERR("%s:ACMP Config Pinmux Error ret:%d\n", __FUNCTION__, ret);
            goto out;
        }
    }
#endif

#if defined(CHIP_GM6621)

    if (config.devIndex == ACMP_DEV_0)
    {
        GPIO_InitStructPins.pin = GPIO_PIN_4 | GPIO_PIN_7; /*XPA4,XPA7*/
    }
    else
    {
        GPIO_InitStructPins.pin = GPIO_PIN_7 | GPIO_PIN_9;/*XPA7,XPA9*/
    }
    ret = ACMP_GPIOPinMuxConfigExt(GPIOA, GPIO_InitStructPins);
    if (ret != RET_OK)
    {
        PR_DEBUG("Err:ACMP_GPIOPinMuxConfigExt\n");
        return ret;
    }

    if (config.devIndex == ACMP_DEV_0)
    {
        GPIO_InitStructOut.pin = GPIO_PIN_10;
    }
    else
    {
        GPIO_InitStructOut.pin = GPIO_PIN_11;
    }
    ret = ACMP_ConfigOutputPinmuxExt(GPIOA, GPIO_InitStructOut);
    if (ret != RET_OK)
    {
        PR_DEBUG("Err:ACMP_ConfigOutputPinmuxExt\n");
        return ret;
    }
#endif

    ret = ACMP_Init(ACMP_DEV, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:ACMP_Init Error ret:%d\n", __FUNCTION__, ret);
        goto out;
    }

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    /* Get Acmp Compare Result */
		while(1)
		{
			SysTick_DelayMS(1000);
      printf("wait voltage compare triger inpterrupt..\n");
			CompareResult = ACMP_GetCompareRestult(&s_Handle);
			if (CompareResult == ACMP_LOW)
			{
					PR_DEBUG("\rpositivePin voltage is less than negativePin voltage\n");
			}
			else
			{
					PR_DEBUG("\rpositivePin voltage is more than negativePin voltage\n");
			}
		}
#elif defined(CHIP_GM6621)
    while (1)
    {
        if (config.batteryEn == ENABLE && config.trigerSource == ACMP_TRI_SEL_NONE)
        {
            if (config.devIndex == ACMP_DEV_0)
            {
                ACMP_BV_SAMPLE_TRIG(gDevice);
            }
            else
            {
                printf("acmp1 not support  voltage detect \n");
            }
        }
        else if (config.batteryEn == ENABLE && config.trigerSource == ACMP_TRI_SEL_RTC)
        {
            if (config.devIndex == ACMP_DEV_0)
            {

#if defined(RTC_TRIGER_ACMP_STOP_MODE)
                while (1)
                {
                    EnterStopMode();
                    printf("\r\n from stop BV STATUS =0x%x\r\n", ACMP_DEV->BVCS);
                    SysTick_DelayMS(1000);
                }
#else
                printf("RTC_TRIGER_ACMP_NORMAL_MODE\r\n");
                /*RTC_TRIGER_ACMP_NORMAL_MODE*/
                RTC_TriggerACMPEnable();
                RTC_DisableWakeupIrq(4);
                while (1);
#endif
            }
            else
            {
                printf("acmp1 not support  voltage detect \n");
            }
        }
        else if (config.trigerSource == ACMP_TRI_SEL_CPU)
        {
            if (config.devIndex == ACMP_DEV_0)
            {
                ACMP_AC0_SAM_TRIG(gDevice);
                outputVal = ACMP_AC0_COMPARE_RESULT(gDevice);
            }
            else
            {
                ACMP_AC1_SAM_TRIG(gDevice);
                outputVal = ACMP_AC1_COMPARE_RESULT(gDevice);
            }
        }
        else
        {
            if (config.devIndex == ACMP_DEV_0)
            {
                outputVal = ACMP_AC0_COMPARE_RESULT(gDevice);
            }
            else
            {
                outputVal = ACMP_AC1_COMPARE_RESULT(gDevice);
            }
            PR_DEBUG("expect real output:%d\n", outputVal);

        }
        SysTick_DelayMS(100);
    }
#endif

out:

    return ret;
}
