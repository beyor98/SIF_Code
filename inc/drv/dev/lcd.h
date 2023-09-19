/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lcd.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/08/17
 *
 * @brief         lcd device header file
 *
 * @note
 *    2016/08/17, mark.lee, V1.0.0
 *        Initial version.
 *
 */
#ifndef _LCD_H_
#define _LCD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/**
 * List of LCD com.
 */
typedef enum
{
    LCD_COM_0 = 0U,    /**< LCD com 0 */
    LCD_COM_1,         /**< LCD com 1 */
    LCD_COM_2,         /**< LCD com 2 */
    LCD_COM_3,         /**< LCD com 3 */
    LCD_COM_4,         /**< LCD com 4 */
    LCD_COM_5,         /**< LCD com 5 */
    LCD_COM_6,         /**< LCD com 6 */
    LCD_COM_7,         /**< LCD com 7 */
} LCD_COM_T;


/** @defgroup LCD_Prescaler
  * @{
  */
#define LCD_Prescaler_0      0x0
#define LCD_Prescaler_1      0x1
#define LCD_Prescaler_2      0x2
#define LCD_Prescaler_3      0x3
#define LCD_Prescaler_4      0x4
#define LCD_Prescaler_5      0x5
#define LCD_Prescaler_6      0x6
#define LCD_Prescaler_7      0x7
#define LCD_Prescaler_8      0x8
#define LCD_Prescaler_9      0x9
#define LCD_Prescaler_10     0xA
#define LCD_Prescaler_11     0xB
#define LCD_Prescaler_12     0xC
#define LCD_Prescaler_13     0xD
#define LCD_Prescaler_14     0xE
#define LCD_Prescaler_15     0xF

/**
  * @}
  */

/** @defgroup LCD_Duty
  * @{
  */

#define LCD_Duty_Static        0x0  /**< Static duty */
#define LCD_Duty_1_2           0x1  /**< 1/2 duty    */
#define LCD_Duty_1_3           0x2  /**< 1/3 duty    */
#define LCD_Duty_1_4           0x3  /**< 1/4 duty    */
#define LCD_Duty_1_5           0x4  /**< 1/5 duty    */
#define LCD_Duty_1_6           0x5  /**< 1/6 duty    */
#define LCD_Duty_1_7           0x6  /**< 1/7 duty    */
#define LCD_Duty_1_8           0x7  /**< 1/8 duty    */

/**
  * @}
  */

/** @defgroup LCD_Bias
  * @{
  */

#define LCD_Bias_Static        0x0  /**< Static Bias */
#define LCD_Bias_1_2           0x1  /**< 1/2 Bias */
#define LCD_Bias_1_3           0x2  /**< 1/3 Bias */

/**
  * @}
  */

/** @defgroup LCD_Voltage_Source
  * @{
  */

#define LCD_VoltageSource_Internal      0x0  /**< Internal voltage source for the LCD */
#define LCD_VoltageSource_External      0x1  /**< External voltage source for the LCD */

/** @defgroup LCD_Display_Mode
  * @{
  */

#define LCD_Display_Line       0x0  /**< Line mode for the LCD display */
#define LCD_Display_Field      0x1  /**< Field mode for the LCD display */

/**
  * @}
  */

/** @defgroup LCD_RAMRegister
  * @{
  */
#define LCD_RAMRegister_0      ((uint32_t)0x00000000) /**< LCD RAM Register 0  */
#define LCD_RAMRegister_1      ((uint32_t)0x00000001) /**< LCD RAM Register 1  */
#define LCD_RAMRegister_2      ((uint32_t)0x00000002) /**< LCD RAM Register 2  */
#define LCD_RAMRegister_3      ((uint32_t)0x00000003) /**< LCD RAM Register 3  */
#define LCD_RAMRegister_4      ((uint32_t)0x00000004) /**< LCD RAM Register 4  */
#define LCD_RAMRegister_5      ((uint32_t)0x00000005) /**< LCD RAM Register 5  */
#define LCD_RAMRegister_6      ((uint32_t)0x00000006) /**< LCD RAM Register 6  */
#define LCD_RAMRegister_7      ((uint32_t)0x00000007) /**< LCD RAM Register 7  */
#define LCD_RAMRegister_8      ((uint32_t)0x00000008) /**< LCD RAM Register 8  */
#define LCD_RAMRegister_9      ((uint32_t)0x00000009) /**< LCD RAM Register 9  */

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#define LCD_RAMRegister_10     ((uint32_t)0x0000000A) /**< LCD RAM Register 10  */
#define LCD_RAMRegister_11     ((uint32_t)0x0000000B) /**< LCD RAM Register 11  */
#define LCD_RAMRegister_12     ((uint32_t)0x0000000C) /**< LCD RAM Register 12  */
#endif
/**
  * @}
  */

