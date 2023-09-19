/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          dma_dev.h
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/07/04
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/07/04, wizephen.wang, V1.0.0
 *        Initial version.
 *    2017/06/12, evan.wu, V1.0.1
 *        Support 6621.
 */

#ifndef _DMA_DEV_H_
#define _DMA_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Base addresses of DMA Registers */
#define DMA0_BASE                               (0x40020000U)

/* DMA Device Pointer */
#define DMA0                                    ((DMA_Device_T *) DMA0_BASE)

/* Maximum DMA block size */
#define MAX_BLOCK_SIZE_LENGTH                   1023

/* DMA Device Number */
#define  MAX_DMA_DEVICE                         (1U)

/** @defgroup DMA_Exported_Macros DMA Exported Macros
 * @{
 */

/**
 * @brief Check whether DMA is active.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_DEV_ISACTIVE(DEVICE)      (IO_BIT_GET((DEVICE)->CTRL, DMA_CTRL_EN))

/**
 * @brief Enable DMA peripheral.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_DEV_ENABLE(DEVICE)        (IO_BIT_SET((DEVICE)->CTRL, DMA_CTRL_EN))

/**
 * @brief Disable DMA peripheral.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_DEV_DISABLE(DEVICE)       (IO_BIT_CLEAR((DEVICE)->CTRL, DMA_CTRL_EN))


#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B ) || defined(CHIP_F6601C) )

/**
 *DMA Register Layout Type Definition
 */
typedef struct
{
    __IO uint32_t CTRL;         /**< DMA Control Register, offset: 0x00 */
    __IO uint32_t RERSERVED0;
    __IO uint32_t SAR;          /**< DMA Source Address Register, offset: 0x08 */
    __IO uint32_t DAR;          /**< DMA Destination Address register, offset: 0x0C */
    __IO uint32_t TCFG;         /**< DMA Transaction Configure Register, offset: 0x10 */
    __IO uint32_t SZ;           /**< DMA Transaction Size Register, offset: 0x14 */
    __IO uint32_t HSCFG;        /**< DMA Handshake Register, offset: 0x18 */
    __IO uint32_t CHN;          /**< DMA Hardware Handshake Channel Register, offset: 0x1C */
    __IO uint32_t IR;           /**< DMA Raw Interrupt Status Register, offset: 0x20 */
    __IO uint32_t IS;           /**< DMA Interrupt Status Register, offset: 0x24 */
    __IO uint32_t IM;           /**< DMA Interrupt Mask Register, offset: 0x28 */
    __IO uint32_t RERSERVED1;
    __IO uint32_t SWHD;         /**< DMA Software Handshaking Control Register, offset: 0x30 */
    __IO uint32_t RERSERVED2;
    __IO uint32_t VER;          /**< DMA Version Register, offset: 0x38 */
} DMA_Device_T;

/********************** DMA Control Register Bitfield ************************/
#define DMA_CTRL_EN                    ((uint32_t) 0x00000001)    /**< DMA enable */
#define DMA_CTRL_TTS                   ((uint32_t) 0x00000100)    /**< DMA transaction trigger and running status */

/************** DMA Transaction Configure Register Bitfield ******************/
#define DMA_TCFG_IE                    ((uint32_t) 0x00000001)    /**< DMA global interrupt enable */
#define DMA_TCFG_DTW_POS               ((uint32_t) 1)
#define DMA_TCFG_DTW_FIELD(x)          ((uint32_t) ((x) << 1))    /**< Destination Transfer Width */
#define DMA_TCFG_STW_POS               ((uint32_t) 4)
#define DMA_TCFG_STW_FIELD(x)          ((uint32_t) ((x) << 4))    /**< Source Transfer Width */
#define DMA_TCFG_DAI_POS               ((uint32_t) 7)
#define DMA_TCFG_DAI_FIELD(x)          ((uint32_t) ((x) << 7))    /**< Destination Address Increment */
#define DMA_TCFG_SAI_POS               ((uint32_t) 9)
#define DMA_TCFG_SAI_FIELD(x)          ((uint32_t) ((x) << 9))    /**< Source Address Increment */
#define DMA_TCFG_TTP_POS               ((uint32_t) 20)
#define DMA_TCFG_TTP_FIELD(x)          ((uint32_t) ((x) << 20))   /**< DMA Transaction Direction */

/*************** DMA Handshaking Configure Register Bitfiled ******************/
#define DMA_HSCFG_DH_POS               ((uint32_t) 10)
#define DMA_HSCFG_DH                   ((uint32_t) (0x00000400))  /**< Handshaking Type for Destinaion */
#define DMA_HSCFG_SH_POS               ((uint32_t) 11)
#define DMA_HSCFG_SH                   ((uint32_t) (0x00000800))  /**< Handshaking Type for Source */

/*************** DMA Handshaking Channel Register Bitfiled ******************/
#define DMA_CHN_SCHN_POS               ((uint32_t) 7)
#define DMA_CHN_SCHN_FIELD(x)          ((uint32_t) ((x) << 7))    /**< Hardware channel for Source */
#define DMA_CHN_DCHN_POS               ((uint32_t) 11)
#define DMA_CHN_DCHN_FIELD(x)          ((uint32_t) ((x) << 11))   /**< Hardware channel for Destinaion */

