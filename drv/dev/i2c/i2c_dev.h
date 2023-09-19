/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          i2c_dev.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/06
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/07/06, wizephen.wang, V1.0.0
 *        Initial version.
 */

#ifndef _I2C_DEV_H_
#define _I2C_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 *I2C Register Layout Type Definition
 */
typedef struct
{
    __IO uint32_t CTRL;        /**< I2C Control Register, offset: 0x00 */
    __IO uint32_t TSAR;        /**< Local & Target Address Register, offset: 0x04 */
    __IO uint32_t FLT;         /**< Filter Register, offset: 0x08 */
    __IO uint32_t HLP;         /**< High & Low Period Clock Register, offset: 0x0C */
    __IO uint32_t SH;          /**< Setup & Hold Time Register, offset: 0x10 */
    __IO uint32_t DT;          /**< Data or Command  Register, offset: 0x14 */
    __IO uint32_t TH;          /**< Threshold for Tx/Rx FIFO Register, offset: 0x18 */
    __IO uint32_t LV;          /**< Tx/Rx FIFO Level Register, offset: 0x1C */
    __IO uint32_t TMO;         /**< Timeout Register, offset: 0x20 */
    __IO uint32_t DMA;         /**< DMA Control Register, offset: 0x24 */
    __IO uint32_t ST;          /**< Status Register, offset: 0x28 */
    __IO uint32_t ERR;         /**< Error Source Register, offset: 0x2C */
    __IO uint32_t IE;          /**< Interrupt Enable Register, offset: 0x30 */
    __IO uint32_t IS;          /**< Interrupt Status Register, offset: 0x34 */
    __IO uint32_t RESERVED[49];
    __IO uint32_t VER;         /**< I2C Version Register, offset: 0xFC */
} I2C_Device_T;

/** Base addresses of I2C0 Peripheral Registers */
#define I2C0_BASE                    (0x4000E000u)
/** Base addresses of I2C1 Peripheral Registers */
#define I2C1_BASE                    (0x4000F000u)
/** Base addresses of I2C2 Peripheral Registers */
#define I2C2_BASE                    (0x40002000u)

/** I2C0 Device Pointer */
#define I2C0                         ((I2C_Device_T *)I2C0_BASE)
/** I2C1 Device Pointer */
#define I2C1                         ((I2C_Device_T *)I2C1_BASE)
/** I2C2 Device Pointer */
#define I2C2                         ((I2C_Device_T *)I2C2_BASE)

/** I2C Device Number */
#define  MAX_I2C_DEVICE              (3U)

#if defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_GM6601B)
/*Threshold depth for Tx FIFO */
#define MAX_TX_FIFO_DEPTH             8
/*Threshold depth for Rx FIFO */
#define MAX_RX_FIFO_DEPTH             8
#else
/*Threshold depth for Tx FIFO */
#define MAX_TX_FIFO_DEPTH             4
/*Threshold depth for Rx FIFO */
#define MAX_RX_FIFO_DEPTH             4
#endif

/********************** I2C Control Register Bitfield ************************/
#define I2C_CTRL_EN                        ((uint32_t) 0x00000001)  /**< I2C enable */
#define I2C_CTRL_SLVM                      ((uint32_t) 0x00000002)  /**< Master or slave enable */
#define I2C_CTRL_AM                        ((uint32_t) 0x00000004)  /**< 7Bit or 10Bit option */
#define I2C_CTRL_RSTA                      ((uint32_t) 0x00000008)  /**< Restart enable */
#define I2C_CTRL_SPDM_POS                  ((uint32_t) 4)
#define I2C_CTRL_SPDM_FIELD(x)             ((uint32_t) ((x) << 4))  /**< Standard or fast speed option */
#define I2C_CTRL_GCA                       ((uint32_t) 0x00000100)  /**< ACK option for general call */
#define I2C_CTRL_GCSTA                     ((uint32_t) 0x00000200)   /**< General call or start byte option */
#define I2C_CTRL_GSEN                      ((uint32_t) 0x00000400)   /**< General call or start byte enable */
#define I2C_CTRL_ABT                       ((uint32_t) 0x00010000)  /**< Abort data transaction in maser mode */
#define I2C_CTRL_RSSE                      ((uint32_t) 0x00020000)  /**< Recover SDA stuck */
#define I2C_CTRL_SLVNA                     ((uint32_t) 0x01000000)  /**< Enable no acknowledge in slave mode */

