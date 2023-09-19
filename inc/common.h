/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          common.h
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         Header file commonly used by all source files.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include <basic_types.h>
#include <string.h>
#include "config.h"
#include <drv/arch/arm_m0.h>

/*
 * Include the cpu specific header file
*/
#if (defined(CHIP_GM6601))
    #include <drv/GM6601.h>
#elif (defined(CHIP_GM6611))
    #include <drv/GM6611.h>
#elif (defined(CHIP_GM6601B))
    #include <drv/GM6601B.h>
#elif (defined(CHIP_GM6621))
    #include <drv/GM6621.h>
#elif (defined(CHIP_GM6611B))
    #include <drv/GM6611B.h>
#elif (defined(CHIP_F6621B))
    #include <drv/F6621B.h>
#elif (defined(CHIP_F6601C))
    #include <drv/F6601C.h>
#else
    #error "No valid CPU defined"
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** IO bit operation macros */
#define IO_BIT_GET(Reg, Bit)    ((Reg)&(Bit))
#define IO_BIT_SET(Reg, Bit)    (Reg)|=(Bit)
#define IO_BIT_CLEAR(Reg, Bit)  (Reg)&=~(Bit)
#define IO_BIT_TOGGLE(Reg, Bit) (Reg)^=(Bit)
#define IO_BITS_GET(Reg, Bits)  ((Reg)&(Bits))
#define IO_BITS_SET(Reg, Bits)  (Reg)|=(Bits)
#define IO_BITS_CLEAR(Reg, Bits) (Reg)&=~(Bits)
#define IO_WRITE_REG(reg, val)  ((reg) = (val))
#define IO_BITMASK_SET(Reg, Bitmask, Setmask)    (Reg)=(((Reg) & (~(Bitmask))) | (Setmask))

#if (defined(CHIP_GM6621))
    #define IO_BITS_SET_SPECAIL_ST(Reg, Bits)  (Reg)=((Reg) & (~(1<<16)))|(Bits)
    #define IO_BITMASK_SET_SPECAIL_ST(Reg, Bitmask, Setmask)    (Reg)=(((Reg) & (~(Bitmask))) | (Setmask)) &(~(1<<16))
#else
    #define IO_BITMASK_SET_SPECAIL_ST(Reg, Bitmask, Setmask)    (Reg)=(((Reg) & (~(Bitmask))) | (Setmask))
#endif
/** Keyword of interrupt routine */
#ifdef TMP89FM42
    #define __INTERRUPT         __interrupt                           /**< interrupt keyword for TMP89xx */
#else
    #define __INTERRUPT
#endif

/** Min/max macros */
/* @{ */
#if !defined(MIN)
    #define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#if !defined(MAX)
    #define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
/* @} */

/** Get container pointer from pointer to memeber of this container */
/* @{ */
#define CONTAINER_OF(ptr, type, member) (type *)((char *)(ptr) - ((uint32_t) &((type *)0)->member))
/* @} */

#ifdef  DEBUG
    /**
    * @brief  The ASSERT macro is used for function's assert declaration.
    * @param  expr: If expr is false, it prints the name of the source file and the source
    *         line number of failed code, otherwise it returns no value.
    * @retval None
    */
    #define ASSERT(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
    void assert_failed(uint8_t *file, uint32_t line);
#else
    #define ASSERT(expr)          do{}while(0)
#endif /* DEBUG */

#define UNUSED(x)             ((void)(0));

/**
 * @brief Intialize debug console to print out debug information.
 *
 * @param  None.
 *
 * @retval None.
 */
void DebugConsoleInit(uint32_t baudrate);


#endif /* _COMMON_H_ */
