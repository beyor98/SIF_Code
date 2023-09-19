/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          spi.c
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         Header file of uart driver.
 *
 * @note
 *    2015/10/07, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/14, wen.liu, V1.1.0
 *        Update coding style.
 *    2020/11/25, Christal.Xu, V1.2.0
 *        Update function.
 */
#include <drv/dev/spi.h>

#ifdef SPI0
    STATIC_ISR(SPI0_IRQn, SPI0_InterruptHandler);
#endif
#ifdef SPI1
    STATIC_ISR(SPI1_IRQn, SPI1_InterruptHandler);
#endif

#define SPI_DMA_STATUS_FINISHED TRUE
#define SPI_DMA_STATUS_STARTED  FALSE

static RET_CODE SPI_ProcessData(SPI_Handle_T *hSPI);

static SPI_Device_T *const s_spiBases[MAX_SPI_DEVICE] = {ALL_SPI_DEVICES};

static IRQn_Type s_spiIrq[MAX_SPI_DEVICE] = {ALL_SPI_INTERRUPTS};

static SPI_Handle_T *s_spiHandle[MAX_SPI_DEVICE];

static void (*s_spiIsr)(SPI_Handle_T *hSPI) = NULL;


static void SPI_InterruptHandler(SPI_Handle_T *hSPI);

static __inline uint32_t SPI_GetIndex(SPI_Device_T *pSPI)
{
    uint32_t i;

    /* Find the  index from base address mappings. */
    for (i = 0; i < MAX_SPI_DEVICE; i++)
    {
        if (s_spiBases[i] == pSPI)
        {
            break;
        }
    }

    ASSERT(i < MAX_SPI_DEVICE);

    return i;
}

static uint32_t SPI_GetFieldFromBits(uint32_t type, uint32_t value)
{
    switch (type)
    {
        case   SPI_CTRL_XCMD:
        case   SPI_CTRL_XDATA:
        case   SPI_CTRL_XADDR:
        case   SPI_CTRL_XDMY:
        default:
            if (value == 8)
            {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
                return 7;
#elif (defined(CHIP_GM6621) || defined(CHIP_F6601C))
                return 0;
#endif
            }
            else if (value == 16)
            {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
                return 15;
#elif (defined(CHIP_GM6621)|| defined(CHIP_F6601C))
                return 1;
#endif
            }
            else if (value == 24)
            {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
                return 23;
#elif (defined(CHIP_GM6621)|| defined(CHIP_F6601C))
                return 2;
#endif
            }
            else if (value == 32)
            {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
                return 31;
#elif (defined(CHIP_GM6621)|| defined(CHIP_F6601C))
                return 3;
#endif
            }
            else
            {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)
                return 7;
#elif (defined(CHIP_GM6621)|| defined(CHIP_F6601C))
                return 0;
#endif
            }
    }
}

RET_CODE SPI_Init(SPI_Device_T *pSPI, SPI_Config_T *pConfig, SPI_Handle_T *hSPI)
{
    uint32_t format, ctrl;
    uint8_t index = MAX_SPI_DEVICE;

    if (!(hSPI && pConfig && pSPI))
    {
        return RET_INVPARAM;
    }

    /* Disable spi device to exit from running operation before configuration */
    IO_BIT_CLEAR(pSPI->CTRL, SPI_CTRL_EN);

    /* Initialize some local variables about spi configurations */
    ctrl = 0;
    format = pConfig->format;

    /* SPI operation mode */
    ctrl |= (pConfig->mode == SPI_MASTER_MODE) ? SPI_CTRL_MS : 0;

    /* SPI format,  freescale/ssi/microwire */
    ctrl |= ((format == SPI_FORMAT_MOTOLORA) ? SPI_CTRL_FMT_FREESCALE :
             ((format == SPI_FORMAT_TI_SSP) ? SPI_CTRL_FMT_TI : 0));
    if (format == SPI_FORMAT_MOTOLORA)
    {
        /* Clock polarity */
        ctrl |= ((pConfig->polarity) ? SPI_CTRL_CPOL : 0);
        /* Clock phase */
        ctrl |= ((pConfig->phase) ? SPI_CTRL_CPHA : 0);
    }
    else
        /*set ti mode polarity low and phase high*/
    {
        /* Clock polarity */
        ctrl &= ~(SPI_CTRL_CPOL);
        /* Clock phase */
        ctrl |= SPI_CTRL_CPHA;
    }
    /* Bit order */
    ctrl |= ((pConfig->bitOrder) ? SPI_CTRL_LSB : 0);
    /* Setup fields of spi control register */
    IO_BITMASK_SET(pSPI->CTRL, SPI_CTRL_MS  | SPI_CTRL_BIDIR | SPI_CTRL_BIDIROE | SPI_CTRL_TMODE | SPI_CTRL_XFRM |
                   SPI_CTRL_FMT | SPI_CTRL_CPOL  | SPI_CTRL_CPHA | SPI_CTRL_LSB, ctrl);

    /* Baud rate */
    pSPI->BR = SYS_CLOCK / pConfig->baudRate;

    /* Get index of this spi device */
    index = SPI_GetIndex(pSPI);
    if (index < MAX_SPI_DEVICE)
    {
        /* Handle of spi device */
        s_spiHandle[index] = hSPI;
        /* Enable NVIC interrupt for this SPI device */
        NVIC_EnableIRQ(s_spiIrq[index]);
        /* Write device base address into handle structure */
        if (hSPI != NULL)
        {
            hSPI->device = pSPI;
            hSPI->callback = pConfig->callback;
            hSPI->index = index;
            hSPI->interruptMask = 0;
            hSPI->txExpectedSize = hSPI->txSize = 0;
            hSPI->rxExpectedSize = hSPI->rxSize = 0;
            hSPI->enableRxDMA = FALSE;
            hSPI->enableTxDMA = FALSE;
        }
    }

    /* spi interrupt handler */
    s_spiIsr = SPI_InterruptHandler;

    return RET_OK;
}

