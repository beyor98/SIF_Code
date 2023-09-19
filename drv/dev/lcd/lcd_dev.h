/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lcd_dev.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/08/21
 *
 * @brief         LCD peripheral PHY device header file
 *
 * @note
 *    2016/08/21, mark.lee, V1.0.0
 *        Initial version.
 *
 */
#ifndef _LCD_DEV_H_
#define _LCD_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
/** LCD - Register Layout Typedef */
typedef struct
{
    __IO uint32_t CR;             /**< LCD Control Register, offset: 0x00 */
    __IO uint32_t SR;             /**< Status Register, offset: 0x04 */
    __IO uint32_t PER[2];         /**< LCD pin enable register, offset: 0x08 ~ 0x0C */
    __IO uint32_t CSR[2];         /**< LCD com pin select register, offset: 0x10 ~ 0x14 */
    __IO uint32_t DMR[13];        /**< LCD display memory Register, offset: 0x18 ~ 0x48 */
} LCD_Device_T;
#endif

#if (defined(CHIP_GM6621))
/** LCD - Register Layout Typedef */
typedef struct
{
    __IO uint32_t CR;             /**< LCD Control Register, offset: 0x00 */
    __IO uint32_t RS;             /**< LCD Resistor Divider Network Setting Register, offset: 0x04 */
    __IO uint32_t SR;             /**< Status Register, offset: 0x08 */
    __IO uint32_t REFRESH;         /**< Refresh SRAM Control Register: 0x0C */
    __IO uint32_t PER[2];         /**< LCD pin enable register, offset: 0x10 ~ 0x14 */
    __IO uint32_t CSR[2];         /**< LCD com pin select register, offset: 0x18 ~ 0x1C */
    __IO uint32_t DMR[10];        /**< LCD display memory Register, offset: 0x20 ~ 0x44 */
} LCD_Device_T;

#endif

/** Base addresses of LCD Control Registers */
#define LCD_BASE        (0x40032000u)

/** LCD Device Pointer */
#define LCD            ((LCD_Device_T *)LCD_BASE)

/** Bit definition for LCD_CR register */
#define LCD_CR_INTEN               (0x01000000U)   /**< LCD Interrupt Enable Bit */

#if (defined(CHIP_GM6621))
#define LCD_REFRESH_EN             (0x00000001U)   /**< LCD Refresh Enable Bit */

#define LCD_CR_CPEN                (0x00000004U)   /**< LCD charge pump enable Bit */

#define LCD_CR_CPBUFEN             (0x00000400U)   /**< LCD charge pump buffer enable Bit */

#define LCD_CR_BLINKEN             (0x00200000U)   /**< LCD blink mode Enable Bit */

#define LCD_CR_BLINKRATE_MASK      (0x00C00000U)   /**< BLINKRATE[1:0] bits (Blink rate selector) */
#define LCD_CR_BLINKRATE_POS       (22U)
#define LCD_CR_BLINKRATE_FIELD(x)  (((uint32_t)(((uint32_t)(x)) << LCD_CR_BLINKRATE_POS)) & LCD_CR_BLINKRATE_MASK)
#endif

#define LCD_CR_VSEL                (0x00100000U)   /**< Voltage source selector Bit */

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#define LCD_CR_OCB_MASK            (0x30000000U)   /**< OCB[1:0] bits (Opamp current bias selector) */
#define LCD_CR_OCB_POS             (28U)
#define LCD_CR_OCB_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LCD_CR_OCB_POS)) & LCD_CR_OCB_MASK)
#endif

#if defined(CHIP_GM6621)
#define LCD_CR_CONTRAST_MASK       (0x000F0000U)   /**< CONTRAST[3:0] bits (Contrast selector) */
#else
#define LCD_CR_CONTRAST_MASK       (0x00070000U)   /**< CONTRAST[2:0] bits (Contrast selector) */
#endif
#define LCD_CR_CONTRAST_POS        (16U)
#define LCD_CR_CONTRAST_FIELD(x)   (((uint32_t)(((uint32_t)(x)) << LCD_CR_CONTRAST_POS)) & LCD_CR_CONTRAST_MASK)

