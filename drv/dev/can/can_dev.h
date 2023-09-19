/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          can_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2017/03/23
 *
 * @brief         CMSIS Peripheral Access Layer for GM6611
 *
 * @note
 *     2017/03/23, Mike.Zheng, V1.0.0
 *        Initial version.
 *     2017/06/12, Mike.Zheng, V1.1.0
 *        Update coding style.
 */

#ifndef _CAN_DEV_H_
#define _CAN_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stdio.h"
#include <common.h>
#include <drv/dev/dma.h>
#include <drv/dev/clock.h>


#define CAN_ERR_PRINTF     1
//#define CAN_DEBUG_PRINTF   1


#ifndef CAN_DEBUG_PRINTF
#define CAN_PR_DEBUG(...)                                   do {} while (0)
#else
#define CAN_PR_DEBUG                                        printf
#endif

#ifndef CAN_ERR_PRINTF
#define CAN_PR_ERR(...)                                     do {} while (0)
#else
#define CAN_PR_ERR                                          printf
#endif	
	

/* CAN control register layout type definition */
typedef struct
{
    __IO uint32_t ID;                                           /* CAN identification register:offest: 0x0 */
    __IO uint32_t RESV0;                                        /* CAN reserver register:offset: 0x04 */
    __IO uint32_t CTRL;                                         /* CAN contral register:offset: 0x8 */
    __IO uint32_t RESV1;                                        /* CAN reserver register:offset: 0x0c */
    __IO uint32_t CFG;                                          /* CAN config register:offset: 0x10 */
    __IO uint32_t IE;                                           /* CAN interrupt enable register:offset: 0x14 */
    __IO uint32_t BUSTIM;                                       /* CAN bus tim register:offset  0x18 */
    __IO uint32_t RESV2;                                        /* CAN reserver register:offset: 0x1c */
#if (defined(CHIP_GM6611))
    __IO uint32_t AC0;                                          /* CAN acceptance code0 register:offset: 0x20 */
    __IO uint32_t AMSK0;                                        /* CAN acceptance mask0 register:offset: 0x24 */
    __IO uint32_t AC1;                                          /* CAN acceptance code1 register:offset: 0x28 */
    __IO uint32_t AMSK1;                                        /* CAN acceptance mask1 register:offset: 0x2c */
#elif (defined(CHIP_GM6611B))
    __IO uint32_t AC;                                           /* CAN acceptance code register:offset: 0x20 */
    __IO uint32_t AMSK;                                         /* CAN acceptance mask register:offset: 0x24 */
    __IO uint32_t ASEL;                                         /* CAN acceptance selection register:offset: 0x28 */
    __IO uint32_t RESV;                                         /* CAN reserved register:offset: 0x2c */
#endif
    __IO uint32_t EWT;                                          /* CAN error warning threshold register:offset: 0x30 */
    __IO uint32_t RESV3;                                        /* CAN reserver register:offset: 0x34 */
    __IO uint32_t OUTCTRL;                                      /* CAN output control register:offset: 0x38 */
    __IO uint32_t RESV4;                                        /* CAN reserver register:offset: 0x3c */
    __IO uint32_t ST;                                           /* CAN status register:offset: 0x40 */
    __IO uint32_t INTST;                                        /* CAN interrupt register:offset: 0x44 */
    __IO uint32_t RBST;                                         /* CAN receive buffer status register:offset: 0x48 */
    __IO uint32_t RESV5;                                        /* CAN reserver register:offset: 0x4c */
    __IO uint32_t TXFRMINFO;                                    /* CAN TX frame information register:offset: 0x50 */
    __IO uint32_t TXID;                                         /* CAN TX identifier register:offset: 0x54 */
    __IO uint32_t TXDATA0;                                      /* CAN TX data byte part0 register:offset: 0x58 */
    __IO uint32_t TXDATA1;                                      /* CAN TX data byte part1 register:offset: 0x5c */
    __IO uint32_t RXFRMINFO;                                    /* CAN RX frame information register:offset: 0x60 */
    __IO uint32_t RXID;                                         /* CAN RX identifier register:offset: 0x64 */
    __IO uint32_t RXDATA0;                                      /* CAN RX data byte part0 register:offset: 0x68 */
    __IO uint32_t RXDATA1;                                      /* CAN RX data byte part1  register:offset: 0x6c */
    __IO uint32_t ALB;                                          /* CAN arbitration lost bit register:offset: 0x70 */
    __IO uint32_t ERRINFO;                                      /* CAN error information register:offset: 0x74 */
    __IO uint32_t ERRCNT;                                       /* CAN error counter register:offset: 0x78 */
#if (defined(CHIP_GM6611))
    __IO uint32_t RXFIFO[16];                                   /* CAN receive fifo register offset:0x80-0xbc */
#elif (defined(CHIP_GM6611B))
    __IO uint32_t RXFIFO[32];                                   /* CAN receive fifo register offset:0x80-0xfc */
#endif


} CAN_Device_T;