/*************** DMA Interrupt Status Register Bitfiled ******************/
#define DMA_IS_DEI                     ((uint32_t) (0x00000001))   /**< DMA Block Transaction Error */
#define DMA_IS_DTDI                    ((uint32_t) (0x00000002))   /**< DMA Transaction Complete for Desination */
#define DMA_IS_STDI                    ((uint32_t) (0x00000004))   /**< DMA Transaction Complete for Source */
#define DMA_IS_BDI                     ((uint32_t) (0x00000008))   /**< DMA Block Transaction Complete */
#define DMA_IS_TDI                     ((uint32_t) (0x00000010))   /**< DMA Transaction Complete */

/*************** DMA Interrupt Mask Register Bitfiled ******************/
#define DMA_IM_DEIM                    ((uint32_t) (0x00000001))   /**< DMA Block Transaction Error Mask */
#define DMA_IM_DTDIM                   ((uint32_t) (0x00000002))   /**< DMA Transaction Complete for Desination Mask */
#define DMA_IM_STDIM                   ((uint32_t) (0x00000004))   /**< DMA Transaction Complete for Source Mask */
#define DMA_IM_BDIM                    ((uint32_t) (0x00000008))   /**< DMA Block Transaction Complete Mask */
#define DMA_IM_TDIM                    ((uint32_t) (0x00000010))   /**< DMA Transaction Complete Mask */

/*************** Software Handshaking Control Register Bitfiled ******************/
#define DMA_SWHD_SRQ                   ((uint32_t) (0x00000001))   /**< Software Transaction Request for Source */
#define DMA_SWHD_DRQ                   ((uint32_t) (0x00000002))   /**< Software Transaction Request for Destination */
#define DMA_SWHD_SSRQ                  ((uint32_t) (0x00000004))   /**< Single Software Transaction Request for Source */
#define DMA_SWHD_SDRQ                  ((uint32_t) (0x00000008))   /**< Single Software Transaction Request for Destination */
#define DMA_SWHD_LSRQ                  ((uint32_t) (0x00000010))   /**< Last Software Transaction Request for Source */
#define DMA_SWHD_LDRQ                  ((uint32_t) (0x00000020))   /**< Last Software Transaction Request for Destination */


/** @defgroup DMA_Exported_Macros DMA Exported Macros
 * @{
 */


/**
 * @brief Trigger a DMA transaction.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_TRANS_TRIGGER(DEVICE)     (IO_BIT_SET((DEVICE)->CTRL, DMA_CTRL_TTS))

/**
 * @brief Enable DMA interrupt.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_INTERRUPT_ENABLE(DEVICE)  (IO_BIT_SET((DEVICE)->TCFG, DMA_TCFG_IE))

/**
 * @brief Disable DMA interrupt.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_INTERRUPT_DISABLE(DEVICE) (IO_BIT_CLEAR((DEVICE)->TCFG, DMA_TCFG_IE))


/**
 * @brief Specify DMA block size.
 * @param DEVICE DMA device handler
 * @param BLKSIZE DMA block size
 * @retval None
 */
#define DMA_SET_BLKSIZE(DEVICE, BLKSIZE)   do{                            \
        if(BLKSIZE > 1023)          \
            return RET_INVPARAM;      \
        DEVICE->SZ = BLKSIZE;       \
    }while(0)

#define ALL_DMA_INTERRUPTS                 0xFF /*DMA interrupt mask*/