#define LCD_CR_PSC_MASK            (0x0000F000U)   /**< PSC[3:0] bits (prescaler selector) */
#define LCD_CR_PSC_POS             (12U)
#define LCD_CR_PSC_FIELD(x)        (((uint32_t)(((uint32_t)(x)) << LCD_CR_PSC_POS)) & LCD_CR_PSC_MASK)

#define LCD_CR_BIAS_MASK           (0x00000300U)   /**< BIAS[1:0] bits (Bias selector) */
#define LCD_CR_BIAS_POS            (8U)
#define LCD_CR_BIAS_FIELD(x)       (((uint32_t)(((uint32_t)(x)) << LCD_CR_BIAS_POS)) & LCD_CR_BIAS_MASK)

#define LCD_CR_DUTY_MASK           (0x00000070U)   /**< DUTY[2:0] bits (Duty selector) */
#define LCD_CR_DUTY_POS            (4U)
#define LCD_CR_DUTY_FIELD(x)       (((uint32_t)(((uint32_t)(x)) << LCD_CR_DUTY_POS)) & LCD_CR_DUTY_MASK)

#define LCD_CR_DISMODE             (0x00000002U)   /**< Display mode selector Bit */

#define LCD_CR_LCDEN               (0x00000001U)   /**< LCD Enable Bit */

/** Bit definition for LCD_SR register */
#define LCD_SR_BUSY                ((uint32_t)0x00000001)     /**< LCD display Bit */
#define LCD_SR_INT                 ((uint32_t)0x00010000)     /**< Frame interrupt Flag Bit */

/** Bit definition for LCD_CSR register */
#define LCD_CSR_COM0_SEL_MASK      (0x0000003FU)   /**< COM[5:0] bits (com0 pin selector) */
#define LCD_CSR_COM0_SEL_POS       (0U)
#define LCD_CSR_COM0_SEL_FIELD(x)  (((uint32_t)(((uint32_t)(x)) << LCD_CSR_COM0_SEL_POS)) & LCD_CSR_COM0_SEL_MASK)

#define LCD_CSR_COM1_SEL_MASK      (0x00003F00U)   /**< COM[5:0] bits (com1 pin selector) */
#define LCD_CSR_COM1_SEL_POS       (8U)
#define LCD_CSR_COM1_SEL_FIELD(x)  (((uint32_t)(((uint32_t)(x)) << LCD_CSR_COM1_SEL_POS)) & LCD_CSR_COM1_SEL_MASK)

#define LCD_CSR_COM2_SEL_MASK      (0x003F0000U)   /**< COM[5:0] bits (com2 pin selector) */
#define LCD_CSR_COM2_SEL_POS       (16U)
#define LCD_CSR_COM2_SEL_FIELD(x)  (((uint32_t)(((uint32_t)(x)) << LCD_CSR_COM2_SEL_POS)) & LCD_CSR_COM2_SEL_MASK)

#define LCD_CSR_COM3_SEL_MASK      (0x3F000000U)   /**< COM[5:0] bits (com3 pin selector) */
#define LCD_CSR_COM3_SEL_POS       (24U)
#define LCD_CSR_COM3_SEL_FIELD(x)  (((uint32_t)(((uint32_t)(x)) << LCD_CSR_COM3_SEL_POS)) & LCD_CSR_COM3_SEL_MASK)

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief  Configures the LCD Contrast.
 * @param  value: specifies the LCD Contrast.
 *   This parameter can be one value of @ref LCD_Contrast.
 * @retval None
 */
#define LCD_CONTRAST_CONFIG(value)        IO_BITMASK_SET(LCD->CR, LCD_CR_CONTRAST_MASK, LCD_CR_CONTRAST_FIELD(value));

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
/**
 * @brief  Configures the LCD Opamp current bias.
 * @param  value: specifies the LCD Opamp current bias.
 *   This parameter can be one value of @ref LCD_Current_Bias.
 * @retval None
 */
#define LCD_OCB_CONFIG(value)             IO_BITMASK_SET(LCD->CR, LCD_CR_OCB_MASK, LCD_CR_OCB_FIELD(value));
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ETM_DEV_H_ */

