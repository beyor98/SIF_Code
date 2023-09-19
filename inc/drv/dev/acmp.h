/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          acmp_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2016/07/14
 *
 * @brief         Header file of acmp driver.
 *
 * @note
 *    2016/07/14, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/11/22, Mike.Zheng, V1.1.0
 *        Update For Release.
 */

#ifndef _ACMP_H_
#define _ACMP_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <common.h>

/**
 * Compare result after positivepin and negativepin compare voltage
 */
typedef enum
{
  ACMP_LOW                      = 0x00,             /**< Positivepin voltage is no more than negativepin voltage */
  ACMP_HIHG                           ,             /**< Positivepin voltage is more than negativepin voltage */
} ACMP_CompareResult_T;

/**
 *  ACMP events
 */
typedef enum
{
  ACMP_MATCH_EVENT              = 0x00,             /**< When positivepin and negativepin match voltage mode which is
                                                     *   by CompareMatchTrigger mode,it will send event to usr
                                                     */
} ACMP_Event_T;

/**
 *  Compare match trigger mode
 */
typedef enum
{
    ANALOG_MATCH_FALLING        = 0x00,             /**< At begining, positivepin voltage is more than negativepin voltage,
                                                     *   sometime later ,when positivepin voltage get the same voltage as negativepin
                                                     *   it will send this event to usr
                                                     */

    ANALOG_MATCH_RISING         = 0x01,             /**< At begining, positivepin voltage is less than negativepin voltage,
                                                     *   sometime later ,when positivepin voltage get the same voltage as negativepin
                                                     *   it will send this event to usr
                                                     */

    ANALOG_MATCH_FALLINGRISING  = 0x03              /**< The case both ANALOG_MATCH_FALLING and ANALOG_MATCH_RISING will sent event to usr */
} ACMP_CompareMatchTrigger_T;

/**
 *  Agalog compare inputpin
 */
typedef enum
{
    ANALOG_COMPARE_PIN0         = 0x00,             /**< Select agalog compare inputpin0 */
    ANALOG_COMPARE_PIN1               ,             /**< Select agalog compare inputpin1 */
    ANALOG_COMPARE_PIN2               ,             /**< Select agalog compare inputpin2 */
    ANALOG_COMPARE_DAC                              /**< Select agalog compare dacoutpin */
} ACMP_AnalogComparePin_T;




#if defined(CHIP_GM6621)
typedef enum
{
	ACMP_DEV_0							= 0x00,
	ACMP_DEV_1
} ACMP_DevIndex_T;

typedef enum
{
	DEBUG_LOW_LEVEL 				   = 0x00,
	DEBUG_HIGH_LEVEL
} ACMP_DebugOutputLevel_T;

typedef enum
{
    ACMP_TRI_SEL_NONE                  = 0x0,
    ACMP_TRI_SEL_CPU                   = 0x1,
    ACMP_TRI_SEL_RTC                   = 0x2,
    ACMP_TRI_SEL_PES_RTC
} ACMP_TrigerSelect_T;

typedef enum
{
	APP_STOP_MODE 				   = 0x00,
	APP_NORMAL_MODE                = 0x01,
	APP_STANDBY_MODE               = 0x02,
} ACMP_AppMode_T;

#endif

/**
 *  DAC refer voltage
 */
typedef enum
{
    DAC_VREF  =  0,                                /**< DAC select vref as refer voltage , value is 1.2 v */
    DAC_VDD50                                      /**< DAC select Vdd50 as refer voltage , value is 5 v */
} ACMP_DacReferVoltage_T;

/**
 *  Usr DAC parameter config
 */
typedef struct
{
    uint8_t                 dacEnbale;             /**< Enable:enable dac convert  Disable:disenable dac convert */
    uint8_t                 dacValue;              /**< DAC convert step value : maxval:63  minval:0
                                                    *  selects the dac output voltage using the given formula
                                                    *   voutput=(vin/64)x(dacvalue+1)
                                                    *   output range is from vin/64 to Vin, the step is vin/64
                                                    */
#if !(defined(CHIP_F6601C))
    ACMP_DacReferVoltage_T referVoltage;           /**< DAC refer voltage */
#endif
}ACMP_DacConfig_T;

/**
 * ACMP user handle structure.
 */
typedef struct
{
    ACMP_Device_T   *device;                       /**< Pointer to acmp device */

} ACMP_Handle_T;

typedef void (*ACMP_Callback_T) (ACMP_Handle_T *hACMP, ACMP_Event_T event, unsigned long Param);


typedef struct
{
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))

    uint8_t                     outputInversionEnable;   /**< Enable: compare result will inversion
                                                          *   Disable: compare result will not inversion
                                                          */

    uint8_t                     trigerAdcEnable;        /**<  Enable: ACMP triger ADC
                                                         *    Disable: ACMP not triger ADC
                                                         */
    uint16_t                    filterThr;              /**<  Config timing threshold value for the filter of analog comparator */

#endif

#if defined(CHIP_GM6621)
    ACMP_DevIndex_T             devIndex;
    uint8_t 	                batteryEn;
    ACMP_TrigerSelect_T         trigerSource;
#endif    
    uint8_t                     outputPinEnable;   /**< Enable:enable extoutpin  Disable:disable extoutpin */
    ACMP_DacConfig_T            dacConfig;         /**< Config dac convert parameter */
    ACMP_AnalogComparePin_T     negativePin;       /**< Select analog negative pin */
    ACMP_AnalogComparePin_T     positivePin;       /**< Select analog positive pin */
    ACMP_CompareMatchTrigger_T  triggerMode;       /**< Inputpin voltage match trigger mode */
    ACMP_Callback_T             callback;          /**< Driver send usr event to call this function */
#if defined(CHIP_GM6621)
    int standby_flag ;
#endif

}ACMP_Config_T;

/**
 * @brief Initializes ACMP device with user configuration
 *
 * This function configures ADC device with the user-defined settings.
 *
 * @param  pACMP                ACMP device point
 * @param  pConfig              pointer to user-defined configuration.
 * @param  hACMP                pointer to ACMP handle.
 *
 * @retval   RET_OK             ACMP_Init success
 * @retval   RET_ERROR          ACMP_Init error
 * @retval   RET_INVPARAM       Usr input paramter is error
 */
RET_CODE ACMP_Init(ACMP_Device_T *pACMP, ACMP_Config_T *pConfig, ACMP_Handle_T *hACMP);

/**
 * @brief Get Rhe ACMP device to compare the voltage between negativepn and positivepin
 *
 * This function is used for get compare result between negativePin and positivepin.
 *
 * @param hACMP Pointer to ACMP handle.
 *
 * @retval ACMP_LOW            Positivepin voltage is no more than negativepin voltage
 * @retval ACMP_HIHG           Positivepin voltage is more than negativepin voltage
 */
ACMP_CompareResult_T ACMP_GetCompareRestult(ACMP_Handle_T *hACMP);

#ifdef __cplusplus
}
#endif

#endif /*_ACMP_H_*/


