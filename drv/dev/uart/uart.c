/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart.c
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
 */
#include <drv/dev/uart.h>


#ifdef CHIP_F6621B
    STATIC_ISR(USART_IRQn, UART_InterruptEntrance);
#else
    #ifdef UART0
        STATIC_ISR(UART0_IRQn, UART0_InterruptHandler);
    #endif
    #ifdef UART1
        STATIC_ISR(UART1_IRQn, UART1_InterruptHandler);
    #endif
    #ifdef UART2
        STATIC_ISR(UART2_IRQn, UART2_InterruptHandler);
    #endif
#endif

#define UART_DMA_STATUS_FINISHED FALSE  /**< No uart dma transaction is ongoing */
#define UART_DMA_STATUS_STARTED  TRUE   /**< One uart dma transaction is ongoing */

#define UART_STATUS_IDLE     0   /**< No uart transaction is ongoing */
#define UART_STATUS_RX_BUSY  1   /**< Uart reception is ongoing */
#define UART_STATUS_TX_BUSY  2   /**< Uart transmission is ongoing */

#ifdef CHIP_F6621B
    #define UART_MUTEX_ENTER(hUART) do { NVIC_DisableIRQ(USART_IRQn); } while (0)
#else
    #define UART_MUTEX_ENTER(hUART) do { NVIC_DisableIRQ(s_uartIrq[hUART->index]); } while (0)
#endif

#ifdef CHIP_F6621B
    #define UART_MUTEX_LEAVE(hUART) do { NVIC_EnableIRQ(USART_IRQn); } while (0)
#else
    #define UART_MUTEX_LEAVE(hUART) do { NVIC_EnableIRQ(s_uartIrq[hUART->index]); } while (0)
#endif

static RET_CODE UART_TxProcessData(UART_Handle_T *hUART);
static RET_CODE UART_RxProcessData(UART_Handle_T *hUART);
static void UART_InterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback);

static IRQn_Type s_uartIrq[MAX_UART_DEVICE] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};
static UART_Handle_T *s_uartHandle[MAX_UART_DEVICE] = {NULL};
static void (*s_uartIsr)(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback) = NULL;
static UART_Callback_T s_uartCallback[MAX_UART_DEVICE] = {NULL, NULL, NULL};

RET_CODE UART_Init(UART_Device_T *pUART, UART_Config_T *pConfig, UART_Handle_T *hUART)
{
    uint32_t div, bps = pConfig->baudRate;
    UART_Parity_T parity = pConfig->parity;
    uint8_t index = MAX_UART_DEVICE;

    /* Sanity check */
    /* Words used likewise: Reality check, Smoke test(Magic smoke ) */
    ASSERT((pUART == UART0) || (pUART == UART1) || (pUART == UART2));

    /* Open clock gate for selected uart */

    /* Disable interrupt */
    pUART->IE = 0;

    /* Set data bits */
    //ASSERT((pConfig->wordLength >= UART_DATABITS_5) && (pConfig->wordLength <= UART_DATABITS_8));
    IO_BITMASK_SET(pUART->C3, UART_C3_DTB, UART_C3_DTB_FIELD(pConfig->wordLength));

    /* Set stop bits */
#if defined(CHIP_F6621B)
    //   set TX_STOP_LEN with stop bit
    //   00 - 1 stopbit
    //   01 - 0.5 stopbit
    //   10 - 2 stopbit
    //   11 - 1.5 stopbit

    uint8_t val = 0;
    if (pConfig->stopBits == UART_STOPBITS_1)
    {
        val = 0;
    }
    else if (pConfig->stopBits == UART_STOPBITS_0_5)
    {
        val = 1;
    }
    else if (pConfig->stopBits == UART_STOPBITS_2)
    {
        val = 2;
    }
    else if (pConfig->stopBits == UART_STOPBITS_1_5)
    {
        val = 3;
    }
    IO_BITMASK_SET(pUART->C2, UART_C2_TXSLEN, UART_C2_TXSLEN_FIELD(val));

#else

    if (pConfig->stopBits == UART_STOPBITS_1)
    {
        IO_BIT_CLEAR(pUART->C3, UART_C3_STB);
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))
        IO_BITMASK_SET(pUART->C2, ((unsigned int)0xFF << 24), ((unsigned int)0x0F << 24));
#endif
    }
    else
    {
        if ((pConfig->wordLength == UART_DATABITS_5 && pConfig->stopBits == UART_STOPBITS_1_5) ||
                (pConfig->wordLength > UART_DATABITS_5 && pConfig->stopBits == UART_STOPBITS_2))
        {
            IO_BIT_SET(pUART->C3, UART_C3_STB);
        }
        else
        {
            return RET_INVPARAM;
        }
    }
#endif

#if (defined(CHIP_GM6621))

    //IO_BIT_SET(SYSDEV->CLK1, 0x2); //sync mode need 16M clock

    if (pUART == UART0)
    {
        IO_BITMASK_SET(SYSDEV->CLK1, (0x1F << 0), (0x00 << 0));
    }
    else if (pUART == UART1)
    {
        IO_BITMASK_SET(SYSDEV->CLK1, (0x1F << 5), (0x00 << 5));
    }
    else if (pUART == UART2)
    {
        IO_BITMASK_SET(SYSDEV->CLK1, (0x1F << 10), (0x00 << 10));
    }

#endif

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    if (pUART == UART0)
    {
        IO_BIT_SET(SYSDEV->MCS, SYS_MCS_UART0_CLKSRC);
    }
    else if (pUART == UART1)
    {
        IO_BIT_SET(SYSDEV->MCS, SYS_MCS_UART1_CLKSRC);
    }
    else if (pUART == UART2)
    {
        IO_BIT_SET(SYSDEV->MCS, SYS_MCS_UART2_CLKSRC);
    }
#endif

    div = SYS_CLOCK / (bps << 4); // SYS_CLOCK=48M, bps=115200 ==> div=26
    IO_BITMASK_SET(pUART->TC, UART_TC_SCLK, UART_TC_SCLK_FIELD(div));   // 48M / 26 / 16 = 115200

    /* Set parity format */
    if (UART_PARITY_NONE == parity)
    {
        IO_BIT_CLEAR(pUART->C3, UART_C3_PEN);
    }
    else
    {
        IO_BIT_SET(pUART->C3, UART_C3_PEN);
        if (UART_PARITY_EVEN == parity)
        {
            IO_BIT_SET(pUART->C3, UART_C3_EOP);
        }
        else if (UART_PARITY_ODD == parity)
        {
            IO_BIT_CLEAR(pUART->C3, UART_C3_EOP);
        }
    }

    /* Disable Rx/tx fifo */
    IO_BIT_CLEAR(pUART->C2, UART_C2_FFEN);

    pUART->LS  = 0;    /* Reset line status */

    //pUART->FCR |= (UART_FCR_FEN | UART_FCR_RFRST | UART_FCR_XFRST);  /* Reset TX and RX FIFO, FIFO threshold 4 bytes */

    IO_BITS_SET(pUART->C3, UART_C3_RTS | UART_C3_DTR); /* Make DTR(DTE ready) and RTS(DTE request DCE to send ) valid, it is modem control signal */

    /* Enable NVIC interrupt */
    if (pUART == UART0)
    {
        index = 0;
    }
    else if (pUART == UART1)
    {
        index = 1;
    }
    else if (pUART == UART2)
    {
        index = 2;
    }

