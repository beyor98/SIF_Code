#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/acmp.h>
#include <drv/dev/adc.h>
#include <drv/dev/pes.h>

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


#define ADC_CONVERT_TIMEOUT     1000000    /* ADC Timeout 1 s */
#define ADC_CHANNEL_LENGTH      16         /* ADC Max Sample channel Num */
#define SEQUENCE_LENGTH         8          /* ADC Max Sequence Sample channel Num For One Time */

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device */
    uint32_t mode;                 /**< Specifies the operating mode for the selected pins */
    uint32_t pull;                 /**< Specifies the Pull-up or Pull-up X2 activation for the selected pins */
} Device_Pinmux_T;

static ADC_Handle_T s_Handle;
static uint32_t s_convertValue[SEQUENCE_LENGTH] = {0};
static ADC_ChannelIndex_T s_sequence[SEQUENCE_LENGTH] =
{
    ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, \
    ADC_CHANNEL_3, ADC_CHANNEL_4, ADC_CHANNEL_5, \
    ADC_CHANNEL_6, ADC_CHANNEL_7
};

static Device_Pinmux_T s_pinmux[ADC_CHANNEL_LENGTH] =
{
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B)||defined(CHIP_F6601C))
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
#endif
};

int main(void);
static RET_CODE ADC_ConfigPinmux(void);
static void ADC_Callback(ADC_Handle_T *pHander, ADC_Event_T event, unsigned long param);


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
    uint8_t i = 0;
    Device_Pinmux_T *pADCPinmux = NULL;

    for (i = 0; i < SEQUENCE_LENGTH ; i++)
    {
        pADCPinmux = &(s_pinmux[s_sequence[i]]);

        memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
        GPIO_InitStruct.pin =  pADCPinmux->pin;
        GPIO_InitStruct.mode = pADCPinmux->mode;
        GPIO_InitStruct.pull = pADCPinmux->pull;
        GPIO_Init(pADCPinmux->device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            PR_ERR("Err:ACMP_ConfigPinmux\n");
            goto out;
        }
    }
out:
    return ret;
}

int main(void)
{
    RET_CODE ret = RET_OK;
    ADC_CommonConfig_T commonConfig;
    ADC_SequenceConfig_T sequenceConfig;
    int i = 0;
#if 0
    ADC_CompareConfig_T compareConfig;
    PES_ChannelIndex_T chIndex;
#endif
    SysTick_Init();
    /* init for uart printf log */
    DebugConsoleInit(115200);
    printf("adc sequence sync demo\n");
    SysTick_DelayMS(2000);
    ret = ADC_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_ConfigPinmux\n", __FUNCTION__);
        goto out;
    }

    /* config adc common parameter */
    commonConfig.continuousMode = ADC_SINGLE_MODE;
    commonConfig.enableDma = DISABLE;
    commonConfig.callback = ADC_Callback;
    ret = ADC_Init(ADC_DEV, &commonConfig, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_Init\n", __FUNCTION__);
        goto out;
    }

    /* config ADC Sequence sample paramter */
    sequenceConfig.resolution = ADC_RESOLUTION_12BIT;
    sequenceConfig.sampleTime = ADC_SAMPLETIME_32CLK;
    sequenceConfig.pSequence = s_sequence;
    sequenceConfig.length = SEQUENCE_LENGTH;
    ret = ADC_SequenceConfig(&s_Handle, &sequenceConfig);
    if (ret != RET_OK)
    {
        PR_ERR("\r%s:Err ADC_SequenceConfig\n", __FUNCTION__);
        goto out;
    }

#if 0
    /* config ADC compare paramter */
    compareConfig.compareMode = ADC_COMPARE_WITHIN_RANGE;
    compareConfig.compareHighValue = 500;
    compareConfig.compareLowValue = 300;
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
        ret = ADC_ReadConvertValue(&s_Handle, s_convertValue, ADC_CONVERT_TIMEOUT);
        if (ret == RET_OK)
        {
            PR_DEBUG("\nADC Sequence Convert Success\n");
            for (i = 0; i < SEQUENCE_LENGTH; i++)
            {
                PR_DEBUG("\rIndexChannle:%d Vaule:%d\n", s_sequence[i], s_convertValue[i]);
            }
        }
        else
        {
            PR_ERR("Err:%d ADC_ReadConvertValue\n", ret);
            goto out;
        }

        SysTick_DelayMS(1000);

    }
out:
    return ret;
}