/********************** I2C Local & Target Register Bitfield ************************/
#define I2C_TSAR_SA_POS                    ((uint32_t) 0)
#define I2C_TSAR_SA_FIELED(x)              ((uint32_t) ((x) << 0))  /**< Target address */
#define I2C_TSAR_TA_POS                    ((uint32_t) 16)
#define I2C_TSAR_TA_FIELED(x)              ((uint32_t) ((x) << 16)) /**< Local address */

/********************** High & Low Period Clock Register ************************/
#define I2C_HLP_LP_POS                     ((uint32_t) 0)
#define I2C_HLP_LP_FIELD(x)                ((uint32_t) ((x) << 0))  /**< Low period clock couter */
#define I2C_HLP_HP_POS                     ((uint32_t) 16)
#define I2C_HLP_HP_FIELD(x)                ((uint32_t) ((x) << 16)) /**< High period clock couter */

/********************** Setup & Hold Time Register ************************/
#define I2C_SH_STP_POS                     ((uint32_t) 0)           /**< SDA Setup configuration */
#define I2C_SH_SHD_POS                     ((uint32_t) 16)          /**< SDA Hold configuration */
#define I2C_SH_SHC                         ((uint32_t) (0x80000000))/**< Enable Tx or Rx hold */

/********************** Data or Command  Register ************************/
#define I2C_DT_RD                          ((uint32_t) (0x00000100)) /**< Data transaction direction (read or write) */
#define I2C_DT_ASTP                        ((uint32_t) (0x00000200)) /**< Stop I2C transaction */
#define I2C_DT_ARST                        ((uint32_t) (0x00000400)) /**< Enable restart before transaction */
#define I2C_DT_FD                          ((uint32_t) (0x00000800)) /**< First data-receive inidcation */

/***************** Threshold for Tx/Rx FIFO Register **********************/
#define I2C_TH_RTHR_POS                    ((uint32_t) 0)
#define I2C_TH_RTHR_FIELD(x)               ((uint32_t) ((x) << 0))   /**< Threshold depth for Rx */
#define I2C_TH_TTHR_POS                    ((uint32_t) 16)
#define I2C_TH_TTHR_FIELD(x)               ((uint32_t) ((x) << 16))  /**< Threshold depth for Tx */

/********************* Tx/Rx FIFO Level Register *************************/
#define I2C_LV_RLVL_POS                    ((uint32_t) 0)
#define I2C_LV_RLVL_FIELD(x)               ((uint32_t) ((x) << 0))   /**< Threshold level for Rx */
#define I2C_LV_TLVL_POS                    ((uint32_t) 16)
#define I2C_LV_TLVL_FIELD(x)               ((uint32_t) ((x) << 16))  /**< Threshold level for Tx */

/********************* DMA Control Register *************************/
#define I2C_DMA_RXEN                       ((uint32_t) (0x00000001)) /**< Enable DMA to receive data */
#define I2C_DMA_RXTHR_POS                  ((uint32_t) 8)
#define I2C_DMA_RXTHR_FIELD(x)             ((uint32_t) ((x) << 8))   /**< Threshold for DMA Rx FIFO */
#define I2C_DMA_TXEN                       ((uint32_t) (0x00010000)) /**< Enable DMA to transfer data */
#define I2C_DMA_TXTHR_POS                  ((uint32_t) 24)
#define I2C_DMA_TXTHR_FIELD(x)             ((uint32_t) ((x) << 24))  /**< Threshold for DMA Tx FIFO */