/**
 * @brief Sets SPI_Config_T structure to default values.
 *
 * The purpose of this API is to get the configuration structure initialized for the SPI_Init().
 * User may modify some configurations before calling SPI_Init(). The function simplify configuration
 * process.

 * @param pConfig Pointer to SPI_Config_T structure
 * @param mode    Master mode or slave mode
 */
RET_CODE SPI_GetDefaultConfig(SPI_Config_T *pConfig, SPI_TransferConfig_T *pTransferConfig, uint8_t mode)
{
    /* Default SPI fixed configuration, such as format, clock polarity, clock phase, bit order, and baud rate. */
    if (pConfig != NULL)
    {
        pConfig->mode = mode;
        pConfig->format = SPI_FORMAT_MOTOLORA;
        pConfig->polarity = SPI_CLOCK_POLARITY_LOW;
        pConfig->phase = SPI_CLOCK_PHASE_MIDDLE;
        pConfig->bitOrder = SPI_BIT_ORDER_MSB;
        pConfig->baudRate = SYS_CLOCK / 0x80;
        pConfig->callback = NULL;
    }

    /* Default SPI configuration related to frames format, coutinous transfer, and dma. */
    if (pTransferConfig != NULL)
    {
        pTransferConfig->duplex = SPI_FULL_DUPLEX;
        pTransferConfig->frameConfig.mask =  SPI_MASK_DATA_FRAME;
        pTransferConfig->frameConfig.dataBits = 8;
        pTransferConfig->frameConfig.commandBits = 8;
        pTransferConfig->frameConfig.addressBits = 8;
        pTransferConfig->frameConfig.dummyBits = 8;
        pTransferConfig->enableRxDMA = FALSE;
        pTransferConfig->enableTxDMA = FALSE;
        pTransferConfig->txWatermark = 2;
        pTransferConfig->rxWatermark = 4;
    }
    return RET_OK;
}

RET_CODE SPI_DeInit(SPI_Handle_T *hSPI)
{
    uint8_t index = MAX_SPI_DEVICE;

    SPI_Device_T *pSPI = hSPI->device;

    SPI_DisableInterrupts(hSPI, SPI_TX_INT | SPI_RX_INT);

    index = SPI_GetIndex(pSPI);

    /* Disable spi device */
    IO_BIT_CLEAR(pSPI->CTRL, SPI_CTRL_EN);

    NVIC_DisableIRQ(s_spiIrq[index]);

    hSPI->device = NULL;

    hSPI->callback = NULL;

    return RET_OK;
}

__STATIC_INLINE DataWriteByWidth(uint8_t *addr, __IO uint32_t val, uint8_t width)
{
    if (width == 1)
    {
        *((uint8_t *)addr) = (uint8_t)val;
    }
    else if (width == 2)
    {
        *((uint16_t *)addr) = (uint16_t)val;
    }
    else if (width == 4)
    {
        *((uint32_t *)addr) = (uint32_t)val;
    }
}

__STATIC_INLINE DataReadByWidth(uint8_t *addr, __IO uint32_t *ioAddr, uint8_t width)
{
    if (width == 1)
    {
        *ioAddr = *((uint8_t *)addr);
    }
    else if (width == 2)
    {
        *ioAddr = *((uint16_t *)addr);
    }
    else if (width == 4)
    {
        *ioAddr = *((uint32_t *)addr);
    }
}

