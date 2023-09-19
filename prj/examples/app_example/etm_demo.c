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
#define CLK_8M       0
#define CLK_48M      1

#define PR_DEBUG(...)       do{}while(0)



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

uint32_t s_period = 0;

/*******************************************************************************
* Static variables
*******************************************************************************/

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



#if (defined CHIP_GM6621)
    /** ETM2 Pinmux Configuration*/
    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA4_AF4_ETM2CH0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_12;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC12_AF3_ETM2CH1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB0_AF4_ETM2CH2;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA7_AF4_ETM2CH3;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** ETM1 Pinmux Configuration
    PB7     ------> ETM1_CH1
    PB8     ------> ETM1_CH2
    */
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB7_AF3_ETM21CH0;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB8_AF3_ETM21CH1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /** ETM2 Pinmux Configuration
    PB10     ------> ETM2_CH1
    PB11     ------> ETM2_CH2
    */
    GPIO_InitStruct.pin = GPIO_PIN_10;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB10_AF3_ETM22CH0;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB11_AF3_ETM22CH1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
#endif

#if defined(CHIP_GM6601B) || defined(CHIP_F6601C)
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC7_AF0_ETM0CH1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC8_AF0_ETM0CH2;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB3_AF0_ETM1CH1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOB4_AF0_ETM1CH2;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC15_AF0_ETM2CH1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD0_AF0_ETM2CH2;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA7_AF0_ETM3CH1;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA9_AF0_ETM3CH2;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC11_AF0_ETM4CH1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_12;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC12_AF0_ETM5CH1;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

#endif

    return ret;
}



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

    /* Start PWM generator */
    ret = ETM_StartPWM(ETM2);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}


int main(void)
{
    RET_CODE ret = RET_OK;
    PeriphClk_Config_T config;

    /* Init system timer */
    SysTick_Init();

    /* Switch ETM IP Clock to 48MHZ */
    config.periphClkMask = CLOCK_PERIPH_ETM_MASK;
    config.etmClock = CLK_48M;
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



    ret = ETM_PWMGeneratorByCenterAlignedMode();
    //ret = ETM_PWMGeneratorByEdgeAlignedMode();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }


    while (1)
    {
        ;
    }

}