#ifdef CHIP_F6621B
    NVIC_ClearPendingIRQ(USART_IRQn);
#else
    NVIC_ClearPendingIRQ(s_uartIrq[index]);
#endif

    if (index < MAX_UART_DEVICE)
    {
        /* Record callback if no handler is used. */
        s_uartCallback[index] = pConfig->callback;
        /* Handle of spi device */
        s_uartHandle[index] = hUART;
        /* uart interrupt handler */
        s_uartIsr = UART_InterruptHandler;
        /* Enable NVIC interrupt for this UART device */
#ifdef CHIP_F6621B
        NVIC_EnableIRQ(USART_IRQn);
#else
        NVIC_EnableIRQ(s_uartIrq[index]);
#endif
        /* Write device base address into handle structure */
        if (hUART != NULL)
        {
            hUART->device = pUART;
            hUART->enableFifo = FALSE;
            hUART->index = index;
            hUART->intmask = 0;
            hUART->callback = pConfig->callback;
            hUART->status = UART_STATUS_IDLE;
            hUART->txExpectedSize = hUART->txSize = 0;
            hUART->rxExpectedSize = hUART->rxSize = 0;
            hUART->rcount = hUART->tcount = 0;
            hUART->lineSavedStatus = 0;
            hUART->rxWatermark = 0;
            hUART->txWatermark = 0;
            hUART->enableRxDMA = FALSE;
            hUART->enableTxDMA = FALSE;
        }
    }

    return RET_OK;
}

void UART_ClearRx(UART_Handle_T *hUART)
{
    UART_Device_T *pUART = hUART->device;
    volatile uint8_t rxChar;
    int32_t rcount = 0;

    if (hUART->enableFifo == FALSE)
    {
        if (IO_BIT_GET(pUART->LS, UART_LS_DR))
        {
            rcount = 1;
        }
    }
    else
    {
        rcount = UART_TH_RFLV_GET(pUART->TH); 
    }
	
    while (rcount-- > 0)
    {
        /* Clear rx data not fetched  */
        rxChar = (uint8_t)(pUART->DR);
    }	
}

void UART_DeInit(UART_Handle_T *hUART)
{	
	if((hUART != NULL) && (hUART->device != NULL))
	{
		/* Disable FIFO */
    	IO_BIT_CLEAR(hUART->device->C2, UART_C2_FFEN);
    	/* Disable all interrupt */
    	UART_DisableInterrupts(hUART, (UART_RX_INT | UART_TX_INT | UART_RX_FIFO_INT |
                                   UART_TX_FIFO_INT | UART_RX_DMA_INT | UART_TX_DMA_INT));
    	/* Disable UART NVIC interrupt */
#ifdef CHIP_F6621B
    NVIC_DisableIRQ(USART_IRQn);
#else
    NVIC_DisableIRQ(s_uartIrq[hUART->index]);
#endif
    	/* Reset uart IP */
	    hUART->device = NULL;
	    hUART->pTxData = NULL;
	    hUART->pRxData = NULL;
	    hUART = NULL;
	}
}

void UART_SetMode(UART_Handle_T *hUART, UART_Mode_T mode)
{
    volatile uint32_t lineStatus, rxChar;
    UART_Device_T *pUART = hUART->device;

    if (UART_MODE_RX == mode)
    {
        UART_ClearRx(hUART);
        IO_BIT_SET(pUART->C2, UART_C2_TXDIS);
        IO_BIT_CLEAR(pUART->C2, UART_C2_RXDIS);
    }
    else if (UART_MODE_TX == mode)
    {
        IO_BIT_SET(pUART->C2, UART_C2_RXDIS);
        IO_BIT_CLEAR(pUART->C2, UART_C2_TXDIS);
    }
    else if (UART_MODE_DISABLE == mode)
    {
        IO_BITS_SET(pUART->C2, (UART_C2_TXDIS | UART_C2_RXDIS));
    }
    else if (UART_MODE_DUPLEX == mode)
    {
        UART_ClearRx(hUART);
        IO_BITS_CLEAR(pUART->C2, (UART_C2_TXDIS | UART_C2_RXDIS));
    }

    return;
}

void UART_PutChar(UART_Device_T *pUART, uint8_t ch)
{
    /* Wait until XMIT FIFO empty or Transmitter Holding Register empty */
    while (!(pUART->LS & UART_LS_THRE));

    /* Send out this character */
    pUART->DR = ch;
}

uint16_t UART_GetChar(UART_Device_T *pUART)
{
    return ((uint16_t)(pUART->DR));
}


static void UART_RxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    RET_CODE ret;
    UART_Handle_T *hUART =  CONTAINER_OF(hDMA, UART_Handle_T, RxDMAHandle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        hUART->rxDMAStatus = UART_DMA_STATUS_FINISHED;
        /* User callback is always called here if rx dma is enabled. */
        if ((hUART->enableRxDMA == TRUE) &&
                (hUART->intmask & UART_RX_DMA_INT))
        {
            ret = UART_RxProcessData(hUART);
            if (ret == RET_OK)
            {
                if (NULL != hUART->callback)
                {
                    hUART->callback(hUART, UART_EVENT_RX_OVER, (ulong_t)0);
                }
            }
        }
#ifdef CHIP_F6621B
        DMA_Abort(hDMA);
#else
        DMA_DeInit(hDMA);
#endif
    }
}

static void UART_TxDMACallback(DMA_Handle_T *hDMA, uint8_t event, ulong_t param)
{
    RET_CODE ret;
    UART_Handle_T *hUART =  CONTAINER_OF(hDMA, UART_Handle_T, TxDMAHandle);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        hUART->txDMAStatus = UART_DMA_STATUS_FINISHED;
        /* User callback is always called here if tx dma is enabled. */
        if ((hUART->enableTxDMA == TRUE) &&
                (hUART->intmask & UART_TX_DMA_INT))
        {
            ret = UART_TxProcessData(hUART);
            if (ret == RET_OK)
            {
                if (NULL != hUART->callback)
                {
                    hUART->callback(hUART, UART_EVENT_TX_OVER, (ulong_t)0);
                }
            }
        }
#ifdef CHIP_F6621B
        DMA_Abort(hDMA);
#else
        DMA_DeInit(hDMA);
#endif
    }
}


