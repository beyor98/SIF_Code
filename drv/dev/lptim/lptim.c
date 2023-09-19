/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          lptim.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/06/05 
 *    
 * @brief        Low Power Timer peripheral APIs for application. 
 * 
 * @note
 *    2017/06/05, mark.lee, V1.0.0
 *        Initial version. 
 */
 
#include <drv/dev/lptim.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
STATIC_ISR(ETM3_IRQn, LPTIM_IRQHandler);

/*******************************************************************************
* Global variables
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/
/** @brief Pointers to ETM handles for each instance. */
static void *s_LPTIMHandle = NULL;

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/

/*******************************************************************************
* Codes
*******************************************************************************/
static void LPTIM_CommonIRQHandler(LPTIM_Device_T *pLPTIM, void *pLPTIMHandle)
{
    LPTIM_Handle_T *pHandle = (LPTIM_Handle_T *)pLPTIMHandle;
    uint32_t interruptMask = 0;
    uint32_t interruptFlags = 0;
    uint32_t event = 0;

    interruptMask = pLPTIM->IER;

    interruptFlags = pLPTIM->SR;

    if ((interruptMask&LPTIM_IT_DOWN)  && (interruptFlags&LPTIM_DOWN_FLAG))
    {
        event |= LPTIM_DOWN_FLAG;
    }
    if ((interruptMask&LPTIM_IT_UP) && (interruptFlags&LPTIM_UP_FLAG))
    {
        event |= LPTIM_UP_FLAG;
    }
    if ((interruptMask&LPTIM_IT_ARROK) && (interruptFlags&LPTIM_ARROK_FLAG))
    {
        event |= LPTIM_ARROK_FLAG;
    }
    if ((interruptMask&LPTIM_IT_CMPOK) && (interruptFlags&LPTIM_CMPOK_FLAG))
    {
        event |= LPTIM_CMPOK_FLAG;
    }     
    if ((interruptMask&LPTIM_IT_EXTTRIG) && (interruptFlags&LPTIM_EXTTRIG_FLAG))
    {
        event |= LPTIM_EXTTRIG_FLAG;
    }
    if ((interruptMask&LPTIM_IT_ARRM) && (interruptFlags&LPTIM_ARRM_FLAG))
    {
        event |= LPTIM_ARRM_FLAG;
    }
    if ((interruptMask&LPTIM_IT_CMPM) && (interruptFlags&LPTIM_CMPM_FLAG))
    {
        event |= LPTIM_CMPM_FLAG;
    }   

    /* Clear the interrupt flag. */
    pLPTIM->SR = event;

    /* Call the callback function after the function has completed. */
    if ((pHandle != NULL) && (pHandle->callback != NULL))
    {
        pHandle->callback(pLPTIM, event, NULL);
    }
}