/*************** CAN CTRL register bitfiled ****************/
#define CAN_CTRL_CDOR_POS                                       ((uint32_t) 17)                                 /* CAN clear data overrun pos */
#define CAN_CTRL_CDOR                                           ((uint32_t) ((uint32_t)1 << CAN_CTRL_CDOR_POS)) /* CAN clear data overrun filed */
#define CAN_CTRL_RRB_POS                                        ((uint32_t) 16)                                 /* CAN release receive buffer pos */
#define CAN_CTRL_RRB                                            ((uint32_t) ((uint32_t)1 << CAN_CTRL_RRB_POS))  /* CAN release receive buffer filed */
#define CAN_CTRL_SRR_POS                                        ((uint32_t) 8)                                  /* CAN self reception request pos */
#define CAN_CTRL_SRR                                            ((uint32_t) ((uint32_t)1 << CAN_CTRL_SRR_POS))  /* CAN self reception request filed */
#define CAN_CTRL_AT_POS                                         ((uint32_t) 1)                                  /* CAN abort transmission pos */
#define CAN_CTRL_AT                                             ((uint32_t) ((uint32_t)1 << CAN_CTRL_AT_POS))   /* CAN abort transmission filed */
#define CAN_CTRL_TR_POS                                         ((uint32_t) 0)                                  /* CAN transmission request pos */
#define CAN_CTRL_TR                                             ((uint32_t) ((uint32_t)1 << CAN_CTRL_TR_POS))   /* CAN transmission request filed */

/*************** CAN CFG register bitfiled *****************/
#define CAN_CFG_SME_POS                                         ((uint32_t) 8)                                  /* CAN sleep mode enable pos */
#define CAN_CFG_SME                                             ((uint32_t) ((uint32_t)1 << CAN_CFG_SME_POS))   /* CAN sleep mode enable filed */
#define CAN_CFG_VM_POS                                          ((uint32_t) 0)                                  /* CAN work mode pos */
#define CAN_CFG_VM                                              ((uint32_t) ((uint32_t)3 << CAN_CFG_VM_POS))    /* CAN work mode filed */

/*************** CAN IE register bitfiled ******************/
#define CAN_IE_WIE_POS                                          ((uint32_t) 24)                                 /* CAN wake up interrupt enable pos */
#define CAN_IE_WIE                                              ((uint32_t) ((uint32_t)1 << CAN_IE_WIE_POS))    /* CAN wake up interrupt enable filed */
#define CAN_IE_BEIE_POS                                         ((uint32_t) 19)                                 /* CAN bus error interrupt enable pos */
#define CAN_IE_BEIE                                             ((uint32_t) ((uint32_t)1 << CAN_IE_BEIE_POS))   /* CAN bus error interrupt enable filed */
#define CAN_IE_EPIE_POS                                         ((uint32_t) 18)                                 /* CAN error passive interrupt enable pos */
#define CAN_IE_EPIE                                             ((uint32_t) ((uint32_t)1 << CAN_IE_EPIE_POS))   /* CAN error passive interrupt enable filed */
#define CAN_IE_EWIE_POS                                         ((uint32_t) 17)                                 /* CAN error warning interrupt enable pos */
#define CAN_IE_EWIE                                             ((uint32_t) ((uint32_t)1 << CAN_IE_EWIE_POS))   /* CAN error warning interrupt enable filed */
#define CAN_IE_ALIE_POS                                         ((uint32_t) 16)                                 /* CAN arbitration lost interrupt enable Pos */
#define CAN_IE_ALIE                                             ((uint32_t) ((uint32_t)1 << CAN_IE_ALIE_POS))   /* CAN arbitration lost interrupt enable filed */
#define CAN_IE_DOIE_POS                                         ((uint32_t) 10)                                 /* CAN data overrun interrupt enable pos */
#define CAN_IE_DOIE                                             ((uint32_t) ((uint32_t)1 << CAN_IE_DOIE_POS))   /* CAN data overrun interrupt enable filed */
#define CAN_IE_TIE_POS                                          ((uint32_t) 9)                                  /* CAN transmit interrupt enable pos */
#define CAN_IE_TIE                                              ((uint32_t) ((uint32_t)1 << CAN_IE_TIE_POS))    /* CAN transmit interrupt enable filed */
#define CAN_IE_RIE_POS                                          ((uint32_t) 8)                                  /* CAN receive interrupt enable pos */
#define CAN_IE_RIE                                              ((uint32_t) ((uint32_t)1 << CAN_IE_RIE_POS))    /* CAN receive interrupt enable filed */
#define CAN_IE_AIE_POS                                          ((uint32_t) 0)                                  /* CAN interrupt enable pos */
#define CAN_IE_AIE                                              ((uint32_t) ((uint32_t)1 << CAN_IE_AIE_POS))    /* CAN interrupt enable filed */

/*************** CAN BUSTIM bitfiled ********************/
#define CAN_BUSTIM_SAM_POS                                      ((uint32_t) 26)                                      /* CAN sampling pos */
#define CAN_BUSTIM_SAM                                          ((uint32_t) ((uint32_t)1 << CAN_BUSTIM_SAM_POS))     /* CAN sampling filed */
#define CAN_BUSTIM_SJW_POS                                      ((uint32_t) 24)                                      /* CAN synchronization jump width pos */
#define CAN_BUSTIM_SJW                                          ((uint32_t) ((uint32_t)3 << CAN_BUSTIM_SJW_POS))     /* CAN synchronization jump width filed */
#define CAN_BUSTIM_TSEG2_POS                                    ((uint32_t) 20)                                      /* CAN Time segment after sample point pos */
#define CAN_BUSTIM_TSEG2                                        ((uint32_t) ((uint32_t)7 << CAN_BUSTIM_TSEG2_POS))   /* CAN Time segment after sample point filed */
#define CAN_BUSTIM_TSEG1_POS                                    ((uint32_t) 16)                                      /* CAN Time segment before sample point pos */
#define CAN_BUSTIM_TSEG1                                        ((uint32_t) ((uint32_t)0xf << CAN_BUSTIM_TSEG1_POS)) /* CAN Time segment before sample point filed */
#define CAN_BUSTIM_BRP_POS                                      ((uint32_t) 0)                                       /* CAN baudrate prescaler pos */
#define CAN_BUSTIM_BRP                                          ((uint32_t) ((uint32_t)0xfff << CAN_BUSTIM_BRP_POS))  /* CAN baudrate prescaler filed */

