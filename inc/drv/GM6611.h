/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          gm6611.h
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2015/10/13, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#if !defined(GM6611_H_)
#define GM6611_H_

/** Interrupt Number Definitions required by CMSIS */
typedef enum IRQn {
    /* Core interrupts */
    NonMaskableInt_IRQn          = -14,            /**< Non Maskable Interrupt */
    HardFault_IRQn               = -13,            /**< Cortex-M0 SV Hard Fault Interrupt */
    SVCall_IRQn                  = -5,             /**< Cortex-M0 SV Call Interrupt */
    PendSV_IRQn                  = -2,             /**< Cortex-M0 Pend SV Interrupt */
    SysTick_IRQn                 = -1,             /**< Cortex-M0 System Tick Interrupt */

    /* Device specific interrupts */
    WDOG_IRQn                    = 0,              /**< Watchdog Interrupt */
    LVD_IRQn                     = 1,              /**< LVD Interrupt */
    RTC_IRQn                     = 2,              /**< UART1 Interrupt */
    Flash_IRQn                   = 3,              /**< Flash Interrupt */
    DMA_IRQn                     = 4,              /**< DMA interrupt */
    GPIO0_IRQn                   = 5,              /**< GPIO0 interrupt */
    GPIO1_IRQn                   = 6,              /**< GPIO1 interrupt */
    GPIO2_IRQn                   = 7,              /**< GPIO2 interrupt */
    GPIO3_IRQn                   = 8,              /**< GPIO3 interrupt */
    ACMP_IRQn                    = 9,              /**< ACMP  interrupt */
    ADC_IRQn                     = 10,             /**< ADC    interrupt */
    ETM0_IRQn                    = 11,             /**< ETM0_IRQn interrupt */
    ETM1_IRQn                    = 12,             /**< ETM1_IRQn interrupt */
    ETM2_IRQn                    = 13,             /**< ETM2_IRQn interrupt */
    ETM3_IRQn                    = 14,             /**< ETM3_IRQn interrupt */
    I2C0_IRQn                    = 15,             /**< I2C0  interrupt */
    I2C1_IRQn                    = 16,             /**< I2C1  interrupt */
    I2C2_IRQn                    = 17,             /**< I2C2  interrupt */
    UART0_IRQn                   = 18,             /**< UART0 interrupt */
    UART1_IRQn                   = 19,             /**< UART1 interrupt */
    UART2_IRQn                   = 20,             /**< UART2 interrupt */
    SPI0_IRQn                    = 21,             /**< SPI0 interrupt */
    HWIR_IRQn                    = 22,             /**< HWIR_IRQn interrupt */
    CRC_IRQn                     = 23,             /**< CRC_IRQn interrupt */
    ETM4_IRQn                    = 24,             /**< ETM4_IRQn interrupt */
    ETM5_IRQn                    = 25,             /**< ETM5_IRQn interrupt */
    GPIO4_IRQn                   = 26,             /**< GPIO4 interrupt */
    GPIO5_IRQn                   = 27,             /**< GPIO5 interrupt */
    CAN_IRQn                     = 28,             /**< CAN interrupt */
    LCD_IRQn                     = 29,             /**< LCD interrupt */
    SPI1_IRQn                    = 30,             /**< SPI1 interrupt */
} IRQn_Type;

/** Interrupt Vector Macro */
#ifndef NonMaskableInt_IRQn_INTVEC
#define NonMaskableInt_IRQn_INTVEC     02
#else
#define INTVEC_02   NonMaskableInt_IRQn_INTVEC
#endif

#ifndef HardFault_IRQn_INTVEC
#define HardFault_IRQn_INTVEC          03
#else
#define INTVEC_03   HardFault_IRQn_INTVEC
#endif

#ifndef SVCall_IRQn_INTVEC
#define SVCall_IRQn_INTVEC             11
#else
#define INTVEC_11   SVCall_IRQn_INTVEC
#endif