#ifdef CHIP_F6621B
static void UART_DMAConfig(UART_Device_T *pUART, uint8_t dmaRead, DMA_Handle_T *pDMAHandle)
{
    DMA_Config_T dmaConfig;
    DMA_Request_Result_T reqResult;
    RET_CODE ret = RET_ERROR;

    if (dmaRead)
    {
        if (pUART == UART0)
        {
            ret = DMA_Request_Channel(DMA_DEV, DMA_REQUEST_USART1_RX, &reqResult);
        }
        else if (pUART == UART1)
        {
            ret = DMA_Request_Channel(DMA_DEV, DMA_REQUEST_USART2_RX, &reqResult);
        }
        else if (pUART == UART2)
        {
            ret = DMA_Request_Channel(DMA_DEV, DMA_REQUEST_USART3_RX, &reqResult);
        }

        if (ret != RET_OK)
        {
            return;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_PERIPH_TO_MEMORY;
        dmaConfig.increment = DMA_SRC_KEEP_DST_INC;
        dmaConfig.width = DMA_WIDTH_8BIT;
        dmaConfig.burst = 0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
        dmaConfig.callback = UART_RxDMACallback;

        //        LL_UART_RX_DMA_RDL(pUART, 0);
        //        LL_UART_RX_DMA_ENABLE(pUART);
        //        LL_UART_TX_DMA_DISABLE(pUART);
        IO_BITMASK_SET(pUART->DMA, ((uint32_t)((uint32_t)0x1F << ((uint32_t) 8))), ((MAX_RX_FIFO_DEPTH / 2)  << ((uint32_t) 8)));
        IO_BIT_SET(pUART->DMA, ((uint32_t)((uint32_t)0x1 << ((uint32_t) 0))));
        IO_BIT_CLEAR(pUART->DMA, ((uint32_t)((uint32_t)0x1 << ((uint32_t) 16))));
    }
    else
    {
        if (pUART == UART0)
        {
            ret = DMA_Request_Channel(DMA_DEV, DMA_REQUEST_USART1_TX, &reqResult);
        }
        else if (pUART == UART1)
        {
            ret = DMA_Request_Channel(DMA_DEV, DMA_REQUEST_USART2_TX, &reqResult);
        }
        else if (pUART == UART2)
        {
            ret = DMA_Request_Channel(DMA_DEV, DMA_REQUEST_USART3_TX, &reqResult);
        }

        if (ret != RET_OK)
        {
            //fail, need return fail status
            return;
        }

        memset(&dmaConfig, 0, sizeof(dmaConfig));
        dmaConfig.channelIdx = reqResult.channelIdx;
        dmaConfig.deviceIdx = reqResult.deviceIdx;
        dmaConfig.direction = DMA_MEMORY_TO_PERIPH;
        dmaConfig.increment = DMA_SRC_INC_DST_KEEP;
        dmaConfig.width = DMA_WIDTH_8BIT;
        dmaConfig.burst = 0;
        dmaConfig.priority = DMA_PRIORITY_HIGH;
        dmaConfig.alter = DMA_PRIMARY;
        dmaConfig.callback = UART_TxDMACallback;

        //       LL_UART_TX_DMA_TDL(pUART, MAX_UART_TX_FIFO_SIZE/2);
        //       LL_UART_TX_DMA_ENABLE(pUART);
        //       LL_UART_RX_DMA_DISABLE(pUART);
        IO_BITMASK_SET(pUART->DMA, (uint32_t)((uint32_t)0x1F << ((uint32_t) 24)), ((MAX_TX_FIFO_DEPTH / 2) << ((uint32_t) 24)));
        IO_BIT_SET(pUART->DMA, ((uint32_t)((uint32_t)0x1 << (uint32_t) 16)));
        IO_BIT_CLEAR(pUART->DMA, ((uint32_t)((uint32_t)0x1 << (uint32_t) 0)));
    }

    DMA_Init(DMA_DEV, &dmaConfig, pDMAHandle);
}
#else
static RET_CODE UART_DMAConfig(UART_Device_T *pUART, uint8_t dmaRead, DMA_Handle_T *pDMAHandle, uint32_t index)
{
    DMA_Config_T dmaConfig;
    DMA_Config_T *pDMAConfig = &dmaConfig;
    memset(&dmaConfig, 0, sizeof(DMA_Config_T));
    if (dmaRead)
    {
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
        pDMAConfig->srcHwHSChannel = ((pUART == UART0) ? DMA_UART0_RX_CHANNEL :
                                      ((pUART == UART1) ? DMA_UART1_RX_CHANNEL :
                                       ((pUART == UART2) ? DMA_UART2_RX_CHANNEL : DMA_INVALID_CHANNEL)));
#elif (defined(CHIP_GM6621))
        pDMAConfig->requestType = ((pUART == UART0) ? DMA_REQUEST_UART0_RX :
                                   ((pUART == UART1) ? DMA_REQUEST_UART1_RX :
                                    ((pUART == UART2) ? DMA_REQUEST_UART2_RX : DMA_REQUEST_INVALID)));
#endif
        pDMAConfig->srcIncrement = DMA_ADDR_CONSTANT;
        pDMAConfig->destIncrement = DMA_ADDR_INCREMENT;
        pDMAConfig->srcHandshake = DMA_HARDWARE_HANDSHAKE;
        pDMAConfig->destHandshake = DMA_SOFTWARE_HANDSHAKE;
        pDMAConfig->direction = DMA_PERIPH_TO_MEMORY;
        pDMAConfig->callback = UART_RxDMACallback;
        pDMAConfig->srcWidth = DMA_WIDTH_8BIT;
        pDMAConfig->destWidth = DMA_WIDTH_8BIT;
        IO_BITMASK_SET(pUART->DMA, UART_DMA_RTH, UART_DMA_RTH_FIELD(0));
        IO_BIT_SET(pUART->DMA, UART_DMA_REN);
        IO_BIT_CLEAR(pUART->DMA, UART_DMA_TEN);
    }
    else
    {
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
        pDMAConfig->destHwHSChannel = ((pUART == UART0) ? DMA_UART0_TX_CHANNEL :
                                       ((pUART == UART1) ? DMA_UART1_TX_CHANNEL :
                                        ((pUART == UART2) ? DMA_UART2_TX_CHANNEL : DMA_INVALID_CHANNEL)));
#elif (defined(CHIP_GM6621))
        pDMAConfig->requestType = ((pUART == UART0) ? DMA_REQUEST_UART0_TX :
                                   ((pUART == UART1) ? DMA_REQUEST_UART1_TX :
                                    ((pUART == UART2) ? DMA_REQUEST_UART2_TX : DMA_REQUEST_INVALID)));
#endif
        pDMAConfig->srcIncrement = DMA_ADDR_INCREMENT;
        pDMAConfig->destIncrement = DMA_ADDR_CONSTANT;
        pDMAConfig->srcHandshake = DMA_SOFTWARE_HANDSHAKE;
        pDMAConfig->destHandshake = DMA_HARDWARE_HANDSHAKE;
        pDMAConfig->direction = DMA_MEMORY_TO_PERIPH;
        pDMAConfig->callback = UART_TxDMACallback;
        pDMAConfig->srcWidth = DMA_WIDTH_8BIT;
        pDMAConfig->destWidth = DMA_WIDTH_8BIT;
        IO_BITMASK_SET(pUART->DMA, UART_DMA_TTH, UART_DMA_TTH_FIELD(MAX_UART_TX_FIFO_SIZE / 2));
        IO_BIT_SET(pUART->DMA, UART_DMA_TEN);
        IO_BIT_CLEAR(pUART->DMA, UART_DMA_REN);
    }

#if (defined(CHIP_GM6621))
    dmaConfig.chnPriority = DMA_CHN_PRIORITY_0;
    dmaConfig.srcReload = DMA_NORMAL_MODE;
    dmaConfig.destReload = DMA_NORMAL_MODE;
    dmaConfig.srcGather = DMA_GATHER_DISABLE;
    dmaConfig.srcGatherInterval = 0;
    dmaConfig.srcGatherCount = 0;
    dmaConfig.destScatter = DMA_SCATTER_DISABLE;
    dmaConfig.destScatterInterval = 0;
    dmaConfig.destScatterCount = 0;
#endif

    DMA_Init(DMA0, pDMAConfig, pDMAHandle);

    return RET_OK;
}
#endif


/**
 * @brief Process all transmitted data.
 *
 * @param hUART     UART device handle
 * @param waitTimer Pointer to timer.
 * @retval          Return RET_BUSY if transfter is ongoing.
 */
static RET_CODE UART_TxProcessData(UART_Handle_T *hUART)
{
    UART_Device_T *pUART = hUART->device;
    /* First stage */
    /* Check dma over flag or transmit/receive data if one transfer is on progress */
    if (hUART->status & UART_STATUS_TX_BUSY)
    {
        /* Wait for dma over status if dma enabled */
        if (hUART->enableTxDMA)
        {
            if (hUART->txDMAStatus == UART_DMA_STATUS_FINISHED)
            {
                hUART->txSize += hUART->tcount;
                hUART->tcount = 0;
            }
        }
        else
        {
            while (hUART->tcount != 0)
            {
                if (hUART->enableFifo == FALSE)
                {
                    if (!IO_BIT_GET(pUART->LS, UART_LS_THRE))
                    {
                        break;
                    }
                }
                else if (UART_TH_TFLV_GET(pUART->TH) >= MAX_UART_TX_FIFO_SIZE)
                {
                    break;
                }
                pUART->DR = hUART->pTxData[hUART->txSize++];
                hUART->tcount--;
            }
        }

        /* One transfer is over */
        if (hUART->tcount == 0)
        {
            /* Total transfer is over if next transfer size == 0 */
            if (hUART->txExpectedSize == hUART->txSize)
            {
                hUART->status &= ~UART_STATUS_TX_BUSY;
                return RET_OK;
            }
        }
    }

    /* Second stage is only valid when the status is idle or last transfer is over */
    if (!(hUART->status & UART_STATUS_TX_BUSY) || hUART->tcount == 0)
    {
        /* Calculate next transfer size, it maybe divided into smaller units, but now only one transfer */
        hUART->tcount = (hUART->txExpectedSize - hUART->txSize);

        if (hUART->tcount == 0)
        {
            hUART->status &= ~UART_STATUS_TX_BUSY;
            return RET_OK;
        }

        /* Transfer on progress */
        hUART->status |= UART_STATUS_TX_BUSY;

        /* Star DMA operation */
        if (hUART->enableTxDMA)
        {
            hUART->txDMAStatus = UART_DMA_STATUS_STARTED;
            DMA_Start(&hUART->TxDMAHandle, (uint32_t)hUART->pTxData, (uint32_t) & (pUART->DR), hUART->tcount);
        }
        else
        {
            /* Send data to data port if dma not enabled */
            while (hUART->tcount != 0)
            {
                if (hUART->enableFifo == FALSE)
                {
                    if (!IO_BIT_GET(pUART->LS, UART_LS_THRE))
                    {
                        break;
                    }
                }
                else if (UART_TH_TFLV_GET(pUART->TH) >= MAX_UART_TX_FIFO_SIZE)
                {
                    break;
                }
                pUART->DR = hUART->pTxData[hUART->txSize++];
                hUART->tcount--;
            }
        }
        if (hUART->tcount == 0)
        {
            hUART->status &= ~UART_STATUS_TX_BUSY;
            return RET_OK;
        }
    }
    return RET_BUSY;
}

void UART_GetTransferStatus(UART_Handle_T *hUART, UART_TransferStatus_T *status)
{
    UART_Device_T *pUART = hUART->device;
    uint32_t lineStatus;

    status->receivedSize = 0;
    status->toBeTransmitedSize = 0;

#if 0
    uint32_t size;
#endif

    UART_MUTEX_ENTER(hUART);

    /* If interrupt enabled, just return saved line status,
            otherwize get status from uart register. */
    if (hUART->intmask & UART_RX_INT)
    {
        /* Get saved rx line status */
        lineStatus = hUART->lineSavedStatus;
        /* Get current line status */
        lineStatus |= ((pUART->LS) & (UART_LS_TIDL | UART_LS_RXE | UART_LS_BRK));
        hUART->lineSavedStatus = 0;
    }
    else
    {
        lineStatus = pUART->LS;
    }

    status->lineStatus = 0;

    /* Translate it into standard rx line status defined in uart api */
    if (lineStatus & UART_LS_OE)
    {
        status->lineStatus |= UART_OVERRUN_ERROR;
    }
    if (lineStatus & UART_LS_PE)
    {
        status->lineStatus |= UART_PARITY_ERROR;
    }
    if (lineStatus & UART_LS_FE)
    {
        status->lineStatus |= UART_FRAME_ERROR;
    }
    if (lineStatus & UART_LS_BRK)
    {
        status->lineStatus |= UART_BREAK_CHARACTER;
    }
    if (lineStatus & UART_LS_RFE)
    {
        status->lineStatus |= UART_FIFO_ERROR;
    }
    if (lineStatus & UART_LS_TIDL)
    {
        status->lineStatus |= UART_TRANSMITTER_IDLE;
    }
    if (lineStatus & (UART_LS_OE | UART_LS_PE | UART_LS_FE | UART_LS_BRK | UART_LS_RFE))
    {
        status->lineStatus |= UART_RX_ERROR;
    }

    if (hUART->enableFifo == FALSE)
    {
        status->receivedSize = (lineStatus & UART_LS_DR) ? 1: 0;
        status->toBeTransmitedSize = (lineStatus & UART_LS_THRE) ? 0: 1;
    }
    else
    {
        status->receivedSize += UART_TH_RFLV_GET(pUART->TH);
        status->toBeTransmitedSize += UART_TH_TFLV_GET(pUART->TH);
    }
    UART_MUTEX_LEAVE(hUART);

}

/**
 * @brief Process all received data.
 *
 * @param hUART     UART device handle
 * @retval          Return RET_BUSY if transfer is ongoing.
 */
static RET_CODE UART_RxProcessData(UART_Handle_T *hUART)
{
    UART_Device_T *pUART = hUART->device;

    /* First stage */
    /* Check dma over flag or transmit/receive data if one transfer is on progress */
    if (hUART->status & UART_STATUS_RX_BUSY)
    {
        /* Wait for dma over status if dma enabled */
        if (hUART->enableRxDMA)
        {
            if (hUART->rxDMAStatus == UART_DMA_STATUS_FINISHED)
            {
                hUART->rxSize += hUART->rcount;
                hUART->rcount = 0;
            }
        }
        else
        {
            if (!(hUART->intmask & UART_RX_INT))
            {
                /* Get saved rx line status */
                hUART->lineSavedStatus |= (UART_LS_RXE | UART_LS_BRK);
            }
            /* Get data from data rx fifo */
            while (hUART->rcount != 0)
            {
                if (hUART->enableFifo == FALSE)
                {
                    if (!IO_BIT_GET(pUART->LS, UART_LS_DR))
                    {
                        break;
                    }
                }
                else if (UART_TH_RFLV_GET(pUART->TH) == 0)
                {
                    break;
                }
                hUART->pRxData[hUART->rxSize++] = pUART->DR;
                hUART->rcount--;
            }
        }
        /* One transfer is over */
        if (hUART->rcount == 0)
        {
            /* Total transfer is over if next transfer size == 0 */
            if (hUART->rxExpectedSize == hUART->rxSize)
            {
                hUART->status &= ~UART_STATUS_RX_BUSY;
                return RET_OK;
            }
        }
    }

    /* Second stage is only valid when the status is idle or last transfer is over */
    if (!(hUART->status & UART_STATUS_RX_BUSY) || hUART->rcount == 0)
    {
        /* Calculate next transfer size */
        hUART->rcount = (hUART->rxExpectedSize - hUART->rxSize);

        if (hUART->rcount != 0)
        {
            /* Transfer on progress */
            hUART->status |= UART_STATUS_RX_BUSY;

            /* Start DMA operation */
            if (hUART->enableRxDMA)
            {
                hUART->rxDMAStatus = UART_DMA_STATUS_STARTED;
                DMA_Start(&hUART->RxDMAHandle, (uint32_t) & (pUART->DR), (uint32_t)(hUART->pRxData), hUART->rcount);
            }
        }
        else
        {
            hUART->status &= ~UART_STATUS_RX_BUSY;
            return RET_OK;
        }
    }

    return RET_BUSY;
}

RET_CODE UART_Receive(UART_Handle_T *hUART, uint8_t *pData, uint16_t expectedSize, uint16_t *pReceivedSize, uint32_t timeout)
{
    WaitTimer_T waitTimer;
    RET_CODE ret;

    /* Sanity check */
    /* If one reception is on progess, next reception is not allowed. */
    if (hUART->status & UART_STATUS_RX_BUSY)
    {
        return RET_BUSY;
    }
    /* Zero transfer not allowed */
    if (expectedSize == 0)
    {
        return RET_ERROR;
    }
    /* The reentry lock should be called hear */

    /* Initialize timeout structure */
    WaitTimerInit(&waitTimer, timeout, 1);

    /* Accept this reception, copy size and buffer into handle structure */
    UART_MUTEX_ENTER(hUART);
    hUART->rxExpectedSize = expectedSize;
    hUART->pRxData = pData;
    hUART->rxSize = 0;
    hUART->rcount = 0;
    hUART->lineSavedStatus = 0;
    UART_MUTEX_LEAVE(hUART);

    if (hUART->enableRxDMA)
    {
#ifdef CHIP_F6621B
        UART_DMAConfig(hUART->device, TRUE, &hUART->RxDMAHandle);
#else
        UART_DMAConfig(hUART->device, TRUE, &hUART->RxDMAHandle, hUART->index);
#endif
    }

    do
    {
        /* It is safe to prevent interrupt handling and api to enter into same function */
        UART_MUTEX_ENTER(hUART);
        ret = UART_RxProcessData(hUART);

        UART_MUTEX_LEAVE(hUART);

        /* Exit if it returns RET_OK. */
        if (ret != RET_BUSY)
        {
            break;
        }
        /* Exit if it is non-blocking call */
        if (timeout == TIMEOUT_NO_BLOCKING)
        {
            ret = RET_ONGOING;
            UART_MUTEX_LEAVE(hUART);
            break;
        }
        /* Wait and do nothing if FIFO interrupt handling is enabled. */
        if (
                        ((hUART->enableRxDMA == FALSE) && (hUART->intmask & UART_RX_INT)) ||
                        ((hUART->enableRxDMA == TRUE) && (hUART->intmask & UART_RX_DMA_INT)))
        {
            while (1)
            {
                /* Exit if all expected user data has been put into user buffer. */
                if (hUART->rxExpectedSize == hUART->rxSize)
                {
                    ret = RET_OK;
                    break;
                }
                /* Wait for a while if it is called in blocking function */
                if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
                {
                    ret = RET_TIMEOUT;
                    hUART->status &= ~UART_STATUS_RX_BUSY;
                    break;
                }
            }
            break;
        }
        else
        {
            /* Wait for a while if it is called in blocking function */
            if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
            {
                ret = RET_TIMEOUT;
                hUART->status &= ~UART_STATUS_RX_BUSY;
                break;
            }
        }

    } while (1);

    /* Timeout/nowait call shall return size of data actually transferred */
    *pReceivedSize = hUART->rxSize;
    /* Processing error status */
    if (hUART->lineSavedStatus & UART_LS_RXE)
    {
        if (ret == RET_OK)
        {
            ret = RET_ERROR;
            hUART->status &= ~UART_STATUS_RX_BUSY;
        }
    }

    return ret;
}

RET_CODE UART_Transmit(UART_Handle_T *hUART, uint8_t *pData, uint16_t size, uint32_t timeout)
{
    WaitTimer_T waitTimer;
    RET_CODE ret;

    /* The reentry lock shall be called hear if multi-thread context */

    /* If one transmit is on progess, next transmit is not allowed. */
    if (hUART->status & UART_STATUS_TX_BUSY)
    {
        return RET_BUSY;
    }

    /* Zero transfer not allowed */
    if (size == 0)
    {
        return RET_ERROR;
    }

    /* Initialize timeout structure */
    WaitTimerInit(&waitTimer, timeout, 1);

    /* Accept this transmit, copy size and buffer address into handle structure */
    /* TX empty interrupt may issue after next transfer starts, so it is safe to use mutex */
    UART_MUTEX_ENTER(hUART);
    hUART->pTxData = pData;
    hUART->txExpectedSize = size;
    hUART->txSize = 0;
    hUART->tcount = 0;

    if (hUART->enableTxDMA)
    {
#ifdef CHIP_F6621B
        UART_DMAConfig(hUART->device, FALSE, &hUART->TxDMAHandle);
#else
        UART_DMAConfig(hUART->device, FALSE, &hUART->TxDMAHandle, hUART->index);
#endif
    }
    UART_MUTEX_LEAVE(hUART);

    do
    {
        /* It is safe to avoid reentry issue */
        UART_MUTEX_ENTER(hUART);
        ret = UART_TxProcessData(hUART);

        UART_MUTEX_LEAVE(hUART);

        /* Return OK only if all data pushed into TX fifo or ring buffer */
        if (ret != RET_BUSY)
        {
            break;
        };
        /* Exit if it is non-blocking call */
        if (timeout == TIMEOUT_NO_BLOCKING)
        {
            ret = RET_ONGOING;
            break;
        }

        /* Wait and do nothing if FIFO interrupt handling is enabled. */
        if (
                        ((hUART->enableTxDMA == FALSE) && (hUART->intmask & UART_TX_INT)) ||
                        ((hUART->enableTxDMA == TRUE) && (hUART->intmask & UART_TX_DMA_INT)))
        {
            while (1)
            {
                /* Exit if all user data has been put into ring buffer. */
                /* But not all data are transmitted by physical bus. */
                if (hUART->txExpectedSize == hUART->txSize)
                {
                    ret = RET_OK;
                    break;
                }
                if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
                {
                    ret = RET_TIMEOUT;
                    hUART->status &= ~UART_STATUS_TX_BUSY;
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
                hUART->status &= ~UART_STATUS_TX_BUSY;
                break;
            };
        }
    } while (1);

    return ret;
}

RET_CODE UART_TransferConfig(UART_Handle_T *hUART, UART_TransferConfig_T *pTransferConfig)
{
    UART_Device_T *pUART = hUART->device;
    volatile uint32_t data;
    uint32_t txWaterVal = 0;
    uint32_t rxWaterVal = 0;

    UART_MUTEX_ENTER(hUART);
    /* Copy all configuration items to handle structure */
    hUART->enableFifo = pTransferConfig->enableFifo;
    hUART->txWatermark = pTransferConfig->txWatermark;
    hUART->rxWatermark = pTransferConfig->rxWatermark;
    hUART->enableRxDMA = pTransferConfig->enableRxDMA;
    hUART->enableTxDMA = pTransferConfig->enableTxDMA;

    hUART->status = UART_STATUS_IDLE;
    hUART->txExpectedSize = hUART->txSize = hUART->tcount = 0;
    hUART->rxExpectedSize = hUART->rxSize = hUART->rcount = 0;

    hUART->lineSavedStatus = 0;

    hUART->rxDMAStatus = hUART->txDMAStatus = UART_DMA_STATUS_FINISHED;

    /* It defines non-fifo mode if both txWatermark and rxWatermark <= 1 */
    if (hUART->enableFifo == FALSE)
    {
        IO_BITS_CLEAR(pUART->C2, UART_C2_FFEN);
        /* Clear pending transmission , should support cancelling for single byte */
        while (!(pUART->LS & UART_LS_TIDL)) {};
        /* Clear pending data read */
        data = pUART->DR;
        /* Clear pending data read */
        data = pUART->DR;
    }
    else
    {
        /* Enable TX/RX fifo */
        IO_BITS_SET(pUART->C2, (UART_C2_FFEN | UART_C2_RFRST | UART_C2_TFRST));
        /* Clear threshold interrupt */
        IO_WRITE_REG(pUART->IS, (UART_IS_TFTH | UART_IS_RFTH));

        /* Set FIFO default threshhold */
        if (hUART->txWatermark > 0 && hUART->txWatermark <= MAX_UART_TX_FIFO_SIZE)
        {
            txWaterVal = hUART->txWatermark;
        }
        else
        {
            txWaterVal = MAX_UART_TX_FIFO_SIZE / 2;
        }
        if (hUART->rxWatermark > 0 && hUART->rxWatermark <= MAX_UART_RX_FIFO_SIZE)
        {
            rxWaterVal = hUART->rxWatermark;
        }
        else
        {
            rxWaterVal = MAX_UART_RX_FIFO_SIZE / 2;
        }
        IO_BITMASK_SET(pUART->TH, UART_TH_TFTH, UART_TH_TFTH_FIELD(txWaterVal));
        IO_BITMASK_SET(pUART->TH, UART_TH_RFTH, UART_TH_RFTH_FIELD(rxWaterVal));
    }

    /* DMA Configuration is delayed till user call UART_Receive/UART_Transmit */
    IO_BITS_CLEAR(pUART->DMA, (UART_DMA_TEN | UART_DMA_REN));

    UART_MUTEX_LEAVE(hUART);

    return RET_OK;
}

void UART_EnableInterrupts(UART_Handle_T *hUART, uint32_t mask)
{
    UART_Device_T *pUART = hUART->device;

    /* Sanity check */
    ASSERT((pUART == UART0) || (pUART == UART1) || (pUART == UART2));

    /* Make sure pending interrupt and pending data are all cleared */
    NVIC_DisableIRQ(s_uartIrq[hUART->index]);

    if (UART_RX_INT & mask)
    {
        /* Enable Received Data Interrupt and (RX) and Receiver Line Status Interrupt(RLS) */
        IO_BIT_SET(pUART->IE, (UART_IE_RX | UART_IE_RLS));
        UART_ClearRx(hUART);
        IO_WRITE_REG(pUART->IS, UART_IS_RLS);
    }

    if (UART_TX_INT & mask)
    {
        /* Enable Transmitter Holding Register Empty Interrupt(THREI)*/
        IO_BIT_SET(pUART->IE, (UART_IE_THRE | UART_IE_TIDL));
        IO_WRITE_REG(pUART->IS, (UART_IS_THRE | UART_IS_TIDL));
    }

    if (UART_RX_FIFO_INT & mask)
    {
        /* Enable RX FIFO threshold interrupt */
        IO_BIT_SET(pUART->IE, (UART_IE_RFTH | UART_IE_RXTM | UART_IE_RLS));
        UART_ClearRx(hUART);
        IO_WRITE_REG(pUART->IS, (UART_IS_RFTH | UART_IS_RLS));
    }

    if (UART_TX_FIFO_INT & mask)
    {
        /* Enable TX FIFO threshold interrupt */
        IO_BIT_SET(pUART->IE, (UART_IE_TFTH | UART_IE_TIDL));
        IO_WRITE_REG(pUART->IS, (UART_IS_TFTH | UART_IS_TIDL));
    }
    hUART->intmask |= mask;

    NVIC_ClearPendingIRQ(s_uartIrq[hUART->index]);
    NVIC_EnableIRQ(s_uartIrq[hUART->index]);

}

void UART_DisableInterrupts(UART_Handle_T *hUART, uint32_t mask)
{
    UART_Device_T *pUART = hUART->device;

    /* Sanity check */
    ASSERT((pUART == UART0) || (pUART == UART1) || (pUART == UART2));

    if (UART_RX_INT & mask)
    {
        /* Disable Received Data Interrupt and (RXI) and Receiver Line Status Interrupt(RLSI) */
        IO_BIT_CLEAR(pUART->IE, (UART_IE_RX | UART_IE_RLS));
    }

    if (UART_TX_INT & mask)
    {
        /* Disable Transmitter Holding Register Empty Interrupt(THREI)*/
        IO_BIT_CLEAR(pUART->IE, (UART_IE_THRE | UART_IE_TIDL));
    }

    if (UART_RX_FIFO_INT & mask)
    {
        /* Disable RX FIFO threshold interrupt */
        IO_BIT_CLEAR(pUART->IE, (UART_IE_RFTH | UART_IE_RXTM | UART_IE_RLS));
    }

    if (UART_TX_FIFO_INT & mask)
    {
        /* Disable TX FIFO threshold interrupt */
        IO_BIT_CLEAR(pUART->IE, (UART_IE_TFTH | UART_IE_TIDL));
    }

    hUART->intmask &= ~mask;
}

void UART_RxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback, uint8_t fifoRxTimeout)
{
    uint8_t fifoDataSize;
    RET_CODE ret;

    if (hUART->enableRxDMA == FALSE)
    {
        /* Always notify user to receive data in interrupt context if user
           does not call UART_Receive in api context */
        if (hUART->rxExpectedSize == hUART->rxSize)
        {
            if (callback != NULL && IO_BIT_GET(pUART->C2, UART_C2_FFEN))
            {
                fifoDataSize = UART_TH_RFLV_GET(pUART->TH);
                /* Watermark event(rx fifo threshhold interrupt) always overlaps
                   rx fifo timeout event */
                if (fifoRxTimeout == TRUE)
                {
                    callback(hUART, UART_EVENT_RXFIFO_TIMEOUT, (ulong_t)fifoDataSize);
                }
                else
                {
                    callback(hUART, UART_EVENT_RX_WATERMARK, (ulong_t)fifoDataSize);
                }
            }
        }
        else
        {
            ret = UART_RxProcessData(hUART);
            if (ret == RET_OK)
            {
                if (callback != NULL)
                {
                    callback(hUART, UART_EVENT_RX_OVER, (ulong_t)0);
                }
            }
        }
    }
}

void UART_TxInterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback)
{
    uint8_t fifoDataSize;
    RET_CODE ret;

    if (hUART->enableTxDMA == FALSE)
    {
        /* Always notify user if user does not call UART_Receive in api context */
        if (hUART->txExpectedSize == hUART->txSize)
        {
            if (NULL != callback)
            {
                fifoDataSize = UART_TH_TFLV_GET(pUART->TH);
                callback(hUART, UART_EVENT_TX_WATERMARK, (ulong_t)fifoDataSize);
            }
        }
        else
        {
            ret = UART_TxProcessData(hUART);
            if (ret == RET_OK)
            {
                if (NULL != callback)
                {
                    callback(hUART, UART_EVENT_TX_OVER, (ulong_t)0);
                }
            }
        }
    }
}

