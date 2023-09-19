/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          irtx.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 *
 * @date          2017/04/05
 *
 * @brief         Header file of  irtx driver.
 *
 * @note
 *                2017/04/05 , Mike.Zheng, V1.0.0
 *                    Initial version.
 *                2018/02/28 , Edward.Zhu, V1.1.0
 *                    Update coding style.
 */

#ifndef _IRTX_H_
#define _IRTX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 *Declaration of IRTX handle structure.
 */
struct IRTX_Handle;

/**
 * General IRTX decode keyvalue success events
 */
typedef enum TX_EVENT
{
    IRTX_EVEN_SEND_SUCCESS = 1,            /**< IRTX decode success will send this event to user */
    IRTX_EVEN_TIME                         /**< IRTX decode failure */
} IRTX_Event_T;

/**
 * Gree Ir proticol type.
 */
typedef enum
{
    IRTX_PROTOCOL_F0             = 0x01,     /**< F0 Code proctcol */
    IRTX_PROTOCOL_F1,                        /**< F1 Code proctcol */
    IRTX_PROTOCOL_F2,                        /**< F2 Code proctcol */
    IRTX_PROTOCOL_F3,                        /**< F3 Code proctcol */
    IRTX_PROTOCOL_F4,                        /**< F4 Code proctcol */
    IRTX_PROTOCOL_F5,                        /**< F5 Code proctcol */
    IRTX_PROTOCOL_F10,                       /**< F10 Code proctcol */
    IRTX_PROTOCOL_FM,                        /**< FM Code proctcol */
    /* Rmote control YB0F(YBOFB) or YT1F(YT1FF) */
    IRTX_PROTOCOL_F0FM,                      /**< F0_100ms_FM Code proctcol */
    IRTX_PROTOCOL_F0F1,                      /**< F0_40ms_F1 Code proctcol */
    IRTX_PROTOCOL_F0F1FM,                    /**< F0_40ms_F1_100_FM Code proctcol */
    /* Rmote control YBOFB1 or YT1F1(YT1FF1) */
    IRTX_PROTOCOL_F0F2,                      /**< F0_40ms_F2 Code proctcol */
    IRTX_PROTOCOL_F0F2F5,                    /**< F0_40ms_F2_40ms_F5 Code proctcol */
    IRTX_PROTOCOL_F0F2F3,                    /**< F0_40ms_F2_40ms_F3 Code proctcol */
    IRTX_PROTOCOL_F0F1F2,                    /**< F0_40ms_F1_40ms_F2 Code proctcol */
    IRTX_PROTOCOL_F0F1F2F3,                  /**< F0_40ms_F1_40ms_F2_40ms_F3 Code proctcol */
    IRTX_PROTOCOL_F0F1F2F5,                  /**< F0_40ms_F1_40ms_F2_40ms_F5 Code proctcol */
    IRTX_PROTOCOL_F0F2F3F5,                  /**< F0_40ms_F2_40ms_F3_45ms_F5 Code proctcol */
    IRTX_PROTOCOL_F0F1F2F3F5                 /**< F0_40ms_F1_40ms_F2_40ms_F3_40ms_F5 Code proctcol */

} IRTX_Protocol_T;

/**
 * FN(F0 F1 F2 F3 F4 F5 F10) code protocol format, refer gree ir protoclo spec
 */
typedef struct
{
    uint32_t data0;          /**< FN protocol first data (low 28bit vaild) */
    uint32_t data1;          /**< FN protocol second data (low 28bit vaild) */
} IRTX_FormatFN_T;

/**
 * Definite IRTX FM code's  data format.
 */
typedef struct
{
    uint8_t data;                   /**< FM protocol data */
} IRTX_FormatFM_T;

/**
 * Definite the FNINTERVAL between F0 code and FM code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;       /**< keyval : F0  protocol wave */
    IRTX_FormatFM_T  FMCode;       /**< keyval : FM  protocol wave */
} IRTX_FormatF0FM_T;

/**
 * Definite the FNINTERVAL between F0 code and F1 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F1Code;        /**< keyval : F1  protocol wave */
} IRTX_FormatF0F1_T;

/**
 * Definite the FNINTERVAL among F0/1/M code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F1Code;        /**< keyval : F1  protocol wave */
    IRTX_FormatFM_T  FMCode;        /**< keyval : FM  protocol wave */
} IRTX_FormatF0F1FM_T;

/**
 * Definite the FNINTERVAL between F0 code and F2 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
} IRTX_FormatF0F2_T;

/**
 * Definite the FNINTERVAL among F0/2/5 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
    IRTX_FormatFN_T  F5Code;        /**< keyval : F5  protocol wave */
} IRTX_FormatF0F2F5_T;

/**
 * Definite the FNINTERVAL among F0/2/3 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
    IRTX_FormatFN_T  F3Code;        /**< keyval : F3  protocol wave */
} IRTX_FormatF0F2F3_T;

