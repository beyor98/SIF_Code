/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          etm_dev.h
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
 *
 */
#ifndef _ETM_DEV_H_
#define _ETM_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if defined(CHIP_GM6601B)
#define MAX_ETM_NUM        6
#elif defined(CHIP_GM6611B)
#define MAX_ETM_NUM        4
#elif defined(CHIP_GM6621)
#define MAX_ETM_NUM        3
#elif defined(CHIP_F6601C)
#define MAX_ETM_NUM        6
	
#endif

#define US_TIMEOUT_VAL     100

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C)) 
/** ETM - Register Layout Typedef */
typedef struct
{
    __IO uint32_t ID;              /**< ID Register, offset: 0x00 */
    __IO uint32_t CR;              /**< Time Base Control Register, offset: 0x04 */
    __IO uint32_t CCR;             /**< Compare/Capture Register, Period value in compare mode;
                                        Counter value in capture mode offset: 0x08 */
    __IO uint32_t OCR;             /**< Output Compare Counter, offset: 0x0C */
    __IO uint32_t WFCR1;           /**< Channel1 Waveform Configuration, offset: 0x10 */
    __IO uint32_t WFCR2;           /**< Channel2 Waveform Configuration, offset: 0x14 */
    __IO uint32_t DTR;             /**< Deadtime Insertion Control, offset: 0x18 */
    __IO uint32_t FOCR;            /**< Software Force Output Control Register, offset: 0x1C */
    __IO uint32_t SYNCONF;         /**< Synchronization Phase, offset: 0x20 */
    __IO uint32_t TRIGEN;          /**< ETM Trigger ADC Enable Regitster, offset: 0x24 */
    __IO uint32_t RESERVED[6];
    __IO uint32_t IER;             /**< Interrupt Status and Control Register, offset: 0x40 */
    __IO uint32_t SR;              /**< Status Register, offset: 0x44 */
} ETM_Device_T;
#endif

#if defined(CHIP_GM6621)
/** ETM - Register Layout Typedef */
typedef struct
{
    __IO uint32_t ID;              /**< ID Register, offset: 0x00 */
    __IO uint32_t CR;              /**< ETM Control Register 0, offset: 0x04 */
    __IO uint32_t CR1;             /**< ETM Control Register 1, offset: 0x08 */
    __IO uint32_t ARR;             /**< ETM auto-reload and repetition counter Register, offset: 0x0C */
    __IO uint32_t CAP1;            /**< ETM capture register 1, offset: 0x10 */
    __IO uint32_t CAP2;            /**< ETM capture register 2, offset: 0x14 */
    __IO uint32_t CAP3;            /**< ETM capture register 3, offset: 0x18 */
    __IO uint32_t CAP4;            /**< ETM capture register 4, offset: 0x1c */
    __IO uint32_t CMP1;            /**< ETM compare register 1, offset: 0x20 */
    __IO uint32_t CMP2;            /**< ETM compare register 2, offset: 0x24 */
    __IO uint32_t WFCR1;           /**< Channel1 Waveform Configuration, offset: 0x28 */
    __IO uint32_t WFCR2;           /**< Channel2 Waveform Configuration, offset: 0x2C */
    __IO uint32_t WFCR3;           /**< Channel3 Waveform Configuration, offset: 0x30 */
    __IO uint32_t WFCR4;           /**< Channel4 Waveform Configuration, offset: 0x34 */
    __IO uint32_t DTR;             /**< Deadtime Insertion Control, offset: 0x38 */
    __IO uint32_t FOCR;            /**< Software Force Output Control Register, offset: 0x3C */
    __IO uint32_t SYNCONF;         /**< Synchronization Phase, offset: 0x40 */
    __IO uint32_t PESCR;           /**< PES Control Register, offset: 0x44 */
    __IO uint32_t SR;              /**< Status Register, offset: 0x48 */
    __IO uint32_t IER;             /**< Interrupt Control Register, offset: 0x4C */
    __IO uint32_t ISR;             /**< Interrupt Status Register, offset: 0x50 */
} ETM_Device_T;
#endif

/** Base addresses of ETM (Advanced Timer Module)Peripheral Control Registers */
#define ETM0_BASE        (0x40007000u)
#define ETM1_BASE        (0x40007100u)
#define ETM2_BASE        (0x40007200u)

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C)) 
#define ETM3_BASE        (0x40007300u)
#endif

