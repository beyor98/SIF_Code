/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          dma.c
 *
 * @author        wizephen.wang
 *
 * @version       1.0.0
 *
 * @date          2016/06/27
 *
 * @brief         Implementation of DMA Driver.
 *
 * @note
 *    2016/06/27, wizephen.wang, V1.0.0
 *        Initial version.
 *    2017/06/12, evan.wu, V1.0.1
 *        Support 6621.
 */

#include <drv/dev/dma.h>

#ifdef CHIP_GM6621
    #include <drv/dev/pes.h>
#endif


#if defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C)


/**
 * DMA peripherals
 */
typedef enum
{
    DMADEV_0 = 0,
    DMADEV_1 = 1
} DMA_Devn_T;

/*Pointers to DMA bases for each instance*/
static DMA_Device_T *const s_dmaBases[MAX_DMA_DEVICE] = {DMA0};
/*Pointers to DMA handles for each instance*/
static DMA_Handle_T *s_dmaHandle[MAX_DMA_DEVICE] = {NULL};

static __inline uint32_t DMA_GetIndex(DMA_Device_T *pDMA)
{
    uint32_t i;

    /* Find the  index from base address mappings. */
    for (i = 0; i < MAX_DMA_DEVICE; i++)
    {
        if (s_dmaBases[i] == pDMA)
        {
            break;
        }
    }

    ASSERT(i < MAX_DMA_DEVICE);

    return i;
}

/*Declarition of DMA interrupt service routine*/
static void DMA0_IRQHandler(void);

#ifdef DMA0
    STATIC_ISR(DMA_IRQn, DMA0_IRQHandler);
#endif

static void DMA_CommonIRQHandler(DMA_Handle_T *hDMA);
static void DMA0_IRQHandler(void)
{
    DMA_CommonIRQHandler(s_dmaHandle[DMADEV_0]);
}

RET_CODE DMA_GetDefaultConfig(DMA_Device_T *pDMA, DMA_Config_T *pDMACfg)
{
    DMA_Config_T DMACfg;

    UNUSED(pDMA);

    if (pDMACfg == NULL)
    {
        return RET_ERROR;
    }

    memset((void *)&DMACfg, 0x00, sizeof(DMA_Config_T) / sizeof(uint8_t));
    pDMACfg = &DMACfg;

    return RET_OK;
}

/**
 * @brief Configure source & destination address and block size for DMA.
 *
 * @param  hDMA           Pointer to a DMA transaction handler structure.
 * @param  srcAddress     DMA source memory buffer address.
 * @param  destAddress    DMA destination memory buffer address.
 * @param  length         The length of data transferred from source to destination address.
 *
 * @retval RET_OK         API executes successfully.
 * @retval RET_INVPARAM   Invalid paramters.
 */
static RET_CODE DMA_SetConfig(struct DMA_Handle *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t length)
{
    /*8Bit Width is possible*/
    if ((hDMA->destWidth == DMA_WIDTH_32BIT && destAddress & 0x03) ||
            (hDMA->destWidth == DMA_WIDTH_16BIT && destAddress & 0x01) ||
            (hDMA->srcWidth  == DMA_WIDTH_32BIT && srcAddress & 0x03)  ||
            (hDMA->srcWidth  == DMA_WIDTH_16BIT && srcAddress & 0x01))
    {
        return RET_INVPARAM;
    }

    if (length > MAX_BLOCK_SIZE_LENGTH)
    {
        return RET_INVPARAM;
    }

    /*Configure source & destination address*/
    hDMA->pDevice->SAR = srcAddress;
    hDMA->pDevice->DAR = destAddress;

    /*Configure DMA block size*/
    DMA_SET_BLKSIZE(hDMA->pDevice, length);

    return RET_OK;
}

/**
 * @brief Initialize DMA transaction handler.
 *
 * This intializes DMA transcation handler elements (specify DMA device handler, etc)
 *
 * @param  pDMA      Pointer to DMA device handler.
 * @param  pConfig   Pointer to DMA configuration structure.
 * @param  hDMA      Pointer to DMA transaction handler.
 *
 * @retval RET_OK    Always returns RET_OK.
 */
static RET_CODE DMA_InitHandle(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA)
{
    ASSERT(hDMA != NULL && pConfig != NULL && pDMA != NULL);

    hDMA->pDevice  = pDMA;
    hDMA->srcWidth = pConfig->srcWidth;
    hDMA->destWidth = pConfig->destWidth;
    hDMA->callback = pConfig->callback;
    /*DMA is now in initial state*/
    hDMA->state    = DMA_STATE_INITIAL;
    hDMA->pParent  = (void *) NULL;

    return RET_OK;
}

