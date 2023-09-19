/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          etm.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/06/21
 *
 * @brief         ETM peripheral PHY device header file
 *
 * @note
 *    2016/06/21, mark.lee, V1.0.0
 *        Initial version.
 *    2019/09/07, edward.zhu, V1.1.0
 *        Initial version.
 *    2021/08/26, chenliwei, V1.1.0
 *        Initial version.
 *
 */
#ifndef _ETM_H_
#define _ETM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <drv/dev/dma.h>

#if !defined(CHIP_F6621B)
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * List of ETM channels.
 */
typedef enum
{
    ETM_CHANNEL_1 = 0U,    /**< ETM channel number 1 */
    ETM_CHANNEL_2,         /**< ETM channel number 2 */
    ETM_CHANNEL_3,         /**< ETM channel number 3 */
    ETM_CHANNEL_4          /**< ETM channel number 4 */
} ETM_Channel_T;

#if defined(CHIP_GM6621)
/**
 * Trigger condition mode setting.
 */
typedef enum
{
    ETM_PWM_TRG_NONE = 0U,    /**< Bypass TC_IN port input */
    ETM_PWM_TRG_START,        /**< Trigger start mode */
    ETM_PWM_TRG_START_STOP,   /**< Trigger start and stop mode */
    ETM_PWM_LEVEL_HDL         /**< Level handle mode */
} ETM_PWMTriggerMode_T;
#endif

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
/**
 * Trigger condition mode setting.
 */
typedef enum
{
    ETM_PWM_TRG_NONE = 0x0,    /**< Disable Break mode */
    ETM_PWM_TRG_TCIN = 0x1,    /**< Break source from TC_IN port */
    ETM_PWM_TRG_ACMP = 0x2,    /**< Break source from ACMP */
    ETM_PWM_TRG_LOCKUP = 0x4,  /**< Break source from system lockup */
    ETM_PWM_TRG_ADC = 0x8,     /**< Break source from ADC */
    ETM_PWM_TRG_HOSC = 0x10,   /**< Break source from HOSC loss */
} ETM_PWMTriggerMode_T;

#elif defined(CHIP_GM6611B)
typedef enum
{
    ETM_PWM_TRG_TCIN = 0x0,         /**< Break source from TC_IN port */
    ETM_PWM_TRG_ACMP = 0x1,         /**< Break source from ACMP */
    ETM_PWM_TRG_ADC = 0x2,          /**< Break source from ADC */
    ETM_PWM_TRG_LOCKUP = 0x4,       /**< Break source from system lockup */
    ETM_PWM_TRG_DEEPSTOP = 0x8,     /**< Break source from DEEP STOP mode */
    ETM_PWM_TRG_STOPMODE = 0x10,    /**< Break source from STOP MODE */
} ETM_PWMTriggerMode_T;
#endif


#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
/** 
 * Timer pre-scaler factor selection for the clock source.
 */
typedef enum
{
    ETM_PSC_DIV_2 = 0U,    /**< Divide by 2 */
    ETM_PSC_DIV_4,         /**< Divide by 4 */
    ETM_PSC_DIV_8,         /**< Divide by 8 */
    ETM_PSC_DIV_16,        /**< Divide by 16 */
    ETM_PSC_DIV_32,        /**< Divide by 32 */
    ETM_PSC_DIV_64,        /**< Divide by 64 */
    ETM_PSC_DIV_128,       /**< Divide by 128 */
    ETM_PSC_DIV_256        /**< Divide by 256 */
} ETM_ClockPrescale_T;
#endif

/**
 * Time base counter mode setting.
 */
typedef enum
{
    ETM_COUNT_DOWN = 0U,    /**< Time base counter in down mode */
    ETM_COUNT_UP,           /**< Time base counter in up mode */
    ETM_COUNT_UPDOWN,       /**< Time base counter in up-down mode */
    ETM_COUNT_INVALID       /**< Reserved */
} ETM_CountMode_T;