#if defined(CHIP_GM6601B) || defined(CHIP_F6601C)
#define ETM4_BASE        (0x40007400u)
#define ETM5_BASE        (0x40007500u)
#endif

/** ETM Device Pointer */
#define ETM0            ((ETM_Device_T *)ETM0_BASE)
#define ETM1            ((ETM_Device_T *)ETM1_BASE)
#define ETM2            ((ETM_Device_T *)ETM2_BASE)

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C)) 
#define ETM3            ((ETM_Device_T *)ETM3_BASE)
#endif

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
#define ETM4            ((ETM_Device_T *)ETM4_BASE)
#define ETM5            ((ETM_Device_T *)ETM5_BASE)
#endif

/** @name TB - Time Base Control */
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C)) 
#define ETM_CR_TBEN                         ((uint32_t)0x80000000U)     /* Enable Time Base */
#define ETM_CR_SHDEN                        ((uint32_t)0x08000000U)     /* Enable Shadow Mode */
#define ETM_CR_SYNCEN                       ((uint32_t)0x00080000U)     /* Enable ETM Synchronization */
#define ETM_CR_SCD                          ((uint32_t)0x00040000U)     /* Sync Counter Direction */
#define ETM_CR_SED                          ((uint32_t)0x00020000U)     /* Sync Event Direction */

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
#define ETM_CR_BKP                          ((uint32_t)0x00008000U)     /* Break Polarity For Break In */
#define ETM_CR_BKP_POS                      (15U)
#define ETM_CR_BKP_FIELD(x)                 (((uint32_t)(((uint32_t)(x)) << ETM_CR_BKP_POS)) & ETM_CR_BKP)

#define ETM_CR_BKSOURCE                     ((uint32_t)0x00007c00U)     /* Break Source Selection */
#define ETM_CR_BKSOURCE_POS                 (10U)
#define ETM_CR_BKSOURCE_FIELD(x)            (((uint32_t)(((uint32_t)(x)) << ETM_CR_BKSOURCE_POS)) & ETM_CR_BKSOURCE)

#define ETM_CR_EXTTC_FIELD(x)                ((uint32_t)(((uint32_t)(x)) << ETM_CR_BKSOURCE_POS))

#define ETM_CR_BKIN                         ((uint32_t)0x00000200U)     /* Break In Mode Selection */
#define ETM_CR_BKIN_POS                     (9U)
#define ETM_CR_BKIN_FIELD(x)                (((uint32_t)(((uint32_t)(x)) << ETM_CR_BKIN_POS)) & ETM_CR_BKIN)
#endif

#define ETM_CR_CAPEN                        ((uint32_t)0x00000100U)     /* Enable Capture Mode */

#define ETM_CR_PSCDIV                       ((uint32_t)0x000000E0U)     /* Counter Soruce Divider */
#define ETM_CR_PSCDIV_POS                   (5U)
#define ETM_CR_PSCDIV_FIELD(x)              (((uint32_t)(((uint32_t)(x)) << ETM_CR_PSCDIV_POS)) & ETM_CR_PSCDIV)

#define ETM_CR_PSCEN                        ((uint32_t)0x00000010U)     /* Use Prescaler */

#define ETM_CR_CD                           ((uint32_t)0x00000006U)     /* Counting Direction */
#define ETM_CR_CD_POS                       (1U)
#define ETM_CR_CD_FIELD(x)                  (((uint32_t)(((uint32_t)(x)) << ETM_CR_CD_POS)) & ETM_CR_CD)

#define ETM_CR_EXTCLK                       ((uint32_t)0x00000001U)     /* Select The External Clock */
#endif

#if defined(CHIP_GM6621)
#define ETM_CR_TBEN                         ((uint32_t)0x80000000U)     /* Enable Time Base */
#define ETM_CR_SHDEN                        ((uint32_t)0x00100000U)     /* Enable Shadow Mode */
#define ETM_CR_SYNCEN                       ((uint32_t)0x00040000U)     /* Enable ETM Synchronization */
#define ETM_CR_SCD                          ((uint32_t)0x00020000U)     /* Sync Counter Direction */
#define ETM_CR_SED                          ((uint32_t)0x00010000U)     /* Sync Event Direction */

#define ETM_CR_BKP                          ((uint32_t)0x00002000U)     /* Break Polarity For Break In */
#define ETM_CR_BKP_POS                      (13U)
#define ETM_CR_BKP_FIELD(x)                 (((uint32_t)(((uint32_t)(x)) << ETM_CR_BKP_POS)) & ETM_CR_BKP)