#ifndef PendSV_IRQn_INTVEC
#define PendSV_IRQn_INTVEC             14
#else
#define INTVEC_14   PendSV_IRQn_INTVEC
#endif

#ifndef SysTick_IRQn_INTVEC
#define SysTick_IRQn_INTVEC             15
#else
#define INTVEC_15   SysTick_IRQn_INTVEC
#endif

#ifndef WDOG_IRQn_INTVEC
#define WDOG_IRQn_INTVEC                16
#else
#define INTVEC_16   WDOG_IRQn_INTVEC
#endif
#ifndef LVD_IRQn_INTVEC
#define LVD_IRQn_INTVEC                 17
#else
#define INTVEC_17   LVD_IRQn_INTVEC
#endif
#ifndef RTC_IRQn_INTVEC
#define RTC_IRQn_INTVEC                 18
#else
#define INTVEC_18   RTC_IRQn_INTVEC
#endif
#ifndef Flash_IRQn_INTVEC
#define Flash_IRQn_INTVEC               19
#else
#define INTVEC_19   Flash_IRQn_INTVEC
#endif
#ifndef DMA_IRQn_INTVEC
#define DMA_IRQn_INTVEC                 20
#else
#define INTVEC_20   DMA_IRQn_INTVEC
#endif
#ifndef GPIO0_IRQn_INTVEC
#define GPIO0_IRQn_INTVEC               21
#else
#define INTVEC_21   GPIO0_IRQn_INTVEC
#endif
#ifndef GPIO1_IRQn_INTVEC
#define GPIO1_IRQn_INTVEC               22
#else
#define INTVEC_22   GPIO1_IRQn_INTVEC
#endif
#ifndef GPIO2_IRQn_INTVEC
#define GPIO2_IRQn_INTVEC               23
#else
#define INTVEC_23   GPIO2_IRQn_INTVEC
#endif
#ifndef GPIO3_IRQn_INTVEC
#define GPIO3_IRQn_INTVEC               24
#else
#define INTVEC_24   GPIO3_IRQn_INTVEC
#endif
#ifndef ACMP_IRQn_INTVEC
#define ACMP_IRQn_INTVEC                25
#else
#define INTVEC_25   ACMP_IRQn_INTVEC
#endif
#ifndef ADC_IRQn_INTVEC
#define ADC_IRQn_INTVEC                 26
#else
#define INTVEC_26   ADC_IRQn_INTVEC
#endif
#ifndef ETM0_IRQn_INTVEC
#define ETM0_IRQn_INTVEC                27
#else
#define INTVEC_27   ETM0_IRQn_INTVEC
#endif
#ifndef ETM1_IRQn_INTVEC
#define ETM1_IRQn_INTVEC                28
#else
#define INTVEC_28   ETM1_IRQn_INTVEC
#endif
#ifndef ETM2_IRQn_INTVEC
#define ETM2_IRQn_INTVEC                29
#else
#define INTVEC_29   ETM2_IRQn_INTVEC
#endif
#ifndef ETM3_IRQn_INTVEC
#define ETM3_IRQn_INTVEC                30
#else
#define INTVEC_30   ETM3_IRQn_INTVEC
#endif
#ifndef I2C0_IRQn_INTVEC
#define I2C0_IRQn_INTVEC                31
#else
#define INTVEC_31   I2C0_IRQn_INTVEC
#endif
#ifndef I2C1_IRQn_INTVEC
#define I2C1_IRQn_INTVEC                32
#else
#define INTVEC_32   I2C1_IRQn_INTVEC
#endif
#ifndef I2C2_IRQn_INTVEC
#define I2C2_IRQn_INTVEC                33
#else
#define INTVEC_33   I2C2_IRQn_INTVEC
#endif
#ifndef UART0_IRQn_INTVEC
#define UART0_IRQn_INTVEC               34
#else
#define INTVEC_34   UART0_IRQn_INTVEC
#endif
#ifndef UART1_IRQn_INTVEC
#define UART1_IRQn_INTVEC               35
#else
#define INTVEC_35   UART1_IRQn_INTVEC
#endif
#ifndef UART2_IRQn_INTVEC
#define UART2_IRQn_INTVEC               36
#else
#define INTVEC_36   UART2_IRQn_INTVEC
#endif
#ifndef SPI0_IRQn_INTVEC
#define SPI0_IRQn_INTVEC               37
#else
#define INTVEC_37   SPI0_IRQn_INTVEC
#endif
#ifndef HWIR_IRQn_INTVEC
#define HWIR_IRQn_INTVEC                38
#else
#define INTVEC_38   HWIR_IRQn_INTVEC
#endif
#ifndef CRC_IRQn_INTVEC
#define CRC_IRQn_INTVEC                39
#else
#define INTVEC_39   CRC_IRQn_INTVEC
#endif
#ifndef ETM4_IRQn_INTVEC
#define ETM4_IRQn_INTVEC               40
#else
#define INTVEC_40   ETM4_IRQn_INTVEC
#endif
#ifndef ETM5_IRQn_INTVEC
#define ETM5_IRQn_INTVEC               41
#else
#define INTVEC_41   ETM5_IRQn_INTVEC
#endif
#ifndef GPIO4_IRQn_INTVEC
#define GPIO4_IRQn_INTVEC               42
#else
#define INTVEC_42   GPIO4_IRQn_INTVEC
#endif
#ifndef GPIO5_IRQn_INTVEC
#define GPIO5_IRQn_INTVEC               43
#else
#define INTVEC_43   GPIO5_IRQn_INTVEC
#endif
#ifndef CAN_IRQn_INTVEC
#define CAN_IRQn_INTVEC                 44
#else
#define INTVEC_44   CAN_IRQn_INTVEC
#endif
#ifndef LCD_IRQn_INTVEC
#define LCD_IRQn_INTVEC                 45
#else
#define INTVEC_45   LCD_IRQn_INTVEC
#endif
#ifndef SPI1_IRQn_INTVEC
#define SPI1_IRQn_INTVEC                46
#else
#define INTVEC_46  SPI1_IRQn_INTVEC
#endif