/**
 * @brief Clear DMA transaction complete interrupt flag.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_TRCMPLT(DEVICE)          (DEVICE->IS = DMA_IS_TDI)

/**
 * @brief Clear DMA block transaction complete interrupt flag.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_BLKTRCMPLT(DEVICE)       (DEVICE->IS = DMA_IS_BDI)

/**
 * @brief Clear DMA source transaction complete interrupt flag.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_SRCTRCMPLT(DEVICE)       (DEVICE->IS = DMA_IS_STDI)

/**
 * @brief Clear DMA destinatin transaction complete interrupt flag.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_DSTTRCMPL(DEVICE)        (DEVICE->IS = DMA_IS_DTDI)

/**
 * @brief Clear DMA transaction error interrupt flag.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_ERR(DEVICE)              (DEVICE->IS = DMA_IS_DEI)

/**
 * @brief Clear DMA combined interrupt flags.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DAM_CLEAR_ALL_INTERRUPTS(DEVICE, INTERRUPT)   (DEVICE->IS = ALL_DMA_INTERRUPTS)

/**
 * @brief Mask transaction complete interrupt. Such interrupt is enabled
 *        only when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_MASK_TRCMPLT(DEVICE)           (IO_BIT_SET((DEVICE)->IM, DMA_IM_TDIM))

/**
 * @brief Mask block transaction complete interrupt. Such interrupt is enabled
 *        only when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_MASK_BLKTRCMPLT(DEVICE)        (IO_BIT_SET((DEVICE)->IM, DMA_IM_BDIM))

/**
 * @brief Mask source transaction complete interrupt. Such interrupt is enabled
 *        only when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_MASK_SRCTRCMPLT(DEVICE)        (IO_BIT_SET((DEVICE)->IM, DMA_IM_STDIM))

/**
 * @brief Mask destination transaction complete interrupt. Such interrupt is enabled
 *        only when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_MASK_DSTTRCMPLT(DEVICE)        (IO_BIT_SET((DEVICE)->IM, DMA_IM_DTDIM))

/**
 * @brief Mask transaction error interrupt. Such interrupt is enabled
 *        only when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_MASK_ERR(DEVICE)               (IO_BIT_SET((DEVICE)->IM, DMA_IM_DEIM))

/**
 * @brief Unmask transaction complete interrupt. Such interrupt
 *        is disabled even when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_UNMASK_TRCMPLT(DEVICE)         (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_TDIM))

/**
 * @brief Unmask block transaction complete interrupt. Such interrupt
 *        is disabled even when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_UNMASK_BLKTRCMPLT(DEVICE)      (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_BDIM))

/**
 * @brief Unmask source transaction complete interrupt. Such interrupt
 *        is disabled even when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_UNMASK_SRCTRCMPLT(DEVICE)      (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_STDIM))

/**
 * @brief Unmask destination transaction complete interrupt. Such interrupt
 *        is disabled even when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_UNMASK_DSTTRCMPLT(DEVICE)      (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_DTDIM))

/**
 * @brief Unmask transaction error interrupt. Such interrupt
 *        is disabled even when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_UNMASK_ERR(DEVICE)             (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_DEIM))

/**
 * @brief Mask DMA combined interrupt flags. Such interrupt is enabled
 *        only when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_MASK_ALL_INTERRUPTS(DEVICE, INTERRUPT)  (DEVICE->IM = ALL_DMA_INTERRUPTS)

/**
 * @brief Unmask DMA combined interrupt flags. Such interrupt
 *        is disabled even when global interrupt is made available.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_UNMASK_ALL_INTERRUPTS(DEVICE)  (DEVICE->IM = 0x00)

/**
 * @brief Issue DMA source software transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_SET_SRC_REQ(DEVICE)            (IO_BIT_SET((DEVICE)->SWHD, DMA_SWHD_SRQ))

/**
 * @brief Clear DMA source software transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_SRC_REQ(DEVICE)          (IO_BIT_CLEAR((DEVICE)->SWHD, DMA_SWHD_SRQ))

/**
 * @brief Issue DMA destination software transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_SET_DST_REQ(DEVICE)            (IO_BIT_SET((DEVICE)->SWHD, DMA_SWHD_DRQ))

/**
 * @brief Clear DMA destination software transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_DST_REQ(DEVICE)          (IO_BIT_CLEAR((DEVICE)->SWHD, DMA_SWHD_DRQ))

/**
 * @brief Issue DMA source software single transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_SET_SRCSGLREQ(DEVICE)          (IO_BIT_SET((DEVICE)->SWHD, DMA_SWHD_SSRQ))

/**
 * @brief Clear DMA source software single transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_SRCSGLREQ(DEVICE)        (IO_BIT_CLEAR((DEVICE)->SWHD, DMA_SWHD_SSRQ))

/**
 * @brief Issue DMA destination software single transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_SET_DSTSGLREQ(DEVICE)          (IO_BIT_SET((DEVICE)->SWHD, DMA_SWHD_SDRQ))

/**
 * @brief Clear DMA destination software single transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_DSTSGLREQ(DEVICE)        (IO_BIT_CLEAR((DEVICE)->SWHD, DMA_SWHD_SDRQ))

/**
 * @brief Issue DMA source software last transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_SET_LSTSRCREQ(DEVICE)          (IO_BIT_SET((DEVICE)->SWHD, DMA_SWHD_LSRQ))

/**
 * @brief Clear DMA source software last transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_LSTSRCREQ(DEVICE)        (IO_BIT_CLEAR((DEVICE)->SWHD, DMA_SWHD_LSRQ))

/**
 * @brief Issue DMA destination software last transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_SET_LSTDSTREQ(DEVICE)          (IO_BIT_SET((DEVICE)->SWHD, DMA_SWHD_LDRQ))

/**
 * @brief Clear DMA destination software last transaction request.
 * @param DEVICE DMA device handler
 * @retval None
 */
#define DMA_CLEAR_LSTDSTREQ(DEVICE)        (IO_BIT_CLEAR((DEVICE)->SWHD, DMA_SWHD_LDRQ))
/**
 * @}
 */


#elif (defined(CHIP_GM6621))

/* Max DMA channel number */
#define  MAX_DMA_CHANNEL    (5U)

typedef struct
{
    __IO uint32_t CTRL;         /**< DMA Control Register, offset: 0x00 */
    __IO uint32_t ID;           /**< DMA ID Register, offset: 0x04*/
    __IO uint32_t SCSEL;        /**< DMA Source Channel Selection Register : 0x08 */
    __IO uint32_t DCSEL;        /**< DMA Destination Channel Selection Register: 0x0C */
    __IO uint32_t IR;           /**< DMA Raw Interrupt Status Register, offset: 0x10 */
    __IO uint32_t IS;           /**< DMA Interrupt Clear Register, offset: 0x14 */
    __IO uint32_t IM;           /**< DMA Interrupt Mask Register, offset: 0x18 */
    __IO uint32_t SWHD;         /**< DMA Software Handshaking Control Register, offset: 0x1C */

    struct
    {
        __IO uint32_t SAR;      /**< DMA Source Address Register, offset: (i+1)*0x20 + 0x00 */
        __IO uint32_t DAR;      /**< DMA Destination Address register, offset: (i+1)*0x20 + 0x04 */
        __IO uint32_t TCFG;     /**< DMA Transaction Configure Register, offset: (i+1)*0x20 + 0x08 */
        __IO uint32_t SZ;       /**< DMA Transaction Size Register, offset: (i+1)*0x20 + 0x0C */
        __IO uint32_t HSCFG;    /**< DMA Handshake Register, offset: (i+1)*0x20 + 0x10 */
        __IO uint32_t SGR;      /**< DMA Source Gather Register, offset: (i+1)*0x20 + 0x14 */
        __IO uint32_t DSR;      /**< DMA Destination Scatter Register, offset: (i+1)*0x20 + 0x18 */
        __IO uint32_t RESERVE;  /**< Reserve, offset: (i+1)*0x20 + 0x1C */
    } DMA_Chn[MAX_DMA_CHANNEL];

} DMA_Device_T;