/********************* I2C STATUS register *************************/
#define I2C_ST_EN                          ((uint32_t) (0x00000001)) /**< I2C enabled*/
#define I2C_ST_SLVDB                       ((uint32_t) (0x00000002)) /**< Transaction disbaled in slave mode, caused by I2C_CTRL_EN */
#define I2C_ST_SLVDL                       ((uint32_t) (0x00000004)) /**< Data lost in slave mode, caused by I2C_CTRL_EN */
#define I2C_ST_ACT                         ((uint32_t) (0x00000100)) /**< I2C is active */
#define I2C_ST_MACT                        ((uint32_t) (0x00000200)) /**< Master mode active */
#define I2C_ST_SACT                        ((uint32_t) (0x00000400)) /**< Slave mode active */
#define I2C_ST_RXF                         ((uint32_t) (0x00001000)) /**< Rx FIFO full flag */
#define I2C_ST_RXE                         ((uint32_t) (0x00002000)) /**< Rx FIFO empty flag */
#define I2C_ST_TXF                         ((uint32_t) (0x00004000)) /**< Tx FIFO full flag */
#define I2C_ST_TXE                         ((uint32_t) (0x00008000)) /**< Tx FIFO empty flag */

/********** I2C Interrupt Enable configure register **************/
#define I2C_IE_RFU                         ((uint32_t) (0x00000001)) /**< Enable Rx FIFO underflow interrupt */
#define I2C_IE_RFO                         ((uint32_t) (0x00000002)) /**< Enable Rx FIFO overflow interrupt */
#define I2C_IE_RFAF                        ((uint32_t) (0x00000004)) /**< Enable Rx FIFO full interrupt */
#define I2C_IE_TFO                         ((uint32_t) (0x00000008)) /**< Enable Tx FIFO overflow interrupt */

#define I2C_IE_TFAE                        ((uint32_t) (0x00000010)) /**< Enable Tx FIFO empty interrupt */
#define I2C_IE_SRREQ                       ((uint32_t) (0x00000020)) /**< Enable read request interrupt,
                                                                          response to the read request from master device */
#define I2C_IE_TXL                         ((uint32_t) (0x00000040)) /**< Enable arbitration interrupt */
#define I2C_IE_SRD                         ((uint32_t) (0x00000080)) /**< Enable receive complete interrupt,
                                                                          master issues a NACK before the STOP signal for SLV_RX_DONE */

#define I2C_IE_ACT                         ((uint32_t) (0x00000100)) /**< Enable i2c activity interrupt */
#define I2C_IE_STD                         ((uint32_t) (0x00000200)) /**< Enable stop detect interrupt */
#define I2C_IE_STAD                        ((uint32_t) (0x00000400)) /**< Enable start detect interrupt */
#define I2C_IE_GC                          ((uint32_t) (0x00000800)) /**< Enable general call interrupt */
#define I2C_IE_SL                          ((uint32_t) (0x00004000)) /**< Enable SCL stuck interrupt */

/*********** I2C Interrupt Status/Clear register ***************/
#define I2C_IS_RFU                         ((uint32_t) (0x00000001)) /**< Rx FIFO underflow interrupt flag */
#define I2C_IS_RFO                         ((uint32_t) (0x00000002)) /**< Rx FIFO overflow interrupt flag */
#define I2C_IS_RFAF                        ((uint32_t) (0x00000004)) /**< Rx FIFO full interrupt flag */
#define I2C_IS_TFO                         ((uint32_t) (0x00000008)) /**< Tx FIFO overflow interrupt flag */
#define I2C_IS_TFAE                        ((uint32_t) (0x00000010)) /**< Tx FIFO empty interrupt flag */
#define I2C_IS_SRREQ                       ((uint32_t) (0x00000020)) /**< Read request interrupt,
                                                                          response to the read request from master device */
#define I2C_IS_TXL                         ((uint32_t) (0x00000040)) /**< Arbitration interrupt flag */
#define I2C_IS_SRD                         ((uint32_t) (0x00000080)) /**< Data-receive complete interrupt flag,
                                                                          master issues a NACK before the STOP signal for SLV_RX_DONE */
#define I2C_IS_ACT                         ((uint32_t) (0x00000100)) /**< I2C working interrupt flag */
#define I2C_IS_STD                         ((uint32_t) (0x00000200)) /**< Stop condition detection interrupt flag */
#define I2C_IS_STAD                        ((uint32_t) (0x00000400)) /**< Start condition detection interrupt flag */
#define I2C_IS_GC                          ((uint32_t) (0x00000800)) /**< General call interrupt flag */
#define I2C_IS_SL                          ((uint32_t) (0x00004000)) /**< Stuck low interrupt flag */
#define I2C_IS_CMI                         ((uint32_t) (0x00010000)) /**< Combined interrupt flag */

