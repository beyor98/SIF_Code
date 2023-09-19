/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          bldc_demo.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/11/23 
 *    
 * @brief        Sample code for BLDC motor control with six-step driving method,
 *               which based on GM6601 demo board and X-NUCLEO-IHM07M1 expansion board.
 * @note
 *    2016/11/23, mark.lee, V1.0.0
 *        Initial version. 
 */
#include <stdio.h>
#include "motor_control.h"

/*******************************************************************************
* Constants and macros
*******************************************************************************/
/* Zero Crossissing parameters */
#define BEMF_THRSLD_DOWN            80      /* Zero Crossing threshold */  
#define BEMF_THRSLD_UP              80      /* Zero Crossing threshold */ 

#define NUMBER_ZCR                  20      /* Number of pulse width change */
#define INITIAL_DEMAGN_DELAY        5       /* Initial value for delay time during startup for Bemf detection */
#define NUMBER_OF_STEPS             6000    /* Time for motor start-UP */ 
#define TIME_FOR_ALIGN              4000    /* Time for alignment */

/*******************************************************************************
* Global variables
*******************************************************************************/
ETM_Handle_T g_PWM0Handle;
ETM_Handle_T g_PWM1Handle;
ETM_Handle_T g_PWM2Handle;

MC_Sixstep_T g_sixStepParameters;
/*******************************************************************************
* Static variables
*******************************************************************************/
static ADC_Handle_T s_Handle;
static uint32_t s_convertValue;

/*******************************************************************************
* Reference variables
*******************************************************************************/

/*******************************************************************************
* Local function prototypes
*******************************************************************************/
static void PreCommutationState(void)
{
    g_sixStepParameters.PWMTicks = 0;

    /* Generate the next step number */
    if (++g_sixStepParameters.stepNumber > 6)
    {
        g_sixStepParameters.stepNumber = 1;
    }

    g_sixStepParameters.demagnCnt = 0;
    MC_SixStep_Table(g_sixStepParameters.stepNumber, g_sixStepParameters.dutyCycle);
}


static void ETM0Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
    if (Event & ETM_TBUDFINT_FLAG)
    {
        if (g_sixStepParameters.status == STARTUP || g_sixStepParameters.status == RUN)
        {
            /* Trigger ADC for Bemf detection in falling edge */
            SET_CHANNEL_FIFO_DEPTH(ADC_DEV, 1);
            switch (g_sixStepParameters.stepNumber)
            {
                case 1:
                case 4:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_4);
                    break;
                case 2:
                case 5:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_15);
                    break;
                case 3:
                case 6:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_3);
                    break;
                default:
                    break;
            }
            ADC_SW_TRIGGER(ADC_DEV);
        }  
    }
}

static void ETM1Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
    if (Event & ETM_TBUDFINT_FLAG)
    {
        if (g_sixStepParameters.status == STARTUP || g_sixStepParameters.status == RUN)
        {
            /* Trigger ADC for Bemf detection in falling edge */
            SET_CHANNEL_FIFO_DEPTH(ADC_DEV, 1);
            switch(g_sixStepParameters.stepNumber)
            {
                case 1:
                case 4:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_4);
                    break;
                case 2:
                case 5:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_15);
                    break;
                case 3:
                case 6:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_3);
                    break;
                default:
                    break;
            }
            ADC_SW_TRIGGER(ADC_DEV);
        }  
    }
}

static void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
    if (Event & ETM_TBUDFINT_FLAG)
    {
        if (g_sixStepParameters.status == STARTUP || g_sixStepParameters.status == RUN)
        {
            /* Trigger ADC for Bemf detection in falling edge */
            SET_CHANNEL_FIFO_DEPTH(ADC_DEV, 1);
            switch(g_sixStepParameters.stepNumber)
            {
                case 1:
                case 4:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_4);
                    break;
                case 2:
                case 5:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_15);
                    break;
                case 3:
                case 6:
                    ADC_SET_CHANNEL_INDEX(ADC_DEV, ADC_CHANNEL_3);
                    break;
                default:
                    break;
            }
            ADC_SW_TRIGGER(ADC_DEV);
        }  
    }
}