/*************** CAN ST bitfiled ************************/
#define CAN_ST_BUSOFF_POS                                       ((uint32_t) 17)                                      /* CAN bus of pos */
#define CAN_ST_BUSOFF                                           ((uint32_t) ((uint32_t)1 << CAN_ST_BUSOFF_POS))      /* CAN bus of filed */
#define CAN_ST_ECST_POS                                         ((uint32_t) 16)                                      /* CAN error counter status pos */
#define CAN_ST_ECST                                             ((uint32_t) ((uint32_t)1 << CAN_ST_ECST_POS))        /* CAN error counter status filed */
#define CAN_ST_RDO_POS                                          ((uint32_t) 10)                                      /* CAN data overflow when receiving message pos */
#define CAN_ST_RDO                                              ((uint32_t) ((uint32_t)1 << CAN_ST_RDO_POS))         /* CAN data overflow when receiving message filed */
#define CAN_ST_RFNE_POS                                         ((uint32_t) 9)                                       /* CAN receive fifo not empty pos */
#define CAN_ST_RFNE                                             ((uint32_t) ((uint32_t)1 << CAN_ST_RFNE_POS))        /* CAN receive fifo not empty filed */
#define CAN_ST_RB_POS                                           ((uint32_t) 8)                                       /* CAN receive busy pos */
#define CAN_ST_RB                                               ((uint32_t) ((uint32_t)1 << CAN_ST_RB_POS))          /* CAN receive busy filed */
#define CAN_ST_TBL_POS                                          ((uint32_t) 2)                                       /* CAN transmit buffer locked pos */
#define CAN_ST_TBL                                              ((uint32_t) ((uint32_t)1 << CAN_ST_TBL_POS))         /* CAN transmit buffer locked filed */
#define CAN_ST_TD_POS                                           ((uint32_t) 1)                                       /* CAN transmit done pos */
#define CAN_ST_TD                                               ((uint32_t) ((uint32_t)1 << CAN_ST_TD_POS))          /* CAN transmit done filed */
#define CAN_ST_TB_POS                                           ((uint32_t) 0)                                       /* CAN transmit busy pos */
#define CAN_ST_TB                                               ((uint32_t) ((uint32_t)1 << CAN_ST_TB_POS))          /* CAN transmit busy filed */

/*************** CAN INTST bitfiled **********************/
#define CAN_INST_WI_POS                                         ((uint32_t) 24)                                      /* CAN wakeup interrupt status pos */
#define CAN_INST_WI                                             ((uint32_t) ((uint32_t)1 << CAN_INST_WI_POS))        /* CAN wakeup interrupt status filed */
#define CAN_INST_BEI_POS                                        ((uint32_t) 19)                                      /* CAN bus error interrupt status pos */
#define CAN_INST_BEI                                            ((uint32_t) ((uint32_t)1 << CAN_INST_BEI_POS))       /* CAN bus error interrupt status filed */
#define CAN_INST_EPI_POS                                        ((uint32_t) 18)                                      /* CAN error passive interrupt status pos */
#define CAN_INST_EPI                                            ((uint32_t) ((uint32_t)1 << CAN_INST_EPI_POS))       /* CAN error passive interrupt status filed */
#define CAN_INST_EWI_POS                                        ((uint32_t) 17)                                      /* CAN error warning interrupt status pos */
#define CAN_INST_EWI                                            ((uint32_t) ((uint32_t)1 << CAN_INST_EWI_POS))       /* CAN error warning interrupt status filed */
#define CAN_INST_ALI_POS                                        ((uint32_t) 16)                                      /* CAN arbitration lost interrupt status pos */
#define CAN_INST_ALI                                            ((uint32_t) ((uint32_t)1 << CAN_INST_ALI_POS))       /* CAN arbitration lost interrupt status filed */
#define CAN_INST_DOI_POS                                        ((uint32_t) 10)                                      /* CAN data overflow interrupt status pos */
#define CAN_INST_DOI                                            ((uint32_t) ((uint32_t)1 << CAN_INST_DOI_POS))       /* CAN data overflow interrupt status filed */
#define CAN_INST_TI_POS                                         ((uint32_t) 9)                                       /* CAN transmit interrupt status pos */
#define CAN_INST_TI                                             ((uint32_t) ((uint32_t)1 << CAN_INST_TI_POS))        /* CAN transmit interrupt filed */
#define CAN_INST_RI_POS                                         ((uint32_t) 8)                                       /* CAN receive interrupt status pos */
#define CAN_INST_RI                                             ((uint32_t) ((uint32_t)1 << CAN_INST_RI_POS))        /* CAN receive interrupt status filed */
#define CAN_INST_AI_POS                                         ((uint32_t) 0)                                       /* CAN interrupt status pos */
#define CAN_INST_AI                                             ((uint32_t) ((uint32_t)1 << CAN_INST_AI_POS))        /* CAN interrupt status filed */

/*************** CAN RBST bitfiled **********************/