RET_CODE DMA_Init(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA)
{
    uint32_t regtmp;

    ASSERT(pDMA != NULL && pConfig != NULL && hDMA != NULL);

    if (!DMA_DEV_ISACTIVE(pDMA))
    {
        /*Disable DMA peripheral*/
        DMA_DEV_DISABLE(pDMA);
        /*Initialize DMA transaction handler*/
        DMA_InitHandle(pDMA, pConfig, hDMA);

        s_dmaHandle[DMA_GetIndex(pDMA)] = hDMA;

        /*Enable DMA interrupt in system level*/
        NVIC_EnableIRQ(DMA_IRQn);

        /*Disable all interrupts and clear all interrupts*/
        DMA_INTERRUPT_DISABLE(pDMA);
        DAM_CLEAR_ALL_INTERRUPTS(pDMA, ALL_DMA_INTERRUPTS);
        DMA_UNMASK_ALL_INTERRUPTS(pDMA);

        regtmp = 0x00000000;
        /*Configure source & destination data-width, address increment, DMA transaction direction*/
        regtmp |= (pConfig->destWidth << DMA_TCFG_DTW_POS);
        regtmp |= (pConfig->srcWidth  << DMA_TCFG_STW_POS);
        regtmp |= (pConfig->destIncrement << DMA_TCFG_DAI_POS);
        regtmp |= (pConfig->srcIncrement << DMA_TCFG_SAI_POS);
        regtmp |= (pConfig->direction << DMA_TCFG_TTP_POS);
        pDMA->TCFG = regtmp;

        regtmp = 0x00000000;
        /*Configure source & destination handshake mode*/
        regtmp |= (pConfig->destHandshake << DMA_HSCFG_DH_POS);
        regtmp |= (pConfig->srcHandshake  << DMA_HSCFG_SH_POS);
        pDMA->HSCFG = regtmp;

        regtmp = 0x00000000;
        /*Configure source & destination hardware handshake channel*/
        if (pConfig->destHandshake == DMA_HARDWARE_HANDSHAKE)
        {
            regtmp |= (pConfig->destHwHSChannel << DMA_CHN_DCHN_POS);
        }
        if (pConfig->srcHandshake == DMA_HARDWARE_HANDSHAKE)
        {
            regtmp |= (pConfig->srcHwHSChannel << DMA_CHN_SCHN_POS);
        }
        pDMA->CHN = regtmp;

        /*DMA is now in ready state*/
        hDMA->state = DMA_STATE_READY;
    }
    else
    {
        return RET_BUSY;
    }

    return RET_OK;
}

RET_CODE DMA_DeInit(DMA_Handle_T *hDMA)
{
    ASSERT(hDMA != NULL);

    if (hDMA->state == DMA_STATE_BUSY)
    {
        return RET_ERROR;
    }

    /*clear each individual interrupt flag*/
    DAM_CLEAR_ALL_INTERRUPTS(hDMA->pDevice, ALL_DMA_INTERRUPTS);
    /*unmask individual interrupt bit*/
    DMA_UNMASK_ALL_INTERRUPTS(hDMA->pDevice);

    /*disable DMA global interrupt flag*/
    DMA_INTERRUPT_DISABLE(hDMA->pDevice);
    /*disable DMA peripheral*/
    DMA_DEV_DISABLE(hDMA->pDevice);

    /*Change DMA state back to DMA_STATE_INITIAL*/
    hDMA->state = DMA_STATE_INITIAL;

    return RET_OK;
}

RET_CODE DMA_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t size)
{
    RET_CODE        retval = RET_ERROR;
    DMA_HSMode_T    handShake;
    DMA_Direction_T direction;

    ASSERT(hDMA != NULL);

    /*Indicating DMA is busy*/
    hDMA->state = DMA_STATE_BUSY;

    /*Enable DMA global interrupt*/
    DMA_INTERRUPT_ENABLE(hDMA->pDevice);

    /*Mask individual interrupt accordingly*/
    DMA_MASK_TRCMPLT(hDMA->pDevice);
    DMA_MASK_BLKTRCMPLT(hDMA->pDevice);
    DMA_MASK_ERR(hDMA->pDevice);

    /*Get destination handshaking*/
    handShake = (DMA_HSMode_T)(((hDMA->pDevice->HSCFG) >> DMA_HSCFG_DH_POS) & 0x01);
    direction = (DMA_Direction_T)(((hDMA->pDevice->TCFG) >> DMA_TCFG_TTP_POS) & 0x03);

    if (handShake == DMA_SOFTWARE_HANDSHAKE)
    {
        if (direction == DMA_MEMORY_TO_PERIPH)
        {
            /*Mask destination transcation complete interrupt*/
            DMA_MASK_DSTTRCMPLT(hDMA->pDevice);
        }
    }

    /*Get source handshaking*/
    handShake = (DMA_HSMode_T)(((hDMA->pDevice->HSCFG) >> DMA_HSCFG_SH_POS) & 0x03);
    if (handShake == DMA_SOFTWARE_HANDSHAKE)
    {
        if (direction == DMA_PERIPH_TO_MEMORY)
        {
            /*Mask destination transcation complete interrupt*/
            DMA_MASK_SRCTRCMPLT(hDMA->pDevice);
        }
    }

    /*Configure DMA source & destination address & block size*/
    retval = DMA_SetConfig(hDMA, srcAddress, destAddress, size);
    if (retval != RET_OK)
    {
        return retval;
    }

    /*Trigger data transaction & ensure interrupt is on the way*/
    DMA_DEV_ENABLE(hDMA->pDevice);
    DMA_TRANS_TRIGGER(hDMA->pDevice);

    return RET_OK;
}