RET_CODE SPI_Flush_FIFO(SPI_Handle_T *hSPI)
{
    RET_CODE ret;
    if (IO_BIT_GET(hSPI->device->CTRL, SPI_CTRL_EN))
    {
        IO_BIT_CLEAR(hSPI->device->CTRL, SPI_CTRL_EN);
        IO_BIT_SET(hSPI->device->CTRL, SPI_CTRL_EN);
    }
    ret = RET_OK;
    return ret;
}

static void SPI_RxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    RET_CODE ret;
    SPI_Handle_T *hSPI =  CONTAINER_OF(hDMA, SPI_Handle_T, rxDMAHandle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        hSPI->rxDMAStatus = SPI_DMA_STATUS_FINISHED;
        /* Close spi rx dma enable, if not , will generate a dma request in next transfer
        when transfer with dma, this request will make dma read a 0x00 from spi*/
        IO_BIT_CLEAR(hSPI->device->RDMA, SPI_RDMA_EN);
        /* User callback is always called here if rx dma is enabled. */
        if (hSPI->enableRxDMA == TRUE)
        {
            ret = SPI_ProcessData(hSPI);
            if (ret == RET_OK)
            {
                if (NULL != hSPI->callback)
                {
                    hSPI->callback(hSPI, SPI_EVENT_TRANSFER_COMPLETE, (ulong_t)0);
                }
            }
        }
        DMA_DeInit(hDMA);
    }
}

static void SPI_TxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    RET_CODE ret;
    SPI_Handle_T *hSPI =  CONTAINER_OF(hDMA, SPI_Handle_T, txDMAHandle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        hSPI->txDMAStatus = SPI_DMA_STATUS_FINISHED;
        IO_BIT_CLEAR(hSPI->device->TDMA, SPI_TDMA_EN);
        /* User callback is always called here if tx dma is enabled. */
        if (hSPI->enableTxDMA == TRUE)
        {
            ret = SPI_ProcessData(hSPI);
            if (ret == RET_OK)
            {
                if (NULL != hSPI->callback)
                {
                    hSPI->callback(hSPI, SPI_EVENT_TRANSFER_COMPLETE, (ulong_t)0);
                }
            }
        }
        DMA_DeInit(hDMA);
    }
}

static RET_CODE SPI_DMAConfig(SPI_Device_T *pSPI, uint8_t dataWidth, uint8_t dmaRead, DMA_Handle_T *pDMAHandle, DMA_Callback_T callback)
{
    DMA_Config_T dmaConfig;
    DMA_Config_T *pDMAConfig = &dmaConfig;
    memset(&dmaConfig, 0, sizeof(DMA_Config_T));

    if (dmaRead)
    {
        /* Configure DMA device for SPI Rx transfer */
        pDMAConfig->srcIncrement = DMA_ADDR_CONSTANT;
        pDMAConfig->destIncrement = DMA_ADDR_INCREMENT;
        pDMAConfig->direction = DMA_PERIPH_TO_MEMORY;
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)|| defined(CHIP_F6601C)
        pDMAConfig->srcHwHSChannel = DMA_SPI_RX_CHANNEL;
#elif defined(CHIP_GM6621)
        pDMAConfig->requestType = ((pSPI == SPI0) ? DMA_REQUEST_SPI0_RX :
                                   ((pSPI == SPI1) ? DMA_REQUEST_SPI1_RX : DMA_REQUEST_INVALID));
        pDMAConfig->chnPriority = DMA_CHN_PRIORITY_1;
        pDMAConfig->srcReload = dmaConfig.destReload = DMA_NORMAL_MODE;
        pDMAConfig->srcGather = DMA_GATHER_DISABLE;
        pDMAConfig->srcGatherInterval = 0;
        pDMAConfig->srcGatherCount = 0;
        pDMAConfig->destScatter = DMA_SCATTER_DISABLE;
        pDMAConfig->destScatterInterval = 0;
        pDMAConfig->destScatterCount = 0;
#endif
        pDMAConfig->srcHandshake = DMA_HARDWARE_HANDSHAKE;
        pDMAConfig->destHandshake = DMA_SOFTWARE_HANDSHAKE;
        pDMAConfig->callback = SPI_RxDMACallback;
        pDMAConfig->srcWidth = ((dataWidth == 1) ? DMA_WIDTH_8BIT :
                                ((dataWidth == 2) ? DMA_WIDTH_16BIT :
                                 ((dataWidth == 4) ? DMA_WIDTH_32BIT : DMA_WIDTH_8BIT)));
        pDMAConfig->destWidth = pDMAConfig->srcWidth;

        IO_BITMASK_SET(pSPI->RDMA, SPI_RDMA_RTH, SPI_RDMA_RTH_FIELD(1));
        IO_BIT_SET(pSPI->RDMA, SPI_RDMA_EN);
    }
    else
    {
        /* Configure DMA device for SPI Tx transfer */
        pDMAConfig->srcIncrement = DMA_ADDR_INCREMENT;
        pDMAConfig->destIncrement = DMA_ADDR_CONSTANT;
        pDMAConfig->direction = DMA_MEMORY_TO_PERIPH;
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B)|| defined(CHIP_F6601C)
        pDMAConfig->destHwHSChannel = DMA_SPI_TX_CHANNEL;