#define CAN_RBST_RBSA_POS                                       ((uint32_t) 16)                                      /* CAN receive buffer start address pos */
#define CAN_RBST_RBSA                                           ((uint32_t) ((uint32_t)0xff << CAN_RBST_RBSA_POS))   /* CAN receive buffer start address filed */
#define CAN_RBST_RMC_POS                                        ((uint32_t) 0)                                       /* CAN receive message counter pos */
#define CAN_RBST_RMC                                            ((uint32_t) ((uint32_t)0x1f << CAN_RBST_RMC_POS))    /* CAN receive message counter filed */

/*************** CAN TX frame information bitfiled ******/
#define CAN_TXFRMINFO_FF_POS                                    ((uint32_t) 7)                                        /* CAN frame format pos */
#define CAN_TXFRMINFO_FF                                        ((uint32_t) ((uint32_t)0x1 << CAN_TXFRMINFO_FF_POS))  /* CAN frame format filed */
#define CAN_TXFRMINFO_RTR_POS                                   ((uint32_t) 6)                                        /* CAN remote transmission request pos */
#define CAN_TXFRMINFO_RTR                                       ((uint32_t) ((uint32_t)0x1 << CAN_TXFRMINFO_RTR_POS)) /* CAN remote transmission request filed */
#define CAN_TXFRMINFO_DLC_POS                                   ((uint32_t) 0)                                        /* CAN data length code pos */
#define CAN_TXFRMINFO_DLC                                       ((uint32_t) ((uint32_t)0xf << CAN_TXFRMINFO_DLC_POS)) /* CAN data length code filed */

/*************** CAN TX identifier bitfiled *************/
#define CAN_TX_ID28TO18_POS                                     ((uint32_t) 18)                                       /* CAN TX identifier bit 18~28 pos */
#define CAN_TX_ID28TO18                                         ((uint32_t) ((uint32_t)0x7ff << CAN_TX_ID28TO18_POS)) /* CAN TX identifier bit 18~28 filed */
#define CAN_TX_ID17TO0_POS                                      ((uint32_t) 0)                                        /* CAN TX identifier bit 17~0 pos */
#define CAN_TX_ID17TO0                                          ((uint32_t) ((uint32_t)0x3ffff << CAN_TX_ID17TO0_POS))/* CAN TX identifier bit 17~0 filed */

