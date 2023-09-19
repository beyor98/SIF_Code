/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          hwir.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2016/07/07
 *
 * @brief         Header file of hwir driver.
 *
 * @note
 *    2016/03/15, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/07, Mike.Zheng, V1.1.0
 *        Update coding style.
 *    2016/11/23, Mike.Zheng, V1.2.0
 *        Update For Release.
 */

#ifndef _HWIR_H_
#define _HWIR_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <common.h>

#define DECODE_SUCCESS_MASK             ((uint32_t)1)

/**
 * General HWIR decode keyvalue success events
 */
typedef enum
{
    HWIR_DECODE_SUCCESS = 1,            /**< HWIR decode success will send this evnet to usr */
} HWIR_Event_T;

/**
 * Gree Ir proticol type.
 */
typedef enum
{
    PROTOCOL_F0             = 0x01,     /**< F0 Code proctcol */
    PROTOCOL_F1                   ,     /**< F1 Code proctcol */
    PROTOCOL_F2                   ,     /**< F2 Code proctcol */
    PROTOCOL_F3                   ,     /**< F3 Code proctcol */
    PROTOCOL_F4                   ,     /**< F4 Code proctcol */
    PROTOCOL_FM                         /**< FM Code proctcol */
} HWIR_Protocol_T;

/**
 * HWIR user handle structure.
 */
typedef struct
{
    HWIR_Device_T    *device;          /**< Pointer to HWIR device */
    HWIR_Protocol_T  prctocol;         /**< Ir protocl */
    HWIR_DevConfig_T *pDevCfg;         /**< Pointer Driver private config */
} HWIR_Handle_T;

/**
 * @brief Callback definition of HWIR interrupt handler to notify user an event with parameters.
 *
 * @param    hHWIR   Pointer to HWIR handler.
 * @param    event   HWIR event, it can be one of enum HWIR_Event_T.
 * @param    param   Parameter of HWIR event.
 * @retval   void
 */
typedef void (*HWIR_Callback_T)(HWIR_Handle_T *hHWIR, HWIR_Event_T event, unsigned long param);

/**
 * FN(F0 F1 F2 F3 F4) code protocol format, refer gree ir protoclo spec
 */
typedef struct
{
    uint32_t dataCode0;                /**< FN protocol first data (low 28bit vaild) */
    uint32_t dataCode1;                /**< FN protocol second data (low 28bit vaild) */
} HWIR_FormatFN_T;

/**
 * FM code protocol format, refer Gree ir protoclo spec
 */
typedef struct
{
    uint8_t dataCode;                  /**< FM protocol data */
} HWIR_FormatFM_T;

/**
 * Gree ir protocol code struct
 */
typedef union
{
   HWIR_FormatFN_T  keyValueFN;        /**< FN protocol key value */
   HWIR_FormatFM_T  keyValueFM;        /**< FM protocol key value */
} HWIR_KeyCode_T;

/**
 * Gree ir protocol key description
 */
typedef struct
{
    HWIR_Protocol_T    protocol;       /**< Ir  protocol */
    HWIR_KeyCode_T     keyValue;       /**< Key value for the prctocol */
} HWIR_KeyDesc_T;

/**
 * HWIR configuration structure.
 */
typedef struct
{
    HWIR_KeyDesc_T     keyDesc;        /**< Ir key descrition */
    HWIR_Callback_T    callback;       /**< Drivr send usr event to call this function */
} HWIR_Config_T;

/**
 * @brief Initializes HWIR device with user configuration and user handle.
 *
 * @param   pHWIR                      HWIR device point
 * @param   pConfig                    Pointer to user-defined configuration.
 * @param   hHWIR                      Pointer to HWIR handle.
 *
 * @retval  RET_OK                     HWIR init success
 * @retval  RET_ERROR                  HWIR init error
 * @retval  RET_INVPARAM               Input paramter is error
 */
RET_CODE HWIR_Init(HWIR_Device_T *pHWIR, HWIR_Config_T *pConfig, HWIR_Handle_T *hHWIR);

/**
 * @brief Enable HWIR decode function.
 *
 * This function use to enable HWIR decode
 *
 * @param   hHWIR                      Pointer to HWIR handle.
 *
 * @return  void
 */
void HWIR_Enable(HWIR_Handle_T *hHWIR);

/**
 * @brief Disable HWIR decode function.
 *
 * This function use to disables HWIR decode
 *
 * @param hHWIR                        Pointer to HWIR handle.
 *
 * @return void
 */
void HWIR_Disable(HWIR_Handle_T *hHWIR);

#ifdef __cplusplus
}
#endif

#endif /*_HWIR_H_*/