/**
 * Definite the FNINTERVAL among F0/2/3/5 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
    IRTX_FormatFN_T  F3Code;        /**< keyval : F3  protocol wave */
    IRTX_FormatFN_T  F5Code;        /**< keyval : F5  protocol wave */
} IRTX_FormatF0F2F3F5_T;

/**
 * Definite the FNINTERVAL among F0/1/2 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F1Code;        /**< keyval : F1  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
} IRTX_FormatF0F1F2_T;

/**
 * Definite the FNINTERVAL among F0/1/2/5 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F1Code;        /**< keyval : F1  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
    IRTX_FormatFN_T  F5Code;        /**< keyval : F5  protocol wave */
} IRTX_FormatF0F1F2F5_T;

/**
 * Definite the FNINTERVAL among F0/1/2/3 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F1Code;        /**< keyval : F1  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
    IRTX_FormatFN_T  F3Code;        /**< keyval : F3  protocol wave */
} IRTX_FormatF0F1F2F3_T;

/**
 * Definite the FNINTERVAL among F0/1/2/3/5 code.
 */
typedef struct
{
    IRTX_FormatFN_T  F0Code;        /**< keyval : F0  protocol wave */
    IRTX_FormatFN_T  F1Code;        /**< keyval : F1  protocol wave */
    IRTX_FormatFN_T  F2Code;        /**< keyval : F2  protocol wave */
    IRTX_FormatFN_T  F3Code;        /**< keyval : F3  protocol wave */
    IRTX_FormatFN_T  F5Code;        /**< keyval : F5  protocol wave */
} IRTX_FormatF0F1F2F3F5_T;

/**
 * Definite the keyval of FM/FN code wave.
 */
typedef struct
{
    IRTX_Protocol_T            protocol;            /**< Remote Control  protocol */
    union
    {
        IRTX_FormatFN_T          F0Code;              /**< keyval : F0  protocol wave */
        IRTX_FormatFN_T          F1Code;              /**< keyval : F1  protocol wave */
        IRTX_FormatFN_T          F2Code;              /**< keyval : F2  protocol wave */
        IRTX_FormatFN_T          F3Code;              /**< keyval : F3  protocol wave */
        IRTX_FormatFN_T          F4Code;              /**< keyval : F4  protocol wave */
        IRTX_FormatFN_T          F5Code;              /**< keyval : F5  protocol wave */
        IRTX_FormatFN_T          F10Code;             /**< keyval : F10 protocol wave */
        IRTX_FormatFM_T          FMCode;              /**< keyval : FM  protocol wave */
        IRTX_FormatF0FM_T        F0FMCode;            /**< keyval : F0_100ms_FM proctcol wave*/
        IRTX_FormatF0F1_T        F0F1Code;            /**< keyval : F0_40ms_F1 proctcol wave*/
        IRTX_FormatF0F1FM_T      F0F1FMCode;          /**< keyval : F0_40ms_F1_100ms_FM  protocol wave */
        IRTX_FormatF0F2_T        F0F2Code;            /**< keyval : F0_40ms_F2 protocol wave */
        IRTX_FormatF0F2F3_T      F0F2F3Code;          /**< keyval : F0_40ms_F2_40ms_F3 protocol wave */
        IRTX_FormatF0F1F2_T      F0F1F2Code;          /**< keyval : F0_40ms_F1_40ms_F2 protocol wave */
        IRTX_FormatF0F1F2F3_T    F0F1F2F3Code;        /**< keyval : F0_40ms_F1_40ms_F2_40ms_F3 protocol wave */
        IRTX_FormatF0F2F5_T      F0F2F5Code;          /**< keyval : F0_40ms_F2_40ms_F5 protocol wave */
        IRTX_FormatF0F1F2F5_T    F0F1F2F5Code;        /**< keyval : F0_40ms_F1_40ms_F2_40ms_F5 protocol wave */
        IRTX_FormatF0F2F3F5_T    F0F2F3F5Code;        /**< keyval : F0_40ms_F2_40ms_F3_40ms_F5 protocol wave */
        IRTX_FormatF0F1F2F3F5_T  F0F1F2F3F5Code;      /**< keyval : F0_40ms_F1_40ms_F2_40ms_F3_40ms_F5 protocol wave */

    } KeyCode;

} IRTX_KeyDesc_T;

/**
 * Definite IRTX's trigger mode.
 */
typedef enum
{
    IRTX_HW_CHANNEL0_TRIGGER     = 0x0,      /* Select channel0 issues the event to IRTX */
    IRTX_HW_CHANNEL1_TRIGGER,                /* Select channel1 issues the event to IRTX */
    IRTX_HW_CHANNEL2_TRIGGER,                /* Select channel2 issues the event to IRTX */
    IRTX_HW_CHANNEL3_TRIGGER,                /* Select channel3 issues the event to IRTX */
    IRTX_HW_CHANNEL4_TRIGGER,                /* Select channel4 issues the event to IRTX */
    IRTX_HW_CHANNEL5_TRIGGER,                /* Select channel5 issues the event to IRTX */
    IRTX_HW_CHANNEL6_TRIGGER,                /* Select channel6 issues the event to IRTX */
    IRTX_HW_CHANNEL7_TRIGGER,                /* Select channel7 issues the event to IRTX */
    IRTX_HW_CHANNEL8_TRIGGER,                /* Select channel8 issues the event to IRTX */
    IRTX_HW_CHANNEL9_TRIGGER,                /* Select channel9 issues the event to IRTX */
    IRTX_HW_CHANNEL10_TRIGGER,               /* Select channel10 issues the event to IRTX */
    IRTX_HW_CHANNEL11_TRIGGER,               /* Select channel11 issues the event to IRTX */
    IRTX_SW_TRIGGER                          /* Select software trigger mode */
} IRTX_TriggerMode_T;