/*************** CAN ALB bitfiled **********************/
#define CAN_ALB_RL_POS                                          ((uint32_t) 31)                                       /* CAN RTR bit arbitration lost status pos */
#define CAN_ALB_RL                                              ((uint32_t) ((uint32_t)1 << CAN_ALB_RL_POS))          /* CAN RTR bit arbitration lost status filed */
#define CAN_ALB_ID0L_POS                                        ((uint32_t) 30)                                       /* CAN 29th bit of identifier (ID[0]) arbitration lost status pos */
#define CAN_ALB_ID0L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID0L_POS))        /* CAN 29th bit of identifier (ID[0]) arbitration lost status filed */
#define CAN_ALB_ID1L_POS                                        ((uint32_t) 29)                                       /* CAN 28th bit of identifier (ID[1]) arbitration lost status pos */
#define CAN_ALB_ID1L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID1L_POS))        /* CAN 28th bit of identifier (ID[1]) arbitration lost status filed */
#define CAN_ALB_ID2L_POS                                        ((uint32_t) 28)                                       /* CAN 27th bit of identifier (ID[2]) arbitration lost status pos */
#define CAN_ALB_ID2L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID2L_POS))        /* CAN 27th bit of identifier (ID[2]) arbitration lost status filed */
#define CAN_ALB_ID3L_POS                                        ((uint32_t) 27)                                       /* CAN 26th bit of identifier (ID[3]) arbitration lost status pos */
#define CAN_ALB_ID3L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID3L_POS))        /* CAN 26th bit of identifier (ID[3]) arbitration lost status filed */
#define CAN_ALB_ID4L_POS                                        ((uint32_t) 26)                                       /* CAN 25th bit of identifier (ID[4]) arbitration lost status pos */
#define CAN_ALB_ID4L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID4L_POS))        /* CAN 25th bit of identifier (ID[4]) arbitration lost status filed */
#define CAN_ALB_ID5L_POS                                        ((uint32_t) 25)                                       /* CAN 24th bit of identifier (ID[5]) arbitration lost status pos */
#define CAN_ALB_ID5L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID5L_POS))        /* CAN 24th bit of identifier (ID[5]) arbitration lost status filed */
#define CAN_ALB_ID6L_POS                                        ((uint32_t) 24)                                       /* CAN 23rd bit of identifier (ID[6]) arbitration lost status pos */
#define CAN_ALB_ID6L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID6L_POS))        /* CAN 23rd bit of identifier (ID[6]) arbitration lost status filed */
#define CAN_ALB_ID7L_POS                                        ((uint32_t) 23)                                       /* CAN 22nd bit of identifier (ID[7]) arbitration lost status pos */
#define CAN_ALB_ID7L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID7L_POS))        /* CAN 22nd bit of identifier (ID[7]) arbitration lost status filed */
#define CAN_ALB_ID8L_POS                                        ((uint32_t) 22)                                       /* CAN 21st bit of identifier (ID[8]) arbitration lost status pos */
#define CAN_ALB_ID8L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID8L_POS))        /* CAN 21st bit of identifier (ID[8]) arbitration lost status filed */
#define CAN_ALB_ID9L_POS                                        ((uint32_t) 21)                                       /* CAN 20th bit of identifier (ID[9]) arbitration lost status pos */
#define CAN_ALB_ID9L                                            ((uint32_t) ((uint32_t)1 << CAN_ALB_ID9L_POS))        /* CAN 20th bit of identifier (ID[9]) arbitration lost status filed */
#define CAN_ALB_ID10L_POS                                       ((uint32_t) 20)                                       /* CAN 19th bit of identifier (ID[10]) arbitration lost status pos */
#define CAN_ALB_ID10L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID10L_POS))       /* CAN 19th bit of identifier (ID[10]) arbitration lost status filed */
#define CAN_ALB_ID11L_POS                                       ((uint32_t) 19)                                       /* CAN 18th bit of identifier (ID[11]) arbitration lost status pos */
#define CAN_ALB_ID11L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID11L_POS))       /* CAN 18th bit of identifier (ID[11]) arbitration lost status filed */
#define CAN_ALB_ID12L_POS                                       ((uint32_t) 18)                                       /* CAN 17th bit of identifier (ID[12]) arbitration lost status pos */
#define CAN_ALB_ID12L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID12L_POS))       /* CAN 17th bit of identifier (ID[12]) arbitration lost status filed */
#define CAN_ALB_ID13L_POS                                       ((uint32_t) 17)                                       /* CAN 16th bit of identifier (ID[13]) arbitration lost status pos */
#define CAN_ALB_ID13L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID13L_POS))       /* CAN 16th bit of identifier (ID[13]) arbitration lost status filed */
#define CAN_ALB_ID14L_POS                                       ((uint32_t) 16)                                       /* CAN 15th bit of identifier (ID[14]) arbitration lost status pos */
#define CAN_ALB_ID14L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID14L_POS))       /* CAN 15th bit of identifier (ID[14]) arbitration lost status filed */
#define CAN_ALB_ID15L_POS                                       ((uint32_t) 15)                                       /* CAN 14th bit of identifier (ID[15]) arbitration lost status pos */
#define CAN_ALB_ID15L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID15L_POS))       /* CAN 14th bit of identifier (ID[15]) arbitration lost status filed */
#define CAN_ALB_ID16L_POS                                       ((uint32_t) 14)                                       /* CAN 13th bit of identifier (ID[16]) arbitration lost status pos */
#define CAN_ALB_ID16L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID16L_POS))       /* CAN 13th bit of identifier (ID[16]) arbitration lost filed */
#define CAN_ALB_ID17L_POS                                       ((uint32_t) 13)                                       /* CAN 12th bit of identifier (ID[17]) arbitration lost status pos */
#define CAN_ALB_ID17L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID17L_POS))       /* CAN 12th bit of identifier (ID[17]) arbitration lost filed */
#define CAN_ALB_IL_POS                                          ((uint32_t) 12)                                       /* CAN IDE bit arbitration lost status pos */
#define CAN_ALB_IL                                              ((uint32_t) ((uint32_t)1 << CAN_ALB_IL_POS))          /* CAN IDE bit arbitration lost status filed */
#define CAN_ALB_SL_POS                                          ((uint32_t) 11)                                       /* CAN SRTR bit arbitration lost status pos */
#define CAN_ALB_SL                                              ((uint32_t) ((uint32_t)1 << CAN_ALB_SL_POS))          /* CAN SRTR bit arbitration lost status filed */
#define CAN_ALB_ID18L_POS                                       ((uint32_t) 10)                                       /* CAN 11th bit of identifier (ID[18]) arbitration lost status pos */
#define CAN_ALB_ID18L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID18L_POS))       /* CAN 11th bit of identifier (ID[18]) arbitration lost status filed */
#define CAN_ALB_ID19L_POS                                       ((uint32_t) 9)                                        /* CAN 10th bit of identifier (ID[19]) arbitration lost status pos */
#define CAN_ALB_ID19L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID19L_POS))       /* CAN 10th bit of identifier (ID[19]) arbitration lost status filed */
#define CAN_ALB_ID20L_POS                                       ((uint32_t) 8)                                        /* CAN 9th bit of identifier (ID[20]) arbitration lost status pos */
#define CAN_ALB_ID20L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID20L_POS))       /* CAN 9th bit of identifier (ID[20]) arbitration lost status filed */
#define CAN_ALB_ID21L_POS                                       ((uint32_t) 7)                                        /* CAN 8th bit of identifier (ID[21]) arbitration lost status pos */
#define CAN_ALB_ID21L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID21L_POS))       /* CAN 8th bit of identifier (ID[21]) arbitration lost status filed */
#define CAN_ALB_ID22L_POS                                       ((uint32_t) 6)                                        /* CAN 7th bit of identifier (ID[22]) arbitration lost status pos */
#define CAN_ALB_ID22L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID22L_POS))       /* CAN 7th bit of identifier (ID[22]) arbitration lost status filed */
#define CAN_ALB_ID23L_POS                                       ((uint32_t) 5)                                        /* CAN 6th bit of identifier (ID[23]) arbitration lost status pos */
#define CAN_ALB_ID23L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID23L_POS))       /* CAN 6th bit of identifier (ID[23]) arbitration lost status filed */
#define CAN_ALB_ID24L_POS                                       ((uint32_t) 4)                                        /* CAN 5th bit of identifier (ID[24]) arbitration lost status pos */
#define CAN_ALB_ID24L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID24L_POS))       /* CAN 5th bit of identifier (ID[24]) arbitration lost status filed */
#define CAN_ALB_ID25L_POS                                       ((uint32_t) 3)                                        /* CAN 4th bit of identifier (ID[25]) arbitration lost status pos */
#define CAN_ALB_ID25L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID25L_POS))       /* CAN 4th bit of identifier (ID[25]) arbitration lost status filed */
#define CAN_ALB_ID26L_POS                                       ((uint32_t) 2)                                        /* CAN 3rd bit of identifier (ID[26]) arbitration lost status pos */
#define CAN_ALB_ID26L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID26L_POS))       /* CAN 3rd bit of identifier (ID[26]) arbitration lost status filed  */
#define CAN_ALB_ID27L_POS                                       ((uint32_t) 1)                                        /* CAN 2nd bit of identifier (ID[27]) arbitration lost status pos */
#define CAN_ALB_ID27L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID27L_POS))       /* CAN 2nd bit of identifier (ID[27]) arbitration lost status filed */
#define CAN_ALB_ID28L_POS                                       ((uint32_t) 0)                                        /* CAN 1st bit of identifier (ID[28]) arbitration lost status pos */
#define CAN_ALB_ID28L                                           ((uint32_t) ((uint32_t)1 << CAN_ALB_ID28L_POS))       /* CAN 1st bit of identifier (ID[28]) arbitration lost status filed */

