/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file          LPower.h
*
* @author        sophia.wu
*
* @version       1.0.0
*
* @date          2019/04/16
*
* @brief         Header file of LPower driver.
*
* @note
*    2019/04/16, sophia.wu, V1.0.0
*        Support 6621.
*/

#ifndef _LPower_H_
#define _LPower_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

#define RET_STOP_OK                     0x00  /**< Function call returns succesfully  */
#define RET_STOP_FLAG_INCOMPATIBLE      0x01  /**< Function fails because flag is incompatible  */


#define Configure_CPU_WFI()  __WFI()
#define Configure_CPU_WFE()  __WFE()

typedef enum
{
    STOP_MODE                    = 0x00,
#if defined(CHIP_GM6611B)
    STOPDEEP_MODE                = 0x01,
#endif
#if defined(CHIP_F6621B)
    STANDBY_MODE                 = 0x02,
#endif
    RESERVED                     = 0xFF,
} LPower_Mode_T;


#if defined(CHIP_GM6621) || defined(CHIP_F6621B)
/**
  * @brief   Periph ClockCfg Init structure definition
  */
typedef enum
{
    Periph_OFF                  = 0x00,
    Periph_ON                   = 0x01,
} Periph_Onoff_T;

typedef enum
{
    EFLASH_Enable                = 0x00,
    EFLASH_Disable               = 0x01,
} LPower_Eflash_En_T;

typedef enum
{
    LCD_ACCESS_Enable            = 0x00,
    LCD_ACCESS_Disable           = 0x01,
} LPower_LCDAcess_En_T;


typedef enum
{
    PERIPHCLOCK_SOURCE_LIRC32K   = 0x00,  /**< PERIPH clock is derived from LIRC 32KHz */
    PERIPHCLOCK_SOURCE_LOSC32K   = 0x01,  /**< PERIPH clock is derived from LOSC 32.768KHz */
} PERIPH_ClkSrc_T;

typedef struct
{
    Periph_Onoff_T     lcdOn;
    Periph_Onoff_T     acmpOn;
    Periph_Onoff_T     wdtOn;
    Periph_Onoff_T     rtcOn;                /** rtc on or off */
} LPower_PeriphClkConfig_T;

/**
  * @brief   LPower_PeriphClk structure definition
  */
typedef struct
{
    PERIPH_ClkSrc_T    PeriphClk;            /**PeriphClk LIRC or LOSC */
} LPower_PeriphClk_T;
#endif


typedef struct
{
    LPower_Mode_T           LpowerMode;      /**Lpower Mode */
#if defined(CHIP_GM6621) || defined(CHIP_F6621B)
    LPower_Eflash_En_T      EflashEn;        /**Enable eFlash deep-standby*/
    LPower_LCDAcess_En_T    LcdAccessEn;     /**Enable LCD Access */
#endif
} LPower_AppMode_T;

/**
 * @brief Enter Sleep Mode
 * @param None
 * @return None.
 */
void SleepMode_Enter(void);

/**
 * @brief Enter DeepSleep Mode
 * @param None
 * @return None.
 */
void DeepSleepMode_Enter(void);
/**
 * @brief Configure LPower PERIPH Clock
 * @param pLPower     PERIPH Clock Switch:LIRC or LOSC
 * @param pCLK        config Periph:RTC,ACMP,WDG,LCD on or off
 * @return None.
 */
#if defined(CHIP_GM6621) || defined(CHIP_F6621B)
RET_CODE LPower_PeriphClockCfg(LPower_PeriphClkConfig_T *pLPower, LPower_PeriphClk_T *pCLK);
#endif

/**
 * @brief LPower_Enter
 * @param pMode    config AppMode
 * @return None.
 */
void LPower_Enter(LPower_AppMode_T *pMode);

/**
 * @brief LPower Exit
 * @param pClk     recover from lpower mode Sys Clock config
 * @param pMode    config AppMode
 * @return LPower exit result.
 */
RET_CODE LPower_Exit(LPower_AppMode_T *pMode);

#ifdef __cplusplus
}
#endif

#endif /*_LPower_H_*/
