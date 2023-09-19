/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          lptim.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/06/05 
 *    
 * @brief         Low Power Timer peripheral PHY device header file
 * 
 * @note
 *    2017/06/05, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _LPTIM_H_
#define _LPTIM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * Low Power Timer pre-scaler factor selection for the clock source.
 */
typedef enum 
{
    LPTIM_PSC_DIV_1 = 0U,    /**< Divide by 1 */
    LPTIM_PSC_DIV_2,         /**< Divide by 2 */
    LPTIM_PSC_DIV_4,         /**< Divide by 4 */
    LPTIM_PSC_DIV_8,         /**< Divide by 8 */
    LPTIM_PSC_DIV_16,        /**< Divide by 16 */
    LPTIM_PSC_DIV_32,        /**< Divide by 32 */
    LPTIM_PSC_DIV_64,        /**< Divide by 64 */
    LPTIM_PSC_DIV_128        /**< Divide by 128 */
} LPTIM_ClockPrescale_T;


/**
 * List of LPTIM interrupts.
 */
typedef enum 
{
    LPTIM_IT_DOWN    = (1U << 6),      /**< Counter direction change up to down Interrupt Enable */
    LPTIM_IT_UP      = (1U << 5),      /**< Counter direction change down to up Interrupt Enable */
    LPTIM_IT_ARROK   = (1U << 4),      /**< Autoreload register update OK Interrupt Enable */
    LPTIM_IT_CMPOK   = (1U << 3),      /**< Compare register update OK Interrupt Enable */
    LPTIM_IT_EXTTRIG = (1U << 2),      /**< External trigger edge event Interrupt Enable */
    LPTIM_IT_ARRM    = (1U << 1),      /**< Autoreload match Interrupt Enable */
    LPTIM_IT_CMPM    = (1U << 0),      /**< Compare match Interrupt Enable */
} LPTIM_Interrupt_T;

/** @defgroup LPTIM_Output_Polarity Output polarity
 * @{
 */
#define LPTIM_OUTPUTPOLARITY_LOW               ((uint32_t)0x00000000U)
#define LPTIM_OUTPUTPOLARITY_HIGH              (LPTIM_CFGR_WAVPOL)
/**
 * @}
 */

/** @defgroup LPTIM_Updating_Mode Updating mode
 * @{
 */
#define LPTIM_UPDATE_IMMEDIATE                  ((uint32_t)0x00000000U)
#define LPTIM_UPDATE_ENDOFPERIOD                LPTIM_CFGR_PRELOAD
/**
 * @}
 */

/** @defgroup LPTIM_Counter_Source Counter source
 * @{
 */
#define LPTIM_COUNTERSOURCE_INTERNAL            ((uint32_t)0x00000000U)
#define LPTIM_COUNTERSOURCE_EXTERNAL            LPTIM_CFGR_COUNTMODE
/**
 * @}
 */
  

/** @defgroup LPTIM_Clock_Sample_Time Clock sample time
 * @{
 */
#define LPTIM_CLOCKSAMPLETIME_DIRECTTRANSITION ((uint32_t)0x00000000U)
#define LPTIM_CLOCKSAMPLETIME_2TRANSITIONS     ((uint32_t)0x00000001U)
#define LPTIM_CLOCKSAMPLETIME_4TRANSITIONS     ((uint32_t)0x00000002U)
#define LPTIM_CLOCKSAMPLETIME_8TRANSITIONS     ((uint32_t)0x00000003U)
/**
 * @}
 */

/** @defgroup LPTIM_Clock_Polarity Clock polarity
 * @{
 */
#define LPTIM_CLOCKPOLARITY_RISING              ((uint32_t)0x00000000U)
#define LPTIM_CLOCKPOLARITY_FALLING             ((uint32_t)0x00000001U)
#define LPTIM_CLOCKPOLARITY_RISING_FALLING      ((uint32_t)0x00000002U)
/**
 * @}
 */

/** @defgroup LPTIM_External_Trigger_Polarity Trigger polarity
 * @{
 */
#define LPTIM_ACTIVEEDGE_RISING                ((uint32_t)0x00000001U)
#define LPTIM_ACTIVEEDGE_FALLING               ((uint32_t)0x00000002U)
#define LPTIM_ACTIVEEDGE_RISING_FALLING        ((uint32_t)0x00000003U)
/**
 * @}
 */

/** @defgroup LPTIM_Trigger_Sample_Time Trigger sample time
 * @{
 */