/*************** CAN ERRINFO bitfiled *****************/
#define CAN_ERRINFO_ECT_POS                                     ((uint32_t) 6)                                        /* CAN error code type pos */
#define CAN_ERRINFO_ECT                                         ((uint32_t) ((uint32_t)3 << CAN_ERRINFO_ECT_POS))     /* CAN error code type filed */
#define CAN_ERRINFO_DIR_POS                                     ((uint32_t) 5)                                        /* CAN dir pos */
#define CAN_ERRINFO_DIR                                         ((uint32_t) ((uint32_t)1 << CAN_ERRINFO_DIR_POS))     /* CAN dir filed */
#define CAN_ERRINFO_SC_POS                                      ((uint32_t) 0)                                        /* CAN segment code pos */
#define CAN_ERRINFO_SC                                          ((uint32_t) ((uint32_t)0x1f << CAN_ERRINFO_SC_POS))   /* CAN segment code filed */

/*************** CAN ERRCNT bitfiled ******************/
#define CAN_ERRCNT_REC_POS                                      ((uint32_t) 16)                                       /* CAN receive error counter pos */
#define CAN_ERRCNT_REC                                          ((uint32_t) ((uint32_t)0xff << CAN_ERRCNT_REC_POS))   /* CAN receive error counter filed */
#define CAN_ERRCNT_TEC_POS                                      ((uint32_t) 0)                                        /* CAN transmit error counter pos */
#define CAN_ERRCNT_TEC                                          ((uint32_t) ((uint32_t)0xff << CAN_ERRCNT_TEC_POS))   /* CAN transmit error counter filed */
#define CAN_BASE                                                (0x40030000u)                                         /* Base addresses of CAN registers */
#define CAN_DEV                                                 ((CAN_Device_T *)CAN_BASE)                            /* CAN device pointer */

#define CAN_MAX_DATA_LENGTH                                     8                                                     /* CAN send one data frame with maximum data length */

#define CAN_RESET_DELAYCNT                                      0xff                                                  /* CAN reset timedelay */

#define CAN_CLOCK_GATE_MASK                                     (1<<16)                                               /* CAN clock gate maskbit */

#define CAN_RX_BUF_FRAME_CNT                                    10                                                    /* CAN RX ringbuf frame cnt */

#define READ_ONE_TIME_FRAME_CNT                                 1                                                     /* One time read frame cnt */

#define WERITE_ONE_TIME_FRAME_CNT                               1                                                     /* One time write frame cnt */

#define MAX_BRP                                                 4095                                                  /* Maximum brp value */

#define MAX_FILTER_NUM                                          16                                                    /* Maximum filter value */

/**
 * CAN node commnicate status
 */
typedef enum
{
    CAN_COMMUNICATE_IDLE                                        = 0x0,                      /**< CAN communicate status is idle */
    CAN_COMMUNICATE_ERR,                                                                    /**< CAN communicate status is error */
    CAN_TX_COMMUNICATE_SUCCESS                                                              /**< CAN TX communicate status is success*/
} CAN_COMMUNICATE_STATUS_T;

/**
 * TX CAN frame mode: Block eead Or noblock
 */
typedef enum
{
    CAN_NOBLOCK                                                 = 0x0,                      /**< Noblock to TX CAN frame, function will return immediately */
    CAN_BLOCK                                                                               /**< Block to TX CAN frame, function will not return until TX finish */
} CAN_BLOCKSTATUS_T;

/**
 * @brief Set stand frame cnt
 *
 * @param  pCAN            CAN devive point
 * @param  id              Stand frame id
 *
 * @retval void
 */
#define SET_STAND_ID(pCAN, id)                                  IO_BITMASK_SET(pCAN->TXID, CAN_TX_ID28TO18, (id<<CAN_TX_ID28TO18_POS))

/**
 * @brief Set extern frame cnt
 *
 * @param  pCAN            CAN devive point
 * @param  id              Extern frame id
 *
 * @retval void
 */
#define SET_EXTEND_ID(pCAN, id)                                 (pCAN->TXID = id)

/**
 * @brief Set to rtr Frame
 *
 * @param  pCAN            CAN devive point
 *
 * @retval void
 */
#define CAN_SET_RTR(pCAN)                                       IO_BIT_SET(pCAN->TXFRMINFO, CAN_TXFRMINFO_RTR)