void UART_InterruptHandler(UART_Device_T *pUART, UART_Handle_T *hUART, UART_Callback_T callback)
{
    uint8_t rxChar;
    uint32_t lsrStatus;

#if (UART_IP_VERSION == 1)
    uint32_t missedThreInterrupt = FALSE;
#endif

    lsrStatus = 0;

#if (UART_IP_VERSION == 1)
    while (0 == (pUART->IS & UART_IS_PI))
    {
        switch (pUART->IS & UART_IS_IID)
        {
            case UART_IS_IID_RLS:   /* LSR error: OE, PE, FE, or BI, priority 1 */
                lsrStatus = pUART->LS;
                /* Rx error status shall be saved before any callback is called, because callback may call GetStatus() */
                if (hUART != NULL)
                {
                    hUART->lineSavedStatus |= (lsrStatus & (UART_LS_RXE | UART_LS_BRK));
                }
                if (lsrStatus & UART_LS_RXE)
                {
                    //UART_PRINTF("Received error character. \n");
                    if (NULL != callback)
                    {
                        callback(hUART, UART_EVENT_RX_ERROR, (ulong_t)0);
                    }

                }
                else if (lsrStatus & UART_LS_BRK)
                {
                    //UART_PRINTF("Received break character. \n");
                }

                break;
            case UART_IS_IID_RXTM:  /* Character Timeout Indication in FIFO mode, priority 2 */
                UART_RxInterruptHandler(pUART, hUART, callback, TRUE);
                break;
            case UART_IS_IID_DR:    /* Received Data Available, priority 2 */
                /* Fix bug which clear thre interrupt when receiving rx interrupt */
                lsrStatus = pUART->LS;
                if (!(pUART->C2 & UART_C2_TXDIS) && (lsrStatus & UART_LS_THRE) && (pUART->IE & UART_IE_THRE))
                {
                    missedThreInterrupt = TRUE;
                }

                if (hUART != NULL && (hUART->rxExpectedSize != hUART->rxSize))
                {
                    /* Get one byte in Non-Fifo mode */
                    UART_RxInterruptHandler(pUART, hUART, callback, FALSE);
                }
                else
                {
                    //lsrStatus = pUART->LS;
                    if (lsrStatus & UART_LS_DR) /* Make sure data is ready */
                    {
                        rxChar = (uint8_t)(pUART->DR);

                        if (NULL != callback)
                        {
                            callback(hUART, UART_EVENT_RX_CHAR, (ulong_t)rxChar);
                        }
                    }
                }
                break;
            case UART_IS_IID_THRE:  /* Transmitter Holding Register Empty, priority 3 */
                /* Make sure empty status to avoid side effect of bug-fixing */
                lsrStatus = pUART->LS;
                if (!(lsrStatus & UART_LS_THRE))
                {
                    break;
                }
                /* Check if there is pending data to be tranmitted */
                if (hUART != NULL && (hUART->txExpectedSize != hUART->txSize))
                {
                    UART_TxInterruptHandler(pUART, hUART, callback);
                }
                else
                {
                    if (NULL != callback)
                    {
                        callback(hUART, UART_EVENT_TX_EMPTY, (ulong_t)0);
                    }
                    /* All data pushed into fifo and transmitted, threshhold interrupt shall count up from zero for next transfer */
                    if (hUART != NULL)
                    {
                        if (hUART->enableFifo == TRUE)
                        {
                            IO_WRITE_REG(pUART->IS, UART_IS_TFTH);
                            IO_BITS_SET(pUART->C2, UART_C2_TFRST);
                        }
                    }
                }
                break;
            case 0x00:  /* Modem status, skip it */
            default:
                break;
        }
    }
#endif

#if (UART_IP_VERSION == 2)
    if ((pUART->IS & UART_IS_RLS) && (IO_BITS_GET(pUART->IE, UART_IS_RLS))) /* LSR error: OE, PE, FE, or BI*/
    {
        IO_WRITE_REG(pUART->IS, UART_IS_RLS);  //clear interrupts
        lsrStatus = pUART->LS;
        /* Rx error status shall be saved before any callback is called, because callback may call GetStatus() */
        if (hUART != NULL)
        {
            hUART->lineSavedStatus |= (lsrStatus & (UART_LS_RXE | UART_LS_BRK));
        }
        if (lsrStatus & UART_LS_RXE)
        {
            //UART_PRINTF("Received error character. \n");
            if (NULL != callback)
            {
                callback(hUART, UART_EVENT_RX_ERROR, (ulong_t)0);
            }
            else if (lsrStatus & UART_LS_BRK)
            {
                //UART_PRINTF("Received break character. \n");
            }
        }
    }

    if ((pUART->IS & UART_IS_RXTM) && (pUART->IE & UART_IE_RXTM)) /* Character Timeout Indication in FIFO mode */
    {
        UART_RxInterruptHandler(pUART, hUART, callback, TRUE);
    }

    if ((hUART->enableFifo) == FALSE && (pUART->IS & UART_IS_RX) && (pUART->IE & UART_IE_RX))  /* Received Data Available */
    {
        lsrStatus = pUART->LS;
        if (hUART != NULL && (hUART->rxExpectedSize != hUART->rxSize))
        {
            /* Get one byte in Non-Fifo mode */
            UART_RxInterruptHandler(pUART, hUART, callback, FALSE);
        }
        else
        {
            if (lsrStatus & UART_LS_DR) /* Make sure data is ready */
            {
                rxChar = (uint8_t)(pUART->DR);
                if (NULL != callback)
                {
                    callback(hUART, UART_EVENT_RX_CHAR, (ulong_t)rxChar);
                }
            }
        }
    }

    if ((pUART->IS & UART_IS_THRE) && (IO_BIT_GET(pUART->IE, UART_IS_THRE)))  /* Transmitter Holding Register Empty */
    {
        IO_WRITE_REG(pUART->IS, UART_IS_THRE); //clear interrupt
        /* Check if there is pending data to be tranmitted */
        if (hUART != NULL && ((hUART->txExpectedSize != hUART->txSize)))
        {
            UART_TxInterruptHandler(pUART, hUART, callback);
        }
        else
        {
            if (NULL != callback)
            {
                callback(hUART, UART_EVENT_TX_EMPTY, (ulong_t)0);
            }
        }
    }
#endif

#if (UART_IP_VERSION == 1)
    if (missedThreInterrupt == TRUE)
    {
        /* Check if there is pending data to be tranmitted */
        if (hUART != NULL && (hUART->txExpectedSize != hUART->txSize))
        {
            UART_TxInterruptHandler(pUART, hUART, callback);
        }
        else
        {
            if (NULL != callback)
            {
                callback(hUART, UART_EVENT_TX_EMPTY, (ulong_t)0);
            }
            /* All data pushed into fifo and transmitted, threshhold interrupt shall count up from zero for next transfer */
            if (hUART != NULL)
            {
                if (hUART->enableFifo == TRUE)
                {
                    IO_WRITE_REG(pUART->IS, UART_IS_TFTH);
                    IO_BITS_SET(pUART->C2, UART_C2_TFRST);
                }
            }
        }
        missedThreInterrupt = FALSE;
    }
#endif
    /* Process data reception in interrupt handler */
    if ((pUART->IS & UART_IS_RFTH) && (pUART->IE & UART_IE_RFTH))
    {
        IO_WRITE_REG(pUART->IS, UART_IS_RFTH);
        UART_RxInterruptHandler(pUART, hUART, callback, FALSE);
    }

    /* Process data transmition in interrupt handler */
    if ((pUART->IS & UART_IS_TFTH) && (pUART->IE & UART_IE_TFTH))
    {
        IO_WRITE_REG(pUART->IS, UART_IS_TFTH);
        UART_TxInterruptHandler(pUART, hUART, callback);
    }

    if ((pUART->IS & UART_IS_TIDL) && (pUART->IE, UART_IE_TIDL))
    {
        IO_WRITE_REG(pUART->IS, UART_IS_TIDL);
        if (NULL != hUART->callback)
        {
            hUART->callback(hUART, UART_EVENT_TX_IDLE, (ulong_t)0);
        }
    }

    return;
}