RET_CODE LPTIM_Init(LPTIM_Device_T *pLPTIM, const LPTIM_Config_T *pConfig, LPTIM_Handle_T *pHandle)
{
    uint32_t tmpcfgr = 0U;

    /* Check the LPTIM handle allocation */
    if (pLPTIM == NULL)
    {
        return RET_ERROR;
    }

    if (pHandle != NULL)
    {
        /* Zero handle */
        memset(pHandle, 0, sizeof(*pHandle));

        /* Set callback */
        pHandle->pDevice = pLPTIM;
        pHandle->callback = pConfig->callback;

        /* Save the context in global variables to support the double weak mechanism */
        s_LPTIMHandle = pHandle;
    }

    /* Enable nvic */
    NVIC_EnableIRQ(ETM3_IRQn);

    /* Get the LPTIMx CFGR value */
    tmpcfgr = pLPTIM->CFGR;

    /* Clear CKSEL, PRESC, TRIGEN, TRGFLT, WAVPOL, PRELOAD & COUNTMODE bits */
    tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_EXTCLK | LPTIM_CFGR_TRIGEN | LPTIM_CFGR_PRELOAD |
                          LPTIM_CFGR_WAVPOL | LPTIM_CFGR_PRESC | LPTIM_CFGR_COUNTMODE ));
    
    if (pConfig->extClock ==  TRUE)
    {
        tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_CKPOL | LPTIM_CFGR_CKFLT));
        tmpcfgr |= (LPTIM_CFGR_EXTCLK | LPTIM_CFGR_CKPOL_FIELD(pConfig->UltraLowPowerClock.polarity)
                    | LPTIM_CFGR_CKFLT_FIELD(pConfig->UltraLowPowerClock.sampleTime));
    }
    
    if ((pConfig->trigger.source) !=  LPTIM_TRIGSOURCE_SOFTWARE)
    {
        /* Enable External trigger and set the trigger source */
        tmpcfgr &= (uint32_t)(~ (LPTIM_CFGR_TRGFLT | LPTIM_CFGR_TRIGSEL));
        tmpcfgr |= (LPTIM_CFGR_TRGFLT_FIELD(pConfig->trigger.sampleTime) | LPTIM_CFGR_TRIGSEL_FIELD(pConfig->trigger.source)
                    | LPTIM_CFGR_TRIGEN_FIELD(pConfig->trigger.activeEdge));
    }

    /* Set initialization parameters */
    tmpcfgr |= (LPTIM_CFGR_PRESC_FIELD(pConfig->prescaler) |
                pConfig->outputPolarity | pConfig->updateMode | pConfig->counterSource);

    /* Write to LPTIMx CFGR */
    pLPTIM->CFGR = tmpcfgr;

    return RET_OK;
}

RET_CODE LPTIM_DeInit(LPTIM_Device_T *pLPTIM)
{
    /* Check the LPTIM handle allocation */
    if (pLPTIM == NULL)
    {
        return RET_ERROR;
    }
    /* Disable the LPTIM Peripheral Clock */
    IO_BIT_CLEAR(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* DeInit the low level hardware: CLOCK, NVIC.*/
    NVIC_DisableIRQ(ETM3_IRQn);

    return RET_OK;
}

RET_CODE LPTIM_StartPWM(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t pulse)
{             
    /* Reset WAVE bit to set PWM mode */
    pLPTIM->CFGR &= ~LPTIM_CFGR_WAVE;

    /* Enable the Peripheral */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Load the period value in the autoreload register */
    pLPTIM->ARR = period;

    /* Load the pulse value in the compare register */
    pLPTIM->CMP = pulse;

    /* Start timer in continuous mode */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_CNTSTRT);

    return RET_OK;
}

RET_CODE LPTIM_StopPWM(LPTIM_Device_T *pLPTIM)
{            
    /* Disable the Peripheral */
    IO_BIT_CLEAR(pLPTIM->CR, LPTIM_CR_ENABLE);

    return RET_OK;
}

RET_CODE LPTIM_StartOnePulse(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t pulse)
{           
    /* Reset WAVE bit to set one pulse mode */
    pLPTIM->CFGR &= ~LPTIM_CFGR_WAVE;

    /* Enable the Peripheral */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Load the period value in the autoreload register */
    pLPTIM->ARR = period;

    /* Load the pulse value in the compare register */
    pLPTIM->CMP = pulse;

    /* Start timer in single mode */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_SNGSTRT);

    return RET_OK;
}

RET_CODE LPTIM_StopOnePulse(LPTIM_Device_T *pLPTIM)
{            
    /* Disable the Peripheral */
    IO_BIT_CLEAR(pLPTIM->CR, LPTIM_CR_ENABLE);

    return RET_OK;
}

RET_CODE LPTIM_StartSetOnce(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t pulse)
{          
    /* Set WAVE bit to enable the set once mode */
    pLPTIM->CFGR |= LPTIM_CFGR_WAVE;

    /* Enable the Peripheral */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Load the period value in the autoreload register */
    pLPTIM->ARR = period;

    /* Load the pulse value in the compare register */
    pLPTIM->CMP = pulse;

    /* Start timer in single mode */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_SNGSTRT);

    return RET_OK;
}