/**
 * @brief clear rtr bit
 *
 * @param  pCAN            CAN devive point
 * @retval void
 */
#define CAN_CLEAR_RTR(pCAN)                                     IO_BIT_CLEAR(pCAN->TXFRMINFO, CAN_TXFRMINFO_RTR)

/**
 * @brief Set data  data length
 *
 * @param  pCAN            CAN devive point
 * @param  val             Data length
 *
 * @retval void
 */
#define CAN_SET_DATA_LENGTH(pCAN, val)                          IO_BITMASK_SET(pCAN->TXFRMINFO, CAN_TXFRMINFO_DLC, (val<<CAN_TXFRMINFO_DLC_POS))

/**
 * @brief Requset for normal send frame
 *
 * @param  pCAN            CAN devive Point
 *
 * @retval void
 */
#define CAN_NORMAL_REQUESET_TX(pCAN)                            IO_BIT_SET(pCAN->CTRL, CAN_CTRL_TR)

/**
 * @brief Requset for normal and abort send frame
 *
 * @param  pCAN            CAN devive point
 *
 * @retval void
 */
#define CAN_NORMAL_REQUESET_ABORT_TX(pCAN)                      IO_BIT_SET(pCAN->CTRL, (CAN_CTRL_AT | CAN_CTRL_TR))

/**
 * @brief Requset for self normal and abort send frame
 *
 * @param  pCAN            CAN devive point
 *
 * @retval void
 */
#define CAN_SELF_REQUESET_ABORT_TX(pCAN)                        IO_BIT_SET(pCAN->CTRL, (CAN_CTRL_AT | CAN_CTRL_SRR))

/**
 * @brief Requset for abort send frame
 *
 * @param  pCAN            CAN devive point
 *
 * @retval void
 */
#define CAN_ABORT_TX(pCAN)                                      IO_BIT_SET(pCAN->CTRL, CAN_CTRL_AT)

/**
 * @brief Requset for self normal mode to  send frame
 *
 * @param  pCAN            CAN Devive Point
 *
 * @retval void
 */
#define CAN_SELF_REQUESET_TX(pCAN)                              IO_BIT_SET(pCAN->CTRL, CAN_CTRL_SRR)

/**
 * @brief Release RX fifo
 *
 * @param  pCAN            CAN Devive Point
 *
 * @retval void
 */
#define RELEASE_RX_FIFO(pCAN)                                   IO_BIT_SET(pCAN->CTRL, CAN_CTRL_RRB)

/**
 * @brief Set work mode to init mode
 *
 * @param  pCAN            CAN Devive Point
 *
 * @retval void
 */
#define SET_INIT_MODE(pCAN)                                     IO_BIT_CLEAR(pCAN->CFG, CAN_CFG_VM)

/**
 * @brief Set work mode
 *
 * @param  pCAN            CAN Devive Point
 * @param  val             Work mode
 *
 * @retval void
 */
#define SET_WORK_MODE(pCAN, val)                                 IO_BITMASK_SET(pCAN->CFG, CAN_CFG_VM, (val<<CAN_CFG_VM_POS))

/**
 * @brief Set waring threshold
 *
 * @param  pCAN            CAN devive point
 * @param  val             Waring threshold
 *
 * @retval void
 */
#define SET_WARNING_THRESHOLD(pCAN, val)                         (pCAN->EWT = (val))

/**
 * @brief Clear pending interrupt bitpos
 *
 * @param  pCAN            CAN devive point
 * @param  mask            Pending interrupt bitmask
 *
 * @retval void
 */
#define CAN_CLEAR_INTERRUPT(pCAN, mask)                          (pCAN->INTST = (mask))


/**
 * @brief Config CAN to sleep mode
 *
 * @param  pCAN            CAN devive point
 * @param  val             Mode value
 *
 * @retval void
 */
#define CAN_CONFIG_SLEEP_MODE(pCAN,val)                          IO_BITMASK_SET(pCAN->CFG, CAN_CFG_SME, (val << CAN_CFG_SME_POS))

/**
 * @brief Set frame format
 *
 * @param  pCAN            CAN devive point
 * @param  val             Format value
 *
 * @retval void
 */
#define SET_FRAME_FORMAT(pCAN, val)                              IO_BITMASK_SET(pCAN->TXFRMINFO, CAN_TXFRMINFO_FF, (val << CAN_TXFRMINFO_FF_POS))

/**
 * @brief Set error cnt
 *
 * @param  pCAN            CAN devive Point
 * @param  etc             Error TX cnt
 * @param  rtc             Error RX Cnt
 *
 * @retval void
 */
#define CAN_SET_ERR_CNT(pCAN, etc, rtc)                         IO_BITMASK_SET(pCAN->ERRCNT,\
        CAN_ERRCNT_TEC | CAN_ERRCNT_REC,\
        (etc << CAN_ERRCNT_TEC_POS) | (rtc << CAN_ERRCNT_REC_POS));
/**
 * @brief Enable CAN interrupt
 *
 * @param  pCAN CAN        CAN devive Point
 * @param  mask            Interrupt bitmask
 *
 * @retval void
 */
#define CAN_INTERRUPT_ENABLE(pCAN, mask)                        IO_BIT_SET(pCAN->IE, mask)

/**
 * @brief Disable CAN interrupt
 *
 * @param  pCAN            CAN devive point
 * @param  mask            Interrupt bitmask
 *
 * @retval void
 */
#define CAN_INTERRUPT_DISABLE(pCAN, mask)                       IO_BIT_CLEAR(pCAN->IE, mask)