/**
 * @brief Handles DMA interrupt request.
 *
 * This function handles DMA each individual interrupt, it clears corresponding interrupt flag,
 * and calls callback function if callback is available.
 *
 * @param  pHndCb   Pointer to a DMAHdlCb_T structure that contains
 *                  DMA transaction handler and callback function.
 * @retval None
 */
static void DMA_CommonIRQHandler(DMA_Handle_T *hDMA)
{
    uint32_t intStatus = hDMA->pDevice->IS;
    DMA_Event_T cbEvt = DMA_EVENT_TRANSFER_DONE;

    if (intStatus & DMA_IS_DEI)
    {
        cbEvt  = DMA_EVENT_TRANSFER_ERROR;

        hDMA->state = DMA_STATE_ERROR;
        /*Clear DMA error interrupt*/
        DMA_CLEAR_ERR(hDMA->pDevice);
    }

    if (intStatus & DMA_IS_DTDI)
    {
        cbEvt = DMA_EVENT_DESTTRANS_CMPLT;

        /*Clear destination DMA transaction complete interrupt*/
        DMA_CLEAR_DSTTRCMPL(hDMA->pDevice);
    }

    if (intStatus & DMA_IS_STDI)
    {
        cbEvt = DMA_EVENT_SRCTRANS_CMPLT;

        /*Clear source DMA transaction complete interrupt*/
        DMA_CLEAR_SRCTRCMPLT(hDMA->pDevice);
    }

    if (intStatus & DMA_IS_BDI)
    {
        cbEvt = DMA_EVENT_BLKTRANS_CMPLT;

        /*Clear DMA block transaction complete interrupt*/
        DMA_CLEAR_BLKTRCMPLT(hDMA->pDevice);

        /*Clear possible pending SRCTRCMPLT or DSTTRCMPLT interrupts*/
        DMA_CLEAR_DSTTRCMPL(hDMA->pDevice);
        DMA_CLEAR_SRCTRCMPLT(hDMA->pDevice);
    }

    if (intStatus & DMA_IS_TDI)
    {
        cbEvt = DMA_EVENT_TRANSFER_DONE;

        /*Clear DMA transaction complete interrupt*/
        DMA_CLEAR_TRCMPLT(hDMA->pDevice);

        /*Disable Global interrupts and clear all interrupts*/
        DMA_INTERRUPT_DISABLE(hDMA->pDevice);
        DMA_UNMASK_ALL_INTERRUPTS(hDMA->pDevice);

        DMA_CLEAR_DSTTRCMPL(hDMA->pDevice);
        DMA_CLEAR_SRCTRCMPLT(hDMA->pDevice);
        DMA_CLEAR_BLKTRCMPLT(hDMA->pDevice);

        /*Change DMA state back to DMA_STATE_INITIAL*/
        hDMA->state = DMA_STATE_INITIAL;
    }

    if (hDMA->callback != NULL)
    {
        /*Call callback function*/
        hDMA->callback(hDMA, cbEvt, 0);
    }
}

#elif (defined CHIP_GM6621)

/**
 * DMA channel mapping
 */
typedef struct
{
    DMA_HsChannelIdx_T hsChannelIdx;        /**< HsChannel index(0~4) */
    uint16_t           hsChannelSel;        /**< HsChannel selection(refer to DMA Handshaking Channel Selection Defination) */
    uint16_t           hsChannelPos;        /**< HsChannel bit position of the channel selection */
} DMA_HsChannel_Map_T;

/*Pointers to DMA bases for each instance*/
static DMA_Device_T *const s_dmaBases = DMA0;

/*Pointers to DMA handles for each instance*/
static DMA_Handle_T *s_dmaHandle[MAX_DMA_CHANNEL] = {NULL};