/**
 * List of ETM interrupts.
 */
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
typedef enum
{
    ETM_INT_CNT_START     = (1U << 0),      /**< Counter start interrupt enable */
    ETM_INT_CNT_OVERFLOW  = (1U << 1),      /**< Counter overflow interrupt enable */
    ETM_INT_CNT_UNDERFLOW = (1U << 2),      /**< Counter underflow interrupt enable */
    ETM_INT_CNT_MATCHED1  = (1U << 3),      /**< Counter duty1 match interrupt enable */
    ETM_INT_CNT_MATCHED2  = (1U << 4),      /**< Counter duty2 match interrupt enable */
    ETM_INT_EDGE_P1       = (1U << 5),      /**< Capture channel1 positive edge interrupt enable */
    ETM_INT_EDGE_N1       = (1U << 6),      /**< Capture channel1 negative edge interrupt enable */
    ETM_INT_EDGE_P2       = (1U << 7),      /**< Capture channel2 positive edge interrupt enable */
    ETM_INT_EDGE_N2       = (1U << 8),      /**< Capture channel2 negative edge interrupt enable */
    ETM_INT_SYNC          = (1U << 13),     /**< Sync event received interrupt enable */
    ETM_INT_BREAKIN       = (1U << 14)      /**< Break in interrupt enable */
} ETM_Interrupt_T;
#endif

#if defined(CHIP_GM6621)
typedef enum
{
    ETM_INT_CNT_START     = (1U << 0),      /**< Counter start interrupt enable */
    ETM_INT_CNT_OVERFLOW  = (1U << 1),      /**< Counter overflow interrupt enable */
    ETM_INT_CNT_UNDERFLOW = (1U << 2),      /**< Counter underflow interrupt enable */
    ETM_INT_CNT_MATCHED1  = (1U << 3),      /**< Counter duty1 match interrupt enable */
    ETM_INT_CNT_MATCHED2  = (1U << 4),      /**< Counter duty2 match interrupt enable */
    ETM_INT_CNT_MATCHED3  = (1U << 5),      /**< Counter duty3 match interrupt enable */
    ETM_INT_CNT_MATCHED4  = (1U << 6),      /**< Counter duty4 match interrupt enable */
    ETM_INT_EDGE_P1       = (1U << 8),      /**< Capture channel1 positive edge interrupt enable */
    ETM_INT_EDGE_N1       = (1U << 9),      /**< Capture channel1 negative edge interrupt enable */
    ETM_INT_EDGE_P2       = (1U << 10),     /**< Capture channel2 positive edge interrupt enable */
    ETM_INT_EDGE_N2       = (1U << 11),     /**< Capture channel2 negative edge interrupt enable */
    ETM_INT_EDGE_P3       = (1U << 12),     /**< Capture channel3 positive edge interrupt enable */
    ETM_INT_EDGE_N3       = (1U << 13),     /**< Capture channel3 negative edge interrupt enable */
    ETM_INT_EDGE_P4       = (1U << 14),     /**< Capture channel4 positive edge interrupt enable */
    ETM_INT_EDGE_N4       = (1U << 15),     /**< Capture channel4 negative edge interrupt enable */
    ETM_DMA_IC1           = (1U << 16),     /**< Capture channel1 DMA request enable */
    ETM_DMA_IC2           = (1U << 17),     /**< Capture channel2 DMA request enable */
    ETM_DMA_IC3           = (1U << 18),     /**< Capture channel3 DMA request enable */
    ETM_DMA_IC4           = (1U << 19),     /**< Capture channel4 DMA request enable */
    ETM_INT_SYNC          = (1U << 23),     /**< Sync event received interrupt enable */
} ETM_Interrupt_T;
#define ETM_INT_BREAKIN       ((uint32_t)0x80000000U)      /**< Break in interrupt enable */
#endif

/**
 * ETM output waveform setting.
 */
typedef enum
{
    ETM_UNCHANGED = 0U,   /**< Remain unchanged  */
    ETM_LEVEL_SET,        /**< Set output */
    ETM_LEVEL_CLEAR,      /**< Clear output */
    ETM_LEVEL_TOGGLE      /**< Toggle output */
} ETM_LevelSetting_T;


/** @defgroup ETM_Output_Idle_State ETM Output Idle State
  * @{
  */
#define ETM_IDLESTATE_HIGH             1
#define ETM_IDLESTATE_LOW              0
/**
  * @}
  */

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
/** @defgroup ADC_external_trigger_edge
  * @{
  */