/**
 * @brief Set work mode to sleep mode
 *
 * @param  pCAN            CAN devive point
 *
 * @retval void
 */
#define CAN_ENTER_SLEEP(pCAN)                                   IO_BIT_SET(pCAN->CFG, CAN_CFG_SME)

/**
 * @brief Software wakeup form sleep mode
 *
 * @param  pCAN            CAN devive point
 *
 * @retval void
 */
#define CAN_SW_WAKEUP(pCAN)                                     IO_BIT_CLEAR(pCAN->CFG, CAN_CFG_SME)


/**
 * @brief set RX address
 *
 * @param  pCAN            CAN devive point
 *
 * @retval  address         RX fifo address
 */
#define CAN_SET_RX_ADDRESS(pCAN, address)                       IO_BITMASK_SET(pCAN->RBST,                   \
        CAN_RBST_RBSA,                \
        (address << CAN_RBST_RBSA_POS))


#define CAN_GET_RX_ADDRESS(pCAN)                                (IO_BIT_GET(pCAN->RBST, CAN_RBST_RBSA) >> CAN_RBST_RBSA_POS)



/**
 * @brief Rest CAN Ip
 *
 * @param  void
 *
 * @retval void
 */

#define CAN_RESET()                                             do{                                                         \
        Clock_ResetPeripherals(CAN_RESET,                       \
                               Hold_ResetStatusTillStable,      \
                               CAN_RESET_DELAYCNT);             \
    }while(0)    /* reset CAN ip */

/**
 * @brief Enable CAN Clock
 *
 * @param  void
 *
 * @retval void
 */
#define CAN_CLOCK_ENABLE()                                      do{                                                   \
        Clock_PeripheralClockEnable(CAN_CLOCK_GATE_MASK); \
    }while(0)


/**
 * @brief Get Current Rx Address
 *
 * @param  pCAN            CAN devive point
 *
 * @retval Current Rx Address
 */
__STATIC_INLINE uint32_t CAN_GetCurrentRxAddress(CAN_Device_T *pCAN)
{
    return (pCAN->RBST & CAN_RBST_RBSA) >> CAN_RBST_RBSA_POS;
}

/**
 * @brief Get current RX cnt
 *
 * @param  pCAN            CAN devive point
 *
 * @retval Current Rx Cnt
 */
__STATIC_INLINE uint32_t CAN_GetCurrentRxCnt(CAN_Device_T *pCAN)
{
    return (pCAN->RBST & CAN_RBST_RMC) >> CAN_RBST_RMC_POS;
}

/**
 * @brief Get RX frame data filed length
 *
 * @param  pCAN            CAN devive point
 *
 * @retval Data filed length
 */
__STATIC_INLINE uint32_t CAN_GetDataLength(CAN_Device_T *pCAN)
{
    return ((pCAN->RXFRMINFO & CAN_TXFRMINFO_DLC) >> CAN_TXFRMINFO_DLC_POS);
}

/**
 * @brief Get RX extern frame id
 *
 * @param  pCAN            CAN devive point
 *
 * @retval RX extern frame id
 */
__STATIC_INLINE uint32_t CAN_GetExtendId(CAN_Device_T *pCAN)
{
    return (pCAN->RXID);
}

/**
 * @brief Get Rx stand frame id
 *
 * @param  pCAN            CAN devive point
 *
 * @retval RX stand frame id
 */
__STATIC_INLINE uint32_t CAN_GetStandId(CAN_Device_T *pCAN)
{
    return (((pCAN->RXID) & CAN_TX_ID28TO18) >> CAN_TX_ID28TO18_POS);
}

/**
 * @brief Get CAN status
 *
 * @param  pCAN            CAN Devive Point
 *
 * @retval Current CAN Status
 */
__STATIC_INLINE uint32_t CAN_GetSatus(CAN_Device_T *pCAN)
{
    return pCAN->ST;
}

/**
 * @brief Check Current Can TX Busy Satus
 *
 * @param  pCAN pCAN Devive Point
 *
 * @retval Can Busy Satus
 */
__STATIC_INLINE uint32_t CAN_TxBusy(CAN_Device_T *pCAN)
{
    return !!(CAN_GetSatus(pCAN) & CAN_ST_TBL);
}

/**
 * @brief Check current CAN work mode
 *
 * @param  pCAN            CAN devive point
 *
 * @retval Work mode
 */
__STATIC_INLINE uint32_t CAN_GetWorkMode(CAN_Device_T *pCAN)
{
    return ((pCAN->CFG & CAN_CFG_VM) >> CAN_CFG_VM_POS);
}

/**
 * @brief Check current frame foramt
 *
 * @param  pCAN            CAN devive point
 *
 * @retval Frame foramt
 */
__STATIC_INLINE uint32_t CAN_GetFrameFormat(CAN_Device_T *pCAN)
{
    return ((pCAN->RXFRMINFO & CAN_TXFRMINFO_FF) >> CAN_TXFRMINFO_FF_POS);
}

/**
 * @brief Check current frame type
 *
 * @param  pCAN            CAN devive point
 *
 * @retval Frame type
 */
__STATIC_INLINE uint32_t CAN_GetFrameType(CAN_Device_T *pCAN)
{
    return ((pCAN->RXFRMINFO & CAN_TXFRMINFO_RTR) >> CAN_TXFRMINFO_RTR_POS);
}

#ifdef __cplusplus
}
#endif


#endif /*_CAN_H_*/































































































































































































































