/* DMA handshake channel mapping, the mapping index is relatied to enum DMA_HsChannel_T */
static DMA_HsChannel_Map_T s_dmaHsChannelMap[] =
{
    /********************** M2P/P2M Channels ***************************/
    /* CHN4 */
    {DMA_CHANNEL4, DMA_SEL_ADC_RX_TO_CHANNEL4,     DMA_HS_CHN4_POS},
    {DMA_CHANNEL4, DMA_SEL_UART1_TX_TO_CHANNEL4,   DMA_HS_CHN4_POS},
    {DMA_CHANNEL4, DMA_SEL_I2C0_TX_TO_CHANNEL4,    DMA_HS_CHN4_POS},
    {DMA_CHANNEL4, DMA_SEL_I2C1_TX_TO_CHANNEL4,    DMA_HS_CHN4_POS},
    {DMA_CHANNEL4, DMA_SEL_SPI0_TX_TO_CHANNEL4,    DMA_HS_CHN4_POS},
    {DMA_CHANNEL4, DMA_SEL_SPI1_TX_TO_CHANNEL4,    DMA_HS_CHN4_POS},
    {DMA_CHANNEL4, DMA_SEL_ETM2CH3_TX_TO_CHANNEL4, DMA_HS_CHN4_POS},
    /* CHN3 */
    {DMA_CHANNEL3, DMA_SEL_UART0_TX_TO_CHANNEL3,   DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_UART1_RX_TO_CHANNEL3,   DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_UART2_TX_TO_CHANNEL3,   DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_I2C0_RX_TO_CHANNEL3,    DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_I2C1_RX_TO_CHANNEL3,    DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_SPI0_RX_TO_CHANNEL3,    DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_SPI1_RX_TO_CHANNEL3,    DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_ETM22CH1_TX_TO_CHANNEL3, DMA_HS_CHN3_POS},
    {DMA_CHANNEL3, DMA_SEL_ETM2CH2_TX_TO_CHANNEL3, DMA_HS_CHN3_POS},
    /* CHN2 */
    {DMA_CHANNEL2, DMA_SEL_UART0_RX_TO_CHANNEL2,   DMA_HS_CHN2_POS},
    {DMA_CHANNEL2, DMA_SEL_UART1_TX_TO_CHANNEL2,   DMA_HS_CHN2_POS},
    {DMA_CHANNEL2, DMA_SEL_UART2_RX_TO_CHANNEL2,   DMA_HS_CHN2_POS},
    {DMA_CHANNEL2, DMA_SEL_I2C1_TX_TO_CHANNEL2,    DMA_HS_CHN2_POS},
    {DMA_CHANNEL2, DMA_SEL_SPI0_RX_TO_CHANNEL2,    DMA_HS_CHN2_POS},
    {DMA_CHANNEL2, DMA_SEL_ETM22CH0_TX_TO_CHANNEL2, DMA_HS_CHN2_POS},
    /* CHN1 */
    {DMA_CHANNEL1, DMA_SEL_UART0_TX_TO_CHANNEL1,   DMA_HS_CHN1_POS},
    {DMA_CHANNEL1, DMA_SEL_UART1_RX_TO_CHANNEL1,   DMA_HS_CHN1_POS},
    {DMA_CHANNEL1, DMA_SEL_I2C0_TX_TO_CHANNEL1,    DMA_HS_CHN1_POS},
    {DMA_CHANNEL1, DMA_SEL_I2C1_RX_TO_CHANNEL1,    DMA_HS_CHN1_POS},
    {DMA_CHANNEL1, DMA_SEL_SPI0_TX_TO_CHANNEL1,    DMA_HS_CHN1_POS},
    {DMA_CHANNEL1, DMA_SEL_SPI1_TX_TO_CHANNEL1,    DMA_HS_CHN1_POS},
    {DMA_CHANNEL1, DMA_SEL_ETM21CH1_TX_TO_CHANNEL1, DMA_HS_CHN1_POS},
    {DMA_CHANNEL1, DMA_SEL_ETM2CH1_TX_TO_CHANNEL1, DMA_HS_CHN1_POS},
    /* CHN0 */
    {DMA_CHANNEL0, DMA_SEL_ADC_TX_TO_CHANNEL0,     DMA_HS_CHN0_POS},
    {DMA_CHANNEL0, DMA_SEL_UART0_RX_TO_CHANNEL0,   DMA_HS_CHN0_POS},
    {DMA_CHANNEL0, DMA_SEL_I2C0_RX_TO_CHANNEL0,    DMA_HS_CHN0_POS},
    {DMA_CHANNEL0, DMA_SEL_SPI1_RX_TO_CHANNEL0,    DMA_HS_CHN0_POS},
    {DMA_CHANNEL0, DMA_SEL_ETM21CH0_TX_TO_CHANNEL0, DMA_HS_CHN0_POS},
    {DMA_CHANNEL0, DMA_SEL_ETM2CH0_TX_TO_CHANNEL0, DMA_HS_CHN0_POS},
    /********************** M2M Channels ***************************/
    {DMA_CHANNEL0, NULL,                           NULL},
    {DMA_CHANNEL1, NULL,                           NULL},
    {DMA_CHANNEL2, NULL,                           NULL},
    {DMA_CHANNEL3, NULL,                           NULL},
    {DMA_CHANNEL4, NULL,                           NULL},
};

