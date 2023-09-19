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

#define OSC_EXTERNAL_CLOCK

#define SYS_CLOCK    48000000
#if defined (CHIP_GM6621)
#undef OSC_EXTERNAL_CLOCK
#endif

#define DEBUG_PRINTF

#endif /* _CONFIG_H_ */