#define ETM_CR_BKIN                         ((uint32_t)0x00001000U)     /* Break In Mode Selection */
#define ETM_CR_BKIN_POS                     (12U)
#define ETM_CR_BKIN_FIELD(x)                (((uint32_t)(((uint32_t)(x)) << ETM_CR_BKIN_POS)) & ETM_CR_BKIN)

#define ETM_CR_CAPEN                        ((uint32_t)0x40000000U)     /* Enable Capture Mode */
#define ETM_CR_ONEPULSEEN                   ((uint32_t)0x20000000U)     /* Enable One Pulse Mode */

#define ETM_CR_PSCDIV                       ((uint32_t)0x0000FFFFU)     /* Counter Soruce Divider */
#define ETM_CR_PSCDIV_POS                   (0U)
#define ETM_CR_PSCDIV_FIELD(x)              (((uint32_t)(((uint32_t)(x)) << ETM_CR_PSCDIV_POS)) & ETM_CR_PSCDIV)

#define ETM_CR_PSCEN                        ((uint32_t)0x10000000U)     /* Use Prescaler */

#define ETM_CR_CD                           ((uint32_t)0x00000030U)     /* Counting Direction */
#define ETM_CR_CD_POS                       (4U)
#define ETM_CR_CD_FIELD(x)                  (((uint32_t)(((uint32_t)(x)) << ETM_CR_CD_POS)) & ETM_CR_CD)

#define ETM_CR_EXTCLK                       ((uint32_t)0x00000001U)     /* Select The External Clock */
#endif

/** Waveform Configuration */
#define ETM_WFCR_CNTST                      ((uint32_t)0x00300000U)     /* The Action Setting Of Counter Start */
#define ETM_WFCR_CNTST_POS                  (20U)
#define ETM_WFCR_CNTST_FIELD(x)             (((uint32_t)(((uint32_t)(x)) << ETM_WFCR_CNTST_POS)) & ETM_WFCR_CNTST)

#define ETM_WFCR_MD2                        ((uint32_t)0x000C0000U)     /* The Action Setting Of Duty2 Register Match When Counts Down */
#define ETM_WFCR_MD2_POS                    (18U)
#define ETM_WFCR_MD2_FIELD(x)               (((uint32_t)(((uint32_t)(x)) << ETM_WFCR_MD2_POS)) & ETM_WFCR_MD2)

#define ETM_WFCR_MD1                        ((uint32_t)0x00030000U)     /* The Action Setting Of Duty1 Register Match When Counts Down */
#define ETM_WFCR_MD1_POS                    (16U)
#define ETM_WFCR_MD1_FIELD(x)               (((uint32_t)(((uint32_t)(x)) << ETM_WFCR_MD1_POS)) & ETM_WFCR_MD1)

#define ETM_WFCR_MU2                        ((uint32_t)0x0000C000U)     /* The Action Setting Of Duty2 Register Match When Counts Up */
#define ETM_WFCR_MU2_POS                    (14U)
#define ETM_WFCR_MU2_FIELD(x)               (((uint32_t)(((uint32_t)(x)) << ETM_WFCR_MU2_POS)) & ETM_WFCR_MU2)

#define ETM_WFCR_MU1                        ((uint32_t)0x00003000U)     /* The Action Setting Of Duty1 Register Match When Counts Up */
#define ETM_WFCR_MU1_POS                    (12U)
#define ETM_WFCR_MU1_FIELD(x)               (((uint32_t)(((uint32_t)(x)) << ETM_WFCR_MU1_POS)) & ETM_WFCR_MU1)

#define ETM_WFCR_UN                         ((uint32_t)0x00000300U)     /* The Action Setting Of Counter Underflow */
#define ETM_WFCR_UN_POS                     (8U)
#define ETM_WFCR_UN_FIELD(x)                (((uint32_t)(((uint32_t)(x)) << ETM_WFCR_UN_POS)) & ETM_WFCR_UN)

#define ETM_WFCR_OV                         ((uint32_t)0x00000030U)     /* The Action Setting Of Counter Overflow */
#define ETM_WFCR_OV_POS                     (4U)
#define ETM_WFCR_OV_FIELD(x)                (((uint32_t)(((uint32_t)(x)) << ETM_WFCR_OV_POS)) & ETM_WFCR_OV)

