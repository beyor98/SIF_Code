 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          can.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2016/09/05
 *
 * @brief         Header file of can driver.
 *
 * @note
 *     2017/03/23, Mike.Zheng, V1.0.0
 *        Initial version.
 *     2017/06/12, Mike.Zheng, V1.1.0
 *        Update coding style.
 */
#include <drv/dev/can.h>

static CAN_Handle_T *s_pHandle = NULL;
CAN_Callback_T s_canCallback = NULL;
STATIC_ISR(CAN_IRQn, CAN_IrqHandler);

static void CAN_IrqHandler(void);
static void CAN_PrintErrorInfor(CAN_Device_T *pCAN);
static void CAN_InterruptEn(CAN_Handle_T *hCAN, uint8_t en);
static uint8_t CAN_RingBufOneFrameToRead(uint32_t rd, uint32_t wr);
static uint8_t  CAN_RingBufOneFrameToWrite(uint32_t rd, uint32_t wr);
static uint8_t CAN_ParseRxFrame(CAN_Handle_T *hCAN, uint8_t vaildFramelength);
static void CAN_CopyToRxBuf(CAN_Device_T *pCAN, uint8_t wr, CAN_FrameInfor_T *pFrameInfor);
static RET_CODE CAN_ConfigBaudRate(CAN_Device_T *pCAN, CAN_BaudRateAttribute_T *pBaudRateAttribute);

static void CAN_PrintErrorInfor(CAN_Device_T *pCAN)
{
    CAN_PR_ERR("Errstat:0x%x Errinfor:0x%x TxErrCnt:0x%x RxErrCnt:0x%x\n",
                pCAN->ST, pCAN->ERRINFO,
                ((pCAN->ERRCNT & CAN_ERRCNT_TEC) >> CAN_ERRCNT_TEC_POS),
                ((pCAN->ERRCNT & CAN_ERRCNT_REC) >> CAN_ERRCNT_REC_POS));
}


static void CAN_IrqHandler(void)
{
    CAN_Event_T event     = CAN_EVENT_WAKEUP;
    CAN_Device_T *pCAN    = s_pHandle->device;
    uint32_t irqStatus    = pCAN->INTST;
    uint32_t irqEn        = pCAN->IE;
    uint8_t vaildFrameCnt = 0;
    uint32_t parm         = 0;

    if (irqStatus & (CAN_INST_RI | CAN_INST_DOI))
    {
        /* parse frame */
        vaildFrameCnt = CAN_GetCurrentRxCnt(pCAN);

        if (irqStatus & CAN_INST_RI)
        {
            CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_RI);
        }
        if (irqStatus & CAN_INST_DOI)
        {
            CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_DOI);
            CAN_PR_ERR("CAN DATA OVER INTERRUPT\n");
        }
        parm  = CAN_ParseRxFrame(s_pHandle, vaildFrameCnt);
        event = CAN_EVENT_RECEIVE_SUCCESS;
    }
    else if ((irqStatus & CAN_INST_TI) && (irqEn & CAN_IE_TIE))
    {
        CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_TI);
        s_pHandle->txStatus = CAN_TX_COMMUNICATE_SUCCESS;
        event               = CAN_EVENT_TRANSMIT_SUCCESS;
        CAN_PR_DEBUG("CAN TX SUCCESS\n");

        if (s_pHandle->txBlockStatus == CAN_BLOCK)
        {
            /* block will not send callback to usr */
            return ;
        }
    }
    else if ((irqStatus & CAN_INST_ALI) && (irqEn & CAN_IE_ALIE))
    {
        CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_ALI);
        event = CAN_EVENT_ARBIT_LOST;
        CAN_PR_DEBUG("CAAN ARBIT LOST INTERRUPT:0x%x\n", pCAN->ALB);
    }
    else if ((irqStatus & CAN_INST_BEI) && (irqEn & CAN_IE_BEIE))
    {
        CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_BEI);
        event               = CAN_EVENT_BUS_ERROR;
        CAN_PR_ERR("CAN BUS ERROR INTERRUPT\n");
        CAN_PrintErrorInfor(pCAN);
    }
    else if ((irqStatus & CAN_INST_EPI) && (irqEn & CAN_IE_EPIE))
    {
        CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_EPI);
        event = CAN_EVENT_ERROR_PASSIVE;
        CAN_PR_ERR("CAN ERROR PASSIVE INTERRUPT\n");
        CAN_PrintErrorInfor(pCAN);
    }
    else if ((irqStatus & CAN_INST_EWI) && (irqEn & CAN_IE_EWIE))
    {
        CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_EWI);
        event = CAN_EVENT_ERROR_WARNING;
        CAN_PR_ERR("CAN ERROR WARNING INTERRUPT\n");
        CAN_PrintErrorInfor(pCAN);
    }
    else if ((irqStatus & CAN_INST_WI) && (irqEn & CAN_IE_WIE))
    {
        CAN_CLEAR_INTERRUPT(pCAN, CAN_INST_WI);
        event = CAN_EVENT_WAKEUP;
        CAN_PR_DEBUG("CAN WAKEUP INTERRUPT\n");
    }
    else
    {
        CAN_PR_ERR("%s:Error Event\n",__FUNCTION__);
    }

    if (s_canCallback)
    {
        s_canCallback(s_pHandle, event, parm);
    }
}