#define ETM_TRIG_ADC_EDGE_NONE           ((uint8_t)0x00)     /**<  ADC No external trigger for regular conversion */
#define ETM_TRIG_ADC_EDGE_RISING         ((uint8_t)0x01)     /**<  ADC external trigger rising edge for regular conversion */
#define ETM_TRIG_ADC_EDGE_FALLING        ((uint8_t)0x02)     /**<  ADC external trigger falling edge for regular conversion */
#define ETM_TRIG_ADC_EDGE_BOTHEDGE       ((uint8_t)0x03)     /**<  ADC ADC external trigger both edges for regular conversion */
/**
  * @}
  */
#endif

/**
 * Configure a ETM channel's PWM waveform.
 */
typedef struct
{
    uint8_t idleState;                    /**< Channel output by default, This parameter can be a value of @ref ETM_Output_Idle_State  */
    ETM_LevelSetting_T counterStart;      /**< Waveform setting of counter start */
    ETM_LevelSetting_T counterDown2;      /**< Waveform setting of duty register2 match when counts down  */
    ETM_LevelSetting_T counterDown1;      /**< Waveform setting of duty register1 match when counts down  */
    ETM_LevelSetting_T counterUp2;        /**< Waveform setting of duty register2 match when counts up  */
    ETM_LevelSetting_T counterUp1;        /**< Waveform setting of duty register1 match when counts up  */
#if defined(CHIP_GM6621)	
	ETM_LevelSetting_T counterDown4;      /**< Waveform setting of duty register4 match when counts down  */
    ETM_LevelSetting_T counterDown3;      /**< Waveform setting of duty register3 match when counts down  */
    ETM_LevelSetting_T counterUp4;        /**< Waveform setting of duty register4 match when counts up  */
    ETM_LevelSetting_T counterUp3;        /**< Waveform setting of duty register3 match when counts up  */
#endif	
    ETM_LevelSetting_T underFlow;         /**< Waveform setting of underflow  */
    ETM_LevelSetting_T overFlow;          /**< Waveform setting of overflow  */
} ETM_PWMConfig_T;

/**
 * List of ETM sync source.
 */
typedef enum
{
    ETM_SYNC_NONE = 0U,    /**< No sync pluse */
    ETM_SYNC_BY_SYS,       /**< The sync signal sourced by system */
    ETM_SYNC_BY_TIME0,     /**< The sync signal sourced by ETM0 */
    ETM_SYNC_CHAINED       /**< the ETM0 sync signal sourced by system; the ETM1 sync signal sourced by ETM0;
                             the ETM2 sync signal sourced by ETM1; the ETM3 sync signal sourced by ETM2 */
} ETM_SyncSource_T;

/**
 * ETM Synchronization setting.
 */
typedef struct
{
    uint8_t  countDir;       /**< True: count up; False: count down */
    uint8_t  eventDir;       /**< True: count up; False: count down */
    uint16_t startCount;     /**< The start value when pose come to load */
    uint16_t eventCount;     /**< send event cnt value */
    uint8_t  syncEn;         /**< True: sync enable */
} ETM_PWMSyncConfig_T;

/**
 * ETM IRQ callback typedef.
 */
typedef void (*ETM_IRQCallback_T)(ETM_Device_T *pETM, uint32_t event, unsigned long param);

#if defined(CHIP_GM6621)
/**
 * ETM OnePulse setting.
 */
typedef struct
{
    uint16_t period;                /**< The period value */
    uint16_t duty;                  /**< The duty match value */
    uint32_t polarity;              /**< Specifies the input polarity */
    uint8_t repetitionCounter;      /**< Specifies the repetition counter value. */
    ETM_IRQCallback_T callback;     /**< Callback pointer to user callback function. */
} ETM_OnePulseConfig_T;
#endif

/**
 * ETM configuration structure
 *
 * This structure holds the configuration settings for the ETM peripheral.
 *
 */
typedef struct
{
    uint8_t prescalerEn;            /**< True: enable prescaler; False: bypass prescaler */
    uint8_t extClock;               /**< True: select the external clock; False: select ETM_IP_CLK */
    uint16_t period;                /**< The period value */
    uint16_t deadTime;              /**< The dead time value */
    uint16_t prescaler;             /**< ETM clock prescale value */
    ETM_CountMode_T countMode;      /**< Time base counter mode setting */
    ETM_IRQCallback_T callback;     /**< Callback pointer to user callback function. */
    uint8_t repetitionCounter;      /**< Specifies the repetition counter value. */
} ETM_Config_T;

/**
 * ETM user handle structure.
 */