/**
 * Definite IRTX work mode.
 */
typedef enum
{
    IRTX_IR                     = 0x0,      /* IRTX work mode */
    IRTX_MANUAL_IR              = 0x1,      /* Manual work mode */
    IRTX_TIME                   = 0x2,      /* Timer work mode */
} IRTX_WorkMode_T;

/**
 * Declaration of IRTX handle structure..
 */
typedef struct IRTX_Handle
{
    uint8_t                      lock ;            /**< IRTX Lockstus */
    IRTX_TriggerMode_T           triggerMode;       /**< IRTX's trigger mode */
    IRTX_TransferStatus_T        status;           /**< IRTX's transfer status */
    IRTX_BlockStatus_T           blockStatus;      /**< IRTX's block status */
    IRTX_WaveConfig_T            wave;             /**< IRTX's wave configuration */
    IRTX_Device_T                *device;          /**< Pointer to irtx device handle */
} IRTX_Handle_T;

/**
 * IRTX's IRQ callback typedef.
 */
typedef void (*IRTX_Callback_T)(IRTX_Handle_T *hIRTX, IRTX_Event_T event, unsigned long param);

/**
 * PWM configuration in Timer work mode.
 */
typedef struct
{
    uint32_t duty;     /* Duty value in PWM mode */
    uint32_t period;   /* Period value in PWM mode */
} IRTX_TimePwmConfig_T;

/**
 * IRTX configuration.
 */
typedef struct
{
    IRTX_WorkMode_T             workMode;             /* IRTX's work mode */
    IRTX_TriggerMode_T          triggerMode;           /* IRTX's trigger mode */
    IRTX_Callback_T             callback;             /* driver send usr event to call this function */
} IRTX_Config_T;

/**
 * PWM configuration in PWM work mode.
 */
typedef struct
{
    uint32_t duty;    /*config by headl reg */
    uint32_t period;  /*config by headh reg */
    uint8_t  startLevel;    /* PWM initial level */
} IRTX_PWMConfig_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief Initializes the IRTX's basic configuration.
 *
 * @param pIRTX      IRTX's peripheral base address.
 * @param pConfig    Pointer to the configuration structure.
 * @param hIRTX      pointer to IRTX_Handle_T structure.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE IRTX_Init(IRTX_Device_T *pIRTX, IRTX_Config_T *pConfig, IRTX_Handle_T *hIRTX);

/**
 * @brief IRTX module send IR signal.
 *
 * @param hIRTX      pointer to IRTX_Handle_T structure.
 * @param pKeyDesc   definite the keyval of FM/FN code wave
 * @param timeoutUs  use to sync machine for block or noblock mode to send key
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE IRTX_SendKey(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc, uint32_t timeoutUs);

/**
 * @brief Initializes the PWM paraments in PWM work mode.
 *
 * @param pIRTX      IRTX's peripheral base address.
 * @param pConfig    Pointer to the configuration structure.
 * @return None
 */
void IRTX_PwmConfig(IRTX_Handle_T *pIRTX, IRTX_PWMConfig_T *pConfig);

/**
 * @brief Initializes the Timer paraments in timer work mode.
 *
 * @param pIRTX      IRTX's peripheral base address.
 * @param cnt        counter value.
 * @return None
 */
void IRTX_TimeConfig(IRTX_Handle_T *pIRTX, uint32_t cnt);

/**
 * @brief Enable timer work mode.
 *
 * @param pIRTX      IRTX's peripheral base address.
 * @return None
 */
void IRTX_TimeEnable(IRTX_Handle_T *pIRTX);

/**
 * @brief Disable timer work mode.
 *
 * @param pIRTX      IRTX's peripheral base address.
 * @return None
 */
void IRTX_TimeDisable(IRTX_Handle_T *pIRTX);

/**
 * @brief Enable PWM work mode.
 *
 * @param pIRTX      IRTX's peripheral base address.
 * @return None
 */
void IRTX_PwmEnable(IRTX_Handle_T *pIRTX);

/**
 * @brief Disable PWM work mode.
 *
 * @param pIRTX      IRTX's peripheral base address.
 * @return None
 */
void IRTX_PwmDisable(IRTX_Handle_T *pIRTX);

#ifdef __cplusplus
}
#endif

#endif /*_IRTX_H_*/