#elif defined(CHIP_GM6621)
        pDMAConfig->requestType = ((pSPI == SPI0) ? DMA_REQUEST_SPI0_TX :
                                   ((pSPI == SPI1) ? DMA_REQUEST_SPI1_TX : DMA_REQUEST_INVALID));
        pDMAConfig->chnPriority = DMA_CHN_PRIORITY_1;
        pDMAConfig->srcReload = dmaConfig.destReload = DMA_NORMAL_MODE;
        pDMAConfig->srcGather = DMA_GATHER_DISABLE;
        pDMAConfig->srcGatherInterval = 0;
        pDMAConfig->srcGatherCount = 0;
        pDMAConfig->destScatter = DMA_SCATTER_DISABLE;
        pDMAConfig->destScatterInterval = 0;
        pDMAConfig->destScatterCount = 0;
#endif
        pDMAConfig->srcHandshake = DMA_SOFTWARE_HANDSHAKE;
        pDMAConfig->destHandshake = DMA_HARDWARE_HANDSHAKE;
        pDMAConfig->callback = SPI_TxDMACallback;
        pDMAConfig->destWidth = ((dataWidth == 1) ? DMA_WIDTH_8BIT :
                                 ((dataWidth == 2) ? DMA_WIDTH_16BIT :
                                  ((dataWidth == 4) ? DMA_WIDTH_32BIT : DMA_WIDTH_8BIT)));
        pDMAConfig->srcWidth = pDMAConfig->destWidth;

        IO_BITMASK_SET(pSPI->TDMA, SPI_TDMA_TTH, SPI_TDMA_TTH_FIELD(1));
        IO_BIT_SET(pSPI->TDMA, SPI_TDMA_EN);
    }

    DMA_Init(DMA0, pDMAConfig, pDMAHandle);

    return RET_OK;
}