/*************** DMA Handshaking Channel Selection Defination ******************/
/* CHANNEL 4 */
#define DMA_SEL_ADC_RX_TO_CHANNEL4       0
#define DMA_SEL_UART1_TX_TO_CHANNEL4     1
#define DMA_SEL_I2C0_TX_TO_CHANNEL4      2
#define DMA_SEL_I2C1_TX_TO_CHANNEL4      3
#define DMA_SEL_SPI0_TX_TO_CHANNEL4      4
#define DMA_SEL_SPI1_TX_TO_CHANNEL4      5
#define DMA_SEL_ETM2CH3_TX_TO_CHANNEL4   6
/* CHANNEL 3 */
#define DMA_SEL_UART0_TX_TO_CHANNEL3     0
#define DMA_SEL_UART1_RX_TO_CHANNEL3     1
#define DMA_SEL_UART2_TX_TO_CHANNEL3     2
#define DMA_SEL_I2C0_RX_TO_CHANNEL3      3
#define DMA_SEL_I2C1_RX_TO_CHANNEL3      4
#define DMA_SEL_SPI0_RX_TO_CHANNEL3      5
#define DMA_SEL_SPI1_RX_TO_CHANNEL3      6
#define DMA_SEL_ETM22CH1_TX_TO_CHANNEL3  7
#define DMA_SEL_ETM2CH2_TX_TO_CHANNEL3   8
/* CHANNEL 2 */
#define DMA_SEL_UART0_RX_TO_CHANNEL2     0
#define DMA_SEL_UART1_TX_TO_CHANNEL2     1
#define DMA_SEL_UART2_RX_TO_CHANNEL2     2
#define DMA_SEL_I2C1_TX_TO_CHANNEL2      3
#define DMA_SEL_SPI0_RX_TO_CHANNEL2      4
#define DMA_SEL_ETM22CH0_TX_TO_CHANNEL2  5
/* CHANNEL 1 */
#define DMA_SEL_UART0_TX_TO_CHANNEL1     0
#define DMA_SEL_UART1_RX_TO_CHANNEL1     1
#define DMA_SEL_I2C0_TX_TO_CHANNEL1      2
#define DMA_SEL_I2C1_RX_TO_CHANNEL1      3
#define DMA_SEL_SPI0_TX_TO_CHANNEL1      4
#define DMA_SEL_SPI1_TX_TO_CHANNEL1      5
#define DMA_SEL_ETM21CH1_TX_TO_CHANNEL1  6
#define DMA_SEL_ETM2CH1_TX_TO_CHANNEL1   7
/* CHANNEL 0 */
#define DMA_SEL_ADC_TX_TO_CHANNEL0       0
#define DMA_SEL_UART0_RX_TO_CHANNEL0     1
#define DMA_SEL_I2C0_RX_TO_CHANNEL0      2
#define DMA_SEL_SPI1_RX_TO_CHANNEL0      3
#define DMA_SEL_ETM21CH0_TX_TO_CHANNEL0  4
#define DMA_SEL_ETM2CH0_TX_TO_CHANNEL0   5


/********************** DMA Control Register Bitfield ************************/
#define DMA_CTRL_EN                     ((uint32_t) 0x00000001)     /**< DMA enable */
#define DMA_CTRL_CHN0_EN                ((uint32_t) 0x00000002)     /**< DMA transaction trigger and running status */
#define DMA_CTRL_CHN1_EN                ((uint32_t) 0x00000004)     /**< DMA transaction trigger and running status */
#define DMA_CTRL_CHN2_EN                ((uint32_t) 0x00000008)     /**< DMA transaction trigger and running status */
#define DMA_CTRL_CHN3_EN                ((uint32_t) 0x00000010)     /**< DMA transaction trigger and running status */
#define DMA_CTRL_CHN4_EN                ((uint32_t) 0x00000020)     /**< DMA transaction trigger and running status */

/*************** DMA Handshaking Channel Selection Register Bitfiled ******************/
#define DMA_HS_CHN0_POS                 ((uint32_t) 0)               /**< DMA peripherals-map-to-ch0*/
#define DMA_HS_CHN1_POS                 ((uint32_t) 4)               /**< DMA peripherals-map-to-ch1*/
#define DMA_HS_CHN2_POS                 ((uint32_t) 8)               /**< DMA peripherals-map-to-ch2*/
#define DMA_HS_CHN3_POS                 ((uint32_t) 12)              /**< DMA peripherals-map-to-ch3*/
#define DMA_HS_CHN4_POS                 ((uint32_t) 16)              /**< DMA peripherals-map-to-ch4*/
#define DMA_HS_CHNX_MASK                (0xF)


