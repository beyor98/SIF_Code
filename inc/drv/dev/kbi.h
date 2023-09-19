/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          kbi.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/03/24
 *    
 * @brief         Header file of kbi driver.
 * 
 * @note
 *    2017/03/24, mark.lee, V1.0.0
 *        Initial version.
 */
 
#ifndef _KBI_H_
#define _KBI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

/** @defgroup KBI_pins_define KBI pins define
  * @{
  */
#define KBI_PIN_0                 ((uint8_t)0x01)  /**< Pin 0 selected */
#define KBI_PIN_1                 ((uint8_t)0x02)  /**< Pin 1 selected */
#define KBI_PIN_2                 ((uint8_t)0x04)  /**< Pin 2 selected */
#define KBI_PIN_3                 ((uint8_t)0x08)  /**< Pin 3 selected */
#define KBI_PIN_4                 ((uint8_t)0x10)  /**< Pin 4 selected */
#define KBI_PIN_5                 ((uint8_t)0x20)  /**< Pin 5 selected */
#define KBI_PIN_6                 ((uint8_t)0x40)  /**< Pin 6 selected */
#define KBI_PIN_7                 ((uint8_t)0x80)  /**< Pin 7 selected */
#define KBI_PIN_All               ((uint8_t)0xFF)  /**< All pins selected */
#define KBI_PIN_MASK              ((uint8_t)0xFF) /**< PIN mask for assert test */

#define IS_KBI_PIN(PIN)           (((PIN) & KBI_PIN_MASK) != (uint32_t)0x00)

/**
  * @}
  */

/** @defgroup KBI_pins_selection KBI pins selection
  * @{
  */
#define KBI_PIN_01                 ((uint32_t)0x00000001)  /**< Pin 01 selected */
#define KBI_PIN_11                 ((uint32_t)0x00000010)  /**< Pin 11 selected */
#define KBI_PIN_21                 ((uint32_t)0x00000100)  /**< Pin 21 selected */
#define KBI_PIN_31                 ((uint32_t)0x00001000)  /**< Pin 31 selected */
#define KBI_PIN_41                 ((uint32_t)0x00010000)  /**< Pin 41 selected */
#define KBI_PIN_51                 ((uint32_t)0x00100000)  /**< Pin 51 selected */
#define KBI_PIN_61                 ((uint32_t)0x01000000)  /**< Pin 61 selected */
#define KBI_PIN_71                 ((uint32_t)0x10000000)  /**< Pin 71 selected */

/**
  * @}
  */
  
/** @defgroup KBI_mode_define KBI mode define
  * @{
  */ 
#define  KBI_MODE_IT_FALLING                   0   /**< External Interrupt Mode with Falling edge trigger detection */    
#define  KBI_MODE_IT_RISING                    1   /**< External Interrupt Mode with Rising edge trigger detection */

/**
  * @}
  */


/** 
  * @brief   GPIO Init structure definition  
  */
typedef struct
{
    uint8_t pin;                  /**< Specifies the KBI pins to be configured.
                                        This parameter can be any value of @ref KBI_pins_define */
    uint8_t mode;                 /**< Specifies the operating mode for the selected pins.
                                        This parameter can be a value of @ref KBI_mode_define */
}KBI_PinConfig_T;

/**
 * @brief  Initializes KBI single pin with user specified attribute.
 *
 * @param pKBI   KBI base address.
 * @param pConfig Pointer to user-defined configuration.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE KBI_Init(KBI_Device_T *pKBI, KBI_PinConfig_T *pConfig);

/**
 * @brief  Deinitializes specific GPIO single pin to default state. 
 *
 * @param pKBI      KBI base address.
 * @param pin       Specifies the KBI pins to be configured.
 *                  This parameter can be any value of @ref KBI_pins_define.
 * @return RET_OK indicates success; Else indicates failure.
 */
RET_CODE KBI_DeInit(KBI_Device_T *pKBI, uint8_t pin);

/**
  * @brief  Enables the KBI interrupts.
  * @param pKBI   KBI base address.
  *
  * @retval None.
  */
#define KBI_ENABLE_IT(pKBI) IO_BIT_SET(pKBI->CR, 0x1)

/**
  * @brief  Disables the KBI interrupts.
  * @param pKBI   KBI base address.
  *
  * @retval None
  */
#define KBI_DISABLE_IT(pKBI) IO_BIT_CLEAR(pKBI->CR, 0x1)

/**
  * @brief  Selects  the KBI valid Input pin.
  * @param pKBI   KBI base address.
  * @param mask   PINX1 mask, This parameter can be any value of @ref KBI_pins_selection.
  * @param value   TRUE indicates PINX1 as valid input pin; FALSE indicates PINX0 as valid input pin.
  * @retval None.
  */
#define KBI_SELECT_PIN(pKBI, mask, value) ((value == TRUE) ? (IO_BIT_SET(pKBI->PIS, mask)) : (IO_BIT_CLEAR(pKBI->PIS, mask)))

/**
  * @brief  Checks whether the specified EXTI line is asserted or not.
  * @param pKBI   KBI base address.
  * @param  exti_line: specifies the EXTI line to check.
  *          This parameter can be KBI_PIN_x where x can be(0..7)
  * @retval The new state of exti_line (SET or RESET).
  */
#define KBI_GET_IT(pKBI, exti_line) (pKBI->SR & (exti_line))

/**
  * @brief  Clears the EXTI's line pending bits.
  * @param pKBI   KBI base address.
  * @param  exti_line: specifies the EXTI lines to clear.
  *          This parameter can be any combination of KBI_PIN_x where x can be (0..7)
  * @retval None
  */
#define KBI_CLEAR_IT(pKBI, exti_line) (pKBI->SR = (exti_line))

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _GPIO_H_ */