RET_CODE SPI_TransferConfig(SPI_Handle_T *hSPI, SPI_TransferConfig_T *pConfig)
{
    uint32_t frameMask, duplex, ctrl;
    SPI_Device_T *pSPI = hSPI->device;
    if (hSPI == NULL)
    {
        return RET_INVPARAM;
    }

    /* Disable spi device to exit from running operation before configuration */
    IO_BIT_CLEAR(pSPI->CTRL, SPI_CTRL_EN);

    hSPI->status = SPI_STATUS_IDLE;
    hSPI->txExpectedSize = hSPI->txSize = hSPI->tcount = 0;
    hSPI->rxExpectedSize = hSPI->rxSize = hSPI->rcount = 0;
    hSPI->rxDMAStatus = hSPI->txDMAStatus = SPI_DMA_STATUS_STARTED;

    hSPI->enableTxDMA = pConfig->enableTxDMA;
    hSPI->enableRxDMA = pConfig->enableRxDMA;

    /* Initialize some local variables about spi configurations */
    ctrl = 0;
    hSPI->frameDelay = 0;
    hSPI->fixedFramesSize = 0;
    frameMask = pConfig->frameConfig.mask;
    duplex = pConfig->duplex;

    /* SPI duplex mode */
    ctrl |= ((duplex == SPI_FULL_DUPLEX) ? SPI_CTRL_TMODE_FULL_DUPLEX :
             ((duplex == SPI_HALF_DUPLEX) ? SPI_CTRL_TMODE_HALF_DUPLEX :
              ((duplex == SPI_TX_ONLY) ? SPI_CTRL_TMODE_TX_ONLY :
               ((duplex == SPI_RX_ONLY) ? SPI_CTRL_TMODE_RX_ONLY : 0))));
    /* Mask of fixed frames */
    if (frameMask & SPI_MASK_COMMAND_FRAME)
    {
        ctrl |= SPI_CTRL_XCMD;
        hSPI->fixedFrames[hSPI->fixedFramesSize++] = pConfig->frameConfig.command;
        hSPI->frameDelay++;
    }
    if (frameMask & SPI_MASK_ADDRESS_FRAME)
    {
        ctrl |= SPI_CTRL_XADDR;
        hSPI->fixedFrames[hSPI->fixedFramesSize++] = pConfig->frameConfig.address;
        hSPI->frameDelay++;
    }
    if (frameMask & SPI_MASK_DUMMY_FRAME)
    {
        ctrl |= SPI_CTRL_XDMY;
        hSPI->fixedFrames[hSPI->fixedFramesSize++] = pConfig->frameConfig.dummy;
#if defined(CHIP_GM6621)||defined(CHIP_F6601C)
#else
        pSPI->DMY = pConfig->frameConfig.dummy;
#endif
        hSPI->frameDelay++;
    }
    if (frameMask & SPI_MASK_DATA_FRAME)
    {
        ctrl |= SPI_CTRL_XDATA;
    }

    /* Get data frames bytes from bits of data frame */
    hSPI->dataWidth = ((pConfig->frameConfig.dataBits <= 8) ? 1 :
                       ((pConfig->frameConfig.dataBits <= 16) ? 2 : 4));

    /* Setup fields of spi control register */
    IO_BITMASK_SET(pSPI->CTRL, SPI_CTRL_TMODE | SPI_CTRL_XFRM | SPI_CTRL_MFRM, ctrl);

    /* Sample timing: frame delay */
    IO_BITMASK_SET(pSPI->STM, SPI_STM_FRM, SPI_STM_FRM_FIELD(hSPI->frameDelay));

    /* Frame bits */
    IO_BITMASK_SET(pSPI->FRM, SPI_FRM_CMD | SPI_FRM_ADDR | SPI_FRM_DUMMY | SPI_FRM_DATA,
                   SPI_FRM_CMD_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XCMD, pConfig->frameConfig.commandBits))  |
                   SPI_FRM_ADDR_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XADDR, pConfig->frameConfig.addressBits)) |
                   SPI_FRM_DUMMY_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XDMY, pConfig->frameConfig.dummyBits))  |
                   SPI_FRM_DATA_FIELD(SPI_GetFieldFromBits(SPI_CTRL_XDATA, pConfig->frameConfig.dataBits)));

    /* FIFO threshhold */
    hSPI->rxfifoTh = pConfig->rxWatermark;
    IO_BITMASK_SET(pSPI->FFCFG, SPI_FFCFG_RFTH, SPI_FFCFG_RFTH_FIELD(pConfig->rxWatermark));
    IO_BITMASK_SET(pSPI->FFCFG, SPI_FFCFG_TFTH, SPI_FFCFG_TFTH_FIELD(pConfig->txWatermark));

    /* DMA disable, avoid last time dma enable but this transfer DMA disable, DMA enable clear is in DMA callback
    DMA enable is in SPI DMA config. when enable tx dma and rx dma, but there is only tx dma callback, rx dma enable
    can't be cleared*/
    if (!pConfig->enableRxDMA)
    {
        IO_BIT_CLEAR(pSPI->RDMA, SPI_RDMA_EN);
    }

    if (!pConfig->enableTxDMA)
    {
        IO_BIT_CLEAR(pSPI->TDMA, SPI_TDMA_EN);
    }
    return RET_OK;
}

/**
 * @brief Process all transferred data.
 *
 * @param hSPI      SPI device handle
 * @param waitTimer Pointer to timer.
 * @retval          Return RET_BUSY if transfter is ongoing.
 */
