/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          wdog.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2016/06/29
 *
 * @brief         Header file of wdog driver.
 *
 * @note
 *    2016/03/01,   Mike.Zheng,   V1.0.0
 *        Initial version.
 *    2016/06/29,   Mike.Zheng,   V1.1.0
 *        Update coding style.
 *    2016/11/22,   Mike.Zheng,   V1.2.0
 *        Update For Release.
 *    2017/03/15    kenny.cmt     V1.3.0
 *        Update For GM6611
 *    2017/05/30    kenny.cmt     V1.3.1
 *        Update For GM6601B
 *    2018/01/19    kenny.cmt     V2.0
 *        Update For GM6611B
 */

#ifndef _WDOG_H_
#define _WDOG_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <common.h>

/**
 * Wdog Clksrc
 */
#if defined(CHIP_GM6601)
typedef enum
{
    CLKSRC_FCLK         = 0,                        /**< Select FCLK As WDOG Clksrc (Sysclk) */
    CLKSRC_OSC_LS_CLK      ,                        /**< Select OSC_LS_CLK As WDOG Clksrc(32.768KHz) */
    CLKSRC_IRC_LS_CLK      ,                        /**< Select IRC_LS_CLK As WDOG Clksrc(1KHz) */
    CLKSRC_PLL_SRC_CLK                              /**< Select PLL_SRC_CLK As WDOG Clksrc(8MHz) */
} WDOG_CLKSRC_T;
#endif

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B) )
typedef enum
{
    CLKSRC_IRC_LS_CLK   = 0,                        /**< Select IRC_LS_CLK As WDOG Clksrc(30KHz) */
} WDOG_CLKSRC_T;
#endif
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6621) || defined(CHIP_F6601C))
typedef enum
{
    CLKSRC_IRC_LS_CLK   = 0,                        /**< Select IRC_LS_CLK As WDOG Clksrc(30KHz) */
    CLKSRC_OSC_LS_CLK      ,                        /**< Select OSC_LS_CLK As WDOG Clksrc(32.768KHz) */
    CLKSRC_PLL_SRC_CLK     ,                        /**< Select PLL_SRC_CLK As WDOG Clksrc(8MHz) */
    CLKSRC_FCLK                                     /**< Select FCLK As WDOG Clksrc (Sysclk) */
} WDOG_CLKSRC_T;
#endif

/**
 * General WDOG Events
 */
typedef enum
{
    WDOG_EVENT_REOOT_WARNING = 0,                   /**< when WDOG almost reboot chip, it will send warning event to notify usr) */
} WDOG_Event_T;

/**
 * WDOG user handle structure
 */
typedef struct
{
    WDOG_Device_T   *device;                       /**< Pointer to WDOG device */
} WDOG_Handle_T;

/**
 * @brief     Callback definition of WDOG interrupt handler to notify user an event with parameters.
 *
 * @param     hWDOG        Pointer to WDOG handler.
 * @param     event        WDOG event, it can be one of enum @ref  WDOG_Event_T.
 * @param     param        None.
 * @retval    None.
 */
typedef void (*WDOG_Callback_T) (WDOG_Handle_T *hWDOG, WDOG_Event_T event, unsigned long Param);

/**
 * WDOG configuration structure.
 */
typedef struct
{
#if ((defined(CHIP_GM6601B)) || (defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621)) || defined(CHIP_F6601C))
    uint32_t              wdogWorkEWIMode;         /**< Config work mode and Early warning interrupt were select */
#endif
    uint32_t              resetTime;               /**< System will reset after resetTime(uint:ms) pass if wdog is not feed again */
#if ((defined(CHIP_GM6601B)) || (defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621)) || defined(CHIP_F6601C))
    uint32_t              windowTime;              /**< config wdog windows time*/
#endif
    WDOG_CLKSRC_T         clkSrc;                  /**< Config srcclk select */
    WDOG_Callback_T       callBack;                /**< Drivr send usr event to call this function */
} WDOG_Config_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief    Initializes WDOG device with user configuration and user handle.
 *
 * @param    pWDOG         WDOG device point
 * @param    pConfig       Pointer to user-defined configuration.
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   RET_OK        WDOG_Init success
 * @retval   RET_ERROR     WDOG_Init error
 * @retval   RET_INVPARAM  Usr input paramter is error
 */
RET_CODE WDOG_Init(WDOG_Device_T *pWDOG, WDOG_Config_T *pConfig, WDOG_Handle_T *hWDOG);

/**
 * @brief    Enable wdog to start work,watdog cnt start to decrease.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_Enable(WDOG_Handle_T *hWDOG);

/**
 * @brief    Disable wdog to work , the watchdog cnt wil not change.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_Disable(WDOG_Handle_T *hWDOG);

/**
 * @brief    Feed wdog to prevent system reset.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_FeedWdog(WDOG_Handle_T *hWDOG);

/**
 * @brief    Watch Dog Interrupt Clear.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_IntClear(WDOG_Handle_T* hWDOG);

/**
 * @brief    Set wdog window mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_SetWindowMode(WDOG_Handle_T *hWDOG);

/**
 * @brief    Set wdog window and EWI mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_SetWindowMode_EWI(WDOG_Handle_T *hWDOG);

/**
 * @brief    Set wdog Normal and EWI mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_SetNormalMode_EWI(WDOG_Handle_T *hWDOG);

/**
 * @brief    Set wdog normal mode.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
void WDOG_SetNormalMode(WDOG_Handle_T *hWDOG);

/**
 * @brief    Get wdog value.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
RET_CODE WDOG_GetCountVal(WDOG_Handle_T *hWDOG);

/**
 * @brief    Get wdog CNT outside window flag.
 *
 * @param    hWDOG         Pointer to WDOG handle.
 *
 * @retval   None
 */
RET_CODE WDOG_GetCntOutWinFlag(WDOG_Handle_T *hWDOG);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _WDOG_H_ */

