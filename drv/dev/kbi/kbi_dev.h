/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          kbi_dev.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/07/19 
 *    
 * @brief         KBI device header file
 * 
 * @note
 *    2017/03/24, mark.lee, V1.0.0
 *        Initial version. 
 *
 */
#ifndef _KBI_DEV_H_
#define _KBI_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/** Base addresses of KBI Control Registers */
#define KBI_BASE       (0x40035000)

/** KBI - Register Layout Typedef */
typedef struct {
    __IO uint32_t ID;            /**< KBI Version Register, Offset: 0x0 */
    __IO uint32_t PE;            /**< KBI Interrupt Pin Enable Register, Offset: 0x4  */
     __IO uint32_t PIS;           /**< KBI Slects Pin Register, Offset: 0x8  */ 
    __IO uint32_t ES;            /**< KBI Interrupt Edge Select Register, Offset: 0xC */
    __IO uint32_t CR;            /**< KBI Interrupt Control Register, Offset: 0x10 */   
    __IO uint32_t SR;            /**< KBI Status Register, Offset: 0x14 */
} KBI_Device_T;


/** GPIO Device Pointer */
#define KBI    ((KBI_Device_T *)KBI_BASE)


/*******************************************************************************
 * Global function
 ******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GPIO_DEV_H_ */

