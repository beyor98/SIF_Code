/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          lptim_dev.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/06/05 
 *    
 * @brief         LPTIM peripheral PHY device header file
 * 
 * @note
 *    2017/06/05, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _LPTIM_DEV_H_
#define _LPTIM_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** LPTIM - Register Layout Typedef */
typedef struct
{
    __IO uint32_t ID;              /**< LPTIM ID Register, offset: 0x00 */
    __IO uint32_t CFGR;            /**< LPTIM Configuration register, offset: 0x04 */ 
    __IO uint32_t CR;              /**< LPTIM Control register, offset: 0x08 */
    __IO uint32_t CMP;             /**< LPTIM Compare register, offset: 0x0C */
    __IO uint32_t ARR;             /**< LPTIM Autoreload register, offset: 0x10 */
    __IO uint32_t CNT;             /**< LPTIM Counter register, offset: 0x14 */
    __IO uint32_t RESERVED[10];      
    __IO uint32_t IER;             /**< LPTIM Interrupt Enable register, offset: 0x40 */
    __IO uint32_t SR;              /**< LPTIM Status Register, offset: 0x44 */
}LPTIM_Device_T;

/** Base addresses of LPTIM (Low Power Timer)Peripheral Control Registers */
#define LPTIM_BASE        (0x4003A000u)

/** LPTIM Device Pointer */
#define LPTIM            ((LPTIM_Device_T *)LPTIM_BASE)

/******************  Bit definition for LPTIM_CFGR register *******************/                                       
#define LPTIM_CFGR_EXTCLK           ((uint32_t)0x00000001U)        /**< Select The External Clock */
#define LPTIM_CFGR_TIMOUT           ((uint32_t)0x00080000U)        /**< Timout enable */
#define LPTIM_CFGR_WAVE             ((uint32_t)0x00100000U)        /**< Waveform shape */
#define LPTIM_CFGR_WAVPOL           ((uint32_t)0x00200000U)        /**< Waveform shape polarity */
#define LPTIM_CFGR_PRELOAD          ((uint32_t)0x00400000U)        /**< Reg update mode */
#define LPTIM_CFGR_COUNTMODE        ((uint32_t)0x00800000U)        /**< Counter mode enable */
#define LPTIM_CFGR_ENC              ((uint32_t)0x01000000U)        /**< Encoder mode enable */

#define LPTIM_CFGR_CKPOL_POS        (1U)                                       
#define LPTIM_CFGR_CKPOL            (0x3U << LPTIM_CFGR_CKPOL_POS)             
#define LPTIM_CFGR_CKPOL_FIELD(x)   (((uint32_t)(((uint32_t)(x)) << LPTIM_CFGR_CKPOL_POS)) & LPTIM_CFGR_CKPOL)     /**< CKPOL[1:0] bits (Clock polarity) */

#define LPTIM_CFGR_CKFLT_POS        (3U)                                       
#define LPTIM_CFGR_CKFLT            (0x3U << LPTIM_CFGR_CKFLT_POS)             
#define LPTIM_CFGR_CKFLT_FIELD(x)   (((uint32_t)(((uint32_t)(x)) << LPTIM_CFGR_CKFLT_POS)) & LPTIM_CFGR_CKFLT)     /**< CKFLT[1:0] bits (Configurable digital filter for external clock) */

#define LPTIM_CFGR_TRGFLT_POS       (6U)                                       
#define LPTIM_CFGR_TRGFLT           (0x3U << LPTIM_CFGR_TRGFLT_POS)            
#define LPTIM_CFGR_TRGFLT_FIELD(x)  (((uint32_t)(((uint32_t)(x)) << LPTIM_CFGR_TRGFLT_POS)) & LPTIM_CFGR_TRGFLT)   /**< TRGFLT[1:0] bits (Configurable digital filter for trigger) */

#define LPTIM_CFGR_PRESC_POS        (9U)                                       
#define LPTIM_CFGR_PRESC            (0x7U << LPTIM_CFGR_PRESC_POS)             
#define LPTIM_CFGR_PRESC_FIELD(x)   (((uint32_t)(((uint32_t)(x)) << LPTIM_CFGR_PRESC_POS)) & LPTIM_CFGR_PRESC)     /**< PRESC[2:0] bits (Clock prescaler) */

#define LPTIM_CFGR_TRIGSEL_POS      (13U)                                      
#define LPTIM_CFGR_TRIGSEL          (0x7U << LPTIM_CFGR_TRIGSEL_POS)          
#define LPTIM_CFGR_TRIGSEL_FIELD(x) (((uint32_t)(((uint32_t)(x)) << LPTIM_CFGR_TRIGSEL_POS)) & LPTIM_CFGR_TRIGSEL) /**< TRIGSEL[2:0]] bits (Trigger selector) */

#define LPTIM_CFGR_TRIGEN_POS       (17U)                                      
#define LPTIM_CFGR_TRIGEN           (0x3U << LPTIM_CFGR_TRIGEN_POS)            
#define LPTIM_CFGR_TRIGEN_FIELD(x)  (((uint32_t)(((uint32_t)(x)) << LPTIM_CFGR_TRIGEN_POS)) & LPTIM_CFGR_TRIGEN)   /**< TRIGEN[1:0] bits (Trigger enable and polarity) */

/******************  Bit definition for LPTIM_CR register  ********************/
#define LPTIM_CR_ENABLE             ((uint32_t)0x00000001U)         /**< LPTIMer enable */
#define LPTIM_CR_SNGSTRT            ((uint32_t)0x00000002U)         /**< Timer start in single mode */
#define LPTIM_CR_CNTSTRT            ((uint32_t)0x00000004U)         /**< Timer start in continuous mode */

typedef enum 
{
    LPTIM_DOWN_FLAG    = (1U << 6),      /**< Counter direction change up to down flag */
    LPTIM_UP_FLAG      = (1U << 5),      /**< Counter direction change down to up flag */
    LPTIM_ARROK_FLAG   = (1U << 4),      /**< Autoreload register update OK flag */
    LPTIM_CMPOK_FLAG   = (1U << 3),      /**< Compare register update OK flag */
    LPTIM_EXTTRIG_FLAG = (1U << 2),      /**< External trigger edge event flag */
    LPTIM_ARRM_FLAG    = (1U << 1),      /**< Autoreload match flag */
    LPTIM_CMPM_FLAG    = (1U << 0),      /**< Compare match flag */
} LPTIM_Status_Flags_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _LPTIM_DEV_H_ */

