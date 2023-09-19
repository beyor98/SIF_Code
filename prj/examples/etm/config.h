/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file        config.h
 *
 * @author      wen.liu
 *
 * @version     1.1.0
 *
 * @date        2016/06/13
 *
 * @brief       Configuration file of this project.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _DEF_CONFIG_H_
#define _DEF_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * System clock configure
 */
#define SYS_CLOCK    48000000
	
/**
 * Use external oscillator, mask it if use IRC
 */
//#define OSC_EXTERNAL_CLOCK


/**
 * Enable operation system or not
 */
//#define OS_ENABLE


/**
 * Enable debug with printf, mask it if disable
 *
 * if you want to use uart to print log to debug
 * you must do some setps as bellow:
 * 1st step : define Marc DEBUG_PRINTF(for example #define DEBUG_PRINTF  1 )
 * 2st step : add printf.c uart.c gpio.c and dma.c to your prj to complie
 *
 * if you dont want to use uart to print log to debug.
 * you must not define Marc DEBUG_PRINTF, So there is
 * no need to add printf.c ,uart.c.
 * only if you want uart driver to do some other thing,
 * you can add uart.c to your prj to complie.
 */
//#define DEBUG_PRINTF


/**
 * use KEIL IDE for develop
 */
#define KEIL



/**
 * Add your project's Macro here
 */



#ifdef __cplusplus
}
#endif

#endif /* _DEF_CONFIG_H_ */