#ifdef UART0
void UART0_InterruptHandler(void)
{
    s_uartIsr(UART0, s_uartHandle[0], s_uartCallback[0]);
}
#endif

#ifdef UART1
void UART1_InterruptHandler(void)
{
    s_uartIsr(UART1, s_uartHandle[1], s_uartCallback[1]);
}
#endif

#ifdef UART2
void UART2_InterruptHandler(void)
{
    s_uartIsr(UART2, s_uartHandle[2], s_uartCallback[2]);
}
#endif

#ifdef CHIP_F6621B
void UART_InterruptEntrance(void)
{
    if (*(volatile unsigned long *)0x40000018 & 0x800)
    {
        s_uartIsr(UART0, s_uartHandle[0], s_uartCallback[0]);
    }
    else if (*(volatile unsigned long *)0x40000018 & 0x400)
    {
        s_uartIsr(UART1, s_uartHandle[1], s_uartCallback[1]);
    }
    else if (*(volatile unsigned long *)0x40000018 & 0x200)
    {
        s_uartIsr(UART2, s_uartHandle[2], s_uartCallback[2]);
    }
}
#endif

void UART_GetFifoData(UART_Device_T *pUART, uint8_t *size, uint8_t *data)
{
    uint8_t i = 0;

    if ((size == NULL) || (data == NULL))
    {
        return;
    }

    *size = (uint8_t)UART_TH_RFLV_GET(pUART->TH);
    while (i < *size)
    {
        data[i++] = UART_GetChar(pUART);
    }
}