/*
 * 70%<(1+bs1)/(1+bs1+bs2)<80% regval tseg1ortseg2 not tq
 * bs1> bs2
 * STW < bs2
 */
static RET_CODE CAN_ConfigBaudRate(CAN_Device_T *pCAN, CAN_BaudRateAttribute_T *pBaudRateAttribute)
{
    RET_CODE ret = RET_OK;
    uint32_t brp = ((pBaudRateAttribute->prescaler) / (2 * (pBaudRateAttribute->baudRate) * ( 1 + \
                     pBaudRateAttribute->bs1 + 1 + pBaudRateAttribute->bs2 + 1))) - 1 ;

    if (brp > MAX_BRP)
    {
        CAN_PR_ERR("%s:config calc brp:%d over MAX_BRP(4095)\n", __FUNCTION__, brp);
        ret = RET_INVPARAM;
        goto out;
    }

    if (pBaudRateAttribute->bs1 > CAN_TSEG_16TQ)
    {
        CAN_PR_ERR("%s:config bs1:%d over CAN_TSEG_16TQ(15)\n",
                    __FUNCTION__, pBaudRateAttribute->bs1);
        ret = RET_INVPARAM;
        goto out;
    }

    if (pBaudRateAttribute->bs2 > CAN_TSEG_8TQ)
    {
        CAN_PR_ERR("%s:config bs2:%d over CAN_TSEG_8TQ(7)\n",
                    __FUNCTION__, pBaudRateAttribute->bs2);
        ret = RET_INVPARAM;
        goto out;
    }

    /* set baud rate prescaler */
    IO_BITMASK_SET(pCAN->BUSTIM, CAN_BUSTIM_BRP, (brp << CAN_BUSTIM_BRP_POS));
    /* set sam */
    IO_BITMASK_SET(pCAN->BUSTIM, CAN_BUSTIM_SAM, \
                  (pBaudRateAttribute->sample << CAN_BUSTIM_SAM_POS));
    /* set sjw */
    IO_BITMASK_SET(pCAN->BUSTIM, CAN_BUSTIM_SJW, \
                  (pBaudRateAttribute->sjw << CAN_BUSTIM_SJW_POS));
    /* set sjw tesg1*/
    IO_BITMASK_SET(pCAN->BUSTIM, CAN_BUSTIM_TSEG1, \
                  (pBaudRateAttribute->bs1 << CAN_BUSTIM_TSEG1_POS));
    /* set sjw tesg2*/
    IO_BITMASK_SET(pCAN->BUSTIM, CAN_BUSTIM_TSEG2, \
                  (pBaudRateAttribute->bs2 << CAN_BUSTIM_TSEG2_POS));
out:
    return ret;
}

