/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          etm_pwm_example.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/06/29
 *
 * @brief        Sample code for etm module.
 *
 * @note
 *    2016/06/29, mark.lee, V1.0.0
 *        Initial version.
 *    2019/04/25, edward.zhu, V1.1.0
 *    2021/12/01, chunyuan.su, V1.1.1
 */
#include <stdio.h>

#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <drv/dev/pes.h>
#include <drv/dev/adc.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define PERIOD       7500
#if defined(CHIP_GM6601B) || defined(CHIP_F6601C)
	#define CLK_8M       0
	#define CLK_48M      1
#elif defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_GM6621)
	#define CLK_FCLK_DIV_1      0
	#define CLK_FCLK_DIV_2      1
	#define CLK_FCLK_DIV_3      2
	#define CLK_FCLK_DIV_4      3
	#define CLK_FCLK_DIV_5      4
	#define CLK_FCLK_DIV_6      5
	#define CLK_FCLK_DIV_7      6
	#define CLK_FCLK_DIV_8      7
#endif

#define PR_DEBUG(...)       do{}while(0)
#define ADC_CONVERT_TIMEOUT     1000000    // ADC Timeout 1 s
#define SEQUENCE_LENGTH         3          // ADC Max Sequence Sample channel Num For One Time 
#define ADC_CHANNEL_LENGTH      8          // ADC Max Sample channel Num 

#define ETM_CAPTURE
#define ETM_Trigger_ADC
#define ETM_Breakin

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

/*******************************************************************************
* Global variables
*******************************************************************************/
ETM_Handle_T g_PWMHandle;
ETM_Handle_T g_CaptureHandle;
uint32_t s_period = 0;

/*******************************************************************************
* Static variables
*******************************************************************************/
static uint8_t s_overflowCnt = 0;
static uint8_t s_interruptCnt = 0;

static uint16_t s_oldCaptureValue = 0;

#if (defined(CHIP_GM6621))
int s_chIndex = 0;
static PES_MasterSource_T s_masterSource;
static ADC_Handle_T s_Handle;
static uint32_t s_convertValue[SEQUENCE_LENGTH] = {0};

static ADC_ChannelIndex_T s_sequence[SEQUENCE_LENGTH] =
{
    ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_12
};