/*************** DMA Interrupt Status Register Bitfiled ******************/
#define DMA_IS_CHN0_TDI                 ((uint32_t) (0x00000001))   /**< DMA Channel 0 Transaction Done */
#define DMA_IS_CHN0_BDI                 ((uint32_t) (0x00000002))   /**< DMA Channel 0 Block Transaction Done */
#define DMA_IS_CHN0_STDI                ((uint32_t) (0x00000004))   /**< DMA Channel 0 Srouce Transaction Done */
#define DMA_IS_CHN0_DTDI                ((uint32_t) (0x00000008))   /**< DMA Channel 0 Dest Transaction Done */
#define DMA_IS_CHN0_DEI                 ((uint32_t) (0x00000010))   /**< DMA Channel 0 Transaction Error */
#define DMA_IS_CHN1_TDI                 ((uint32_t) (0x00000020))   /**< DMA Channel 1 Transaction Done */
#define DMA_IS_CHN1_BDI                 ((uint32_t) (0x00000040))   /**< DMA Channel 1 Block Transaction Done */
#define DMA_IS_CHN1_STDI                ((uint32_t) (0x00000080))   /**< DMA Channel 1 Srouce Transaction Done */
#define DMA_IS_CHN1_DTDI                ((uint32_t) (0x00000100))   /**< DMA Channel 1 Dest Transaction Done */
#define DMA_IS_CHN1_DEI                 ((uint32_t) (0x00000200))   /**< DMA Channel 1 Transaction Error */
#define DMA_IS_CHN2_TDI                 ((uint32_t) (0x00000400))   /**< DMA Channel 2 Transaction Done */
#define DMA_IS_CHN2_BDI                 ((uint32_t) (0x00000800))   /**< DMA Channel 2 Block Transaction Done */
#define DMA_IS_CHN2_STDI                ((uint32_t) (0x00001000))   /**< DMA Channel 2 Srouce Transaction Done */
#define DMA_IS_CHN2_DTDI                ((uint32_t) (0x00002000))   /**< DMA Channel 2 Dest Transaction Done */
#define DMA_IS_CHN2_DEI                 ((uint32_t) (0x00004000))   /**< DMA Channel 2 Transaction Error */
#define DMA_IS_CHN3_TDI                 ((uint32_t) (0x00008000))   /**< DMA Channel 3 Transaction Done */
#define DMA_IS_CHN3_BDI                 ((uint32_t) (0x00010000))   /**< DMA Channel 3 Block Transaction Done */
#define DMA_IS_CHN3_STDI                ((uint32_t) (0x00020000))   /**< DMA Channel 3 Srouce Transaction Done */
#define DMA_IS_CHN3_DTDI                ((uint32_t) (0x00040000))   /**< DMA Channel 3 Dest Transaction Done */
#define DMA_IS_CHN3_DEI                 ((uint32_t) (0x00080000))   /**< DMA Channel 3 Transaction Error */
#define DMA_IS_CHN4_TDI                 ((uint32_t) (0x00100000))   /**< DMA Channel 4 Transaction Done */
#define DMA_IS_CHN4_BDI                 ((uint32_t) (0x00200000))   /**< DMA Channel 4 Block Transaction Done */
#define DMA_IS_CHN4_STDI                ((uint32_t) (0x00400000))   /**< DMA Channel 4 Srouce Transaction Done */
#define DMA_IS_CHN4_DTDI                ((uint32_t) (0x00800000))   /**< DMA Channel 4 Dest Transaction Done */
#define DMA_IS_CHN4_DEI                 ((uint32_t) (0x01000000))   /**< DMA Channel 4 Transaction Error */