static void ADC_Callback(ADC_Handle_T *pHander, ADC_Event_T event, unsigned long param)
{
    if (event == ADC_EVENT_FINISHED)
    {
        if(g_sixStepParameters.demagnCnt > INITIAL_DEMAGN_DELAY)
        {
            g_sixStepParameters.bemf = s_convertValue;

            /* Compute the zero crossing detection */
            if (((g_sixStepParameters.stepNumber == 1 || g_sixStepParameters.stepNumber == 3 
                || g_sixStepParameters.stepNumber == 5) && g_sixStepParameters.bemf < BEMF_THRSLD_DOWN)
                || ((g_sixStepParameters.stepNumber == 2 || g_sixStepParameters.stepNumber == 4 
                || g_sixStepParameters.stepNumber == 6) && g_sixStepParameters.bemf > BEMF_THRSLD_UP)) 
            {
                /* If zero crossing event occur and the number of pulse width change
                 * during the startup is larger, modify motor status 
                 */
                if (g_sixStepParameters.counter > NUMBER_ZCR)
                {
                    g_sixStepParameters.status = RUN;
                    PreCommutationState();
                }
            }
        }
        else
        {
            g_sixStepParameters.demagnCnt++;
        }
    }
}


static RET_CODE PINMUX_Config(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;

    RET_CODE ret = RET_OK;
    
    memset(&GPIO_InitStruct,0,sizeof(GPIO_PinConfig_T));

    /**ADC Channel Configuration
      * PB12  ------> AIN3
      * PB8   ------> AIN15
      * PB13  ------> AIN4
      */
    GPIO_InitStruct.pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
	
    /**ETM0 GPIO Configuration
      * PC6   ------> ETM0_TCIN
      * PC7   ------> ETM0_CH1
      */
    GPIO_InitStruct.pin = GPIO_PIN_6;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
#if (defined(CHIP_GM6601))
    GPIO_InitStruct.alternate = GPIOC6_AF0_ETM0TCIN;
#endif
    ret = GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
	
    GPIO_InitStruct.pin = GPIO_PIN_7;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
#if (defined(CHIP_GM6601))
    GPIO_InitStruct.alternate = GPIOC7_AF0_ETM0CH1;
#endif
    ret = GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /**ETM1 GPIO Configuration
      * PB2  ------> ETM1_TCIN
      * PB3  ------> ETM1_CH1
      */	
    GPIO_InitStruct.pin = GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
#if (defined(CHIP_GM6601))
    GPIO_InitStruct.alternate = GPIOB2_AF0_ETM1TCIN;		
#endif
    ret = GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
#if (defined(CHIP_GM6601))
    GPIO_InitStruct.alternate = GPIOB3_AF0_ETM1CH1;
#endif
    ret = GPIO_Init(GPIOB, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
    

    /**ETM2 GPIO Configuration
      * PC14  ------> ETM2_TCIN
      * PC15  ------> ETM2_CH1
      */
    GPIO_InitStruct.pin = GPIO_PIN_14;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
#if (defined(CHIP_GM6601))
    GPIO_InitStruct.alternate = GPIOC14_AF0_ETM2TCIN;
#endif
    ret = GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
	
    GPIO_InitStruct.pin = GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
#if (defined(CHIP_GM6601))
    GPIO_InitStruct.alternate = GPIOC15_AF0_ETM2CH1;
#endif
    ret = GPIO_Init(GPIOC, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Configure GPIOA2/3/8 as output pin, enable or disable input channel for L6230 */
    GPIO_InitStruct.pin = GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

/* Generating edge-aligned PWM waveforms */
static RET_CODE ETM_PWMGeneratorByEdgeAlignedMode(ETM_Device_T *pETM)
{
    ETM_Config_T etmInfo;
    ETM_PWMConfig_T pwmOutputInfo;
    RET_CODE ret = RET_OK;

    memset(&etmInfo,0,sizeof(ETM_Config_T));

    /* ETM device configuration */
    etmInfo.extClock = FALSE;
    etmInfo.prescalerEn = FALSE;
#if (defined(CHIP_GM6601))
    etmInfo.prescaler = ETM_PSC_DIV_2;
#endif
    etmInfo.countMode = ETM_COUNT_DOWN;
    etmInfo.deadTime = 0;
    etmInfo.period = 1500;

    if (pETM == ETM0)
    {
        //etmInfo.callback = ETM0Callback;
			  ETM0Callback(ETM0, ETM_TBUDFINT_FLAG, NULL);
        ret = ETM_InitTimeBase(pETM, &etmInfo, &g_PWM0Handle);
    }
    else if (pETM == ETM1)
    {
        //etmInfo.callback = ETM1Callback;
			  ETM1Callback(ETM1, ETM_TBUDFINT_FLAG, NULL);
        ret = ETM_InitTimeBase(pETM, &etmInfo, &g_PWM1Handle);
    }
    else if (pETM == ETM2)
    {
        //etmInfo.callback = ETM2Callback;
			  ETM2Callback(ETM2, ETM_TBUDFINT_FLAG, NULL);
        ret = ETM_InitTimeBase(pETM, &etmInfo, &g_PWM2Handle);
    }
    else
    {
        etmInfo.callback = NULL;
        ret = ETM_InitTimeBase(pETM, &etmInfo, NULL);
    }
    
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Sets comparator that monitor the value of the counter */
    ETM_SetPWMDuty(pETM, TRUE, 200, 375);

    /* Config break in */
#if (defined(CHIP_GM6601))
    ETM_SetTriggerMode(pETM, ETM_PWM_TRG_START_STOP, TRUE);
#endif
    memset(&pwmOutputInfo,0,sizeof(ETM_PWMConfig_T));

    /* PWM Channel1 pin idle output polarity setting */
    pwmOutputInfo.idleState = ETM_IDLESTATE_LOW;

    /* For each event, the effect on each output PWM signal is programmable: 
     * it can be left alone (ignoring the event), it can be toggled, 
     * it can be driven Low, or it can be driven High.
     */
    pwmOutputInfo.counterStart = ETM_UNCHANGED;
    pwmOutputInfo.counterDown2 = ETM_UNCHANGED;
    pwmOutputInfo.counterUp2 = ETM_UNCHANGED;
    pwmOutputInfo.counterDown1 = ETM_LEVEL_TOGGLE;
    pwmOutputInfo.counterUp1 = ETM_UNCHANGED;
    pwmOutputInfo.overFlow = ETM_UNCHANGED;
    pwmOutputInfo.underFlow = ETM_LEVEL_TOGGLE;

    ETM_SetPWMChannel(pETM, ETM_CHANNEL_1, &pwmOutputInfo);

    ETM_EnableInterrupts(pETM, ETM_INT_CNT_UNDERFLOW);

    return RET_OK;
}

/* Initialize ADC module */
static RET_CODE ADC_Convert_Init(void)
{
    RET_CODE ret = RET_OK;
    ADC_CommonConfig_T commonConfig; 
    ADC_ChannelConfig_T channleConfig;
    PeriphClk_Config_T clkConfig;
	
    /* Config ADC module clock to 16MHz */	
    clkConfig.periphClkMask = CLOCK_PERIPH_ADC_MASK;
#if (defined(CHIP_GM6601))
    clkConfig.adcClock = ADC_CLOCK_16MHZ;
#endif
    PeriphClk_SetConfig(&clkConfig);
    
    /* Config ADC common parameter */
    commonConfig.continuousMode = ADC_SINGLE_MODE;
    commonConfig.enableDma = DISABLE;
    commonConfig.callback = ADC_Callback;
    ret = ADC_Init(ADC_DEV, &commonConfig, &s_Handle);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Config ADC channel sample paramter */
    channleConfig.resolution = ADC_RESOLUTION_12BIT;
    channleConfig.sampleTime = ADC_SAMPLETIME_6CLK;
    channleConfig.channelIndex = ADC_CHANNEL_3;
    ret = ADC_ChannelConfig(&s_Handle, &channleConfig);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }    
	
    ret = ADC_ReadConvertValue(&s_Handle, &s_convertValue, TIMEOUT_NO_BLOCKING);	
	
    return RET_OK;
}


int main(void)
{
    RET_CODE ret = RET_OK;
    PeriphClk_Config_T config;
    
    SysTick_Init();

    /* Switch ETM IP Clock to 48MHZ */
    config.periphClkMask = CLOCK_PERIPH_ETM_MASK;
    config.etmClock = 0x1;
    ret = PeriphClk_SetConfig(&config);
    
    ret = PINMUX_Config();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
    
    ret = ETM_PWMGeneratorByEdgeAlignedMode(ETM0);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    ret = ETM_PWMGeneratorByEdgeAlignedMode(ETM1);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    ret = ETM_PWMGeneratorByEdgeAlignedMode(ETM2);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
    
    ret = ADC_Convert_Init();
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    MC_SixStep_Init(&g_sixStepParameters);

    while(1)
    {  		
        switch (g_sixStepParameters.status)
        {
            case IDLE:
            {
                MC_StartMotor(&g_sixStepParameters);

                /* To start the motor, according to the initial rotor position, 
                 * a specific motor phase must be energized.
                 */
                g_sixStepParameters.stepNumber = 1;
                g_sixStepParameters.dutyCycle = 200;
                MC_SixStep_Table(g_sixStepParameters.stepNumber, g_sixStepParameters.dutyCycle);
                break;
            }
            case ALIGNMENT:
            {
                /* Give the motor enough time to move to the given initial commutation position */
                if (++g_sixStepParameters.PWMTicks > TIME_FOR_ALIGN)
                {
                    g_sixStepParameters.demagnCnt = 0;
                    g_sixStepParameters.status = STARTUP;
                    g_sixStepParameters.PWMTicks = 0;
                    PreCommutationState();
                }
                break;
            }
            case STARTUP:
            {
                /* Open loop acceleration, in order to detect the BEMF as soon as possible */
                if (++g_sixStepParameters.PWMTicks > NUMBER_OF_STEPS)
                {
                    g_sixStepParameters.PWMTicks = 0;
                    if(g_sixStepParameters.dutyCycle > MAX_DUTY_CYCLE)
                    {
                        MC_StopMotor(&g_sixStepParameters);
                    }
                    else
                    {
                        /* Modify pulse width during the startup */
                        g_sixStepParameters.dutyCycle += 2;
                        PreCommutationState();
                        g_sixStepParameters.counter++;
                    }	
                }
                break;
            }
            case RUN:
                SysTick_DelayMS(8000);
                if (g_sixStepParameters.speedLoop == 0)
                {
                    /* Increase motor speed */
                    g_sixStepParameters.dutyCycle += 100;
                    if (g_sixStepParameters.dutyCycle > MAX_DUTY_CYCLE/2)
                    {
                        g_sixStepParameters.dutyCycle = MAX_DUTY_CYCLE/2;
                        g_sixStepParameters.speedLoop = 1;
                    }
                }
                else
                {
                    /* Decrease motor speed */
                    g_sixStepParameters.dutyCycle -= 100;
                    if (g_sixStepParameters.dutyCycle < MIN_DUTY_CYCLE)
                    {
                        g_sixStepParameters.dutyCycle = MIN_DUTY_CYCLE;
                        g_sixStepParameters.speedLoop = 0;
                    }
                }
                break;
            default:
                break;
        }
    }
}