/*control register*/
/** @defgroup I2C_Exported_Macros I2C Exported Macros
 * @{
 */

/**
 * @brief  Enable I2C peripheral.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_ENABLE(DEVICE)           (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_EN))

/**
 * @brief  Disable I2C peripheral.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_DISABLE(DEVICE)          (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_EN))

/**
 * @brief  Enable I2C slave mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_SLV_MODE_ENABLE(DEVICE)      (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_SLVM))

/**
 * @brief  Enable I2C master mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_MST_MODE_ENABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_SLVM))

/**
 * @brief  Enable I2C restart mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_RESTART_ENABLE(DEVICE)       (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_RSTA))

/**
 * @brief  Disable I2C restart mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_RESTART_DISABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_RSTA))

/**
 * @brief  Enable I2C general call or start byte mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_GCSTART_ENABLE(DEVICE)       (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_GSEN))

/**
 * @brief  Disable I2C general call or start byte mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_GCSTART_DISABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_GSEN))
#define I2C_GCSTARTSEL_GC(DEVICE)        (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_GCSTA))
#define I2C_GCSTARTSEL_START(DEVICE)     (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_GCSTA))
#define I2C_GCACK_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_GCA))
#define I2C_GCACK_DISABLE(DEVICE)        (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_GCA))

/**
 * @brief  Enable 7Bit address mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_7BITADDR_ENABLE(DEVICE)      (IO_BIT_CLEAR((DEVICE)->CTRL, I2C_CTRL_AM))

/**
 * @brief  Enable 10Bit address mode.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_10BITADDR_ENABLE(DEVICE)     (IO_BIT_SET((DEVICE)->CTRL, I2C_CTRL_AM))


/*status register*/
/* 1: indicates I2C is enabled, and vice versa */

/**
 * @brief  I2C peripheral is enabled.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_ISENABLED(DEVICE)        (IO_BIT_GET((DEVICE)->ST, I2C_CTRL_EN))

/**
 * @brief  I2C peripheral is active.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_DEV_ISACTIVE(DEVICE)         (IO_BIT_GET((DEVICE)->ST,I2C_ST_ACT))

/**
 * @brief  I2C peripheral Tx FIFO is full.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISTXFIFO_FULL(DEVICE)        (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_TXFIFOFUL))

/**
 * @brief  I2C peripheral Tx FIFO is empty.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISTXFIFO_EMPTY(DEVICE)       (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_TXFIFOEMP))

/**
 * @brief  I2C peripheral Rx FIFO is full.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISRXFIFO_FULL(DEVICE)        (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_RXFIFOFUL))

/**
 * @brief  I2C peripheral Rx FIFO is empty.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_ISRXFIFO_EMPTY(DEVICE)       (IO_BIT_GET((DEVICE)->ST,I2C_STATUS_RXFIFOEMP))

/**
 * @brief  Get RX FIFO level.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_RXFIFO_LEVEL(DEVICE)     ((uint8_t)((DEVICE)->LV))

/**
 * @brief  Get TX FIFO level.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_TXFIFO_LEVEL(DEVICE)     ((uint8_t)(((DEVICE)->LV) >> 16 & 0xFF))

/*set HPCC_LPCC register*/
#define I2C_SET_HPLPCC_HIGHCNT(DEVICE, HIGHCNT, LOWCNT) ((DEVICE)->HLP = ((HIGHCNT & 0xFFFF) << 16 | (LOWCNT & 0xFFFF)))

/**
 * @brief  Set TX FIFO threshold.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_SET_FIFOTHRD_TX(DEVICE, TXTHRD)   IO_BITMASK_SET((DEVICE)->TH, (0xFF << 16), ((uint8_t)(TXTHRD) << 16))

/**
 * @brief  Set RX FIFO threshold.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_SET_FIFOTHRD_RX(DEVICE, RXTHRD)   IO_BITMASK_SET((DEVICE)->TH, 0xFF, ((uint8_t)(RXTHRD)))

/**
 * @brief  Get TX FIFO threshold.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_FIFOTHRD_TX(DEVICE)  ((uint8_t)(((DEVICE)->TH) >> 16 & 0xFF))

/**
 * @brief  Get RX FIFO threshold.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_GET_FIFOTHRD_RX(DEVICE)  ((uint8_t)(((DEVICE)->TH) & 0xFF))

/*Get Error Sources*/
#define I2C_GET_ERR_SOURCES(DEVICE, SOURCE) (IO_BITS_GET((DEVICE)->ERR,  SOURCE))

