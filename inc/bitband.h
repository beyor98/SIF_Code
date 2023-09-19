 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          bitband.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date          2018/04/24
 *    
 * @brief         This file introduces two ways to use bit-band.
 * 
 * @note
 *    2018/04/24, terry.wu, V1.0.0
 *        Initial version.
 */
 
#ifndef _BITBAND_H_
#define _BITBAND_H_

#include <common.h>

/*
 * The first way to use bit-band.
 */
#define BIT_BAND(addr, bit)           ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bit << 2))
#define BYTE_BIT(byteAddr, bitPos)    (*(volatile unsigned long *)(BIT_BAND(byteAddr, bitPos)))


/*
 * The second way to use bit-band.
 */

/*
 * @brief bit band type.
 *
 */
typedef struct
{
		uint32_t bBit0;				    /*!< aliase to 0th bit */
		uint32_t bBit1;				    /*!< aliase to 1th bit */
		uint32_t bBit2;				    /*!< aliase to 2th bit */
		uint32_t bBit3;				    /*!< aliase to 3th bit */
		uint32_t bBit4;				    /*!< aliase to 4th bit */
		uint32_t bBit5;				    /*!< aliase to 5th bit */
		uint32_t bBit6;				    /*!< aliase to 6th bit */
		uint32_t bBit7;				    /*!< aliase to 7th bit */
		uint32_t bBit8;				    /*!< aliase to 8th bit */
		uint32_t bBit9;				    /*!< aliase to 9th bit */
		uint32_t bBit10;				/*!< aliase to 10th bit */
		uint32_t bBit11;				/*!< aliase to 11th bit */
		uint32_t bBit12;				/*!< aliase to 12th bit */
		uint32_t bBit13;				/*!< aliase to 13th bit */
		uint32_t bBit14;				/*!< aliase to 14th bit */
		uint32_t bBit15;				/*!< aliase to 15th bit */
		uint32_t bBit16;				/*!< aliase to 16th bit */
		uint32_t bBit17;				/*!< aliase to 17th bit */
		uint32_t bBit18;				/*!< aliase to 18th bit */
		uint32_t bBit19;				/*!< aliase to 19th bit */
		uint32_t bBit20;				/*!< aliase to 20th bit */
		uint32_t bBit21;				/*!< aliase to 21th bit */
		uint32_t bBit22;				/*!< aliase to 22th bit */
		uint32_t bBit23;				/*!< aliase to 23th bit */
		uint32_t bBit24;				/*!< aliase to 24th bit */
		uint32_t bBit25;				/*!< aliase to 25th bit */
		uint32_t bBit26;				/*!< aliase to 26th bit */
		uint32_t bBit27;				/*!< aliase to 27th bit */
		uint32_t bBit28;				/*!< aliase to 28th bit */
		uint32_t bBit29;				/*!< aliase to 29th bit */
		uint32_t bBit30;				/*!< aliase to 30th bit */
		uint32_t bBit31;				/*!< aliase to 31th bit */
}BIT_BandType,*BIT_BandPtr;

/**
 * @brief bit-band initialize pointer, so that invoke the pointer to access alisaed bitband.
 * @param[in]  pVariableAddress - point to variable.
 * @param[in]  pBitbandPtr - point to alisaed bitband address. 
 * @return none
 */
__STATIC_INLINE void BIT_BandVariableInit( uint32_t *pVariableAddress,BIT_BandPtr *pBitbandPtr )
{
    *pBitbandPtr = (BIT_BandPtr)(((uint32_t)pVariableAddress-0x20000000)*32+0x22000000);   
}

#endif