static Device_Pinmux_T s_pinmux[ADC_CHANNEL_LENGTH] =
{
    {GPIO_PIN_9,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, // ADC_CHANNEL_0 pinmux config paramter
    {GPIO_PIN_8,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, // ADC_CHANNEL_1 pinmux config paramter
    {GPIO_PIN_13, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, // ADC_CHANNEL_12 pinmux config paramter
};
#endif

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/
void ETM0Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
    static uint16_t dutyVal = 100;

    dutyVal += 100;

    if (dutyVal >= PERIOD)
    {
        dutyVal = PERIOD - 1;
    }

    /* Modify comparator in shadow mode */
    ETM_SetPWMDuty(ETM0, TRUE, dutyVal, 0);
}

#ifdef ETM_CAPTURE
void ETM1Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
    uint16_t value = 0;

    if (Event & ETM_TBOVFINT_FLAG)
    {
        s_overflowCnt++;
    }

    if (Event & ETM_CH1PEINT_FLAG)
    {
        s_interruptCnt++;
        value = ETM_GetCaptureValue(ETM1, ETM_CHANNEL_1);

        if (s_interruptCnt >= 2)
        {
            /* Measure the time elapsed between two rising edges of PWM signal */
            if (value < s_oldCaptureValue)
            {
                s_period = ((0xffff - s_oldCaptureValue) + value) + 1;
            }
            else
            {
                s_period =  value - s_oldCaptureValue;
            }

            s_overflowCnt = 0;
            s_interruptCnt = 0;
            s_oldCaptureValue = 0;
        }
        else
        {
            s_oldCaptureValue = value;
        }

    }
}
#endif

#ifdef ETM_Trigger_ADC
void ADC_Callback(ADC_Handle_T *pHander, ADC_Event_T event, unsigned long param)
{
    if (event == ADC_EVENT_COMPERE_MATCH)
    {
        PR_DEBUG("ADC_EVENT_COMPERE_MATCH\n");
    }
    else if (event == ADC_EVENT_OVERFLOW)
    {
        PR_DEBUG("ADC_EVENT_OVERFLOW\n");
    }
}
#endif

static RET_CODE PINMUX_Config(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

#if (defined(CHIP_GM6611B))
    /** ETM0 GPIO Configuration
    PD5     ------> ETM0_CH1
    PD4     ------> ETM0_CH2
    */
    GPIO_InitStruct.pin = GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD5_AF1_ETM0CH1;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD4_AF0_ETM0CH2;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** ETM1 GPIO Configuration
    PD7     ------> ETM1_CH1
    PD8     ------> ETM1_CH2
    */
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD7_AF1_ETM1CH1;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }


    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD8_AF1_ETM1CH2;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }


    /** ETM2 GPIO Configuration
    PE6     ------> ETM2_CH1
    PE7     ------> ETM2_CH2
    */
    GPIO_InitStruct.pin = GPIO_PIN_6;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOE6_AF1_ETM2CH1;
    ret = GPIO_Init(GPIOE, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOE7_AF1_ETM2CH2;
    ret = GPIO_Init(GPIOE, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** ETM3 GPIO Configuration
    PA11     ------> ETM3_CH1
    PA12     ------> ETM3_CH2
    */
    GPIO_InitStruct.pin = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA11_AF0_ETM3CH1;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_12;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA12_AF0_ETM3CH2;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
#endif

#ifdef ETM_BreakIn
    GPIO_InitStruct.pin = GPIO_PIN_10;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_PinWrite(GPIOB10, GPIO_PIN_RESET);
#endif

#if (defined CHIP_GM6621)
    /** ETM2 Pinmux Configuration
    PA4     ------> ETM0_CH1
    PA5     ------> ETM0_CH2
    PA6     ------> ETM0_CH3
    PA7     ------> ETM0_CH4
    PA9     ------> ETM0_ETR
    */
    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA4_AF4_ETM2CH0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_12;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC12_AF3_ETM2CH1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB0_AF4_ETM2CH2;
    GPIO_Init(GPIOB, &GPIO_InitStruct);


    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA7_AF4_ETM2CH3;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA9_AF4_ETM2EXTCLK;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    /** ETM1 Pinmux Configuration
    PB7     ------> ETM1_CH1
    PB8     ------> ETM1_CH2
    PB9     ------> ETM1_ETR
    */
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB7_AF3_ETM21CH0;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB8_AF3_ETM21CH1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_13;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB13_AF3_ETM21EXTCLK;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    /** ETM2 Pinmux Configuration
    PB10     ------> ETM2_CH1
    PB11     ------> ETM2_CH2
    PB12     ------> ETM2_ETR
    */
    GPIO_InitStruct.pin = GPIO_PIN_10;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB10_AF3_ETM22CH0;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB11_AF3_ETM22CH1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_12;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB12_AF3_ETM22EXTCLK;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

#if defined(CHIP_GM6601B) || defined(CHIP_F6601C)
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC7_AF0_ETM0CH1;//GPIOC13_AF1_ETM0CH1
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC8_AF0_ETM0CH2;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC5_AF0_ETM0EXTCLK;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_6;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC6_AF0_ETM0TCIN;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB3_AF0_ETM1CH1;//GPIOC14_AF1_ETM1CH1
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB4_AF0_ETM1CH2;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_1;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB1_AF1_ETM1EXTCLK;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB2_AF1_ETM1TCIN;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC15_AF0_ETM2CH1;//GPIOC11_AF1_ETM2CH1
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD0_AF0_ETM2CH2;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_13;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC13_AF1_ETM2EXTCLK;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_14;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC14_AF1_ETM2TCIN;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA7_AF0_ETM3CH1;//GPIOC12_AF1_ETM3CH1
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA9_AF0_ETM3CH2;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD8_AF1_ETM3EXTCLK;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD9_AF1_ETM3TCIN;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC11_AF0_ETM4CH1;//GPIOC15_AF1_ETM4CH1
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC9_AF1_ETM4EXTCLK;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_10;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC10_AF1_ETM4TCIN;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_12;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC12_AF0_ETM5CH1;//GPIOD0_AF1_ETM5CH1
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD4_AF1_ETM5EXTCLK;
    GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.pin = GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD5_AF1_ETM5TCIN;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif

    return RET_OK;
}

#if (defined(CHIP_GM6621))
#ifdef ETM_Trigger_ADC
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
            PR_DEBUG("Err:ACMP_ConfigPinmux\n");
            goto out;
        }
    }
out:
    return ret;
}
#endif
#endif