/* Enable Int register */
#define MASK_ALL_INTERRUPTS                             0x0000
#define I2C_DISABLE_ALLINTS(DEVICE)                     ((DEVICE)->IE = MASK_ALL_INTERRUPTS)
#define I2C_ENABLE_INTERRUPT(DEVICE, INTERRUPT)         (IO_BIT_SET((DEVICE)->IE, INTERRUPT))
#define I2C_DISABLE_INTERRUPT(DEVICE, INTERRUPT)        (IO_BIT_CLEAR((DEVICE)->IE, INTERRUPT))

/* 1: indicates INTERRUPT is enabled, and vice versa */
#define I2C_INTERRUPT_ISENABLED(DEVICE, INTERRUPT)      (IO_BIT_GET((DEVICE)->IE, INTERRUPT))
#define I2C_GET_ENABLE_INTERRUPT(DEVICE)                ((uint32_t)((DEVICE)->IE))

/*Interrupt status register*/
/* 1: indicats INTERRUPT is active */
#define I2C_INTERRUPT_ISACTIVE(DEVICE, INTERRUPT)       (IO_BIT_GET((DEVICE)->IS, INTERRUPT))

/* Clear individual interrupt */
/**
 * @brief Clear stuck interrupt.
 * @param DEVICE I2C device handler
 * @retval None
 */
#define I2C_CLEAR_STUCKLOW(DEVICE)              ((DEVICE)->IS = I2C_IS_SL)
#define I2C_CLEAR_GC(DEVICE)                    ((DEVICE)->IS = I2C_IS_GC)
#define I2C_CLEAR_SATRT_DET(DEVICE)             ((DEVICE)->IS = I2C_IS_STAD)
#define I2C_CLEAR_STOP_DET(DEVICE)              ((DEVICE)->IS = I2C_IS_STD)
#define I2C_CLEAR_ACTIVITY(DEVICE)              ((DEVICE)->IS = I2C_IS_ACT)
#define I2C_CLEAR_RX_DONE(DEVICE)               ((DEVICE)->IS = I2C_IS_SRD)
#define I2C_CLEAR_ABRT_LOST(DEVICE)             ((DEVICE)->IS = I2C_IS_TXL)
#define I2C_CLEAR_SLV_RD_REQ(DEVICE)            ((DEVICE)->IS = I2C_IS_SRREQ)
#define I2C_CLEAR_TXFIFO_ALMOSTEMPTY(DEVICE)    ((DEVICE)->IS = I2C_IS_TFAE)
#define I2C_CLEAR_TXFIFO_OVER(DEVICE)           ((DEVICE)->IS = I2C_IS_TFO)
#define I2C_CLEAR_RXFIFO_ALMOSTFULL(DEVICE)     ((DEVICE)->IS = I2C_IS_RFAF)
#define I2C_CLEAR_RXFIFO_OV(DEVICE)             ((DEVICE)->IS = I2C_IS_RFO)
#define I2C_CLEAR_RXFIFO_UNDER(DEVICE)          ((DEVICE)->IS = I2C_IS_RFU)

/* Clear combined interrupts, write 1 to clear all interrupts */
#define I2C_CLEAR_COMBINEDINT(DEVICE)           ((DEVICE)->IS = I2C_IS_CMI)

/* I2C DMA control register */
/**
 * @brief  Enable DMA Tx.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_DMACTRL_TXEN_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->DMA, I2C_DMA_TXEN))

/**
 * @brief  Enable DMA Rx.
 * @param  DEVICE I2C device handler
 * @retval None
 */
#define I2C_DMACTRL_RXEN_ENABLE(DEVICE)         (IO_BIT_SET((DEVICE)->DMA, I2C_DMA_RXEN))
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /*_I2C_H_*/
