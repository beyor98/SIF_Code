/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lvd.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/08/17
 *
 * @brief         lvd device header file
 *
 * @note
 *    2016/08/17, mark.lee, V1.0.0
 *  2017/05/19, yangy, V1.0.1   Modified LVD_Init to enable that VD1 and VD2 can work together.
 *        Initial version.
 *
 */
#ifndef _LVD_H_
#define _LVD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** Bit definition for Analog PMU Control Register  */

#if defined (CHIP_GM6611B)
#define LVD_ACR_OCP_EN              (0x00010000U)   /* Over current limit protection Enable Bit */
#define LVD_ACR_OCP_TRH             (0x00060000U)   /* Threshold for over current protection */
#define LVD_ACR_OCP_TRH_POS         (17U)
#define LVD_ACR_OCP_TRH_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_ACR_OCP_TRH_POS)) & LVD_ACR_OCP_TRH)
#elif defined (CHIP_F6601C)
#define LVD_ACR_OCP_EN              (0x00080000U)   /* Over current limit protection Enable Bit */
#define LVD_ACR_OCP_TRH             (0x00100000U)   /* Threshold for over current protection */
#else
#define LVD_ACR_OCP_EN              (0x00080000U)   /* Over current limit protection Enable Bit */
#define LVD_ACR_OCP_TRH             (0x00300000U)   /* Threshold for over current protection */
#define LVD_ACR_OCP_TRH_POS         (20U)
#define LVD_ACR_OCP_TRH_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_ACR_OCP_TRH_POS)) & LVD_ACR_OCP_TRH)
#endif


#if defined (CHIP_GM6621) || defined (CHIP_GM6611B) || defined (CHIP_F6621B)
/** Bit definition for Voltage Detector Control Register  */
#define LVD_CR_VOLDT1_EN            (0x00000001U)   /* The operation of voltage detection 1 Enable Bit */
#define LVD_CR_VOLDT2_EN            (0x00000001U)   /* The operation of voltage detection 2 Enable Bit */
#define LVD_CR_VOLDT3_EN            (0x00000001U)   /* The operation of voltage detection 3 Enable Bit */

#define LVD_CR_VOLDT2_INTEN         (0x00000008U)   /* The operation of voltage detection 2 interrupt Enable Bit */
#define LVD_CR_VOLDT1_RSTEN         (0x00000010U)   /* Voltage detection 1 reset chip Enable Bit */
#define LVD_CR_VOLDT3_RSTEN         (0x00000010U)   /* Voltage detection 3 reset chip Enable Bit */

#define LVD_CR_TRH1                 (0x00FF0000U)   /* Timing threshold for filter the glitch of voltage detector 1 */
#define LVD_CR_TRH1_POS             (16U)
#define LVD_CR_TRH1_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LVD_CR_TRH1_POS)) & LVD_CR_TRH1)
#define LVD_CR_TRH2                 (0x00FF0000U)   /* Timing threshold for filter the glitch of voltage detector 2 */
#define LVD_CR_TRH2_POS             (16U)
#define LVD_CR_TRH2_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LVD_CR_TRH2_POS)) & LVD_CR_TRH2)

#if defined (CHIP_F6621B)
#define LVD_FILTER_CLK_PSC2         (0x00007000U)   /* Timing threshold for filter the glitch of voltage detector 2 */
#define LVD_FILTER_CLK_PSC2_POS     (12U)
#define LVD_FILTER_CLK_PSC2_FIELD(x) (((uint32_t)(((uint32_t)(x)) << LVD_FILTER_CLK_PSC2_POS)) & LVD_FILTER_CLK_PSC2)
#endif

#define LVD_CR_TRH3                 (0x00FF0000U)   /* Timing threshold for filter the glitch of voltage detector 3 */
#define LVD_CR_TRH3_POS             (16U)
#define LVD_CR_TRH3_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LVD_CR_TRH3_POS)) & LVD_CR_TRH3)

#define LVD_CR_DT1LEVEL             (0x00000700U)   /* Voltage detection 1 level */
#define LVD_CR_DT1LEVEL_POS         (8U)
#define LVD_CR_DT1LEVEL_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_CR_DT1LEVEL_POS)) & LVD_CR_DT1LEVEL)
#define LVD_CR_DT2LEVEL             (0x00000700U)   /* Voltage detection 2 level */
#define LVD_CR_DT2LEVEL_POS         (8U)
#define LVD_CR_DT2LEVEL_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_CR_DT2LEVEL_POS)) & LVD_CR_DT2LEVEL)
#define LVD_CR_DT3LEVEL             (0x00000300U)   /* Voltage detection 3 level */
#define LVD_CR_DT3LEVEL_POS         (8U)
#define LVD_CR_DT3LEVEL_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_CR_DT3LEVEL_POS)) & LVD_CR_DT3LEVEL)