typedef struct
{
    ETM_Device_T      *pDevice;      /**< ETM peripheral base address */
    ETM_IRQCallback_T callback;      /**< Callback pointer to user callback function */

#if defined(CHIP_GM6621)
    DMA_Handle_T      *hDMA;         /**< DMA device handler */
#endif
} ETM_Handle_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief Sets counter trigger mode.
 *
 * @param pETM           ETM peripheral base address.
 * @param triggerMode     Trigger condition mode setting.
 * @param level          True: low level trigger; False: high level trigger.
 * @retval None
 */
void ETM_SetTriggerMode(ETM_Device_T *pETM, ETM_PWMTriggerMode_T triggerMode, uint8_t level);

/**
 * @brief  Synchronization setting.
 *
 * @param pETM       ETM peripheral base address.
 * @param countMode  select count mode; options are 0, 1, 2.
 * @param pConfig        Pointer to ETM_PWMSyncCfg_T structure.
 * @retval None
 */
void ETM_ConfigSync(ETM_Device_T *pETM, ETM_CountMode_T countMode, ETM_PWMSyncConfig_T *pConfig);

/**
 * @brief  Synchronization source setting.
 *
 * @param syncSource  select sync source.
 * @retval None
 */
void ETM_SetSyncSource(ETM_SyncSource_T syncSource);

/**
 * @brief Sets the duty value of an active PWM signal.
 *
 * @param pETM          ETM peripheral base address.
 * @param shadowMode    True: loading immediately; False: loading next cycle.
 * @param duty1         The duty1 match value.
 * @param duty2         The duty2 match value.
 * @retval None
 *
 */
void ETM_SetPWMDuty(ETM_Device_T *pETM, uint8_t shadowMode, uint16_t duty1, uint16_t duty2);

#if defined(CHIP_GM6621)
/**
 * @brief Sets the output compare value of an active PWM signal.
 *
 * @param pETM          ETM peripheral base address.
 * @param shadowMode    True: loading immediately; False: loading next cycle.
 * @param channel       The channel number.
 * @param pulse         Compare Register value.
 * @retval None
 *
 */
void ETM_OCConfig(ETM_Device_T *pETM, uint8_t shadowMode, ETM_Channel_T channel, uint16_t pulse);

#endif

/**
 * @brief Gets the capture value.
 *
 * @param pETM ETM peripheral base address.
 * @param channel      The channel number.
 *
 * @return the capture value.
 *
 */
uint16_t ETM_GetCaptureValue(ETM_Device_T *pETM, ETM_Channel_T channel);

/**
 * @brief a ETM channel's PWM waveform setting.
 *
 * @param pETM       ETM peripheral base address.
 * @param channel    The channel number
 * @param pConfig    Pointer to ETM_PWMOutputCfg_T structure.
 * @retval None
 */
void ETM_SetPWMChannel(ETM_Device_T *pETM, ETM_Channel_T channel, ETM_PWMConfig_T *pConfig);

/**
 * @brief Sets the channel software force output control value.
 *
 * @param pETM       ETM peripheral base address.
 * @param channel    The channel number.
 * @param level      force output level value.
 * @retval None
 */
void ETM_SetSoftwareForceOutput(ETM_Device_T *pETM, ETM_Channel_T channel, ETM_LevelSetting_T level);

/**
 * @brief Initializes the ETM time base unit.
 *
 * @param pETM    ETM peripheral base address.
 * @param pConfig      Pointer to the configuration structure.
 * @param pHandle  pointer to ETM_Handle_T structure.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE ETM_InitTimeBase(ETM_Device_T *pETM, const ETM_Config_T *pConfig, ETM_Handle_T *pHandle);

/**
 * @brief Starts the time base generation.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the time base start was successful;
 *         RET_TIMEOUT on failure..
 *
 */
RET_CODE ETM_StartTimeBase(ETM_Device_T *pETM);


/**
 * @brief Stops the time base generation.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the time base stop was successful;
 *         RET_TIMEOUT on failure.
 *
 */
RET_CODE ETM_StopTimeBase(ETM_Device_T *pETM);

/**
 * @brief Starts the Pulse signal generation.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the PWM start was successful;
 *         RET_TIMEOUT on failure..
 *
 */
RET_CODE ETM_StartPWM(ETM_Device_T *pETM);

/**
 * @brief Stops the Pulse signal generation.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the PWM stop was successful;
 *         RET_TIMEOUT on failure.
 *
 */