static RET_CODE SPI_ProcessData(SPI_Handle_T *hSPI)
{
    SPI_Device_T *pSPI = hSPI->device;
    uint8_t  index, fifoSize, size, dataWidth = hSPI->dataWidth;

    /* First stage */
    /* Check dma over flag or transmit/receive data if one transfer is on progress */
    if (hSPI->status == SPI_STATUS_BUSY)
    {
        /* Wait for dma over status if dma enabled */
        if (hSPI->enableTxDMA)
        {
            if (hSPI->txDMAStatus == SPI_DMA_STATUS_FINISHED)
            {
                hSPI->txSize += hSPI->tcount;
                hSPI->tcount = 0;
            }
        }
        else
        {
            if (!hSPI->enableTxDMA)
            {
                /* Send data to data port if dma not enabled */
                while (hSPI->tcount != 0)
                {
                    if (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL))
                    {
                        break;
                    }
                    DataReadByWidth(hSPI->pTxData, &pSPI->DR, dataWidth);
                    hSPI->txSize ++;
                    hSPI->tcount --;
                    hSPI->pTxData += dataWidth;
                }
            }
        }

        if (hSPI->enableRxDMA)
        {
            if (hSPI->rxDMAStatus == SPI_DMA_STATUS_FINISHED)
            {
                hSPI->rxSize += hSPI->rcount;
                hSPI->rcount = 0;
            }
        }
        else
        {
            /* Get data from data rx fifo */
            /* cpu code-execute is much faster than SPI data-transfer
             * thus, you may be get a smaller value than expected;
             */
            fifoSize = SPI_FFLV_RF_GET(pSPI->FFLV);
            while (hSPI->rcount != 0)
            {
                if (fifoSize == 0)
                {
                    break;
                }
                DataWriteByWidth(hSPI->pRxData, pSPI->DR, dataWidth);
                hSPI->rxSize ++;
                hSPI->rcount --;
                hSPI->pRxData += dataWidth;
                fifoSize--;
            }
        }

        /* One transfer is over */
        if (hSPI->tcount == 0 && hSPI->rcount == 0)
        {
            /* Should I wait for fifo empty? */
            if (IO_BIT_GET(pSPI->CTRL, SPI_CTRL_MS))
            {
                while (!IO_BIT_GET(pSPI->SR, SPI_SR_TFET));
            }
            /* Disable all interrupt after one transfer is over */
            IO_BIT_CLEAR(pSPI->CI, SPI_CI_EN);
            /* Total transfer is over if next transfer size == 0 */
            if ((hSPI->txExpectedSize == hSPI->txSize) && (hSPI->rxExpectedSize == hSPI->rxSize))
            {
                hSPI->status = SPI_STATUS_IDLE;
                return RET_OK;
            }
        }
    }

    /* Second stage is only valid when the status is idle or last transfer is over */
    if ((hSPI->status == SPI_STATUS_IDLE) ||
            (hSPI->tcount + hSPI->rcount) == 0)
    {
        /* Calculate next transfer size */
        hSPI->tcount = (hSPI->txExpectedSize - hSPI->txSize);
        hSPI->rcount = (hSPI->rxExpectedSize - hSPI->rxSize);

        /* when there is no tx, rx data and fixed frame, return */
        if (((hSPI->tcount + hSPI->rcount) == 0) && (hSPI->fixedFramesSize == 0))
        {
            return RET_OK;
        }
        /* Transfer on progress */
        hSPI->status = SPI_STATUS_BUSY;

        /* DMA disable for transmit fix frames */
        IO_BIT_CLEAR(pSPI->TDMA, SPI_TDMA_EN);

        /* Push fixed frames into fifo */
        if (!hSPI->fixedFramesSend)
        {
            size = hSPI->fixedFramesSize;
            index = 0;
            while (size != 0)
            {
                /* Need to wait for tx fifo not full when there is 3 fixed frames because tx fifo depth is only 2 */
                while (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL));
                pSPI->DR = hSPI->fixedFrames[index++];
                //              hSPI->fixedFramesSize--;
                size--;
            }
        }
        /* Flag to set fixed frames send status, avoid send fixed frames twice when there is no tx and rx data frame but only fixed frames */
        hSPI->fixedFramesSend = 1;
        /* Recover DMA enable */
        if (hSPI->enableTxDMA)
        {
            IO_BIT_SET(pSPI->TDMA, SPI_TDMA_EN);
        }

        /* Star DMA operation */
        if (hSPI->enableTxDMA && hSPI->tcount != 0)
        {
            hSPI->txDMAStatus = SPI_DMA_STATUS_STARTED;
            /* need to wait for tx fifo empty, a request has been generated before DMA start. once start, DMA will write data to
            tx fifo, and this byte will be lost if tx fifo is full*/
            while (!IO_BIT_GET(pSPI->SR, SPI_SR_TFET));
            DMA_Start(&hSPI->txDMAHandle, (((uint32_t)hSPI->pTxData) + hSPI->txSize), (uint32_t)(&hSPI->device->DR), hSPI->tcount);
        }
        if (hSPI->enableRxDMA && hSPI->rcount != 0)
        {
            hSPI->rxDMAStatus = SPI_DMA_STATUS_STARTED;
            DMA_Start(&hSPI->rxDMAHandle, (uint32_t)(&hSPI->device->DR), (((uint32_t)hSPI->pRxData) + hSPI->rxSize), hSPI->rcount);
        }

        if (hSPI->fixedFramesSize == 0 && hSPI->tcount == 0)
        {
            //pSPI->DR = 0;
        }
        else
        {
            /* Trigger master spi by pushing data into tx fifo */
            if (!hSPI->enableTxDMA)
            {
                /* Send data to data port if dma not enabled */
                while (hSPI->tcount != 0)
                {
                    if (!IO_BIT_GET(pSPI->SR, SPI_SR_TFNFL))
                    {
                        break;
                    }
                    DataReadByWidth(hSPI->pTxData, &pSPI->DR, dataWidth);
                    hSPI->txSize ++;
                    hSPI->tcount --;
                    hSPI->pTxData += dataWidth;
                }
            }
        }
        IO_BIT_SET(pSPI->CI, SPI_CI_EN);
    }

    /* Set receive fifo threshold when receive last few data to ensure enter interrupt, and recover the setting threshold in last interrrupt */
    if ((hSPI->rxExpectedSize - hSPI->rxSize) < hSPI->rxfifoTh)
    {
        IO_BITMASK_SET(pSPI->FFCFG, SPI_FFCFG_RFTH, SPI_FFCFG_RFTH_FIELD(hSPI->rxExpectedSize - hSPI->rxSize));
    }

    return RET_BUSY;
}