/* Generating edge-aligned PWM waveforms */
static RET_CODE ETM_PWMGeneratorByEdgeAlignedMode(void)
{
    ETM_Config_T etmInfo;
    ETM_PWMConfig_T pwmOutputInfo;
    RET_CODE ret = RET_OK;

    memset(&etmInfo, 0, sizeof(ETM_Config_T));

    /* ETM device configuration */
    etmInfo.extClock = FALSE;
    etmInfo.prescalerEn = TRUE;
    etmInfo.prescaler = 7;
    etmInfo.countMode = ETM_COUNT_UP;
    etmInfo.deadTime = 0;
    etmInfo.period = PERIOD;
    ETM0Callback(ETM0, ETM_TBOVFINT_FLAG, NULL);

    ret = ETM_InitTimeBase(ETM0, &etmInfo, &g_PWMHandle);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Sets comparator that monitor the value of the counter */
    ETM_SetPWMDuty(ETM0, TRUE, PERIOD / 2, 375);

    memset(&pwmOutputInfo, 0, sizeof(ETM_PWMConfig_T));

    /* For each event, the effect on each output PWM signal is programmable:
     * it can be left alone (ignoring the event), it can be toggled,
     * it can be driven Low, or it can be driven High.
     */
    pwmOutputInfo.idleState = ETM_IDLESTATE_LOW;
    pwmOutputInfo.counterStart = ETM_UNCHANGED;
    pwmOutputInfo.counterDown2 = ETM_UNCHANGED;
    pwmOutputInfo.counterUp2 = ETM_UNCHANGED;
    pwmOutputInfo.counterDown1 = ETM_UNCHANGED;
    pwmOutputInfo.counterUp1 = ETM_LEVEL_TOGGLE;
    pwmOutputInfo.overFlow = ETM_LEVEL_TOGGLE;
    pwmOutputInfo.underFlow = ETM_UNCHANGED;

    ETM_SetPWMChannel(ETM0, ETM_CHANNEL_1, &pwmOutputInfo);

    ETM_SetPWMChannel(ETM0, ETM_CHANNEL_2, &pwmOutputInfo);

    ETM_EnableInterrupts(ETM0, ETM_INT_CNT_OVERFLOW);

#if (defined(CHIP_GM6621))
#ifdef ETM_Breakin
    ETM_SetTriggerMode(ETM0, ETM_PWM_TRG_START_STOP, TRUE);
#endif
#endif

    /* Start PWM generator */
    ret = ETM_StartPWM(ETM0);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

/* Generating two center-aligned PWM waveforms which frequency is 16kHZ */
static RET_CODE ETM_PWMGeneratorByCenterAlignedMode(void)
{
    ETM_Config_T etmInfo;
    ETM_PWMConfig_T pwmOutputInfo;
    RET_CODE ret = RET_OK;

    memset(&etmInfo, 0, sizeof(ETM_Config_T));

    /* ETM device configuration */
    etmInfo.extClock = FALSE;
    etmInfo.prescalerEn = FALSE;
    etmInfo.prescaler = 1;
    etmInfo.countMode = ETM_COUNT_UPDOWN;
    etmInfo.deadTime = 0;
    etmInfo.period = 1500;
    etmInfo.callback = NULL;

    /* If don't need handle ISR callback, set handle structure to NULL */
    ret = ETM_InitTimeBase(ETM2, &etmInfo, NULL);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Sets comparator that monitor the value of the counter */
    ETM_SetPWMDuty(ETM2, TRUE, 750, 375);

    memset(&pwmOutputInfo, 0, sizeof(ETM_PWMConfig_T));

    /* ETM2 Channel1 pin idle output polarity setting */
    pwmOutputInfo.idleState = ETM_IDLESTATE_LOW;

    /* For each event, the effect on each output PWM signal is programmable:
     * it can be left alone (ignoring the event), it can be toggled,
     * it can be driven Low, or it can be driven High.
     */
    pwmOutputInfo.counterStart = ETM_UNCHANGED;
    pwmOutputInfo.counterDown2 = ETM_UNCHANGED;
    pwmOutputInfo.counterUp2 = ETM_UNCHANGED;
    pwmOutputInfo.counterDown1 = ETM_LEVEL_TOGGLE;
    pwmOutputInfo.counterUp1 = ETM_LEVEL_TOGGLE;
    pwmOutputInfo.overFlow = ETM_UNCHANGED;
    pwmOutputInfo.underFlow = ETM_UNCHANGED;

    ETM_SetPWMChannel(ETM2, ETM_CHANNEL_1, &pwmOutputInfo);

    /* ETM2 Channel2 pin idle output polarity setting */
    pwmOutputInfo.idleState = ETM_IDLESTATE_HIGH;
    ETM_SetPWMChannel(ETM2, ETM_CHANNEL_2, &pwmOutputInfo);

#if (defined(CHIP_GM6621))
#ifdef ETM_Breakin
    ETM_SetTriggerMode(ETM0, ETM_PWM_TRG_START_STOP, TRUE);
#endif
#endif

    /* Start PWM generator */
    ret = ETM_StartPWM(ETM2);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

#ifdef ETM_CAPTURE
/* Measure the time elapsed between two rising edges of ETM0 channel 1 PWM signal */
static RET_CODE ETM_CaptureFunc(void)
{
    ETM_Config_T etmInfo;
    RET_CODE ret = RET_OK;

    memset(&etmInfo, 0, sizeof(ETM_Config_T));

    etmInfo.extClock = FALSE;
    etmInfo.prescalerEn = TRUE;
    etmInfo.prescaler = 7;
    etmInfo.callback = ETM1Callback;
    ret = ETM_InitCapture(ETM1, &etmInfo, &g_CaptureHandle);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Enable interrupts */
    ETM_EnableInterrupts(ETM1, ETM_INT_EDGE_P1 | ETM_INT_CNT_OVERFLOW);

    s_overflowCnt = 0;
    s_interruptCnt = 0;
    s_oldCaptureValue = 0;

    ret = ETM_StartCapture(ETM1);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}
#endif

#if (defined(CHIP_GM6621))
#ifdef ETM_Trigger_ADC
static void ADC_PesTrigerSrcInit(ADC_Handle_T *pHander)
{
    static int i = 0;

    if (s_masterSource == PES_MASTER_ETM20)
    {
        printf("PES_MASTER_ETM20 ADC_PesCallback\n");
        if (i == 0)
        {
            /*ETM0 As Master generate Event*/
            ETM_asMasterConfig(ETM0);
            i++;
        }
    }
    else
    {
        printf("Warning ,you need select pes triger source\n");
    }
}

static RET_CODE ADC_AsSlaveConfigPes(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    ret = PES_ConfigEventChannel(pchCfg, chIndex);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err:PES_ConfigEventChannel\n", __FUNCTION__);
    }

    return ret;
}

RET_CODE ETM_TriggerADC(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;
    ADC_CommonConfig_T commonConfig;
    ADC_CompareConfig_T compareConfig;
    ADC_SequenceConfig_T sequenceConfig;
    PES_ChannelConfig_T  chCfg;

    uint8_t i = 0;
    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
    s_masterSource = PES_MASTER_ETM20;
    chCfg.masterSrcConfig.masterSource = s_masterSource;
    chCfg.masterSrcConfig.masterTrigerOutput.etm20Output = PES_ETM20_CH0_POSEDGE;
    chCfg.syncMode = PES_SYNC_RISING_EDGE;

    ret = ADC_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err ADC_ConfigPinmux\n", __FUNCTION__);
        goto out;
    }

    /* config adc common parameter */
    commonConfig.continuousMode = ADC_SINGLE_MODE;
    commonConfig.enableDma = DISABLE;

    commonConfig.trigerMode = ADC_HW_TRIGER;
    commonConfig.lowPowerEn = DISABLE;
    commonConfig.enableDisconMode = DISABLE;
    commonConfig.pestrigersrcfunc = ADC_PesTrigerSrcInit;
    commonConfig.callback = ADC_Callback;

    if (commonConfig.trigerMode == ADC_HW_TRIGER)
    {
        s_chIndex = PES_RequestEventChannel();
        if (PES_INVAILD_CHANNEL == s_chIndex)
        {
            ret = RET_ERROR;
            PR_DEBUG("%s:Err:PES_RequestEventChannel\n", __FUNCTION__);
            return ret;
        }
    }

    ret = ADC_Init(ADC_DEV, &commonConfig, &s_Handle);
    if (ret != RET_OK)
    {
        PR_DEBUG("%s:Err ADC_Init\n", __FUNCTION__);
        goto out;
    }

    /* config ADC Sequence sample paramter */
    sequenceConfig.resolution = ADC_RESOLUTION_12BIT;
    sequenceConfig.sampleTime = ADC_SAMPLETIME_6CLK;
    sequenceConfig.sampleEdge = ADC_SAMPLE_POSITIVE;
    sequenceConfig.DataFifoFullWrEn = DISABLE;
    sequenceConfig.pSequence = s_sequence;
    sequenceConfig.length = SEQUENCE_LENGTH;

    ret = ADC_SequenceConfig(&s_Handle, &sequenceConfig);
    if (ret != RET_OK)
    {
        PR_DEBUG("\r%s:Err ADC_SequenceConfig\n", __FUNCTION__);
        goto out;
    }

    if (commonConfig.trigerMode == ADC_HW_TRIGER)
    {
        ADC_AsSlaveConfigPes(&chCfg, (PES_ChannelIndex_T)s_chIndex);
    }

    /* config ADC compare paramter */
    compareConfig.compareMode = ADC_COMPARE_WITHIN_RANGE;
    compareConfig.compareHighValue = 500;
    compareConfig.compareLowValue = 300;
    ret = ADC_CompareConfig(&s_Handle, &compareConfig);
    if (ret != RET_OK)
    {
        PR_DEBUG("\r%s:Err ADC_CompareConfig\n", __FUNCTION__);
        goto out;
    }
    ADC_CompareModeEnable(&s_Handle);

    /* read adc convert result every 500ms*/
    while (1)
    {
        ret = ADC_ReadConvertValue(&s_Handle, s_convertValue, TIMEOUT_NO_BLOCKING);
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
            PR_DEBUG("Err:%d ADC_ReadConvertValue\n", ret);
            goto out;
        }

        SysTick_DelayMS(1000);

    }