/** Software Force Output Control */
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
#define ETM_FOCR_CH2                        ((uint32_t)0x00030000U)
#define ETM_FOCR_CH2_POS                    (16U)

#define ETM_FOCR_CH1                        ((uint32_t)0x00000003U)
#endif

#if defined(CHIP_GM6621)
#define ETM_FOCR_CH4                        ((uint32_t)0x000C0000U)
#define ETM_FOCR_CH4_POS                    (18U)

#define ETM_FOCR_CH3                        ((uint32_t)0x00030000U)
#define ETM_FOCR_CH3_POS                    (16U)

#define ETM_FOCR_CH2                        ((uint32_t)0x0000000CU)
#define ETM_FOCR_CH2_POS                    (2U)

#define ETM_FOCR_CH1                        ((uint32_t)0x00000003U)
#endif

/** ETM Status */
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C) ) 
#define ETM_SR_CNT                          ((uint32_t)0x00010000U)     /* Counter current value */
#define ETM_SR_CNTDIR                       ((uint32_t)0x00008000U)     /* Counter Direction */
#define ETM_SR_RSE                          ((uint32_t)0x00002000U)     /* Receive Sync Event */
#define ETM_SR_STOP                         ((uint32_t)0x00001000U)     /* Counter stop process flag */
#define ETM_SR_START                        ((uint32_t)0x00000800U)     /* Counter start process flag */
#define ETM_SR_EDGE_N2                      ((uint32_t)0x00000100U)     /* Capture ch2 Negative edge interrupt occur */
#define ETM_SR_EDGE_P2                      ((uint32_t)0x00000080U)     /* Capture ch2 Positive edge interrupt occur */
#define ETM_SR_EDGE_N1                      ((uint32_t)0x00000040U)     /* Capture ch1 Negative edge interrupt occur */
#define ETM_SR_EDGE_P1                      ((uint32_t)0x00000020U)     /* Capture ch1 Positive edge interrupt occur */

#define ETM_SR_MATCH2                       ((uint32_t)0x00000010U)     /* Time base counter compare duty2 match interrupt occur */
#define ETM_SR_MATCH1                       ((uint32_t)0x00000008U)     /* Time base counter compare duty1 match interrupt occur */
#define ETM_SR_UD                           ((uint32_t)0x00000004U)     /* Time base counter underflow interrupt occur */
#define ETM_SR_OV                           ((uint32_t)0x00000002U)     /* Time base counter overflow interrupt occur */
#define ETM_SR_ST                           ((uint32_t)0x00000001U)     /* Time base counter start interrupt occur */
#endif

#if defined(CHIP_GM6621)
#define ETM_SR_CNT                          ((uint32_t)0x00000000U)     /* Counter current value */
#define ETM_SR_CNTDIR                       ((uint32_t)0x00004000U)     /* Counter Direction */
#define ETM_SR_RSE                          ((uint32_t)0x00800000U)     /* Receive Sync Event */
#define ETM_SR_STOP                         ((uint32_t)0x00020000U)     /* Counter stop process flag */
#define ETM_SR_START                        ((uint32_t)0x00010000U)     /* Counter start process flag */

#define ETM_SR_EDGE_N4                      ((uint32_t)0x00008000U)     /* Capture ch4 Negative edge interrupt occur */
#define ETM_SR_EDGE_P4                      ((uint32_t)0x00004000U)     /* Capture ch4 Positive edge interrupt occur */
#define ETM_SR_EDGE_N3                      ((uint32_t)0x00002000U)     /* Capture ch3 Negative edge interrupt occur */
#define ETM_SR_EDGE_P3                      ((uint32_t)0x00001000U)     /* Capture ch3 Positive edge interrupt occur */
#define ETM_SR_EDGE_N2                      ((uint32_t)0x00000800U)     /* Capture ch2 Negative edge interrupt occur */
#define ETM_SR_EDGE_P2                      ((uint32_t)0x00000400U)     /* Capture ch2 Positive edge interrupt occur */
#define ETM_SR_EDGE_N1                      ((uint32_t)0x00000200U)     /* Capture ch1 Negative edge interrupt occur */
#define ETM_SR_EDGE_P1                      ((uint32_t)0x00000100U)     /* Capture ch1 Positive edge interrupt occur */