RET_CODE SPI_Receive(SPI_Handle_T *hSPI, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    return SPI_TransmitReceive(hSPI, NULL, pData, 0, size, timeout);
}

RET_CODE SPI_Transmit(SPI_Handle_T *hSPI, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    return SPI_TransmitReceive(hSPI, pData, NULL, size, 0, timeout);
}

static void SPI_InterruptHandler(SPI_Handle_T *hSPI)
{
    SPI_Device_T *pSPI = hSPI->device;
    uint32_t status;
    RET_CODE ret;
    status = pSPI->IS;
    /* Clear spi interrupt */
    pSPI->IS = status;

    /* Process data transfer in context of interrupt handler */
    ret = SPI_ProcessData(hSPI);

    /* Notify user by callback */
    if (ret == RET_OK)
    {
        hSPI->status = SPI_STATUS_IDLE;
        /* Set receive fifo threshold to spi config setting */
        IO_BITMASK_SET(pSPI->FFCFG, SPI_FFCFG_RFTH, SPI_FFCFG_RFTH_FIELD(hSPI->rxfifoTh));
        if (hSPI->callback != NULL)
        {
            hSPI->callback(hSPI, SPI_EVENT_TRANSFER_COMPLETE, NULL);
        }
    }
    else if (ret == RET_ERROR)
    {
        hSPI->status = SPI_STATUS_ERROR;
        if (hSPI->callback != NULL)
        {
            hSPI->callback(hSPI, SPI_EVENT_TRANSFER_ERROR, NULL);
        }
    }
    return;
}

