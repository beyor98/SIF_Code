/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          crc_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 * 
 * @date          2016/07/05 
 *    
 * @brief         CMSIS Peripheral Access Layer for GM6601
 * 
 * @note
 *    2016/04/01, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/05, Mike.Zheng, V1.1.0
 *        Update coding style.
 *    2016/11/23, Mike.Zheng, V1.2.0
 *        Update For Release.
 */
#ifndef _CRC_DEV_H_
#define _CRC_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <common.h>   
#include <drv/dev/clock.h>

//#define CRC_DEBUG_PRINTF
//#define CRC_WARNING_PRINTF
//#define CRC_ERR_PRINTF

#ifndef CRC_DEBUG_PRINTF
    #define CRC_PR_DEBUG(...)      do {} while (0)
#else
    #define CRC_PR_DEBUG           printf
#endif

#ifndef CRC_ERR_PRINTF
    #define CRC_PR_ERR(...)        do {} while (0)
#else
    #define CRC_PR_ERR             printf
#endif

#ifndef CRC_WARNING_PRINTF
    #define CRC_PR_WARNINR(...)    do {} while (0)
#else
    #define CRC_PR_WARNINR         printf
#endif

#define CRC_ACCUMULATE_TIEMOUT_US        10000

/**
 * CRC Control Register Layout Type Definition 
 */
typedef struct
{
    __IO uint32_t CTRL;                 			  /* CRC Contral Register:offset: 0x00 */
    __IO uint32_t POL;                 		 		  /* CRC Polynomia Register:offset: 0x04 */
    __IO uint32_t IVAL;                 			  /* CRC InitValue Register:offset: 0x08 */
    __IO uint32_t DA;                 			      /* CRC Data Register:offset: 0x0c */
    __IO uint32_t XOR;                  			  /* CRC Xorvalue Register:offset: 0x10 */
    __IO uint32_t RST;               			      /* CRC Accumulate :Result Register:offset: 0x14 */
    __IO uint32_t ST;                   			  /* CRC Status Register:offset: 0x18 */
	__IO uint32_t RESV0[56];         			      /* CRC Reserved Register:offset: 0x1c~0xf8 */
    __IO uint32_t VERNUM;              			      /* CRC Version Num Register:offset: 0xfc */  
} CRC_Device_T;

#define CRC_BASE                         (0x4000a000u)  /* Base Addresses of CRC Registers */

#define CRC_DEV                          ((CRC_Device_T *)CRC_BASE) /* CRC Device Pointer */

#define CRC_RESET_DELAYCNT               (0xff)/* CRC Module Reset Timedelay */

#define CRC_CLOCK_GATE_MASK              (0x1<<8)/* CRC Clock Gate MaskBIt */

/*************** CRC Contral Register Bitfiled ******************/
#define CRC_CTRL_IE_POS                  ((uint32_t) 16)                                    /* CRC INT Enable Pos */
#define CRC_CTRL_IE                      ((uint32_t) ((uint32_t)1 << CRC_CTRL_IE_POS))      /* CRC INT Enable Filed */
#define CRC_CTRL_XOE_POS                 ((uint32_t) 12)                                    /* CRC Xor Output Enable Pos */
#define CRC_CTRL_XOE                     ((uint32_t) ((uint32_t)1 << CRC_CTRL_XOE_POS))     /* CRC Xor Output Enable Filed */
#define CRC_CTRL_RRO_POS                 ((uint32_t) 11)                                    /* Control Bit Reversal Of The Output CRC Calc Result POS */
#define CRC_CTRL_RRO                     ((uint32_t) ((uint32_t)1 << CRC_CTRL_RRO_POS))     /* Control Bit Reversal Of The Output CRC Calc Result Filed */
#define CRC_CTRL_RDI_POS                 ((uint32_t) 8)                                     /* Control The Reversal of The Bit Order Of The Input Data Pos */        
#define CRC_CTRL_RDI                     ((uint32_t) ((uint32_t)3 << CRC_CTRL_RDI_POS))     /* Control The Reversal of The Bit Order Of The Input Data Filed */
#define CRC_CTRL_DIW_POS                 ((uint32_t) 6)                                     /* The Input Data Width Pos */
#define CRC_CTRL_DIW                     ((uint32_t) ((uint32_t)3 << CRC_CTRL_DIW_POS))     /* The Input Data Width Filed */
#define CRC_CTRL_PS_POS                  ((uint32_t) 4)                                     /* Control The Size Of The Polynomial Pos */
#define CRC_CTRL_PS                      ((uint32_t) ((uint32_t)3 << CRC_CTRL_PS_POS))      /* Control The Size Of The Polynomial Filed */
#define CRC_CTRL_CRST_POS                ((uint32_t) 0)                                     /* Reset CRC Calculation Pos */
#define CRC_CTRL_CRST                    ((uint32_t) ((uint32_t)1 << CRC_CTRL_CRST_POS))    /* Reset CRC Calculation Filed */



