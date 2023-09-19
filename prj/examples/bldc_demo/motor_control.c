/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          motor_control.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/11/23 
 *    
 * @brief        This file provides the set of functions for Motor Control library.
 * 
 * @note
 *    2016/11/23, mark.lee, V1.0.0
 *        Initial version. 
 */
 
#include "motor_control.h"

/*******************************************************************************
* Constants and macros
*******************************************************************************/


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
/**
 * @brief  Enable Input channel CH1 and CH2 for L6230
 *
 * @param stepNumber step number selected
 * @param dutyCycle   duty cycle value
 * @retval None
 */
static void MC_PWM_CH1CH2_ON(uint8_t stepNumber, uint16_t dutyCycle);

/**
 * @brief  Enable Input channel CH1 and CH3 for L6230
 *
 * @param stepNumber step number selected
 * @param dutyCycle   duty cycle value
 * @retval None
 */
static void MC_PWM_CH1CH3_ON(uint8_t stepNumber, uint16_t dutyCycle);

/**
 * @brief  Enable Input channel CH2 and CH3 for L6230
 *
 * @param stepNumber step number selected
 * @param dutyCycle   duty cycle value
 * @retval None
 */
static void MC_PWM_CH2CH3_ON(uint8_t stepNumber, uint16_t dutyCycle);

/**
 * @brief  Disable all channels for L6230
 *
 * @param  None
 * @retval None
 */
static void MC_PWM_CH1CH2CH3_OFF(void);

/*******************************************************************************
* Codes
*******************************************************************************/
static void MC_PWM_CH1CH2_ON(uint8_t stepNumber, uint16_t dutyCycle)
{
    ETM_StopPWM(ETM0);
    if (stepNumber == 1)
    {
        ETM_SetPWMDuty(ETM2, TRUE, dutyCycle, 0);
        ETM_StartPWM(ETM2);
        ETM_StopPWM(ETM1);
    }		

    if (stepNumber == 4)
    {
        ETM_SetPWMDuty(ETM1, TRUE, dutyCycle, 0); 
        ETM_StartPWM(ETM1);    
        ETM_StopPWM(ETM2); 
    }		

    /* Enable input channel 1/2, disable input channel 3 */
    GPIO_PinWrite(GPIOA2, GPIO_PIN_SET);
    GPIO_PinWrite(GPIOA3, GPIO_PIN_SET);
    GPIO_PinWrite(GPIOA8, GPIO_PIN_RESET);
}

static void MC_PWM_CH1CH3_ON(uint8_t stepNumber, uint16_t dutyCycle)
{
    ETM_StopPWM(ETM1);
    if (stepNumber == 5)
    {
        ETM_SetPWMDuty(ETM0, TRUE, dutyCycle, 0);
        ETM_StartPWM(ETM0);        
        ETM_StopPWM(ETM2);
    }		

    if (stepNumber == 2)
    {
        ETM_SetPWMDuty(ETM2, TRUE, dutyCycle, 0); 
        ETM_StartPWM(ETM2);
        ETM_StopPWM(ETM0);
    }				

    /* Enable input channel 1/3, disable input channel 2 */
    GPIO_PinWrite(GPIOA2, GPIO_PIN_SET);
    GPIO_PinWrite(GPIOA3, GPIO_PIN_RESET);
    GPIO_PinWrite(GPIOA8, GPIO_PIN_SET);
}

static void MC_PWM_CH2CH3_ON(uint8_t stepNumber, uint16_t dutyCycle)
{
    ETM_StopPWM(ETM2);
    if (stepNumber == 3)
    {
        ETM_SetPWMDuty(ETM1, TRUE, dutyCycle, 0);
        ETM_StartPWM(ETM1);
        ETM_StopPWM(ETM0); 
    }		

    if (stepNumber == 6)
    {
        ETM_SetPWMDuty(ETM0, TRUE, dutyCycle, 0) ;
        ETM_StartPWM(ETM0);   
        ETM_StopPWM(ETM1);
    }		

    /* Enable input channel 2/3, disable input channel 1 */
    GPIO_PinWrite(GPIOA2, GPIO_PIN_RESET);
    GPIO_PinWrite(GPIOA3, GPIO_PIN_SET);
    GPIO_PinWrite(GPIOA8, GPIO_PIN_SET);
}

static void MC_PWM_CH1CH2CH3_OFF(void)
{
    /* Disable input channel 1/2/3 */
    GPIO_PinWrite(GPIOA2, GPIO_PIN_RESET);
    GPIO_PinWrite(GPIOA3, GPIO_PIN_RESET);
    GPIO_PinWrite(GPIOA8, GPIO_PIN_RESET);
}

void MC_SixStep_Table(uint8_t stepNumber, uint16_t dutyCycle)
{
    if (dutyCycle > MAX_DUTY_CYCLE)
    {
        dutyCycle = MAX_DUTY_CYCLE;
    }
    else if (dutyCycle < MIN_DUTY_CYCLE)
    {
        dutyCycle = MIN_DUTY_CYCLE;
    }
	
    switch (stepNumber)
    {
        case 1:
        case 4:
            MC_PWM_CH1CH2_ON(stepNumber, dutyCycle);
            break;
        case 2:
        case 5:
            MC_PWM_CH1CH3_ON(stepNumber, dutyCycle);
            break;
        case 3:
        case 6:
            MC_PWM_CH2CH3_ON(stepNumber, dutyCycle);
            break;
        default:
            break;
    }     
}

void MC_SixStep_Init(MC_Sixstep_T *pCfg)
{
    pCfg->stepNumber = 0;
    pCfg->demagnCnt = 0;
    pCfg->counter = 0;
    pCfg->speedLoop = 0;
    pCfg->dutyCycle = 0;
    pCfg->PWMTicks = 0;
    pCfg->bemf = 0;
    pCfg->status = IDLE;
    MC_PWM_CH1CH2CH3_OFF();

    GPIO_PinWrite(GPIOD9, GPIO_PIN_SET);
}

void MC_StartMotor(MC_Sixstep_T *pCfg)
{
    ETM_StartPWM(ETM2);
    ETM_StartPWM(ETM1);
    ETM_StartPWM(ETM0);

    pCfg->status = ALIGNMENT;
}

void MC_StopMotor(MC_Sixstep_T *pCfg)
{
    MC_PWM_CH1CH2CH3_OFF();
    ETM_StopPWM(ETM2);
    ETM_StopPWM(ETM1);
    ETM_StopPWM(ETM0);
    pCfg->status = STOP;
}