RET_CODE CAN_ConfigFilter(CAN_Handle_T *hCAN, CAN_FilterConfig_T *pFilter)
{
    RET_CODE ret       = RET_OK;
    CAN_Device_T *pCAN = NULL;
#if defined(CHIP_GM6611B)
    uint8_t i = 0;
#endif

    if (!(hCAN && pFilter))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    pCAN = hCAN->device;
    /* Makesure CAN Enter to Init Mode*/
    if (CAN_GetWorkMode(pCAN) != CAN_INIT_MODE)
    {
        SET_INIT_MODE(pCAN);
    }

#if defined(CHIP_GM6611)
    pCAN->AC0   = pFilter->filterCode0;
    pCAN->AMSK0 = pFilter->filterMask0;
    pCAN->AC1   = pFilter->filterCode1;
    pCAN->AMSK1 = pFilter->filterMask1;
#elif defined(CHIP_GM6611B)
    for (i = 0 ; i < MAX_FILTER_NUM ; i++ )
    {
        pCAN->ASEL = i;
        pCAN->AC   = pFilter->filterCode[i];
        pCAN->AMSK = pFilter->filterMask[i];
    }
#endif

out:
   return ret;
}


static void CAN_InterruptEn(CAN_Handle_T *hCAN, uint8_t en)
{
    uint32_t irqMask = (CAN_IE_AIE  |\
                        CAN_IE_WIE  |\
                        CAN_IE_BEIE |\
                        CAN_IE_EPIE |\
                        CAN_IE_EWIE |\
                        CAN_IE_ALIE |\
                        CAN_IE_DOIE |\
                        CAN_IE_TIE  |\
                        CAN_IE_RIE);
    if (en == ENABLE)
    {
        CAN_INTERRUPT_ENABLE(hCAN->device, irqMask);
        NVIC_EnableIRQ(CAN_IRQn);

    }
    else
    {
        CAN_INTERRUPT_DISABLE(hCAN->device, irqMask);
        NVIC_DisableIRQ(CAN_IRQn);
    }
}

RET_CODE CAN_Init(CAN_Device_T *pCAN, CAN_Config_T *pConfig, CAN_Handle_T *hCAN)
{
    RET_CODE ret = RET_OK;

    if (ret != RET_OK)
    {
        CAN_PR_ERR("%s:Err:CAN Config Filter\n",__FUNCTION__);
        goto out;

    }
    if (!(pCAN && pConfig && hCAN))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    s_pHandle      = hCAN;
    s_canCallback  = pConfig->callback ;
    hCAN->device   = CAN_DEV;
    hCAN->workMode = pConfig->workMode;

    CAN_CLOCK_ENABLE();
    CAN_RESET();

    /* config to init mode */
    SET_INIT_MODE(pCAN);
    /* config filter to decide which message to receive */
    ret = CAN_ConfigFilter(hCAN, &(pConfig->filter));
    if(ret != RET_OK)
    {
        CAN_PR_ERR("%s:Err:CAN Config Filter\n",__FUNCTION__);
        goto out;
    }
    /* config etc and rtc */
    CAN_SET_ERR_CNT(pCAN, pConfig->tec, pConfig->rec);
    /* config buarate */
    ret = CAN_ConfigBaudRate(pCAN, &(pConfig->baudRateAttribute));
    if(ret != RET_OK)
    {
        goto out;
    }
    /* set warning threshold */
    SET_WARNING_THRESHOLD(pCAN, pConfig->errWarnThreshold);
    /* config interrupt */
    CAN_InterruptEn(hCAN, TRUE);
    /* generate set normal mode for tx and rx , ready for tx/rx */
    SET_WORK_MODE(pCAN, pConfig->workMode);
out:
    return ret;
}