/** Bit definition for Voltage Detector Status Register  */
#define LVD_CR_VOL1DT_INTFLG        (0x00000002U)   /* The operation of voltage detection 1 interrupt Flag Bit */
#define LVD_CR_VOL2DT_INTFLG        (0x00000002U)   /* The operation of voltage detection 2 interrupt Flag Bit */
#define LVD_CR_VOL3DT_INTFLG        (0x00000002U)   /* The operation of voltage detection 3 interrupt Flag Bit */

/** Bit definition for Voltage Detector Status Register  */
#define LVD_CR_VOL1DT_OUT        (0x00000001U)
#define LVD_CR_VOL2DT_OUT        (0x00000001U)
#define LVD_CR_VOL3DT_OUT        (0x00000001U)

#else

/** Bit definition for Voltage Detector Control Register  */
#define LVD_CR_VOLDT1_EN            (0x00000001U)   /* The operation of voltage detection 1 Enable Bit */
#define LVD_CR_VOLDT2_EN            (0x00010000U)   /* The operation of voltage detection 2 Enable Bit */
#define LVD_CR_VOLDT3_EN            (0x00000001U)   /* The operation of voltage detection 3 Enable Bit */
#define LVD_CR_VOLDT1_INTEN         (0x00000008U)   /* The operation of voltage detection 1 interrupt Enable Bit */
#define LVD_CR_VOLDT2_INTEN         (0x00080000U)   /* The operation of voltage detection 2 interrupt Enable Bit */
#define LVD_CR_VOLDT3_INTEN         (0x00000008U)   /* The operation of voltage detection 3 interrupt Enable Bit */
#define LVD_CR_VOLDT1_RSTEN         (0x00000010U)   /* Voltage detection 1 reset chip Enable Bit */
#define LVD_CR_VOLDT2_RSTEN         (0x00100000U)   /* Voltage detection 2 reset chip  Enable Bit */
#define LVD_CR_VOLDT3_RSTEN         (0x00000010U)   /* Voltage detection 3 reset chip Enable Bit */

#define LVD_CR_TRH1                 (0x0000FF00U)   /* Timing threshold for filter the glitch of voltage detector 1 */
#define LVD_CR_TRH1_POS             (8U)
#define LVD_CR_TRH1_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LVD_CR_TRH1_POS)) & LVD_CR_TRH1)
#define LVD_CR_TRH2                 (0xFF000000U)   /* Timing threshold for filter the glitch of voltage detector 2 */
#define LVD_CR_TRH2_POS             (24U)
#define LVD_CR_TRH2_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LVD_CR_TRH2_POS)) & LVD_CR_TRH2)
#define LVD_CR_TRH3                 (0x0000FF00U)   /* Timing threshold for filter the glitch of voltage detector 3 */
#define LVD_CR_TRH3_POS             (8U)
#define LVD_CR_TRH3_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LVD_CR_TRH3_POS)) & LVD_CR_TRH3)

#define LVD_CR_DT1LEVEL             (0x00000006U)   /* Voltage detection 1 level */
#define LVD_CR_DT1LEVEL_POS         (1U)
#define LVD_CR_DT1LEVEL_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_CR_DT1LEVEL_POS)) & LVD_CR_DT1LEVEL)
#define LVD_CR_DT2LEVEL             (0x00060000U)   /* Voltage detection 2 level */
#define LVD_CR_DT2LEVEL_POS         (17U)
#define LVD_CR_DT2LEVEL_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_CR_DT2LEVEL_POS)) & LVD_CR_DT2LEVEL)
#define LVD_CR_DT3LEVEL             (0x00000006U)   /* Voltage detection 3 level */
#define LVD_CR_DT3LEVEL_POS         (1U)
#define LVD_CR_DT3LEVEL_FIELD(x)    (((uint32_t)(((uint32_t)(x)) << LVD_CR_DT3LEVEL_POS)) & LVD_CR_DT3LEVEL)

/** Bit definition for Voltage Detector Status Register  */
#define LVD_CR_VOL1DT_INTFLG        (0x00000002U)   /* The operation of voltage detection 1 interrupt Flag Bit */
#define LVD_CR_VOL2DT_INTFLG        (0x00020000U)   /* The operation of voltage detection 2 interrupt Flag Bit */
#define LVD_CR_VOL3DT_INTFLG        (0x00000002U)   /* The operation of voltage detection 3 interrupt Flag Bit */

/** Bit definition for Voltage Detector Status Register  */
#define LVD_CR_VOL1DT_OUT        (0x00000001U)
#define LVD_CR_VOL2DT_OUT        (0x00010000U)
#define LVD_CR_VOL3DT_OUT        (0x00000001U)
#endif

/**
 * List of low voltage detection modules.
 */