RET_CODE ETM_StopPWM(ETM_Device_T *pETM);

/**
 * @brief Initializes the ETM input capture.
 *
 * @param pETM    ETM peripheral base address.
 * @param pConfig      Pointer to the configuration structure.
 * @param pHandle  pointer to ETM_Handle_T structure.
 *
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE ETM_InitCapture(ETM_Device_T *pETM, const ETM_Config_T *pConfig, ETM_Handle_T *pHandle);

/**
 * @brief Starts the input capture measurement.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the capture start was successful;
 *         RET_TIMEOUT on failure..
 *
 */
RET_CODE ETM_StartCapture(ETM_Device_T *pETM);


/**
 * @brief Stops the input capture measurement.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the capture stop was successful;
 *         RET_TIMEOUT on failure.
 *
 */
RET_CODE ETM_StopCapture(ETM_Device_T *pETM);

#if defined(CHIP_GM6621)
/**
 * @brief  Starts the Input Capture measurement on in DMA mode.
 * @param  pETM    ETM peripheral base address.
 * @param  channel The channel number
 * @param  pData: The destination Buffer address.
 * @param  length: The length of data to be transferred from ETM peripheral to memory.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE ETM_StartCaptureByDMA(ETM_Handle_T *pETMHandle, ETM_Channel_T channel, uint32_t *pData, uint16_t length);

/**
 * @brief  Stops the Input Capture measurement on in DMA mode.
 * @param  pETM    ETM peripheral base address.
 * @param  channel The channel number
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE ETM_StopCaptureByDMA(ETM_Handle_T *pETMHandle, ETM_Channel_T channel);

/**
 * @brief Initializes the ETM one pulse.
 *
 * @param pETM       ETM peripheral base address.
 * @param pConfig    Pointer to ETM_OnePulseConfig_T structure.
  * @param pHandle  pointer to ETM_Handle_T structure.
 * @retval RET_OK indicates success; Else indicates failure.
 */
RET_CODE ETM_InitOnePulse(ETM_Device_T *pETM, ETM_OnePulseConfig_T *pConfig, ETM_Handle_T *pHandle);

/**
 * @brief Starts the One Pulse signal generation.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the capture start was successful;
 *         RET_TIMEOUT on failure..
 *
 */
RET_CODE ETM_StartOnePulse(ETM_Device_T *pETM);


/**
 * @brief Stops the One Pulse signal generation.
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK if the capture stop was successful;
 *         RET_TIMEOUT on failure.
 *
 */
RET_CODE ETM_StopOnePulse(ETM_Device_T *pETM);

/**
 * @brief Enable event input
 *
 * @param pETM   ETM peripheral base address.
 *
 * @return RET_OK indicates success; Else indicates failure.
 *
 */
void ETM_PESConfig(ETM_Device_T *pETM, uint8_t chIndex);

/**
 * @brief Enable ETM event output
 *
 * @param pETM   ETM peripheral base address
 * @param chIndex Enable event channel input.
 * @return RET_OK indicates success; Else indicates failure.
 *
 */
void ETM_asMasterConfig(ETM_Device_T *pETM);

/**
 * @brief Disable ETM event output
 *
 * @param pETM   ETM peripheral base address
 *
 * @return RET_OK indicates success; Else indicates failure.
 *
 */
void ETM_PESEVDisable(ETM_Device_T *pETM);
#endif

/**
 * @brief Enables the selected ETM interrupts.
 *
 * @param pETM ETM peripheral base address.
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *             @ref ETM_Interrupt_T
 * @retval None
 */
void ETM_EnableInterrupts(ETM_Device_T *pETM, uint32_t mask);

/**
 * @brief Disables the selected ETM interrupts.
 *
 * @param pETM ETM peripheral base address.
 * @param mask The interrupts to enable. This is a logical OR of members of the
 *            @ref ETM_Interrupt_T
 * @retval None
 */
void ETM_DisableInterrupts(ETM_Device_T *pETM, uint32_t mask);

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
/**
 * @brief a ETM Trigger ADC.
 *
 * @param pETM       ETM peripheral base address.
 * @param channel    The channel number
 * @param polarity    Trigger edge, This parameter can be a value of @ref ADC_external_trigger_edge.
 * @retval None
 */
void ETM_TriggerADC(ETM_Device_T *pETM, ETM_Channel_T channel,  uint8_t polarity);
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ETM_H_ */