RET_CODE CAN_TransferMessage(CAN_Handle_T *hCAN, CAN_FrameInfor_T *pFrameInfor, int32_t timeoutMs)
{
    RET_CODE ret       = RET_OK;
    CAN_Device_T *pCAN = hCAN->device;
    WaitTimer_T waitTimer;
    /* check work mode is not init mode */
    if (!(CAN_GetWorkMode(pCAN) & CAN_CFG_VM))
    {
        CAN_PR_ERR("%s:can is not at workmode:mode:%d\n",__FUNCTION__,CAN_GetWorkMode(pCAN));
        ret = RET_ERROR;
        goto out;
    }

    if (CAN_TxBusy(pCAN))
    {
        CAN_PR_ERR("Can device is busy\n");
        ret = RET_ONGOING;
        goto out;
    }

    if (pFrameInfor->dataLength > CAN_MAX_DATA_LENGTH)
    {
        CAN_PR_ERR("Usr config Err tx dataLength :%d\n",pFrameInfor->dataLength);
        ret = RET_INVPARAM;
        goto out;
    }

    /* init can tx status */
    hCAN->txStatus = CAN_COMMUNICATE_IDLE;

    if(timeoutMs)
    {
        /*block*/
        hCAN->txBlockStatus = CAN_BLOCK;
    }
    else
    {
        /* noblock*/
        hCAN->txBlockStatus = CAN_NOBLOCK;
    }

    /* config stand or extern frame */
    SET_FRAME_FORMAT(pCAN, pFrameInfor->frameFormat);

    /* config id */
    if (pFrameInfor->frameFormat == CAN_STAND_FRAME)
    {
        SET_STAND_ID(pCAN, pFrameInfor->id);
    }
    else
    {
        SET_EXTEND_ID(pCAN, pFrameInfor->id);
    }

    /* config remote or data frame */
    if ((pFrameInfor->frameType == CAN_SELF_REMOTE_FRAME) ||\
        (pFrameInfor->frameType == CAN_REMOTE_FRAME))
    {
        /* set remote frame RTR */
        CAN_SET_RTR(pCAN);
         /* set data length to zero */
        CAN_SET_DATA_LENGTH(pCAN, 0);
    }
    else
    {
        /* clear remote frame RTR */
        CAN_CLEAR_RTR(pCAN);

        /* set data length */
        CAN_SET_DATA_LENGTH(pCAN, pFrameInfor->dataLength);

        /* fill data */
        pCAN->TXDATA0  = pFrameInfor->data[0]        |\
                         pFrameInfor->data[1] << 8   |\
                         pFrameInfor->data[2] << 16  |\
                         pFrameInfor->data[3] << 24;

        pCAN->TXDATA1  = pFrameInfor->data[4]        |\
                         pFrameInfor->data[5] << 8   |\
                         pFrameInfor->data[6] << 16  |\
                         pFrameInfor->data[7] << 24;
    }

    /* requeset to tx msg */
    if ((pFrameInfor->frameType == CAN_SELF_DATA_FRAME) ||\
        (pFrameInfor->frameType == CAN_SELF_REMOTE_FRAME)){
         /* set workmode to selftest */
         CAN_SELF_REQUESET_TX(pCAN);
    }
    else
    {
        /* set work mode to normal */
        CAN_NORMAL_REQUESET_TX(pCAN);
    }

    if (hCAN->txBlockStatus == CAN_BLOCK)
    {
        /* initialize timeout structure */
        WaitTimerInit(&waitTimer, timeoutMs, 1);
        while(1)
        {
            if (hCAN->txStatus == CAN_TX_COMMUNICATE_SUCCESS)
            {
                break;
            }
            else
            {
                /* exit when timeout */
                if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
                {
                    ret = RET_TIMEOUT;
                    goto out;
                }
            }
        }
    }
    else
    {
         /* async mode tx */
         ret = RET_ONGOING;
    }

out:
    return ret ;

}

static uint8_t CAN_RingBufOneFrameToRead(uint32_t rd, uint32_t wr)
{
    return (!!(rd != wr));
}

static uint8_t  CAN_RingBufOneFrameToWrite(uint32_t rd, uint32_t wr)
{
    uint8_t ret = FALSE;

    if (rd == wr)
    {
        /* buf empty */
        ret = TRUE;
    }
    else if (rd > wr)
    {
        if ((rd-(wr+1)) >= WERITE_ONE_TIME_FRAME_CNT)
        {
            ret = TRUE;
        }
        else
        {
            ret = FALSE;
        }
    }
    else
    {
        if (((CAN_RX_BUF_FRAME_CNT + rd - (wr + 1))) >= WERITE_ONE_TIME_FRAME_CNT)
        {
            ret = TRUE;
        }
        else
        {
            ret = FALSE;
        }
    }

    return ret;
}