RET_CODE LPTIM_StopSetOnce(LPTIM_Device_T *pLPTIM)
{              
    /* Disable the Peripheral */
    IO_BIT_CLEAR(pLPTIM->CR, LPTIM_CR_ENABLE);

    return RET_OK;
}

RET_CODE LPTIM_StartEncoder(LPTIM_Device_T *pLPTIM, uint16_t period)
{
    uint32_t tmpcfgr = 0U;

    /* Configure edge sensitivity for encoder mode */
    /* Get the LPTIMx CFGR value */
    tmpcfgr = pLPTIM->CFGR;

    /* Clear CKSEL, PRESC bits */
    tmpcfgr &= (uint32_t)(~(LPTIM_CFGR_EXTCLK | LPTIM_CFGR_PRESC));
    
    /* Clear CKPOL bits */
    //tmpcfgr &= (uint32_t)(~LPTIM_CFGR_CKPOL);

    /* Set Input polarity */
    //tmpcfgr |=  hlptim->Init.UltraLowPowerClock.Polarity;

    /* Write to LPTIMx CFGR */
    pLPTIM->CFGR = tmpcfgr;

    /* Set ENC bit to enable the encoder interface */
    pLPTIM->CFGR |= LPTIM_CFGR_ENC;

    /* Enable the Peripheral */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Load the period value in the autoreload register */
    pLPTIM->ARR = period;

    /* Start timer in continuous mode */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_CNTSTRT);

    return RET_OK;
}

RET_CODE LPTIM_StopEncoder(LPTIM_Device_T *pLPTIM)
{           
    /* Disable the Peripheral */
    IO_BIT_CLEAR(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Reset ENC bit to disable the encoder interface */
    pLPTIM->CFGR &= ~LPTIM_CFGR_ENC;

    return RET_OK;
}

RET_CODE LPTIM_StartTimeOut(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t timeout)
{        
    /* Set TIMOUT bit to enable the timeout function */
    pLPTIM->CFGR |= LPTIM_CFGR_TIMOUT;

    /* Enable the Peripheral */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Load the period value in the autoreload register */
    pLPTIM->ARR = period;

    /* Load the Timeout value in the compare register */
    pLPTIM->CMP = timeout;

    /* Start timer in continuous mode */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_CNTSTRT);

    return RET_OK;
}

RET_CODE LPTIM_StopTimeOut(LPTIM_Device_T *pLPTIM)
{
    /* Disable the Peripheral */
    IO_BIT_CLEAR(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Disable the timeout function */
    pLPTIM->CFGR &= ~LPTIM_CFGR_TIMOUT;

    return RET_OK;
}

RET_CODE LPTIM_StartCounter(LPTIM_Device_T *pLPTIM, uint16_t period)
{          
    /* Enable the Peripheral */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_ENABLE);

    /* Load the period value in the autoreload register */
    pLPTIM->ARR = period;

    /* Start timer in continuous mode */
    IO_BIT_SET(pLPTIM->CR, LPTIM_CR_CNTSTRT);

    return RET_OK;
}

RET_CODE LPTIM_StopCounter(LPTIM_Device_T *pLPTIM)
{
    /* Disable the Peripheral */
    IO_BIT_CLEAR(pLPTIM->CR, LPTIM_CR_ENABLE);
    
    return RET_OK;
}

void LPTIM_EnableInterrupts(LPTIM_Device_T *pLPTIM, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pLPTIM->IER;

    pLPTIM->IER = interruptMask | mask;
}

void LPTIM_DisableInterrupts(LPTIM_Device_T *pLPTIM, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pLPTIM->IER;

    pLPTIM->IER = interruptMask & (~ mask);
}

void LPTIM_IRQHandler(void)
{
    LPTIM_CommonIRQHandler(LPTIM, s_LPTIMHandle);
}


