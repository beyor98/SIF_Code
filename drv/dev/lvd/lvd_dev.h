/***********************************************************************
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @filename rtc_drv.h
 *
 * @author yangy
 *
 * @version 1.0.0
 *
 * @date 2017/04/19
 *
 * @description LVD Header For GM6621
 *
 * @history
 *  Date                   By         Comments                Ver.
 *  =========       =========        =========           =========
*   2017/05/26       Yang Y          Create                  1.0.0
 **********************************************************************/

#ifndef _LVD_DEV_H_
#define _LVD_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>

#if defined (CHIP_GM6621) || defined (CHIP_GM6611B) || defined (CHIP_F6621B)

/* RTC Controller Register Layout Type Definition */
typedef struct
{
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    __IO uint32_t RESERVED[33];                 /**< RESERVED: 0x0~0x80 */
    __IO uint32_t VD1CR;                        /**< Voltage Detector1 Control Register: 0x84 */
    __IO uint32_t VD1SR;                        /**< Voltage Detector1 Status Register: 0x88  */
    __IO uint32_t RESERVED2[7];                 /**< RESERVED: 0x8C~0xA4 */
    __IO uint32_t VD2CR;                        /**< Voltage Detector2 Control Register: 0xA8 */
    __IO uint32_t VD2SR;                        /**< Voltage Detector2 Status Register: 0xAC */
    __IO uint32_t VD3CR;                        /**< Voltage Detector3 Control Register: 0xB0 */
    __IO uint32_t VD3SR;                        /**< Voltage Detector3 Status Register: 0xB4 */
#elif defined (CHIP_F6621B)
    __IO uint32_t RESERVED[37];              /**< RESERVED: 0x0~0x90 */
    __IO uint32_t VD1CR;                     /**< Voltage Detector1 Control Register: 0x94 */
    __IO uint32_t VD1SR;                     /**< Voltage Detector1 Status Register: 0x98  */
    __IO uint32_t VD2CR;                     /**< Voltage Detector2 Control Register: 0x9C */
    __IO uint32_t VD2SR;                     /**< Voltage Detector2 Status Register: 0xA0*/
    __IO uint32_t VD3CR;                     /**< Voltage Detector3 Control Register: 0xA4 */
    __IO uint32_t VD3SR;                     /**< Voltage Detector3 Status Register: 0xA8 */
#endif
} LVD_Device_T;

#if defined (CHIP_GM6621)
#define LVD_BASE        (0x40039000u)
#elif defined (CHIP_GM6611B) || defined (CHIP_F6621B)
#define LVD_BASE        (0x40000000u)
#endif
#define LVD             ((LVD_Device_T *)LVD_BASE)


#endif


#ifdef __cplusplus
}
#endif


#endif //_LVD_DEV_H_

