/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gpio_example.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/06/29
 *
 * @brief        Sample code for gpio operation.
 *
 * @note
 *    2016/06/29, mark.lee, V1.0.0
 *        Initial version.
 */
#include <stdio.h>

#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>


/*******************************************************************************
* Constants and macros
*******************************************************************************/
STATIC_ISR(GPIO0_IRQn, GPIOA_IRQHandler);

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
static void GPIOA_IRQHandler(void)
{
    if (GPIO_EXTI_GET_IT(GPIOA, GPIO_PIN_2))
    {
        GPIO_EXTI_CLEAR_IT(GPIOA, GPIO_PIN_2);
    }

    /* Toggle GPIOD9 output */
    GPIO_PinToggle(GPIOD9);
}

static RET_CODE PINMUX_Config(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

#if (defined(CHIP_GM6611))
    /** ETM0 GPIO Configuration
    PD3     ------> ETM0_CH1
    */
    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD3_AF0_ETM0CH1;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
#elif (defined(CHIP_GM6601))
    /** ETM0 GPIO Configuration
    PC7     ------> ETM0_CH1
    */
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC7_AF0_ETM0CH1;
    ret = GPIO_Init(GPIOC, &GPIO_InitStruct);
#elif (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    /** ETM0 GPIO Configuration
    PC7     ------> ETM0_CH1
    */
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOC7_AF0_ETM0CH1;
    ret = GPIO_Init(GPIOC, &GPIO_InitStruct);
#elif (defined(CHIP_GM6611B))
    /** ETM0 GPIO Configuration
    PD3     ------> ETM0_CH1
    */
    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOD3_AF0_ETM0CH1;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
#elif (defined(CHIP_GM6621))
    /** ETM0 GPIO Configuration
    PA4    ------> ETM0_CH1
    */
    GPIO_InitStruct.pin = GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA4_AF4_ETM2CH0;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif

    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Configure GPIOD9 as output pin */
    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_PinWrite(GPIOD9, GPIO_PIN_RESET);

    /* Configure GPIOA2 as external interrupt lines pin */
    GPIO_InitStruct.pin = GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
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
#if (!defined(CHIP_GM6621))
    etmInfo.prescaler = ETM_PSC_DIV_128;
#else
    etmInfo.prescaler = 127;
#endif
    etmInfo.countMode = ETM_COUNT_UP;
    etmInfo.deadTime = 0;
    etmInfo.period = 7500;
    etmInfo.callback = NULL;

    ret = ETM_InitTimeBase(ETM0, &etmInfo, NULL);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Sets comparator that monitor the value of the counter */
    ETM_SetPWMDuty(ETM0, TRUE, 3750, 375);

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

    /* Start PWM generator */
    ret = ETM_StartPWM(ETM0);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

int main(void)
{
    RET_CODE ret = RET_OK;
    GPIO_InputGlitchFilter_T  GPIO_InputGlitchFilter;

    SysTick_Init();
		SysTick_DelayMS(2000);
	
		 *((uint32_t *)(0x40000124)) |= (1<<6); 

//    ret = PINMUX_Config();
//    if (ret != RET_OK)
//    {
//        return RET_ERROR;
//    }

//    memset(&GPIO_InputGlitchFilter, 0, sizeof(GPIO_InputGlitchFilter_T));

//    /* A filter is implemented for each port pin that is configured as a digital input.
//     * It can be used as a simple low-pass filter to filter any glitch that is introduced from the pins of GPIO.
//     * The following code shows how to use filter.
//     */

//    /* Enable glitch filter */
//    GPIO_InputGlitchFilter.bypass = FALSE;
//    /* selects glitch filter clock */
//    GPIO_InputGlitchFilter.clock = PAD_FLT_CLK0;
//    /* Divided FILTER clock by 2, removed noise: 500ns */
//    GPIO_InputGlitchFilter.prescaler = FLT_CLK0_PSC_DIV_2;
//    GPIO_SetPinFilter(GPIOA2, &GPIO_InputGlitchFilter);

//    /* Generating PWM waveforms, trigger external interrupt lines */
//    ret = ETM_PWMGeneratorByEdgeAlignedMode();
//    if (ret != RET_OK)
//    {
//        return RET_ERROR;
//    }

    while (1)
    {
        ;
    }
}