RET_CODE SPI_TransmitReceive(SPI_Handle_T *hSPI, uint8_t *pTxData, uint8_t *pRxData, uint16_t txSize, uint16_t rxSize, uint32_t timeout)
{
    SPI_Device_T *pSPI = hSPI->device;
    WaitTimer_T waitTimer;
    RET_CODE ret;
    uint8_t dataWidth = hSPI->dataWidth;

    /* Clear FIFO Dirty Data */
    SPI_Flush_FIFO(hSPI);

    /* Return immediately if SPI is on progress */
    if (hSPI->status == SPI_STATUS_BUSY)
    {
        return RET_BUSY;
    }

    /* Set continue data in each transfer */
    IO_BIT_SET(pSPI->CTRL, SPI_CTRL_MFRM);
    pSPI->CCNT = (txSize > rxSize) ? txSize : rxSize;

    /* Process null pointers and zero transfer size */
    if (pTxData == NULL)
    {
        txSize = 0;
    }
    if (pRxData == NULL)
    {
        rxSize = 0;
    }
    if (txSize == 0 && rxSize == 0 && hSPI->fixedFramesSize == 0)
    {
        return RET_ERROR;
    }

    /* Initialize variables */
    hSPI->txSize = hSPI->rxSize = 0;
    /* Address should be aligned to boundary by data width (byte/half word/word) */
    if (((uint32_t)pRxData) & ((uint32_t)(dataWidth - 1)))
    {
        return RET_ERROR;
    };
    if (((uint32_t)pTxData) & ((uint32_t)(dataWidth - 1)))
    {
        return RET_ERROR;
    };
    /* Size should be aligned to boundary by data width (byte/half word/word) */
    hSPI->txExpectedSize = txSize;
    hSPI->rxExpectedSize = rxSize;
    hSPI->pRxData = pRxData;
    hSPI->pTxData = pTxData;
    hSPI->fixedFramesSend = 0;//fixed frames have not been send

    /* Enable spi device */
    if (!IO_BIT_GET(pSPI->CTRL, SPI_CTRL_EN))
    {
        IO_BIT_SET(pSPI->CTRL, SPI_CTRL_EN);
    }

    /* Configure DMA parameters */
    if (hSPI->enableTxDMA && txSize != 0)
    {
        SPI_DMAConfig(pSPI, dataWidth, FALSE, &hSPI->txDMAHandle, SPI_TxDMACallback);
        hSPI->txDMAStatus = SPI_DMA_STATUS_STARTED;
    }
    if (hSPI->enableRxDMA && rxSize != 0)
    {
        SPI_DMAConfig(pSPI, dataWidth, TRUE, &hSPI->rxDMAHandle, SPI_RxDMACallback);
        hSPI->rxDMAStatus = SPI_DMA_STATUS_STARTED;
    }

    /* Initialize timeout structure */
    if (timeout != TIMEOUT_NO_BLOCKING)
    {
        WaitTimerInit(&waitTimer, timeout, 1);
    }

    do
    {
        ret = SPI_ProcessData(hSPI);

        /* Exit if it returns RET_OK. */
        if (ret != RET_BUSY)
        {
            break;
        }
        /* Exit if it is non-blocking call */
        if (timeout == TIMEOUT_NO_BLOCKING)
        {
            ret = RET_ONGOING;
            break;
        }

        /* Wait and do nothing if TX  or Rx interrupt handling is enabled. */
        if (hSPI->interruptMask & (SPI_RX_INT | SPI_TX_INT))
        {
            while (1)
            {
                /* Exit if all expected user data has been put into user buffer. */
                if ((hSPI->txExpectedSize == hSPI->txSize) && (hSPI->rxExpectedSize == hSPI->rxSize))
                {
                    //while (IO_BIT_GET(pSPI->SR, SPI_SR_BUSY));
                    ret = RET_OK;
                    break;
                }

                /* Wait for a while if it is called in blocking function */
                if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
                {
                    ret = RET_TIMEOUT;
                    break;
                };
            };
            break;
        }
        else
        {
            /* Wait for a while if it is called in blocking function */
            if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
            {
                ret = RET_TIMEOUT;
                break;
            };
        }

    } while (1);
    if (ret == RET_TIMEOUT)
    {
        /* DMA need to deinit, it was configured but didn't work before timeout
        if don't deinit, the next transfer will request two dma transfer, and the data will be 0x00*/
        hSPI->txDMAHandle.state = DMA_STATE_INITIAL;
        DMA_DeInit(&hSPI->txDMAHandle);
        hSPI->rxDMAHandle.state = DMA_STATE_INITIAL;
        DMA_DeInit(&hSPI->rxDMAHandle);
        SPI_Flush_FIFO(hSPI);
    }
    /* wait until spi is not busy, because the waveform may not fully produced */
    while (!IO_BIT_GET(pSPI->SR, SPI_SR_TFET));
    //while (IO_BIT_GET(pSPI->SR, SPI_SR_BUSY));
    return ret;
}

RET_CODE SPI_EnableInterrupts(SPI_Handle_T *hSPI, uint32_t mask)
{
    SPI_Device_T *pSPI = hSPI->device;
    if (mask & SPI_TX_INT)
    {
        IO_BIT_SET(pSPI->IE, (SPI_IE_TFOV | SPI_IE_TFTH));
    }
    if (mask & SPI_RX_INT)
    {
        IO_BIT_SET(pSPI->IE, (SPI_IE_RFUF | SPI_IE_RFOV | SPI_IE_RFTH));
    }
    hSPI->interruptMask |= mask;
    return RET_OK;
}

RET_CODE SPI_DisableInterrupts(SPI_Handle_T *hSPI, uint32_t mask)
{
    SPI_Device_T *pSPI = hSPI->device;
    if (mask & SPI_TX_INT)
    {
        IO_BIT_CLEAR(pSPI->IE, (SPI_IE_TFOV | SPI_IE_TFTH));
    }
    if (mask & SPI_RX_INT)
    {
        IO_BIT_CLEAR(pSPI->IE, (SPI_IE_RFOV | SPI_IE_RFTH | SPI_IE_RFUF));
    }
    hSPI->interruptMask &= ~mask;
    return RET_OK;
}


#ifdef SPI0
void SPI0_InterruptHandler(void)
{
    s_spiIsr(s_spiHandle[0]);
}
#endif

#ifdef SPI1
void SPI1_InterruptHandler(void)
{
    s_spiIsr(s_spiHandle[1]);
}
#endif