/*************** CRC Status Register Bitfiled ******************/
#define CRC_ST_DNE_POS                     ((uint32_t) 0)                                   /* CRC Calculation Finish Pos */
#define CRC_ST_DNE                         ((uint32_t) ((uint32_t)1 << CRC_ST_DNE_POS))     /* CRC Calculation Finish Filed */

/**
 * CRC Accumulate Datewidth For One Time 
 */
typedef enum  
{
    DATA_PROCESS_WIDTH_8B  = 0x00,               	             /**< CRC Calc 8bit Datewidth For one time */    
    DATA_PROCESS_WIDTH_16B       ,                               /**< CRC Calc 16bit Datewidth For one time */
    DATA_PROCESS_WIDTH_32B                      	             /**< CRC Calc 32bit Datewidth For one time */
} CRC_ProcessWidth_T;

/**
 * @brief Set CRC Accumulate InitValue
 * @param pCRC CRC Devive Point
 * @param initValue CRC Accumulate InitValue
 * @retval None
 */
#define SET_INITVAL(pCRC, initValue)            		         (pCRC->IVAL = initValue)

/**
 * @brief Set CRC Accumulate Polynomia
 * @param pCRC CRC Devive Point
 * @param value CRC Accumulate polynomia
 * @retval None
 */
 
#define SET_POL(pCRC, value)                                     (pCRC->POL = value)     

/**
 * @brief Set CRC Accumulate mode
 * @param pCRC CRC Devive Point
 * @param polyLength CRC Accumulate Mode
 * @Pol_Bit_Width = CRC_POLYLENGTH_8B  : crc8
 * @Pol_Bit_Width = CRC_POLYLENGTH_16B : crc16
 * @Pol_Bit_Width = CRC_POLYLENGTH_32B : crc32
 * @Pol_Bit_Width = CRC_POLYLENGTH_7B  : crc7
 * @retval None
 */

#define SET_POL_BITWIDTH(pCRC, polyLength)                      IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_PS,                 \
                                                                               (polyLength << CRC_CTRL_PS_POS))

/**
 * @brief Set HW Accumulate DataBit Width One Time
 * @param pCRC pCRC Devive Point
 * @param dataBitWidth  HW Accumulate DataBit Width One Time
 * @retval None
 */
#define SET_DATA_PROCESS_BITWIDTH(pCRC, dataBitWidth)     						    									\
						                                        IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_DIW, 		        \
                                                                              (dataBitWidth << CRC_CTRL_DIW_POS))

/**
 * @brief Set Inputdata Inversion Mode
 * @param pCRC CRC Devive Point
 * @param intputInversion Inputdata Inversion Mode
 * @retval None
 */
#define SET_INPUTDATA_INVERSION(pCRC,intputInversion)     															    \
    		                                      				IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_RDI, 		        \
                                                         			   		  (intputInversion << CRC_CTRL_RDI_POS))

/**
 * @brief Set Outputdata Inversion Mode
 * @param pCRC CRC Devive Point
 * @param outputInversion Outputdata Inversion Mode
 * @retval None
 */
#define SET_OUTPUTDATA_INVERSION(pCRC,outputInversion)     															    \
                                          						IO_BITMASK_SET(pCRC->CTRL, CRC_CTRL_RRO, 	            \
                                                                	  		  (outputInversion << CRC_CTRL_RRO_POS))

/**
 * @brief Enable CRC Xorval 
 * @param pCRC CRC Devive Point
 * @retval None
 */
#define XORVAL_ENABLE(pCRC)				                        IO_BIT_SET(pCRC->CTRL, CRC_CTRL_XOE)

/**
 * @brief  Set CRC Xor Value 
 * @param pCRC CRC Devive Point
 * @param Value CRC Xor Value 
 * @retval None
 */
#define SET_XORVAl(pCRC,Value)                	                (pCRC->XOR = Value)
													
/**
 * @brief  Load Initvalue And Ready To Accumulate
 * @param pCRC CRC Devive Point
 * @retval None
 */
#define CALC_RESTART(pCRC)                                      IO_BIT_SET(pCRC->CTRL, CRC_CTRL_CRST)

/**
 * @brief Disable CRC Interrupt 
 * @param pCRC CRC Devive Point
 * @retval None
 */
#define INTERRUPT_DISABLE(pCRC)                                 IO_BIT_CLEAR(pCRC->CTRL, CRC_CTRL_IE)

/**
 * @brief Resert CRC Mode
 * @retval None
 */
#define CRC_RESET()                                             do{                                                    \
                                                                    Clock_ResetPeripherals(CRC_RESET,                  \
                                                                                           Hold_ResetStatusTillStable, \
                                                                                           CRC_RESET_DELAYCNT);        \
                                                                }while (0)
/**
 * @brief Enable CRC Clock
 * @retval None
 */
#define CRC_CLOCK_ENABLE()                                      do{                                                    \
                                                                    Clock_PeripheralClockEnable(CRC_CLOCK_GATE_MASK);  \
                                                                }while (0)    /* enable CRC ip clk */


#ifdef __cplusplus
}
#endif

#endif /*_CRC_DEV_H_*/












