/** @defgroup LCD_Contrast
  * @{
  */
#if defined(CHIP_GM6621)
#define LCD_Contrast_Level_0        0x0  /**< -12%      */
#define LCD_Contrast_Level_1        0x1  /**< -10.5%    */
#define LCD_Contrast_Level_2        0x2  /**< -9%       */
#define LCD_Contrast_Level_3        0x3  /**< -7.5%     */
#define LCD_Contrast_Level_4        0x4  /**< -6%       */
#define LCD_Contrast_Level_5        0x5  /**< -4.5%     */
#define LCD_Contrast_Level_6        0x6  /**< -3%       */
#define LCD_Contrast_Level_7        0x7  /**< -1.5%     */
#define LCD_Contrast_Level_8        0x8  /**< -0%       */
#define LCD_Contrast_Level_9        0x9  /**< 1.5%      */
#define LCD_Contrast_Level_10       0xA  /**< 3%        */
#define LCD_Contrast_Level_11       0xB  /**< 4.5%      */
#define LCD_Contrast_Level_12       0xC  /**< 6%        */
#define LCD_Contrast_Level_13       0xD  /**< 7.5%      */
#define LCD_Contrast_Level_14       0xE  /**< 9.5%      */
#define LCD_Contrast_Level_15       0xF  /**< 10.5%     */
#else
#define LCD_Contrast_Level_0        0x0  /**< Maximum Voltage = 2.60V    */
#define LCD_Contrast_Level_1        0x1  /**< Maximum Voltage = 2.73V    */
#define LCD_Contrast_Level_2        0x2  /**< Maximum Voltage = 2.86V    */
#define LCD_Contrast_Level_3        0x3  /**< Maximum Voltage = 2.99V    */
#define LCD_Contrast_Level_4        0x4  /**< Maximum Voltage = 3.12V    */
#define LCD_Contrast_Level_5        0x5  /**< Maximum Voltage = 3.25V    */
#define LCD_Contrast_Level_6        0x6  /**< Maximum Voltage = 3.38V    */
#define LCD_Contrast_Level_7        0x7  /**< Maximum Voltage = 3.51V    */
#endif
/**
  * @}
  */

/** @defgroup LCD_Current_Bias
  * @{
  */

#define LCD_Current_Bias_Level_0        0x0  /**< current bias = 1uA    */
#define LCD_Current_Bias_Level_1        0x1  /**< current bias = 2uA    */
#define LCD_Current_Bias_Level_2        0x2  /**< current bias = 5uA    */
#define LCD_Current_Bias_Level_3        0x3  /**< current bias = 10uA    */

/**
  * @}
  */

#if (defined(CHIP_GM6621))
/** @defgroup LCD_BlinkFrequency LCD Blink Frequency
  * @{
  */

#define LCD_BLINKFREQUENCY_DIV32        0x0  /**< The Blink frequency = fLCD/32   */
#define LCD_BLINKFREQUENCY_DIV64        0x1  /**< The Blink frequency = fLCD/64   */
#define LCD_BLINKFREQUENCY_DIV128       0x2  /**< The Blink frequency = fLCD/128  */
#define LCD_BLINKFREQUENCY_DIV256       0x3  /**< The Blink frequency = fLCD/256  */
/**
  * @}
  */
#endif

/**
 * LCD configuration structure
 *
 * This structure holds the configuration settings for LCD display.
 *
 */