typedef enum
{
    LVD_VOLDT_1 = 1U,     /**< Voltage detection 1*/
    LVD_VOLDT_2 = 2U,     /**< Voltage detection 2*/
    LVD_VOLDT_3 = 3U      /**< Voltage detection 2*/
} LVD_VoltageDetection_T;

/**
 * List of threshold for over current protection .
 */
typedef enum
{
    LVD_Current_80 = 0U,     /**< 80  mA*/
    LVD_Current_120,         /**< 120 mA*/
    LVD_Current_250,         /**< 250 mA*/
    LVD_Current_300,         /**< 300 mA*/
    LVD_Current_350,         /**< 350 mA*/
    LVD_Current_400          /**< 400 mA*/
} LVD_OverCurrentThr_T;


/** @defgroup Detection voltage level
  * @{
  */
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
                                         /* GM6621                 * GM6611B            */
#define LVD_DTVOL1_Level_0        0x0    /* 1.80+0.05V/-0.05V  */ /* 4.50+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_1        0x1    /* 1.90+0.05V/-0.05V  */ /* 4.20+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_2        0x2    /* 2.00+0.05V/-0.05V  */ /* 3.90+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_3        0x3    /* 2.20+0.05V/-0.05V  */ /* 3.60+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_4        0x4    /* 2.40+0.05V/-0.05V  */ /* 3.30+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_5        0x5    /* 2.60+0.05V/-0.05V  */ /* 3.00+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_6        0x6    /* 2.80+0.05V/-0.05V  */ /* 2.70+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_7        0x7    /* 3.00+0.05V/-0.05V  */ /* 2.40+0.15V/-0.15V  */ //POR Reset at level 2.4v

#define LVD_DTVOL2_Level_0        0x0    /* 1.80+0.05V/-0.05V  */ /* 4.50+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_1        0x1    /* 1.90+0.05V/-0.05V  */ /* 4.20+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_2        0x2    /* 2.00+0.05V/-0.05V  */ /* 3.90+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_3        0x3    /* 2.20+0.05V/-0.05V  */ /* 3.60+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_4        0x4    /* 2.40+0.05V/-0.05V  */ /* 3.30+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_5        0x5    /* 2.60+0.05V/-0.05V  */ /* 3.00+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_6        0x6    /* 2.80+0.05V/-0.05V  */ /* 2.70+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_7        0x7    /* 3.00+0.05V/-0.05V  */ /* 2.40+0.15V/-0.15V  */

#define LVD_DTVOL3_Level_0        0x0    /* 1.25+0.05V/-0.05V  */ /* 1.25+0.05V/-0.05V  */
#define LVD_DTVOL3_Level_1        0x1    /* 1.30+0.05V/-0.05V  */ /* 1.30+0.05V/-0.05V  */
#define LVD_DTVOL3_Level_2        0x2    /* 1.35+0.05V/-0.05V  */ /* 1.35+0.05V/-0.05V  */
#define LVD_DTVOL3_Level_3        0x3    /* 1.40+0.05V/-0.05V  */ /* 1.40+0.05V/-0.05V  */

#elif defined (CHIP_GM6601B) || defined (CHIP_F6601C)
                                         /* GM6601B                 * F6601C          */
#define LVD_DTVOL1_Level_0        0x0    /* 4.50+0.15V/-0.15V  *//* 2.70+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_1        0x1    /* 3.90+0.15V/-0.15V  *//* 3.30+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_2        0x2    /* 3.30+0.15V/-0.15V  *//* 3.90+0.15V/-0.15V  */
#define LVD_DTVOL1_Level_3        0x3    /* 2.70+0.15V/-0.15V  *//* 4.50+0.15V/-0.15V  */

#define LVD_DTVOL2_Level_0        0x0    /* 4.20+0.15V/-0.15V  *//* 2.40+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_1        0x1    /* 3.60+0.15V/-0.15V  *//* 3.00+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_2        0x2    /* 3.00+0.15V/-0.15V  *//* 3.60+0.15V/-0.15V  */
#define LVD_DTVOL2_Level_3        0x3    /* 2.40+0.15V/-0.15V  *//* 4.20+0.15V/-0.15V  */

#define LVD_DTVOL3_Level_0        0x0    /* 1.25+0.05V/-0.05V  */
#define LVD_DTVOL3_Level_1        0x1    /* 1.30+0.05V/-0.05V  */
#define LVD_DTVOL3_Level_2        0x2    /* 1.35+0.05V/-0.05V  */
#define LVD_DTVOL3_Level_3        0x3    /* 1.40+0.05V/-0.05V  */
#endif //!GM6621


