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
 *    2015/08/21, Wen.Liu, V1.0.0
 *        Initial version.
 *    2016/06/13, Wen.Liu, V1.1.0
 *        Update coding style.
 *    2016/11/22, Mike.Zheng, V1.1.0
 *        Update for how to config for 
 *        uart print log.
 */
#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SYS_CLOCK    8000000

//#define OSC_EXTERNAL_CLOCK


/**
 * if you want to use uart to print log to debug
 * you must do some setps as bellow:
 * 1st step : define Marc DEBUG_PRINTF(for example #define DEBUG_PRINTF  1 )
 * 2st step : add printf.c and uart.c to your prj to complie
 *
 * if you dont want to use uart to print log to debug.
 * you must not define Marc DEBUG_PRINTF, So there is
 * no need to add printf.c and uart.c, only if you want
 * uart driver to do some other thing,you can add uart.c
 * to your prj to complie.
 */
#define DEBUG_PRINTF  1 

#ifdef __cplusplus
}
#endif
  
#endif /* _CONFIG_H_ */
