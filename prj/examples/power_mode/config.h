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
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Chipset select
 */


#define DEBUG_PRINTF

#ifndef DEBUG_PRINTF
#define PR_DEBUG(...)       do {} while (0)
#else
#define PR_DEBUG            printf
#endif

#define SYS_UART_BAUD_RATE8M 19200

#define SYS_UART_BAUD_RATE48M 115200
/**
 * System clock configure
 */


#define SYS_CLOCK 48000000

/**
 * Add your project's Macro here
 */

#define DEBUG_PRINTF

#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_H_ */