#define LPTIM_TRIGSAMPLETIME_DIRECTTRANSITION   ((uint32_t)0x00000000U)
#define LPTIM_TRIGSAMPLETIME_2TRANSITIONS       ((uint32_t)0x00000001U)
#define LPTIM_TRIGSAMPLETIME_4TRANSITIONS       ((uint32_t)0x00000002U)
#define LPTIM_TRIGSAMPLETIME_8TRANSITIONS       ((uint32_t)0x00000003U)
/**
 * @}
 */
  
/** @defgroup LPTIM_Trigger_Source Trigger source
 * @{
 */
#define LPTIM_TRIGSOURCE_SOFTWARE               ((uint32_t)0x0000FFFFU)
#define LPTIM_TRIGSOURCE_EXTIO                  ((uint32_t)0x00000000U)
#define LPTIM_TRIGSOURCE_RTCOUT                 ((uint32_t)0x00000001U)
#define LPTIM_TRIGSOURCE_RTCTAMP1               ((uint32_t)0x00000003U)
#define LPTIM_TRIGSOURCE_RTCTAMP2               ((uint32_t)0x00000004U)
#define LPTIM_TRIGSOURCE_ACMP0OUT               ((uint32_t)0x00000006U)
#define LPTIM_TRIGSOURCE_ACMP1OUT               ((uint32_t)0x00000007U)
/**
 * @}
 */

/**
 * LPTIM IRQ callback typedef.
 */
typedef void (*LPTIM_IRQCallback_T) (LPTIM_Device_T *pLPTIM, uint32_t event, uint32_t param);

/** 
  * @brief  LPTIM ULP Clock configuration definition
  */
typedef struct
{
  uint32_t polarity;      /**< Selects the polarity of the active edge for the counter unit
                           if the ULPTIM input is selected.
                           Note: This parameter is used only when Ultra low power clock source is used.
                           Note: If the polarity is configured on 'both edges', an auxiliary clock
                           (one of the Low power oscillator) must be active.
                           This parameter can be a value of @ref LPTIM_Clock_Polarity */ 
  
  uint32_t sampleTime;     /**< Selects the clock sampling time to configure the clock glitch filter.
                           Note: This parameter is used only when Ultra low power clock source is used.
                           This parameter can be a value of @ref LPTIM_Clock_Sample_Time */  
  
}LPTIM_ULPClockConfig;


/** 
  * @brief  LPTIM Trigger configuration structure
  */
typedef struct
{
    uint32_t source;        /**< Selects the Trigger source.
                                 This parameter can be a value of @ref LPTIM_Trigger_Source */
    uint32_t activeEdge;    /**< Selects the Trigger active edge.
                                 Note: This parameter is used only when an external trigger is used.
                                 This parameter can be a value of @ref LPTIM_External_Trigger_Polarity */
    uint32_t sampleTime;    /**< Selects the trigger sampling time to configure the clock glitch filter.
                                 Note: This parameter is used only when an external trigger is used.
                                 This parameter can be a value of @ref LPTIM_Trigger_Sample_Time  */  
}LPTIM_TriggerConfig;


/** 
  * @brief  LPTIM Initialization Structure definition  
  */
typedef struct
{ 
    uint8_t    extClock;                       /**< True: select the external clock; False: select internal clock */
    uint32_t   prescaler;                      /**< Specifies the counter clock Prescaler.
                                                    This parameter can be a value of @ref LPTIM_ClockPrescale_T */
    uint32_t   outputPolarity;                 /**< Specifies the Output polarity.
                                                    This parameter can be a value of @ref LPTIM_Output_Polarity */ 
    uint32_t   updateMode;                     /**< Specifies whether the update of the autorelaod and the compare
                                                    values is done immediately or after the end of current period.
                                                This parameter can be a value of @ref LPTIM_Updating_Mode */
    uint32_t   counterSource;                  /**< Specifies whether the counter is incremented each internal event
                                                    or each external event. This parameter can be a value of @ref LPTIM_Counter_Source */ 
    LPTIM_ULPClockConfig  UltraLowPowerClock;  /**< Specifies the Ultra Low Power clock parameters */                                             
    LPTIM_TriggerConfig   trigger;             /**< Specifies the Trigger parameters */ 
    LPTIM_IRQCallback_T   callback;            /**< Callback pointer to user callback function. */

}LPTIM_Config_T;