/*************** DMA Interrupt Mask Register Bitfiled ******************/
#define DMA_IM_CHN0_TDIM                ((uint32_t) (0x00000001))   /**< DMA Channel 0 Transaction Done Mask */
#define DMA_IM_CHN0_BDIM                ((uint32_t) (0x00000002))   /**< DMA Channel 0 Block Transaction Done Mask */
#define DMA_IM_CHN0_STDIM               ((uint32_t) (0x00000004))   /**< DMA Channel 0 Source Transaction Done Mask */
#define DMA_IM_CHN0_DTDIM               ((uint32_t) (0x00000008))   /**< DMA Channel 0 Dest Transaction Done Mask */
#define DMA_IM_CHN0_DEIM                ((uint32_t) (0x00000010))   /**< DMA Channel 0 Transaction Error Mask */
#define DMA_IM_CHN1_TDIM                ((uint32_t) (0x00000020))   /**< DMA Channel 1 Transaction Done Mask */
#define DMA_IM_CHN1_BDIM                ((uint32_t) (0x00000040))   /**< DMA Channel 1 Block Transaction Done Mask */
#define DMA_IM_CHN1_STDIM               ((uint32_t) (0x00000080))   /**< DMA Channel 1 Source Transaction Done Mask */
#define DMA_IM_CHN1_DTDIM               ((uint32_t) (0x00000100))   /**< DMA Channel 1 Dest Transaction Done Mask */
#define DMA_IM_CHN1_DEIM                ((uint32_t) (0x00000200))   /**< DMA Channel 1 Transaction Error Mask */
#define DMA_IM_CHN2_TDIM                ((uint32_t) (0x00000400))   /**< DMA Channel 2 Transaction Done Mask */
#define DMA_IM_CHN2_BDIM                ((uint32_t) (0x00000800))   /**< DMA Channel 2 Block Transaction Done Mask */
#define DMA_IM_CHN2_STDIM               ((uint32_t) (0x00001000))   /**< DMA Channel 2 Source Transaction Done Mask */
#define DMA_IM_CHN2_DTDIM               ((uint32_t) (0x00002000))   /**< DMA Channel 2 Dest Transaction Done Mask */
#define DMA_IM_CHN2_DEIM                ((uint32_t) (0x00004000))   /**< DMA Channel 2 Transaction Error Mask */
#define DMA_IM_CHN3_TDIM                ((uint32_t) (0x00008000))   /**< DMA Channel 3 Transaction Done Mask */
#define DMA_IM_CHN3_BDIM                ((uint32_t) (0x00010000))   /**< DMA Channel 3 Block Transaction Done Mask */
#define DMA_IM_CHN3_STDIM               ((uint32_t) (0x00020000))   /**< DMA Channel 3 Source Transaction Done Mask */
#define DMA_IM_CHN3_DTDIM               ((uint32_t) (0x00040000))   /**< DMA Channel 3 Dest Transaction Done Mask */
#define DMA_IM_CHN3_DEIM                ((uint32_t) (0x00080000))   /**< DMA Channel 3 Transaction Error Mask */
#define DMA_IM_CHN4_TDIM                ((uint32_t) (0x00100000))   /**< DMA Channel 4 Transaction Done Mask */
#define DMA_IM_CHN4_BDIM                ((uint32_t) (0x00200000))   /**< DMA Channel 4 Block Transaction Done Mask */
#define DMA_IM_CHN4_STDIM               ((uint32_t) (0x00400000))   /**< DMA Channel 4 Source Transaction Done Mask */
#define DMA_IM_CHN4_DTDIM               ((uint32_t) (0x00800000))   /**< DMA Channel 4 Dest Transaction Done Mask */
#define DMA_IM_CHN4_DEIM                ((uint32_t) (0x01000000))   /**< DMA Channel 4 Transaction Error Mask */

/*************** Software Transaction Request Register Bitfiled ******************/
#define DMA_SWHD_CHN0_SRQ               ((uint32_t) (0x00000001))   /**< Source Request */
#define DMA_SWHD_CHN0_DRQ               ((uint32_t) (0x00000002))   /**< Dest Request */
#define DMA_SWHD_CHN0_SSRQ              ((uint32_t) (0x00000004))   /**< Source Single Request */
#define DMA_SWHD_CHN0_DSRQ              ((uint32_t) (0x00000008))   /**< Dest Single Request */
#define DMA_SWHD_CHN1_SRQ               ((uint32_t) (0x00000010))   /**< Source Request */
#define DMA_SWHD_CHN1_DRQ               ((uint32_t) (0x00000020))   /**< Dest Request */
#define DMA_SWHD_CHN1_SSRQ              ((uint32_t) (0x00000040))   /**< Source Single Request */
#define DMA_SWHD_CHN1_DSRQ              ((uint32_t) (0x00000080))   /**< Dest Single Request */
#define DMA_SWHD_CHN2_SRQ               ((uint32_t) (0x00000100))   /**< Source Request */
#define DMA_SWHD_CHN2_DRQ               ((uint32_t) (0x00000200))   /**< Dest Request */
#define DMA_SWHD_CHN2_SSRQ              ((uint32_t) (0x00000400))   /**< Source Single Request */
#define DMA_SWHD_CHN2_DSRQ              ((uint32_t) (0x00000800))   /**< Dest Single Request */
#define DMA_SWHD_CHN3_SRQ               ((uint32_t) (0x00001000))   /**< Source Request */
#define DMA_SWHD_CHN3_DRQ               ((uint32_t) (0x00002000))   /**< Dest Request */
#define DMA_SWHD_CHN3_SSRQ              ((uint32_t) (0x00004000))   /**< Source Single Request */
#define DMA_SWHD_CHN3_DSRQ              ((uint32_t) (0x00008000))   /**< Dest Single Request */
#define DMA_SWHD_CHN4_SRQ               ((uint32_t) (0x00010000))   /**< Source Request */
#define DMA_SWHD_CHN4_DRQ               ((uint32_t) (0x00020000))   /**< Dest Request */
#define DMA_SWHD_CHN4_SSRQ              ((uint32_t) (0x00040000))   /**< Source Single Request */
#define DMA_SWHD_CHN4_DSRQ              ((uint32_t) (0x00080000))   /**< Dest Single Request */