/* DMA Request periph type mapping, relatied to DMA_RequestType_T */
static DMA_HsChannel_T s_dmaRequestTypeMap[][MAX_DMA_CHANNEL] =
{
    /* ADC */
    {DMA_ADC_TX_TO_CHANNEL0, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},     /* ADC_TX */
    {DMA_ADC_RX_TO_CHANNEL4, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},     /* ADC_RX */
    /* UART */
    {DMA_UART0_TX_TO_CHANNEL1, DMA_UART0_TX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* UART0_TX */
    {DMA_UART0_RX_TO_CHANNEL0, DMA_UART0_RX_TO_CHANNEL2, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* UART0_RX */
    {DMA_UART1_TX_TO_CHANNEL2, DMA_UART1_TX_TO_CHANNEL4, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* UART1_TX */
    {DMA_UART1_RX_TO_CHANNEL1, DMA_UART1_RX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* UART1_RX */
    {DMA_UART2_TX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},   /* UART2_TX */
    {DMA_UART2_RX_TO_CHANNEL2, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},   /* UART2_RX */
    /* I2C */
    {DMA_I2C0_TX_TO_CHANNEL1, DMA_I2C0_TX_TO_CHANNEL4, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* I2C0_TX */
    {DMA_I2C0_RX_TO_CHANNEL0, DMA_I2C0_RX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* I2C0_RX */
    {DMA_I2C1_TX_TO_CHANNEL2, DMA_I2C1_TX_TO_CHANNEL4, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* I2C1_TX */
    {DMA_I2C1_RX_TO_CHANNEL1, DMA_I2C1_RX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* I2C1_RX */
    /* SPI */
    {DMA_SPI0_TX_TO_CHANNEL1, DMA_SPI0_TX_TO_CHANNEL4, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* SPI0_TX */
    {DMA_SPI0_RX_TO_CHANNEL2, DMA_SPI0_RX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* SPI0_RX */
    {DMA_SPI1_TX_TO_CHANNEL1, DMA_SPI1_TX_TO_CHANNEL4, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* SPI1_TX */
    {DMA_SPI1_RX_TO_CHANNEL0, DMA_SPI1_RX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},  /* SPI1_RX */
    /* ETM */
    {DMA_ETM2CH0_TX_TO_CHANNEL0, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID}, /* ETM2CH0_TX */
    {DMA_ETM2CH1_TX_TO_CHANNEL1, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID}, /* ETM2CH1_TX */
    {DMA_ETM2CH2_TX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID}, /* ETM2CH2_TX */
    {DMA_ETM2CH3_TX_TO_CHANNEL4, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID}, /* ETM2CH3_TX */
    {DMA_ETM21CH0_TX_TO_CHANNEL0, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* ETM21CH0_TX */
    {DMA_ETM21CH1_TX_TO_CHANNEL1, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* ETM21CH1_TX */
    {DMA_ETM22CH0_TX_TO_CHANNEL2, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* ETM22CH0_TX */
    {DMA_ETM22CH1_TX_TO_CHANNEL3, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID, DMA_HSCHANNEL_INVALID},/* ETM22CH1_TX */
    /* M2M */
    {DMA_M2M_CHANNEL0, DMA_M2M_CHANNEL1, DMA_M2M_CHANNEL2, DMA_M2M_CHANNEL3, DMA_M2M_CHANNEL4},                               /* M2M */
};


/*Declarition of DMA interrupt service routine*/
static void DMA0_IRQHandler(void);

#ifdef DMA0
    STATIC_ISR(DMA_IRQn, DMA0_IRQHandler);
#endif

static void DMA_CommonIRQHandler(DMA_Handle_T **hDMA);
static void DMA0_IRQHandler(void)
{
    DMA_CommonIRQHandler(s_dmaHandle);
}


/**
 * @brief Initialize DMA transaction handler.
 *
 * This intializes DMA transcation handler elements (specify DMA device handler, etc)
 *
 * @param  pDMA      Pointer to DMA device handler.
 * @param  pConfig   Pointer to DMA configuration structure.
 * @param  hDMA      Pointer to DMA transaction handler.
 *
 * @retval RET_OK    Always returns RET_OK.
 */
static RET_CODE DMA_InitHandle(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA)
{
    uint32_t i, j;
    uint8_t freeHandleFound = FALSE;
    DMA_HsChannelIdx_T hsChannelIdx;
    DMA_HsChannelIdx_T hsChannelIdxTmp;
    DMA_HsChannel_T    hsChannel;
    DMA_HsChannel_T    hsChannel2;

    ASSERT(hDMA != NULL && pConfig != NULL && pDMA != NULL);

    if (pDMA != s_dmaBases)
    {
        return RET_INVPARAM;
    }

    /* check whether channel has already been used */
    for (i = 0; i < MAX_DMA_CHANNEL; i++)
    {
        hsChannel = s_dmaRequestTypeMap[pConfig->requestType][i];
        if (hsChannel != DMA_HSCHANNEL_INVALID)
        {
            hsChannelIdx = s_dmaHsChannelMap[hsChannel].hsChannelIdx;

            /* check whether handle is used */
            for (j = 0; j < MAX_DMA_CHANNEL; j++)
            {
                if (s_dmaHandle[j] != NULL)
                {
                    hsChannelIdxTmp = s_dmaHsChannelMap[s_dmaHandle[j]->hsChannel].hsChannelIdx;
                    if ((s_dmaHandle[j]->pDevice == pDMA) && (hsChannelIdxTmp == hsChannelIdx))
                    {
                        break;
                    }
                }
            }

            if (j >= MAX_DMA_CHANNEL)
            {
                /* usable channel found */
                break;
            }
        }
    }
    if (i >= MAX_DMA_CHANNEL)
    {
        /* no usable channel */
        return RET_BUSY;
    }

    /* find dest peripheral channel for P2P mode */
    if (pConfig->direction == DMA_PERIPH_TO_PERIPH)
    {
        for (i = 0; i < MAX_DMA_CHANNEL; i++)
        {
            hsChannel2 = s_dmaRequestTypeMap[pConfig->requestType2][i];
            if (hsChannel2 != DMA_HSCHANNEL_INVALID)
            {
                hsChannelIdxTmp = s_dmaHsChannelMap[hsChannel2].hsChannelIdx;
                if (hsChannelIdxTmp == hsChannelIdx)
                {
                    /* success find out the same channel with src peripheral */
                    break;
                }
            }
        }
        if (i >= MAX_DMA_CHANNEL)
        {
            /* not found the correct dest peripheral channel */
            return RET_ERROR;
        }
    }

    /* find free handle */
    for (i = 0; i < MAX_DMA_CHANNEL; i++)
    {
        if (s_dmaHandle[i] == NULL)
        {
            freeHandleFound = TRUE;
            break;
        }
    }
    if (!freeHandleFound)
    {
        return RET_BUSY;
    }

    s_dmaHandle[i] = hDMA;
    hDMA->pDevice = pDMA;
    hDMA->hsChannelIdx = hsChannelIdx;
    hDMA->hsChannel = hsChannel;
    if (pConfig->direction == DMA_PERIPH_TO_PERIPH)
    {
        hDMA->hsChannel2 = hsChannel2;
    }
    hDMA->srcWidth = pConfig->srcWidth;
    hDMA->destWidth = pConfig->destWidth;
    hDMA->callback = pConfig->callback;
    hDMA->pesEnable = pConfig->pesEnable;
    hDMA->pesChnIndex = pConfig->pesChnIndex;
    /*DMA is now in initial state*/
    hDMA->state = DMA_STATE_INITIAL;
    hDMA->pParent  = (void *) NULL;

    return RET_OK;
}

static RET_CODE DMA_Set_SEL(DMA_Device_T *pDevice, DMA_HsChannel_T hsSrcChannel, DMA_HsChannel_T hsDstChannel)
{
    RET_CODE ret = RET_OK;
    if (hsSrcChannel != DMA_HSCHANNEL_INVALID)
    {
        pDevice->SCSEL &= (~(DMA_HS_CHNX_MASK << s_dmaHsChannelMap[hsSrcChannel].hsChannelPos));
        pDevice->SCSEL |= (s_dmaHsChannelMap[hsSrcChannel].hsChannelSel << s_dmaHsChannelMap[hsSrcChannel].hsChannelPos);
    }

    if (hsDstChannel != DMA_HSCHANNEL_INVALID)
    {
        pDevice->DCSEL &= (~(DMA_HS_CHNX_MASK << s_dmaHsChannelMap[hsDstChannel].hsChannelPos));
        pDevice->DCSEL |= (s_dmaHsChannelMap[hsDstChannel].hsChannelSel << s_dmaHsChannelMap[hsDstChannel].hsChannelPos);
    }

    return ret;
}

static RET_CODE DMA_Enable_PES(DMA_Device_T *pDevice, DMA_HsChannelIdx_T hsSrcChannel, PES_ChannelIndex_T pesChnIndex)
{
    RET_CODE ret = RET_OK;

    pDevice->DMA_Chn[4].RESERVE  &= (~(DMA_PES_MASK << (hsSrcChannel * 5)));
    pDevice->DMA_Chn[4].RESERVE  |= (pesChnIndex << (hsSrcChannel * 5));
    pDevice->DMA_Chn[4].RESERVE  |= ((1 << DMA_PES_EN_POS) << (hsSrcChannel * 5));

    return ret;
}

RET_CODE DMA_Init(DMA_Device_T *pDMA, DMA_Config_T *pConfig, DMA_Handle_T *hDMA)
{
    uint32_t regtmp;
    RET_CODE ret = RET_ERROR;
    DMA_HsChannelIdx_T hsChannelIdx;

    ASSERT(pDMA != NULL && pConfig != NULL && hDMA != NULL);

    if (!DMA_DEV_ISACTIVE(pDMA))
    {
        DMA_DEV_ENABLE(pDMA);
        NVIC_EnableIRQ(DMA_IRQn);
    }

    /*Initialize DMA transaction handler*/
    ret = DMA_InitHandle(pDMA, pConfig, hDMA);
    if (ret != RET_OK)
    {
        return ret;
    }

    hsChannelIdx = s_dmaHsChannelMap[hDMA->hsChannel].hsChannelIdx;

    DMA_CHANNEL_DISABLE(hDMA->pDevice, hsChannelIdx);
    DMA_INTERRUPT_DISABLE(hDMA->pDevice, hsChannelIdx);
    DAM_CLEAR_ALL_INTERRUPTS(hDMA->pDevice, hsChannelIdx);
    DMA_UNMASK_ALL_INTERRUPTS(hDMA->pDevice, hsChannelIdx);

    /* TCFG */
    regtmp = 0x00000000;
    regtmp |= (pConfig->destWidth << DMA_TCFG_DTW_POS);
    regtmp |= (pConfig->srcWidth  << DMA_TCFG_STW_POS);
    regtmp |= (pConfig->destIncrement << DMA_TCFG_DAI_POS);
    regtmp |= (pConfig->srcIncrement << DMA_TCFG_SAI_POS);
    regtmp |= (pConfig->direction << DMA_TCFG_TTP_POS);
    hDMA->pDevice->DMA_Chn[hsChannelIdx].TCFG = regtmp;

    /* HSCFG */
    hDMA->pDevice->DMA_Chn[hsChannelIdx].HSCFG |= (pConfig->chnPriority << DMA_HSCFG_PRI_POS);
    hDMA->pDevice->DMA_Chn[hsChannelIdx].HSCFG |= (pConfig->srcHandshake << DMA_HSCFG_SH_POS);
    hDMA->pDevice->DMA_Chn[hsChannelIdx].HSCFG |= (pConfig->destHandshake << DMA_HSCFG_DH_POS);
    hDMA->pDevice->DMA_Chn[hsChannelIdx].HSCFG |= (pConfig->srcReload << DMA_HSCFG_SRL_POS);
    hDMA->pDevice->DMA_Chn[hsChannelIdx].HSCFG |= (pConfig->destReload << DMA_HSCFG_DRL_POS);

    /* SRC_SEL/DST_SEL */
    if (pConfig->direction == DMA_MEMORY_TO_PERIPH)
    {
        DMA_Set_SEL(hDMA->pDevice, DMA_HSCHANNEL_INVALID, hDMA->hsChannel);
    }
    else if (pConfig->direction == DMA_PERIPH_TO_MEMORY)
    {
        DMA_Set_SEL(hDMA->pDevice, hDMA->hsChannel, DMA_HSCHANNEL_INVALID);
    }
    else if (pConfig->direction == DMA_PERIPH_TO_PERIPH)
    {
        DMA_Set_SEL(hDMA->pDevice, hDMA->hsChannel, hDMA->hsChannel2);
    }

    /* Gather */
    if (pConfig->srcGather == DMA_GATHER_ENABLE)
    {
        IO_BIT_SET(hDMA->pDevice->DMA_Chn[hsChannelIdx].TCFG, DMA_GATHER_EN);
        hDMA->pDevice->DMA_Chn[hsChannelIdx].SGR |= (pConfig->srcGatherInterval << DMA_SRC_GATHER_INTERVAL_POS);
        hDMA->pDevice->DMA_Chn[hsChannelIdx].SGR |= (pConfig->srcGatherCount << DMA_SRC_GATHER_COUNT_POS);
    }

    /* Scatter */
    if (pConfig->destScatter == DMA_SCATTER_ENABLE)
    {
        IO_BIT_SET(hDMA->pDevice->DMA_Chn[hsChannelIdx].TCFG, DMA_SCATTER_EN);
        hDMA->pDevice->DMA_Chn[hsChannelIdx].DSR |= (pConfig->destScatterInterval << DMA_DST_SCATTER_INTERVAL_POS);
        hDMA->pDevice->DMA_Chn[hsChannelIdx].DSR |= (pConfig->destScatterCount << DMA_DST_SCATTER_COUNT_POS);
    }

    /* interrupt mask */
    DMA_MASK_TRCMPLT(hDMA->pDevice, hsChannelIdx);
    DMA_MASK_ERR(hDMA->pDevice, hsChannelIdx);
    if (0)//(pConfig->direction == DMA_PERIPH_TO_PERIPH)
    {
        /* according to the spec, do not enable SRC/DST interrupt when SRC or DST is memory */
        DMA_MASK_SRCTRCMPLT(hDMA->pDevice, hsChannelIdx);
        DMA_MASK_DSTTRCMPLT(hDMA->pDevice, hsChannelIdx);
        DMA_MASK_BLKTRCMPLT(hDMA->pDevice, hsChannelIdx);
    }
    if (pConfig->srcReload || pConfig->destReload)
    {
        DMA_MASK_BLKTRCMPLT(hDMA->pDevice, hsChannelIdx);
    }

    hDMA->state = DMA_STATE_READY;
    return RET_OK;
}


RET_CODE DMA_DeInit(DMA_Handle_T *hDMA)
{
    DMA_HsChannelIdx_T hsChannelIdx;
    uint32_t i = 0;

    ASSERT(hDMA != NULL);

    /* already DeInit */
    if (hDMA->pDevice == NULL)
    {
        return RET_OK;
    }

    if (hDMA->state == DMA_STATE_BUSY)
    {
        return RET_ERROR;
    }

    hsChannelIdx = s_dmaHsChannelMap[hDMA->hsChannel].hsChannelIdx;

    DMA_CHANNEL_DISABLE(hDMA->pDevice, hsChannelIdx);
    DMA_INTERRUPT_DISABLE(hDMA->pDevice, hsChannelIdx);
    DAM_CLEAR_ALL_INTERRUPTS(hDMA->pDevice, hsChannelIdx);
    DMA_UNMASK_ALL_INTERRUPTS(hDMA->pDevice, hsChannelIdx);

    for (i = 0; i < MAX_DMA_CHANNEL; i++)
    {
        if (s_dmaHandle[i] == hDMA)
        {
            memset(s_dmaHandle[i], 0, sizeof(DMA_Handle_T));
            s_dmaHandle[i]  = NULL;
            break;
        }
    }

    /*Change DMA state back to DMA_STATE_INITIAL*/
    hDMA->state = DMA_STATE_INITIAL;

    return RET_OK;
}


RET_CODE DMA_Start(DMA_Handle_T *hDMA, uint32_t srcAddress, uint32_t destAddress, uint32_t size)
{
    DMA_HsChannelIdx_T hsChannelIdx;

    ASSERT(hDMA != NULL);

    if (size > MAX_BLOCK_SIZE_LENGTH)
    {
        return RET_INVPARAM;
    }

    /*Indicating DMA is busy*/
    hDMA->state = DMA_STATE_BUSY;

    hsChannelIdx = s_dmaHsChannelMap[hDMA->hsChannel].hsChannelIdx;

    hDMA->pDevice->DMA_Chn[hsChannelIdx].SAR =  srcAddress;
    hDMA->pDevice->DMA_Chn[hsChannelIdx].DAR =  destAddress;
    hDMA->pDevice->DMA_Chn[hsChannelIdx].SZ =  size;

    DMA_INTERRUPT_ENABLE(hDMA->pDevice, hsChannelIdx);
    if (hDMA->pesEnable)
    {
        DMA_Enable_PES(hDMA->pDevice, hsChannelIdx, hDMA->pesChnIndex);
    }
    else
    {
        DMA_CHANNEL_ENABLE(hDMA->pDevice, hsChannelIdx);
    }

    return RET_OK;
}

/**
 * @brief Handles DMA interrupt request.
 *
 * This function handles DMA each individual interrupt, it clears corresponding interrupt flag,
 * and calls callback function if callback is available.
 *
 * @param  pHndCb    Pointer to a DMAHdlCb_T structure that contains
 *                   DMA transaction handler and callback function.
 * @retval None
 */
static void DMA_CommonIRQHandler(DMA_Handle_T **hDMA)
{
    DMA_Device_T *dmaDevice = DMA0;
    uint32_t intStatus = dmaDevice->IS;
    DMA_Event_T cbEvt = DMA_EVENT_TRANSFER_DONE;
    uint32_t i = 0;
    DMA_HsChannelIdx_T hsChannelIdx;

#if 0
    if (intStatus != 0)
    {
        /* Clear all IS */
        dmaDevice->IS = 0xFFFFFFFF;
    }
#endif

    for (i = 0; i < MAX_DMA_CHANNEL; i++)
    {
        if (hDMA[i] != NULL)
        {
            /* only process the callback of dest channel, because the callback of source will be the same as dest */
            hsChannelIdx = s_dmaHsChannelMap[hDMA[i]->hsChannel].hsChannelIdx;

            if (intStatus & (1 << (hsChannelIdx * MAX_DMA_CHANNEL)))
            {
                cbEvt  = DMA_EVENT_TRANSFER_DONE;
                DMA_CLEAR_TRCMPLT(hDMA[i]->pDevice, hsChannelIdx);
                hDMA[i]->state = DMA_STATE_INITIAL;
                if (hDMA[i]->callback != NULL)
                {
                    hDMA[i]->callback(hDMA[i], cbEvt, 0);
                }
            }

            if (intStatus & (1 << (hsChannelIdx * MAX_DMA_CHANNEL + 1)))
            {
                cbEvt  = DMA_EVENT_BLKTRANS_CMPLT;
                DMA_CLEAR_BLKTRCMPLT(hDMA[i]->pDevice, hsChannelIdx);
                if (hDMA[i]->callback != NULL)
                {
                    hDMA[i]->callback(hDMA[i], cbEvt, 0);
                }
            }

            if (intStatus & (1 << (hsChannelIdx * MAX_DMA_CHANNEL + 2)))
            {
                cbEvt  = DMA_EVENT_SRCTRANS_CMPLT;
                DMA_CLEAR_SRCTRCMPLT(hDMA[i]->pDevice, hsChannelIdx);
                if (hDMA[i]->callback != NULL)
                {
                    hDMA[i]->callback(hDMA[i], cbEvt, 0);
                }
            }

            if (intStatus & (1 << (hsChannelIdx * MAX_DMA_CHANNEL + 3)))
            {
                cbEvt  = DMA_EVENT_DESTTRANS_CMPLT;
                DMA_CLEAR_DSTTRCMPLT(hDMA[i]->pDevice, hsChannelIdx);
                if (hDMA[i]->callback != NULL)
                {
                    hDMA[i]->callback(hDMA[i], cbEvt, 0);
                }
            }

            if (intStatus & (1 << (hsChannelIdx * MAX_DMA_CHANNEL + 4)))
            {
                cbEvt  = DMA_EVENT_TRANSFER_ERROR;
                DMA_CLEAR_ERR(hDMA[i]->pDevice, hsChannelIdx);
                hDMA[i]->state = DMA_STATE_ERROR;
                if (hDMA[i]->callback != NULL)
                {
                    hDMA[i]->callback(hDMA[i], cbEvt, 0);
                }
            }
        }
    }
}

#else
#error Not_Support_Chipset
#endif