#if defined (CHIP_F6621B)
#define LVD_DTVOL1_Level_0        0x0    /* 1.9V(falling)  */ /* 2.0V(rising)  */
#define LVD_DTVOL1_Level_1        0x1    /* 2.1V(falling)  */ /* 2.2V(rising)  */
#define LVD_DTVOL1_Level_2        0x2    /* 2.3V(falling)  */ /* 2.4V(rising)  */
#define LVD_DTVOL1_Level_3        0x3    /* 2.5V(falling)  */ /* 2.6V(rising)  */
#define LVD_DTVOL1_Level_4        0x4    /* 2.7V(falling)  */ /* 2.8V(rising)  */
#define LVD_DTVOL1_Level_5        0x5    /* 2.9V(falling)  */ /* 3.0V(rising)  */
#define LVD_DTVOL1_Level_6        0x6    /* 4.0V(falling)  */ /* 4.1V(rising)  */
#define LVD_DTVOL1_Level_7        0x7    /* 4.4V(falling)  */ /* 4.5V(rising)  */

#define LVD_DTVOL2_Level_0        0x0    /* 2.0V(falling)  */ /* 2.1V(rising)  */
#define LVD_DTVOL2_Level_1        0x1    /* 2.2V(falling)  */ /* 2.3V(rising)  */
#define LVD_DTVOL2_Level_2        0x2    /* 2.4V(falling)  */ /* 2.5V(rising)  */
#define LVD_DTVOL2_Level_3        0x3    /* 2.6V(falling)  */ /* 2.7V(rising)  */
#define LVD_DTVOL2_Level_4        0x4    /* 2.8V(falling)  */ /* 2.9V(rising)  */
#define LVD_DTVOL2_Level_5        0x5    /* 3.0V(falling)  */ /* 3.1V(rising)  */
#define LVD_DTVOL2_Level_6        0x6    /* 4.1V(falling)  */ /* 4.2V(rising)  */
#define LVD_DTVOL2_Level_7        0x7    /* 4.6V(falling)  */ /* 4.7V(rising)  */

#define LVD_DTVOL3_Level_0        0x0    /* 1.05V(falling)  */ /* 1.09V(rising)  */
#define LVD_DTVOL3_Level_1        0x1    /* 1.39V(falling)  */ /* 1.44V(rising)  */
#define LVD_DTVOL3_Level_2        0x2    /* 1.44V(falling)  */ /* 1.49V(rising)  */
#define LVD_DTVOL3_Level_3        0x3    /* 1.64V(falling)  */ /* 1.69V(rising)  */
#endif



/**
 * @defgroup Callback_Macros
 * Specify macros of Callback
 * @{
 */
typedef void (*LVD_Callback)(int);

/**
  * @}
  */

/**
 * LVD configuration structure
 *
 * This structure holds the configuration settings for Voltage detection.
 *
 */
typedef struct
{
    uint8_t threshold;         /**< Configures timing threshold for filter the glitch of voltage detector */
    uint8_t detectionLevel;    /**< Configures the voltage detection level */
    uint8_t reset;             /**< reset chip when VDD is less than detectionLevel */
    uint8_t int_en;            /**< enable the interrupt of voltage detection */
    uint8_t enable;            /**< enable the operation of voltage detection */
    uint8_t filterclk;         /**< Configures Prescaler of the voltage detector 2's filter clock */
    LVD_Callback int_call;     /**< vd interrupt user callback */
} LVD_Config_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief Initializes the LVD module according to the specified parameters.
 *
 * @param pConfig1  Pointer to the configuration structure of VD1
 * @param pConfig2  Pointer to the configuration structure of VD2
 * @param pConfig3  Pointer to the configuration structure of VD3
 * @retval None.
 */
int LVD_Init(const LVD_Config_T *pConfig1, const LVD_Config_T *pConfig2,
             const LVD_Config_T *pConfig3);


/**
 * @brief Enables voltage detection interrupt.
 *
 * @param voltage Selects low voltage detection module.
 * @retval None.
 */
void LVD_EnableInterrupt(LVD_VoltageDetection_T voltage);

/**
 * @brief Disables voltage detection interrupt.
 *
 * @param voltage Selects low voltage detection module.
 * @retval None.
 */
void LVD_DisableInterrupt(LVD_VoltageDetection_T voltage);

/**
 * @brief  Clear interrupt flag.
 *
 * @param  voltage  Selects low voltage detection module.
 * @retval None.
 */
void LVD_ClearInterruptFlag(LVD_VoltageDetection_T voltage);

/**
 * @brief  Sets over current protection.
 *
 * @param  enable     Enable or disable over current protection.
 * @param  threshold  Threshold for over current protection.
 * @retval None.
 */
void LVD_SetOverCurrentProtection(uint8_t enable, LVD_OverCurrentThr_T threshold);


/**
 * @brief  Query VDx detected Flag
 *
 * @param  vdIdx    source VD Index
 * @retval 1 - voltage detector has detected voltage case
 */
int LVD_GetVDXFlag(int vdIdx);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _LVD_H_ */