/**
 * LPTIM user handle structure.
 */
typedef struct
{
    LPTIM_Device_T *pDevice;          /**< LPTIM peripheral base address */
    LPTIM_IRQCallback_T callback;     /**< Callback pointer to user callback function */
} LPTIM_Handle_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief Initializes the LPTIM according to the specified parameters in the
 *         LPTIM_Config_T.
 *
 * @param pLPTIM    LPTIM peripheral base address.
 * @param pConfig   Pointer to the configuration structure.
 * @param pHandle   pointer to LPTIM_Handle_T structure.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_Init(LPTIM_Device_T *pLPTIM, const LPTIM_Config_T *pConfig, LPTIM_Handle_T *pHandle);

/**
 * @brief  DeInitializes the LPTIM peripheral. 
 * @param  pLPTIM    LPTIM peripheral base address.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_DeInit(LPTIM_Device_T *pLPTIM);

/**
 * @brief  Starts the LPTIM PWM generation.
 * @param  pLPTIM   LPTIM peripheral base address.
 * @param  period   Specifies the Autoreload value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @param  pulse    Specifies the compare value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StartPWM(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t pulse);

/**
 * @brief  Stops the LPTIM PWM generation.
 * @param  pLPTIM LPTIM peripheral base address.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StopPWM(LPTIM_Device_T *pLPTIM);

/**
 * @brief  Starts the LPTIM One pulse generation.
 * @param  pLPTIM   LPTIM peripheral base address.
 * @param  period   Specifies the Autoreload value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @param  pulse    Specifies the compare value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StartOnePulse(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t pulse);

/**
 * @brief  Stops the LPTIM One pulse generation.
 * @param  pLPTIM LPTIM peripheral base address.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StopOnePulse(LPTIM_Device_T *pLPTIM);

/**
 * @brief  Starts the LPTIM in Set once mode.
 * @param  pLPTIM   LPTIM peripheral base address.
 * @param  period   Specifies the Autoreload value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @param  pulse    Specifies the compare value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StartSetOnce(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t pulse);

/**
 * @brief  Stops the LPTIM Set once mode.
 * @param  pLPTIM LPTIM peripheral base address.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StopSetOnce(LPTIM_Device_T *pLPTIM);

/**
 * @brief  Starts the Encoder interface.
 * @param  pLPTIM   LPTIM peripheral base address.
 * @param  period   Specifies the Autoreload value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StartEncoder(LPTIM_Device_T *pLPTIM, uint16_t period);

/**
 * @brief  Stops the Encoder interface.
 * @param  pLPTIM LPTIM peripheral base address.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StopEncoder(LPTIM_Device_T *pLPTIM);

/**
 * @brief  Starts the Timeout function. The first trigger event will start the
 *         timer, any successive trigger event will reset the counter and
 *         the timer restarts.
 * @param  pLPTIM   LPTIM peripheral base address.
 * @param  period   Specifies the Autoreload value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @param  timeout  Specifies the TimeOut value to rest the counter.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StartTimeOut(LPTIM_Device_T *pLPTIM, uint16_t period, uint16_t timeout);

/**
 * @brief  Stops the Timeout function.
 * @param  pLPTIM LPTIM peripheral base address.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StopTimeOut(LPTIM_Device_T *pLPTIM);

/**
 * @brief  Starts the Counter mode.
 * @param  pLPTIM   LPTIM peripheral base address.
 * @param  period   Specifies the Autoreload value.
 *                  This parameter must be a value between 0x0000 and 0xFFFF.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StartCounter(LPTIM_Device_T *pLPTIM, uint16_t period);

/**
 * @brief  Stops the Counter mode.
 * @param  pLPTIM LPTIM peripheral base address.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE LPTIM_StopCounter(LPTIM_Device_T *pLPTIM);

/**
 * @brief Enables the selected LPTIM interrupts.
 *
 * @param pLPTIM LPTIM peripheral base address.
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             @ref LPTIM_Interrupt_T
 * @retval None
 */
void LPTIM_EnableInterrupts(LPTIM_Device_T *pLPTIM, uint32_t mask);

/**
 * @brief Disables the selected LPTIM interrupts.
 *
 * @param pLPTIM LPTIM peripheral base address.
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *            @ref LPTIM_Interrupt_T
 * @retval None
 */
void LPTIM_DisableInterrupts(LPTIM_Device_T *pLPTIM, uint32_t mask);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _LPTIM_H_ */