/**
 * Cortex M0 Core Configuration
 */
#define __CM0_REV                      0         /**< Core Revision r0p0 */
#define __MPU_PRESENT                  0         /**< Not provide MPU */
#define __NVIC_PRIO_BITS               2         /**< Uses 2 Bits for the Priority Levels */
#define __Vendor_SysTickConfig         0         /**< Standard ARM SysTick is used  */

#include "core_cm0.h"                            /**< Core Peripheral Access Layer */

#include <drv/arch/arm_m0.h>
#include <drv/arch/interrupt.h>
#include <drv/arch/systick.h>
#include <drv/dev/sys.h>
#include <../drv/dev/uart/uart_dev.h>
#include <../drv/dev/spi/spi_dev.h>
#include <../drv/dev/nvm/flash_dev.h>
#include <../drv/dev/etm/etm_dev.h>
#include <../drv/dev/gpio/gpio_dev.h>
#include <../drv/dev/dma/dma_dev.h>
#include <../drv/dev/i2c/i2c_dev.h>
#include <../drv/dev/adc/adc_dev.h>
#include <../drv/dev/crc/crc_dev.h>
#include <../drv/dev/acmp/acmp_dev.h>
#include <../drv/dev/wdog/wdog_dev.h>
#include <../drv/dev/hwir/hwir_dev.h>
#include <../drv/dev/lcd/lcd_dev.h>
#include <../drv/dev/can/can_dev.h>
#include <../drv/dev/lvd/lvd_dev.h>
#include <../drv/dev/rtc/rtc_dev.h>

#endif  /* _GM6611_H_  */