RET_CODE CAN_GetMessage(CAN_Handle_T *hCAN, CAN_FrameInfor_T **pFrameInfor, uint32_t timeoutMs)
{
    RET_CODE ret = RET_OK;
    uint8_t cnt  = READ_ONE_TIME_FRAME_CNT;
    uint8_t rd   = hCAN->rd;
    uint8_t wr   = hCAN->wr;
    WaitTimer_T waitTimer;

    if((!hCAN) || (!pFrameInfor))
    {
        ret = RET_INVPARAM;
        CAN_PR_ERR("%s:Err Input Parm\n", __FUNCTION__);
        goto out;
    }

    /* Initialize timeout structure */
    WaitTimerInit(&waitTimer, timeoutMs, 1);
    while (1)
    {
        rd = hCAN->rd;
        wr = hCAN->wr;
        /* If there is one frame space to read */
        if (CAN_RingBufOneFrameToRead(rd, wr) == TRUE)
        {
            break;
        }
        /* Exit when timeout */
        if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
        {
            ret = RET_TIMEOUT;
            goto out;
        }
    }

    *pFrameInfor = &(hCAN->frameInforBuf[rd]);
    if ((rd + cnt) >= CAN_RX_BUF_FRAME_CNT)
    {
         hCAN->rd= rd + cnt - CAN_RX_BUF_FRAME_CNT;
    }
    else
    {
         hCAN->rd = rd + cnt;
    }

out:
    return ret;
}

static void CAN_CopyToRxBuf(CAN_Device_T *pCAN, uint8_t wr, CAN_FrameInfor_T *pFrameInfor)
{
    pFrameInfor[wr].frameFormat = (CAN_FRAME_FORMAT_T)(CAN_GetFrameFormat(pCAN));

    if (pFrameInfor[wr].frameFormat == CAN_STAND_FRAME)
    {
        pFrameInfor[wr].id = CAN_GetStandId(pCAN);
    }
    else
    {
        pFrameInfor[wr].id = CAN_GetExtendId(pCAN);
    }

    pFrameInfor[wr].frameType = (CAN_FRAME_TYPE_T)(CAN_GetFrameType(pCAN));

    if (pFrameInfor[wr].frameType == CAN_DATA_FRAME)
    {
        pFrameInfor[wr].dataLength= CAN_GetDataLength(pCAN);

        switch (pFrameInfor[wr].dataLength)
        {
        case 1:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            break;
        case 2:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            pFrameInfor[wr].data[1] = (pCAN->RXDATA0 &0xff00)     >> 8;
            break;
        case 3:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            pFrameInfor[wr].data[1] = (pCAN->RXDATA0 &0xff00)     >> 8;
            pFrameInfor[wr].data[2] = (pCAN->RXDATA0 &0xff0000)   >> 16;
            break;
        case 4:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            pFrameInfor[wr].data[1] = (pCAN->RXDATA0 &0xff00)     >> 8;
            pFrameInfor[wr].data[2] = (pCAN->RXDATA0 &0xff0000)   >> 16;
            pFrameInfor[wr].data[3] = (pCAN->RXDATA0 &0xff000000) >> 24;
            break;
        case 5:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            pFrameInfor[wr].data[1] = (pCAN->RXDATA0 &0xff00)     >> 8;
            pFrameInfor[wr].data[2] = (pCAN->RXDATA0 &0xff0000)   >> 16;
            pFrameInfor[wr].data[3] = (pCAN->RXDATA0 &0xff000000) >> 24;
            pFrameInfor[wr].data[4] =  pCAN->RXDATA1 &0xff;
            break;
        case 6:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            pFrameInfor[wr].data[1] = (pCAN->RXDATA0 &0xff00)     >> 8;
            pFrameInfor[wr].data[2] = (pCAN->RXDATA0 &0xff0000)   >> 16;
            pFrameInfor[wr].data[3] = (pCAN->RXDATA0 &0xff000000) >> 24;
            pFrameInfor[wr].data[4] =  pCAN->RXDATA1 &0xff;
            pFrameInfor[wr].data[5] = (pCAN->RXDATA1 &0xff00)     >> 8;
            break;
        case 7:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            pFrameInfor[wr].data[1] = (pCAN->RXDATA0 &0xff00)     >> 8;
            pFrameInfor[wr].data[2] = (pCAN->RXDATA0 &0xff0000)   >> 16;
            pFrameInfor[wr].data[3] = (pCAN->RXDATA0 &0xff000000) >> 24;
            pFrameInfor[wr].data[4] =  pCAN->RXDATA1 &0xff;
            pFrameInfor[wr].data[5] = (pCAN->RXDATA1 &0xff00)     >> 8;
            pFrameInfor[wr].data[6] = (pCAN->RXDATA1 &0xff0000)   >> 16;
            break;
        case 8:
            pFrameInfor[wr].data[0] =  pCAN->RXDATA0 &0xff;
            pFrameInfor[wr].data[1] = (pCAN->RXDATA0 &0xff00)     >> 8;
            pFrameInfor[wr].data[2] = (pCAN->RXDATA0 &0xff0000)   >> 16;
            pFrameInfor[wr].data[3] = (pCAN->RXDATA0 &0xff000000) >> 24;
            pFrameInfor[wr].data[4] =  pCAN->RXDATA1 &0xff;
            pFrameInfor[wr].data[5] = (pCAN->RXDATA1 &0xff00)     >> 8;
            pFrameInfor[wr].data[6] = (pCAN->RXDATA1 &0xff0000)   >> 16;
            pFrameInfor[wr].data[7] = (pCAN->RXDATA1 &0xff000000) >> 24;
            break;

        default:
            return ;
        }
    }
}