#define ETM_SR_MATCH4                       ((uint32_t)0x00000040U)     /* Time base counter compare duty4 match interrupt occur */
#define ETM_SR_MATCH3                       ((uint32_t)0x00000020U)     /* Time base counter compare duty3 match interrupt occur */
#define ETM_SR_MATCH2                       ((uint32_t)0x00000010U)     /* Time base counter compare duty2 match interrupt occur */
#define ETM_SR_MATCH1                       ((uint32_t)0x00000008U)     /* Time base counter compare duty1 match interrupt occur */
#define ETM_SR_UD                           ((uint32_t)0x00000004U)     /* Time base counter underflow interrupt occur */
#define ETM_SR_OV                           ((uint32_t)0x00000002U)     /* Time base counter overflow interrupt occur */
#define ETM_SR_ST                           ((uint32_t)0x00000001U)     /* Time base counter start interrupt occur */
#endif

/** List of ETM flags */
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C) )
typedef enum
{
    ETM_TBSTINT_FLAG  = (1U << 0),       /**< Counter start interrupt flag */
    ETM_TBOVFINT_FLAG = (1U << 1),       /**< Counter overflow interrupt flag */
    ETM_TBUDFINT_FLAG = (1U << 2),       /**< Counter underflow interrupt flag */
    ETM_TBD1MINT_FLAG = (1U << 3),       /**< Counter duty1 match interrupt flag */
    ETM_TBD2MINT_FLAG = (1U << 4),       /**< Counter duty2 match interrupt flag */
    ETM_CH1PEINT_FLAG = (1U << 5),       /**< Capture channel1 positive edge interrupt flag */
    ETM_CH1NEINT_FLAG = (1U << 6),       /**< Capture channel1 negative edge interrupt flag */
    ETM_CH2PEINT_FLAG = (1U << 7),       /**< Capture channel2 positive edge interrupt flag */
    ETM_CH2NEINT_FLAG = (1U << 8),       /**< Capture channel2 negative edge interrupt flag */
    ETM_SYNCINT_FLAG  = (1U << 13),      /**< Sync event received interrupt flag */
    ETM_BREAKIN_FLAG  = (1U << 14)       /**< Break in interrupt flag */
} ETM_Status_Flags_T;
#endif

#if defined(CHIP_GM6621)
typedef enum
{
    ETM_TBSTINT_FLAG  = (1U << 0),       /**< Counter start interrupt flag */
    ETM_TBOVFINT_FLAG = (1U << 1),       /**< Counter overflow interrupt flag */
    ETM_TBUDFINT_FLAG = (1U << 2),       /**< Counter underflow interrupt flag */
    ETM_TBD1MINT_FLAG = (1U << 3),       /**< Counter duty1 match interrupt flag */
    ETM_TBD2MINT_FLAG = (1U << 4),       /**< Counter duty2 match interrupt flag */
    ETM_TBD3MINT_FLAG = (1U << 5),       /**< Counter duty3 match interrupt flag */
    ETM_TBD4MINT_FLAG = (1U << 6),       /**< Counter duty4 match interrupt flag */
    ETM_CH1PEINT_FLAG = (1U << 8),       /**< Capture channel1 positive edge interrupt flag */
    ETM_CH1NEINT_FLAG = (1U << 9),       /**< Capture channel1 negative edge interrupt flag */
    ETM_CH2PEINT_FLAG = (1U << 10),      /**< Capture channel2 positive edge interrupt flag */
    ETM_CH2NEINT_FLAG = (1U << 11),      /**< Capture channel2 negative edge interrupt flag */
    ETM_CH3PEINT_FLAG = (1U << 12),      /**< Capture channel3 positive edge interrupt flag */
    ETM_CH3NEINT_FLAG = (1U << 13),      /**< Capture channel3 negative edge interrupt flag */
    ETM_CH4PEINT_FLAG = (1U << 14),      /**< Capture channel4 positive edge interrupt flag */
    ETM_CH4NEINT_FLAG = (1U << 15),      /**< Capture channel4 negative edge interrupt flag */
    ETM_SYNCINT_FLAG  = (1U << 23),      /**< Sync event received interrupt flag */
} ETM_Status_Flags_T;
#define ETM_BREAKIN_FLAG   ((uint32_t)0x80000000U)       /**< Break in interrupt flag */
#endif

/*******************************************************************************
 * Global function
 ******************************************************************************/


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ETM_DEV_H_ */