/************** DMA Transaction Configure Register Bitfield ******************/
#define DMA_TCFG_IE                    ((uint32_t) 0x00000001)      /**< DMA global interrupt enable */
#define DMA_TCFG_DTW_POS               ((uint32_t) 1)
#define DMA_TCFG_DTW_FIELD(x)          ((uint32_t) ((x) << 1))       /**< Destination Transfer Width */
#define DMA_TCFG_STW_POS               ((uint32_t) 4)
#define DMA_TCFG_STW_FIELD(x)          ((uint32_t) ((x) << 4))       /**< Source Transfer Width */
#define DMA_TCFG_DAI_POS               ((uint32_t) 7)
#define DMA_TCFG_DAI_FIELD(x)          ((uint32_t) ((x) << 7))       /**< Destination Address Increment */
#define DMA_TCFG_SAI_POS               ((uint32_t) 9)
#define DMA_TCFG_SAI_FIELD(x)          ((uint32_t) ((x) << 9))       /**< Source Address Increment */
#define DMA_TCFG_TTP_POS               ((uint32_t) 20)
#define DMA_TCFG_TTP_FIELD(x)          ((uint32_t) ((x) << 20))      /**< DMA Transaction Direction */
#define DMA_GATHER_EN                  ((uint32_t) 0x00020000)      /**< DMA Source Gather Enable */
#define DMA_SCATTER_EN                 ((uint32_t) 0x00040000)      /**< DMA Destination Scatter Enable */

/*************** DMA Handshaking Configure Register Bitfiled ******************/
#define DMA_HSCFG_PRI_POS              ((uint32_t) 5)
#define DMA_HSCFG_DH_POS               ((uint32_t) 10)
#define DMA_HSCFG_DH                   ((uint32_t) (0x00000400))   /**< Handshaking Type for Destinaion */
#define DMA_HSCFG_SH_POS               ((uint32_t) 11)
#define DMA_HSCFG_SH                   ((uint32_t) (0x00000800))   /**< Handshaking Type for Source */
#define DMA_HSCFG_SRL_POS              ((uint32_t) 30)
#define DMA_HSCFG_SRL                  ((uint32_t) (0x40000000))
#define DMA_HSCFG_DRL_POS              ((uint32_t) 31)
#define DMA_HSCFG_DRL                  ((uint32_t) (0x80000000))

/*************** DMA Source Gather Register Bitfiled ******************/
#define DMA_SRC_GATHER_INTERVAL_POS    ((uint32_t) 0)
#define DMA_SRC_GATHER_COUNT_POS       ((uint32_t) 20)

/*************** DMA Dest Scatter Register Bitfiled ******************/
#define DMA_DST_SCATTER_INTERVAL_POS   ((uint32_t) 0)
#define DMA_DST_SCATTER_COUNT_POS      ((uint32_t) 20)

/*************** DMA PES Register Bitfiled ******************/
#define DMA_PES_SEL_POS                (0x0)
#define DMA_PES_EN_POS                 (0x4)
#define DMA_PES_MASK                   (0xF)


/*DMA interrupt mask*/
#define ALL_DMA_INTERRUPTS             (0x1F)

/*Check whether DMA channel x is active */
#define DMA_CHANNEL_ISACTIVE(DEVICE, CHANNEL)  do{                           \
        if((CHANNEL)==0)          \
            (IO_BIT_GET((DEVICE)->CTRL, DMA_CTRL_CHN0_EN));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_GET((DEVICE)->CTRL, DMA_CTRL_CHN1_EN));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_GET((DEVICE)->CTRL, DMA_CTRL_CHN2_EN));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_GET((DEVICE)->CTRL, DMA_CTRL_CHN3_EN));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_GET((DEVICE)->CTRL, DMA_CTRL_CHN4_EN));       \
    }while(0)

/* Enable DMA channel x */
#define DMA_CHANNEL_ENABLE(DEVICE, CHANNEL)    do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->CTRL, DMA_CTRL_CHN0_EN));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->CTRL, DMA_CTRL_CHN1_EN));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->CTRL, DMA_CTRL_CHN2_EN));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->CTRL, DMA_CTRL_CHN3_EN));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->CTRL, DMA_CTRL_CHN4_EN));       \
    }while(0)

/* Disable DMA channel x */
#define DMA_CHANNEL_DISABLE(DEVICE, CHANNEL)   do{                                \
        if((CHANNEL)==0)               \
            (IO_BIT_CLEAR((DEVICE)->CTRL, DMA_CTRL_CHN0_EN));       \
        else if((CHANNEL)==1)          \
            (IO_BIT_CLEAR((DEVICE)->CTRL, DMA_CTRL_CHN1_EN));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_CLEAR((DEVICE)->CTRL, DMA_CTRL_CHN2_EN));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_CLEAR((DEVICE)->CTRL, DMA_CTRL_CHN3_EN));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_CLEAR((DEVICE)->CTRL, DMA_CTRL_CHN4_EN));       \
    }while(0)


/* Enable DMA channel x interrupt */
#define DMA_INTERRUPT_ENABLE(DEVICE, CHANNEL)  (IO_BIT_SET((DEVICE)->DMA_Chn[CHANNEL].TCFG, DMA_TCFG_IE))

/* Disable DMA channel x interrupt */
#define DMA_INTERRUPT_DISABLE(DEVICE, CHANNEL) (IO_BIT_CLEAR((DEVICE)->DMA_Chn[CHANNEL].TCFG, DMA_TCFG_IE))

/* Clear DMA transaction complete interrupt flag of channel x */
#define DMA_CLEAR_TRCMPLT(DEVICE, CHANNEL)     do{                                \
        if((CHANNEL)==0)               \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN0_TDI));       \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN1_TDI));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN2_TDI));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN3_TDI));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN4_TDI));       \
    }while(0)

