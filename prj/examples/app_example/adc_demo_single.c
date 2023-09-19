/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file adc_demo.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/27
 *
 * @contents Verification and example code of Cyclic redundancy check (CRC).
 *
 * @history
 *     Date             By            Comments              Ver
 *  =========       =========        =========           =========
 *  2016/07/27      Mike Zheng        Create                   1.0.0
 *    2016/11/22         Mike.Zheng             Update For Release       1.1.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/adc.h>
#include <drv/dev/gpio.h>


/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif
#ifdef ERR_PRINTF
    #undef ERR_PRINTF
#endif
#ifdef WARNING_PRINTF
    #undef WARNING_PRINTF
#endif

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printf
#endif

#define ADC_CHANNEL_LENGTH      16
#define ADC_CONVERT_TIMEOUT     1000000    /* ADC Timeout 1 s */
/******************************************************************************
* Local types
******************************************************************************/
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device */
    uint32_t mode;                 /**< Specifies the operating mode for the selected pins */
    uint32_t pull;                 /**< Specifies the Pull-up or Pull-up X2 activation for the selected pins */
} Device_Pinmux_T;

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
static ADC_Handle_T s_Handle;
static uint32_t s_convertValue;
static Device_Pinmux_T s_pinmux[ADC_CHANNEL_LENGTH] =
{
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B))
    {GPIO_PIN_9,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_0 pinmux config paramter */
    {GPIO_PIN_10, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_1 pinmux config paramter */
    {GPIO_PIN_11, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_2 pinmux config paramter */
    {GPIO_PIN_12, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_3 pinmux config paramter */
    {GPIO_PIN_13, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_4 pinmux config paramter */
    {GPIO_PIN_14, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_5 pinmux config paramter */
    {GPIO_PIN_15, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_6 pinmux config paramter */
    {GPIO_PIN_0,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_7 pinmux config paramter */
    {GPIO_PIN_1,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_8 pinmux config paramter */
    {GPIO_PIN_2,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_9 pinmux config paramter */
    {GPIO_PIN_3,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_10 pinmux config paramter */
    {GPIO_PIN_4,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_11 pinmux config paramter */
    {GPIO_PIN_5,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_12 pinmux config paramter */
    {GPIO_PIN_6,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_13 pinmux config paramter */
    {GPIO_PIN_7,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_14 pinmux config paramter */
    {GPIO_PIN_8,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_15 pinmux config paramter */
#elif defined(CHIP_GM6611B)
    {GPIO_PIN_9,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_0 pinmux config paramter */
    {GPIO_PIN_8,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_1 pinmux config paramter */
    {GPIO_PIN_7,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_2 pinmux config paramter */
    {GPIO_PIN_6,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_3 pinmux config paramter */
    {GPIO_PIN_5,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_4 pinmux config paramter */
    {GPIO_PIN_4,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_5 pinmux config paramter */
    {GPIO_PIN_3,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_6 pinmux config paramter */
    {GPIO_PIN_2,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_7 pinmux config paramter */
    {GPIO_PIN_1,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_8 pinmux config paramter */
    {GPIO_PIN_0,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_9 pinmux config paramter */
    {GPIO_PIN_15, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_10 pinmux config paramter */
    {GPIO_PIN_14, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_11 pinmux config paramter */
    {GPIO_PIN_13, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_12 pinmux config paramter */
    {GPIO_PIN_12, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_13 pinmux config paramter */
    {GPIO_PIN_11, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_14 pinmux config paramter */
    {GPIO_PIN_10, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_15 pinmux config paramter */
#elif defined(CHIP_GM6611)
    {GPIO_PIN_9,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_0 pinmux config paramter */
    {GPIO_PIN_8,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_1 pinmux config paramter */
    {GPIO_PIN_7,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_2 pinmux config paramter */
    {GPIO_PIN_6,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_3 pinmux config paramter */
    {GPIO_PIN_5,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_4 pinmux config paramter */
    {GPIO_PIN_4,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_5 pinmux config paramter */
    {GPIO_PIN_3,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_6 pinmux config paramter */
    {GPIO_PIN_2,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_7 pinmux config paramter */
    {GPIO_PIN_1,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_8 pinmux config paramter */
    {GPIO_PIN_0,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_9 pinmux config paramter */
    {GPIO_PIN_15, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_10 pinmux config paramter */
    {GPIO_PIN_14, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_11 pinmux config paramter */
    {GPIO_PIN_13, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_12 pinmux config paramter */
    {GPIO_PIN_12, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_13 pinmux config paramter */
    {GPIO_PIN_11, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_14 pinmux config paramter */
    {GPIO_PIN_10, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_15 pinmux config paramter */
#elif defined(CHIP_GM6621)
    {GPIO_PIN_0,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_0 pinmux config paramter */
    {GPIO_PIN_1,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_1 pinmux config paramter */
    {GPIO_PIN_2,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_2 pinmux config paramter */
    {GPIO_PIN_3,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_3 pinmux config paramter */
    {GPIO_PIN_4,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_4 pinmux config paramter */
    {GPIO_PIN_7,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_7 pinmux config paramter */
    {GPIO_PIN_8,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_8 pinmux config paramter */
    {GPIO_PIN_9,  GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_9 pinmux config paramter */
    {GPIO_PIN_10, GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_10 pinmux config paramter */
    {GPIO_PIN_11, GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_11 pinmux config paramter */
    {GPIO_PIN_12, GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_12 pinmux config paramter */
    {GPIO_PIN_13, GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_13 pinmux config paramter */
    {GPIO_PIN_14, GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_14 pinmux config paramter */
    {GPIO_PIN_15, GPIOA, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_15 pinmux config paramter */
#endif
};

/******************************************************************************
* Local functions
******************************************************************************/
int main(void);
static RET_CODE ADC_ConfigPinmux(void);
static void ADC_Callback(ADC_Handle_T *pHander, ADC_Event_T event, unsigned long param);

/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/

static void ADC_Callback(ADC_Handle_T *pHander, ADC_Event_T event, unsigned long param)
{
    if (event == ADC_EVENT_COMPERE_MATCH)
    {
        PR_DEBUG("ADC_EVENT_COMPERE_MATCH\n");
    }
    else if (event == ADC_EVENT_OVERFLOW)
    {
        PR_WARNINR("ADC_EVENT_OVERFLOW\n");
    }

}

static RET_CODE ADC_ConfigPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    Device_Pinmux_T *pADCPinmux = &s_pinmux[ADC_CHANNEL_3];

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  pADCPinmux->pin;
    GPIO_InitStruct.mode = pADCPinmux->mode;
    GPIO_InitStruct.pull = pADCPinmux->pull;
    GPIO_Init(pADCPinmux->device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        PR_ERR("Err:ACMP_ConfigPinmux\n");
    }

    return ret;
}

int main(void)
{
    RET_CODE ret = RET_OK;
    ADC_CommonConfig_T commonConfig;
    ADC_ChannelConfig_T channleConfig;
#if defined(CHIP_GM6621)
    uint8_t ExternSequenceChannel[] = {ADC_CHANNEL_3};
#endif
    ADC_CompareConfig_T compareConfig;
#if defined(CHIP_GM6601) || defined(CHIP_GM6611B)
    /* remove complier waring for 6611/6601B */
    PeriphClk_Config_T clkConfig;
#endif
    SysTick_Init();
    /* init for uart printf log */
    DebugConsoleInit(115200);
    SysTick_DelayMS(2000);
    ret = ADC_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_ConfigPinmux\n", __FUNCTION__);
        goto out;
    }
#if defined(CHIP_GM6601)
    /* config ADC module clock to 512kHz */
    clkConfig.periphClkMask = CLOCK_PERIPH_ADC_MASK;
    clkConfig.adcClock = ADC_CLOCK_512KHZ;
    PeriphClk_SetConfig(&clkConfig);
#endif
#if defined(CHIP_GM6611B)
    memset(&clkConfig, 0, sizeof(PeriphClk_Config_T));
    clkConfig.periphClkMask = CLOCK_PERIPH_ADC_MASK;
    clkConfig.adcClock = ADC_CLOCK_8MHZ;
    PeriphClk_SetConfig(&clkConfig);
#endif
    /* config ADC common parameter */
    commonConfig.continuousMode = ADC_SINGLE_MODE;
    commonConfig.enableDma = DISABLE;
#if (defined(CHIP_GM6611) || defined(CHIP_GM6601B) ||  defined(CHIP_GM6611B))
    commonConfig.trigerSource = ADC_SW_TRIGER;
#elif defined(CHIP_GM6621)
    /*commonConfig.trigerSource = ADC_HW_TRIGER_PESCH0;*/
    commonConfig.trigerMode = ADC_SW_TRIGER;
#endif

    commonConfig.callback = ADC_Callback;
    ret = ADC_Init(ADC_DEV, &commonConfig, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_Init\n", __FUNCTION__);
        goto out;
    }

    /* config ADC channel sample paramter */
    channleConfig.resolution = ADC_RESOLUTION_12BIT;
    channleConfig.sampleTime = ADC_SAMPLETIME_32CLK;
#if (defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_GM6621))
    channleConfig.sampleEdge = ADC_SAMPLE_POSITIVE;
    channleConfig.DataFifoFullWrEn = DISABLE;
#endif
    channleConfig.channelIndex = ADC_CHANNEL_3;
    ret = ADC_ChannelConfig(&s_Handle, &channleConfig);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_ChannelSampleConfig\n", __FUNCTION__);
        goto out;
    }

#if defined(CHIP_GM6621)
    ADC_SelectExternChannel(&s_Handle, &ExternSequenceChannel[0], sizeof(ExternSequenceChannel));
#endif

#if 0
    /* config ADC compare paramter */
    compareConfig.compareMode = ADC_COMPARE_WITHIN_RANGE;
    compareConfig.compareHighValue = 800;
    compareConfig.compareLowValue = 600;
    ret = ADC_CompareConfig(&s_Handle, &compareConfig);
    if (ret != RET_OK)
    {
        PR_ERR("\r%s:Err ADC_CompareConfig\n", __FUNCTION__);
        goto out;
    }
    ADC_CompareModeEnable(&s_Handle);
#endif

    /* read adc convert result every 500ms*/
    while (1)
    {
        ret = ADC_ReadConvertValue(&s_Handle, &s_convertValue, ADC_CONVERT_TIMEOUT);
        if (ret == RET_OK)
        {
            PR_DEBUG("Channel:%d Convert Success Value:%d\n", channleConfig.channelIndex, s_convertValue);
        }
        else
        {
            PR_ERR("%s:Err:%d ADC_ReadConvertValue\n", __FUNCTION__, ret);
            goto out;
        }

        SysTick_DelayMS(1000);
    }

out:
    return ret;
}