static uint8_t CAN_ParseRxFrame(CAN_Handle_T *hCAN, uint8_t vaildFramelength)
{
    uint8_t cnt = 0;
    /* check receive is stand or extend frame */
    uint32_t rd = hCAN->rd;
    uint32_t wr = hCAN->wr;
    CAN_FrameInfor_T *pFrameInfor = hCAN->frameInforBuf;
    CAN_Device_T *pCAN = s_pHandle->device;

    for (cnt = 0; cnt < vaildFramelength ;cnt++)
    {
        if (CAN_RingBufOneFrameToWrite(rd, wr) == TRUE)
        {
            /* real fill data to buf */
            CAN_CopyToRxBuf(pCAN, wr, pFrameInfor);
            /* update wr index */
            if ((wr + WERITE_ONE_TIME_FRAME_CNT) >= CAN_RX_BUF_FRAME_CNT)
            {
                wr = wr + WERITE_ONE_TIME_FRAME_CNT-CAN_RX_BUF_FRAME_CNT;
            }
            else
            {
                wr = wr + WERITE_ONE_TIME_FRAME_CNT;
            }
            /* release hw fifo index */
            RELEASE_RX_FIFO(pCAN);
        }
        else
        {
            CAN_PR_ERR("Ring Buf Full, Usr Read Slow\n");
            break;
        }
    }

    hCAN->wr = wr;

    return cnt ;
}


void CAN_Stop(CAN_Handle_T *hCAN)
{
    uint32_t irqMask = (CAN_IE_AIE  |\
                        CAN_IE_WIE  |\
                        CAN_IE_BEIE |\
                        CAN_IE_EPIE |\
                        CAN_IE_EWIE |\
                        CAN_IE_ALIE |\
                        CAN_IE_DOIE |\
                        CAN_IE_TIE  |\
                        CAN_IE_RIE);

    /* disable irq and clear pre pending */
    CAN_InterruptEn(hCAN, DISABLE);
    /* clear pending irq */
    CAN_CLEAR_INTERRUPT(hCAN->device, irqMask);
    /* set can to stop mode */
    SET_INIT_MODE(hCAN->device);
}

void CAN_Start(CAN_Handle_T *hCAN)
{
    /* set can to stop mode */
    SET_WORK_MODE(hCAN->device, hCAN->workMode);
    /* enabel irq and clear pending */
    CAN_InterruptEn(hCAN, ENABLE);
}