/* Clear DMA block transaction complete interrupt flag of channel x */
#define DMA_CLEAR_BLKTRCMPLT(DEVICE, CHANNEL)  do{                                \
        if((CHANNEL)==0)               \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN0_BDI));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN1_BDI));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN2_BDI));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN3_BDI));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN4_BDI));       \
    }while(0)

/* Clear DMA transaction error interrupt flag of channel x */
#define DMA_CLEAR_ERR(DEVICE, CHANNEL)         do{                                \
        if((CHANNEL)==0)               \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN0_DEI));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN1_DEI));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN2_DEI));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN3_DEI));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN4_DEI));       \
    }while(0)

/* Clear DMA source transaction complete interrupt flag of channel x */
#define DMA_CLEAR_SRCTRCMPLT(DEVICE, CHANNEL)  do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN0_STDI));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN1_STDI));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN2_STDI));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN3_STDI));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN4_STDI));       \
    }while(0)

/*Clear DMA destinatin transaction complete interrupt flag of channel x */
#define DMA_CLEAR_DSTTRCMPLT(DEVICE, CHANNEL)   do{                          \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN0_DTDI));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN1_DTDI));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN2_DTDI));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN3_DTDI));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IS, DMA_IS_CHN4_DTDI));       \
    }while(0)


/* Clear DMA combined interrupt flags of channel x */
#define DAM_CLEAR_ALL_INTERRUPTS(DEVICE, CHANNEL)  (DEVICE->IS |= (ALL_DMA_INTERRUPTS << (CHANNEL * MAX_DMA_CHANNEL)))

/* Mask transaction complete interrupt of channel x */
#define DMA_MASK_TRCMPLT(DEVICE, CHANNEL)       do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN0_TDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN1_TDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN2_TDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN3_TDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN4_TDIM));       \
    }while(0)

/* Mask block transaction complete interrupt of channel x */
#define DMA_MASK_BLKTRCMPLT(DEVICE, CHANNEL)   do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN0_BDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN1_BDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN2_BDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN3_BDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN4_BDIM));       \
    }while(0)

/* Mask source transaction complete interrupt of channel x */
#define DMA_MASK_SRCTRCMPLT(DEVICE, CHANNEL)   do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN0_STDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN1_STDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN2_STDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN3_STDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN4_STDIM));       \
    }while(0)

/* Mask destination transaction complete interrupt of channel x */
#define DMA_MASK_DSTTRCMPLT(DEVICE, CHANNEL)   do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN0_DTDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN1_DTDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN2_DTDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN3_DTDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN4_DTDIM));       \
    }while(0)

/* Mask transaction error interrupt of channel x */
#define DMA_MASK_ERR(DEVICE, CHANNEL)          do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN0_DEIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN1_DEIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN2_DEIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN3_DEIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_SET((DEVICE)->IM, DMA_IM_CHN4_DEIM));       \
    }while(0)

/* Unmask transaction complete interrupt of channel x */
#define DMA_UNMASK_TRCMPLT(DEVICE, CHANNEL)    do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN0_TDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN1_TDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN2_TDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN3_TDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN4_TDIM));       \
    }while(0)

/* Unmask block transaction complete interrupt of channel x */
#define DMA_UNMASK_BLKTRCMPLT(DEVICE, CHANNEL) do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN0_BDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN1_BDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN2_BDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN3_BDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN4_BDIM));       \
    }while(0)

/* Unmask source transaction complete interrupt of channel x */
#define DMA_UNMASK_SRCTRCMPLT(DEVICE, CHANNEL) do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN0_STDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN1_STDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN2_STDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN3_STDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN4_STDIM));       \
    }while(0)

/* Unmask destination transaction complete interrupt of channel x */
#define DMA_UNMASK_DSTTRCMPLT(DEVICE, CHANNEL) do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN0_DTDIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN1_DTDIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN2_DTDIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN3_DTDIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN4_DTDIM));       \
    }while(0)

/* Unmask transaction error interrupt of channel x */
#define DMA_UNMASK_ERR(DEVICE, CHANNEL)        do{                            \
        if((CHANNEL)==0)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN0_DEIM));      \
        else if((CHANNEL)==1)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN1_DEIM));       \
        else if((CHANNEL)==2)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN2_DEIM));       \
        else if((CHANNEL)==3)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN3_DEIM));       \
        else if((CHANNEL)==4)          \
            (IO_BIT_CLEAR((DEVICE)->IM, DMA_IM_CHN4_DEIM));       \
    }while(0)

/* Mask DMA combined interrupt flags of channel x */
#define DMA_MASK_ALL_INTERRUPTS(DEVICE, CHANNEL)  (DEVICE->IM |= (ALL_DMA_INTERRUPTS << (CHANNEL * MAX_DMA_CHANNEL)))

/* Unmask DMA combined interrupt flags of channel x */
#define DMA_UNMASK_ALL_INTERRUPTS(DEVICE, CHANNEL)  (DEVICE->IM &= ~(ALL_DMA_INTERRUPTS << (CHANNEL * MAX_DMA_CHANNEL)))

#endif

#ifdef __cplusplus
}
#endif

#endif /*_DMA_DEV_H_*/