out:
    return ret;
}
#endif

#ifdef ETM_Breakin
static RET_CODE ETM_AsSlaveConfig(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;
    PES_ChannelConfig_T chCfg;
    PES_ChannelIndex_T chIndex;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InputGlitchFilter_T  GPIO_InputGlitchFilter;

    memset(&GPIO_InputGlitchFilter, 0, sizeof(GPIO_InputGlitchFilter_T));

    /* A filter is implemented for each port pin that is configured as a digital input.
     * It can be used as a simple low-pass filter to filter any glitch that is introduced from the pins of GPIO.
     * The following code shows how to use filter.
     */

    /* Enable glitch filter */
    GPIO_InputGlitchFilter.bypass = FALSE;
    /* selects glitch filter clock */
    GPIO_InputGlitchFilter.clock = PAD_FLT_CLK2;
    /* Divided FILTER clock by 2, removed noise: 500ns */
    GPIO_InputGlitchFilter.prescaler = FLT_CLK2_PSC_DIV_64K;
    GPIO_SetPinFilter(GPIOA0, &GPIO_InputGlitchFilter);

    SysTick_DelayMS(5000);

    /*master pesout : config receive master event config */
    /*request pes channel for cpu ->peschannel out*/
    chIndex = PES_RequestEventChannel();

    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
    chCfg.syncMode = PES_SYNC_BYPASS;
    chCfg.masterSrcConfig.masterSource = PES_MASTER_IO;
    /* select xpa11 */
    chCfg.masterSrcConfig.masterTrigerOutput.ioPinOutput = IO_PIN_XPA14;

    ret = PES_ConfigEventChannel(&chCfg, chIndex);
    if (ret != RET_OK)
    {
        goto out;
    }

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin = GPIO_PIN_14;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = GPIOA14_AF5_PESCH10;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    ETM_PESConfig(pETM, chIndex);
    ETM_SetTriggerMode(ETM0, /*ETM_PWM_TRG_START_STOP*/ETM_PWM_TRG_START, TRUE);

out:
    return ret;
}
#endif
#endif

int main(void)
{
    RET_CODE ret = RET_OK;
    PeriphClk_Config_T config;

    /* Init system timer */
    SysTick_Init();

    /* Switch ETM IP Clock to 48MHZ */
    config.periphClkMask = CLOCK_PERIPH_ETM_MASK;
#if defined(CHIP_GM6601B) || defined(CHIP_F6601C) 
	config.etmClock = CLK_48M;
#elif defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_GM6621)
    config.etmClock = CLK_FCLK_DIV_8;
#endif
    ret = PeriphClk_SetConfig(&config);

    ret = PINMUX_Config();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    ret = ETM_PWMGeneratorByEdgeAlignedMode();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

#ifdef ETM_CAPTURE
    ret = ETM_CaptureFunc();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
#endif

    ret = ETM_PWMGeneratorByCenterAlignedMode();
    //ret = ETM_PWMGeneratorByEdgeAlignedMode();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

#if (defined(CHIP_GM6621))
#ifdef ETM_Trigger_ADC
    ret = ETM_TriggerADC(ETM0);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
#endif

#ifdef ETM_Breakin
    ETM_PESEVDisable(ETM0);

    ret = ETM_AsSlaveConfig(ETM0);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
#endif
#endif

    while (1)
    {
        ;
    }

}



