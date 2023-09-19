/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lpstm_demo.c
 *
 * @author        edward.zhu
 *
 * @version       1.0.0
 *
 * @date          2019/04/11
 *
 * @brief        sample code for lpstm module.
 *
 * @note
 *    2019/04/11, edward.zhu, V1.0.0
 *        Initial version.
 */
#include <stdio.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <drv/dev/lptim.h>
#include <drv/dev/rtc.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/


/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Static variables
*******************************************************************************/
LPTIM_Handle_T g_LPTIMHandle;

/*******************************************************************************
* Reference variables
*******************************************************************************/
#define PERIOD 7500

/*******************************************************************************
* Local function prototypes
*******************************************************************************/
static uint8_t cnt = 0;

void LPTIMCallback(LPTIM_Device_T *pLPTIM, uint32_t Event, unsigned long Param)
{
    static uint16_t dutyVal = 100;

    dutyVal += 100;

    if (dutyVal >= 7500)
    {
        dutyVal = 200;
    }

    if (Event & LPTIM_CMPM_FLAG)
    {
        printf("lptim cmp match\n");

        if (cnt == 0)
        {
            LPTIM->CFGR &= ~LPTIM_CFGR_PRELOAD;

            /* Load the period value in the autoreload register */
            LPTIM->ARR = 0xff00;

            /* Load the Timeout value in the compare register */
            LPTIM->CMP = 0xf000;
        }
        cnt = 1;
    }

    if (Event & LPTIM_IT_CMPOK)
    {
        printf("lptim cmp update ok\n");
    }

    if (Event & LPTIM_IT_ARROK)
    {
        printf("lptim period update ok\n");
    }

    if (Event & LPTIM_IT_ARRM)
    {
        printf("lptim over flow\n");
    }
}

static RET_CODE PINMUX_Config(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /**LPTIM GPIO Configuration
    PA0     ------> LPTIM_IN0
    PA1     ------> LPTIM_OUT
    PA2     ------> LPTIM_IN1
    */
    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;//GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA0_AF3_LPETMIN0;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_1;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA1_AF3_LPETMOUT;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA2_AF3_LPETMIN1;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA3_AF3_LPETMEXTTRIG;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
    return RET_OK;
}

/* Initialize LPTIM module */
static RET_CODE LPTIM_PWMInit(void)
{
    LPTIM_Config_T pInfo;

    memset(&pInfo, 0, sizeof(LPTIM_Config_T));

    pInfo.extClock = FALSE;
    pInfo.outputPolarity = LPTIM_OUTPUTPOLARITY_LOW;
    pInfo.counterSource = LPTIM_COUNTERSOURCE_INTERNAL;
    pInfo.updateMode = LPTIM_UPDATE_IMMEDIATE;//LPTIM_UPDATE_ENDOFPERIOD;
    pInfo.prescaler = LPTIM_PSC_DIV_8;
    pInfo.trigger.source = LPTIM_TRIGSOURCE_SOFTWARE;
    LPTIMCallback(LPTIM, LPTIM_IT_ARRM | LPTIM_IT_CMPOK, NULL);

    if (pInfo.extClock == TRUE)
    {
        pInfo.UltraLowPowerClock.polarity = LPTIM_CLOCKPOLARITY_RISING;
        pInfo.UltraLowPowerClock.sampleTime = LPTIM_CLOCKSAMPLETIME_4TRANSITIONS;//LPTIM_CLOCKSAMPLETIME_2TRANSITIONS;
    }

    if (pInfo.trigger.source != LPTIM_TRIGSOURCE_SOFTWARE)
    {
        pInfo.trigger.activeEdge = LPTIM_ACTIVEEDGE_RISING;//LPTIM_ACTIVEEDGE_FALLING;//LPTIM_ACTIVEEDGE_RISING;
        pInfo.trigger.sampleTime = LPTIM_TRIGSAMPLETIME_2TRANSITIONS;
    }

    LPTIM_Init(LPTIM, &pInfo, &g_LPTIMHandle);
    LPTIM_EnableInterrupts(LPTIM, LPTIM_IT_ARRM);
    LPTIM_EnableInterrupts(LPTIM, LPTIM_IT_CMPOK | LPTIM_IT_ARROK | LPTIM_IT_CMPM);

    return RET_OK;
}

int main(void)
{
    RET_CODE ret = RET_OK;

    SysTick_Init();

    DebugConsoleInit(19200);

    ret = PINMUX_Config();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    ret = LPTIM_PWMInit();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    LPTIM_StartPWM(LPTIM, 7500, 3750);

    while (1)
    {

    }
}