typedef struct
{
    uint8_t prescaler;         /**< Configures the LCD Prescaler.
                                   This parameter can be one value of @ref LCD_Prescaler */
    uint8_t duty;              /**< Configures the LCD Duty.
                                   This parameter can be one value of @ref LCD_Duty */
    uint8_t bias;              /**< Configures the LCD Bias.
                                   This parameter can be one value of @ref LCD_Bias */
    uint8_t voltageSource;     /**< Selects the LCD Voltage source.
                                   This parameter can be one value of @ref LCD_Voltage_Source */
    uint8_t displayMode;       /**< Selects the LCD Display mode.
                                   This parameter can be one value of @ref LCD_Display_Mode */
    uint8_t contrast;          /**< specifies the LCD Contrast.
                                   This parameter can be one value of @ref LCD_Contrast */
} LCD_Config_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief Initializes the LCD peripheral according to the specified parameters.
 *
 * @param pConfig      Pointer to the configuration structure.
 * @retval None.
 */
void LCD_Init(const LCD_Config_T *pConfig);

/**
 * @brief Starts LCD display.
 *
 * @retval None
 *
 */
void LCD_StartDisplay(void);


/**
 * @brief Stops LCD display.
 *
 * @retval None.
 *
 */
void LCD_StopDisplay(void);

/**
 * @brief Enable I/O as LCD pin.
 *
 * @param  index: The index of LCD pin enable register which can be (0, 1).
 * @param  data: specifies LCD Pin Value to be written.
 * @retval None
 *
 */
void LCD_EnablePin(uint8_t index, uint32_t data);

/**
 * @brief Disable I/O as LCD pin.
 *
 * @param  index: The index of LCD pin enable register which can be (0, 1).
 * @param  data: specifies LCD Pin Value to be written.
 * @retval None
 *
 */
void LCD_DisablePin(uint8_t index, uint32_t data);

/**
 * @brief Select com pin.
 *
 * @param  com: com number.
 * @param  pin: pin number which can be (0..51).
 * @retval None
 *
 */
void LCD_SelectComPin(LCD_COM_T com, uint8_t pin);

/**
 * @brief Writes a word in the specific LCD RAM.
 *
 * @param  LCD_RAMRegisterIndex: specifies the LCD RAM index.
 *   This parameter can be one value of @ref LCD_RAMRegister.
 * @param  RAMRegisterMask: specifies the LCD RAM Register Data Mask.
 * @param  data: specifies LCD Data Value to be written.
 * @retval None
 *
 */
void LCD_Write(uint32_t LCD_RAMRegisterIndex, uint32_t RAMRegisterMask, uint32_t data);

/**
 * @brief Clear LCD display memory.
 *
 * @retval None.
 *
 */
void LCD_ClearDisplay(void);

#if (defined(CHIP_GM6621))
/**
 * @brief  Configures the LCD Blink mode and Blink frequency.
 * @param  onOff: Enable or Disable LCD blink mode.
 * @param  blinkFrequency: specifies the LCD blink frequency.
 *         This parameter can be one value of @ref LCD_BlinkFrequency.
 * @retval None
 */
void LCD_BlinkConfig(uint8_t onOff, uint8_t blinkFrequency);

/**
 * @brief  Configures the LCD Refresh.
 * @param  onOff: Enable or Disable LCD Refresh.
 * @param  loopCnt: specifies the number of frame.
 * @param  addr: specifies the address of SDRAM.
 *
 * @retval None
 */
void LCD_RefreshConfig(uint8_t onOff, uint8_t loopCnt, uint32_t addr);

/**
 * @brief  Configures the LCD Charge Pump Mode.
 * @param  onOff: Enable or Disable LCD Charge Pump Mode.
 * @param  prescaler: Configures the LCD Charge pump clock Prescaler which can be(0 ~7).
 * @param  vol: LCD voltage level selection in charge bump mode, 0: 3V; 1: 5V.
 *
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE LCD_ChargePumpConfig(uint8_t onOff, uint8_t prescaler, uint8_t vol);


#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _LCD_H_ */

