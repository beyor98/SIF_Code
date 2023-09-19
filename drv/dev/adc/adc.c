/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file          adc.c
*
* @author        Mike.Zheng
*
* @version       1.1.0
*
* @date          2016/07/05
*
* @brief         Header file of adc driver.
*
* @note
*    2016/03/01, Mike.Zheng, V1.0.0
*        Initial version.
*    2016/07/05, Mike.Zheng, V1.1.0
*        Update coding style.
*    2016/11/22  Mike.Zheng, V1.2.0
*/

#include <drv/dev/adc.h>

#if defined(CHIP_GM6621)
    #include <drv/dev/acmp.h>
    #include <drv/dev/pes.h>
#endif

STATIC_ISR(ADC_IRQn, ADC_IrqHandler);
static ADC_Handle_T *s_pHandle = NULL;
ADC_Callback_T s_adcCallback = NULL;

static DMA_Handle_T s_DMAHandle;

static RET_CODE ADC_Start(ADC_Handle_T *hADC);
static RET_CODE ADC_Stop(ADC_Handle_T *hADC);
static RET_CODE ADC_TryLock(ADC_Handle_T *hADC);
static void ADC_Unlock(ADC_Handle_T *hADC);
static int ADC_Triger(ADC_Handle_T *hADC);
static void ADC_InterruptConfig(ADC_Handle_T *hADC);
static RET_CODE ADC_DmaConfig(ADC_Handle_T *hADC, uint32_t *pConvertData, uint8_t length);
static void ADC_DmaCallback(struct DMA_Handle *pDMAHandle, uint8_t event, unsigned long param);
static void ADC_IrqHandler(void);
static RET_CODE ADC_SelectChannel(ADC_Device_T *pDevice, uint8_t *pSequenceChannel, uint8_t length);
static uint32_t ADC_ConvertToResolutionValue(ADC_Resolution_T resolution, uint32_t data);
static void ADC_FifoConvertValue(ADC_Device_T *pDevice, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length);
static void ADC_BufConvertValue(ADC_Device_T *pDevice, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length);
#if defined(CHIP_GM6611B)
    int HARDWARE_TRIGER_ADC(ADC_TrigerSource_T sel);
#endif

int GPIO_TRIGER_ADC(void);


static RET_CODE ADC_TryLock(ADC_Handle_T *hADC)
{
    if (hADC->lock == LOCKED)
    {
        return RET_BUSY;
    }
    else
    {
        hADC->lock = LOCKED;
        return RET_OK;
    }
}

static void ADC_Unlock(ADC_Handle_T *hADC)
{
    hADC->lock = UNLOCKED;
}

static void ADC_InterruptConfig(ADC_Handle_T *hADC)
{

#if defined(CHIP_GM6621)
    if (hADC->enableDisconMode == ENABLE)
    {
        ADC_INTERRUPT_ENABLE(hADC->device, ADC_IE_CMIE  | \
                             ADC_IE_DOFIE | \
                             ADC_IE_EOCE);
    }
    else
    {
        /*make sure overfow match eos interrupt is enable */
        ADC_INTERRUPT_ENABLE(hADC->device, ADC_IE_CMIE  | \
                             ADC_IE_DOFIE | \
                             ADC_IE_EOSE);
    }
#else
    /*make sure overfow match eos interrupt is enable */
    ADC_INTERRUPT_ENABLE(hADC->device, ADC_IE_CMIE  | \
                         ADC_IE_DOFIE | \
                         ADC_IE_EOSE);
#if 0
    ADC_INTERRUPT_ENABLE(hADC->device, ADC_IE_CMIE  | \
                         ADC_IE_DOFIE | \
                         ADC_IE_EOCE);
#endif


#endif
    NVIC_EnableIRQ(ADC_IRQn);
}

static void ADC_DmaCallback(struct DMA_Handle *pDMAHandle, uint8_t event, unsigned long param)
{
    ADC_Handle_T *hADC = GET_PARENTHDL_FROM_DMAHDL(pDMAHandle, ADC_Handle_T);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine */
            hADC->status = DMA_FINISH_STATUS;
        }
        else
        {
            /* noblock send event to usr */
            ADC_BufConvertValue(hADC->device, hADC->resolution, \
                                hADC->pConvertData, hADC->length);
            ADC_Unlock(hADC);

            s_adcCallback(hADC, ADC_EVENT_FINISHED, 0);

            hADC->status = IDLE_STATUS;
        }
    }
    else if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine */
            hADC->status = DMA_ERROR_STATUS;
        }
        else
        {
            ADC_Unlock(hADC);
            /* no block send event error event to usr */
            s_adcCallback(hADC, ADC_EVENT_CONVERT_ERROR, 0);
            hADC->status = DMA_ERROR_STATUS;
        }
    }
    else
    {
        ADC_Unlock(hADC);
    }

    /* release DMA device */
    DMA_DeInit(pDMAHandle);
}

static void ADC_IrqHandler(void)
{
    ADC_Handle_T *hADC = s_pHandle;
    ADC_Device_T *pDevice = hADC->device;
    if (hADC->pConvertData == NULL)
    {
        /*ADC_PR_ERR("Error:hADC->pConvertData is NULL  \r\n");*/
        return ;
    }
    /* all the irq must hander to usr */
    /* cpu mode need send event by irqstatus */
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_GM6621))
    if (IO_BITS_GET(pDevice->ST, ADC_ST_EOCE) && \
            IO_BITS_GET(pDevice->IE, ADC_IE_EOCE))
    {
        ADC_CLEAR_INTERRUPT(pDevice, ADC_ST_EOCE);

        if (hADC->expectFinishStatus == DMA_FINISH_STATUS)
        {
            /*
             * DMA sync Machine will process at ADC_DmaCallback
             * so here only clear irq and return
             */
            return ;
        }
        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine status*/
            hADC->status = CPU_FINISH_STATUS;
        }
        else
        {
            /*cpu mode*/
            ADC_FifoConvertValue(pDevice, hADC->resolution, hADC->pConvertData, 1);
            /* no block send convert finish event to usr */
            if (s_adcCallback)
            {
                ADC_Unlock(hADC);
                s_adcCallback(hADC, ADC_EVENT_FINISHED, 0);
            }
            else
            {
                ADC_Unlock(hADC);
            }

            hADC->status = IDLE_STATUS;
        }
    }
#endif

    if (IO_BITS_GET(pDevice->ST, ADC_ST_DOFI) && \
            IO_BITS_GET(pDevice->IE, ADC_IE_DOFIE))
    {
        /* clear overflower irq and sent this event */
        ADC_CLEAR_INTERRUPT(pDevice, ADC_ST_DOFI);
        if (s_adcCallback)
        {
            s_adcCallback(hADC, ADC_EVENT_OVERFLOW, 0);
        }
    }
    if (IO_BITS_GET(pDevice->ST, ADC_ST_CMI) && \
            IO_BITS_GET(pDevice->IE, ADC_IE_CMIE))
    {
        /* clear compare match irq and sent this event */
        ADC_CLEAR_INTERRUPT(pDevice, ADC_ST_CMI);
        if (s_adcCallback)
        {
            s_adcCallback(hADC, ADC_EVENT_COMPERE_MATCH, 0);
        }
    }
    if (IO_BITS_GET(pDevice->ST, ADC_ST_EOSI) && \
            IO_BITS_GET(pDevice->IE, ADC_IE_EOSE))
    {
        /* clear eos irq */
        ADC_CLEAR_INTERRUPT(pDevice, ADC_ST_EOSI);

        if (hADC->expectFinishStatus == DMA_FINISH_STATUS)
        {
            /*
             * DMA sync Machine will process at ADC_DmaCallback
             * so here only clear irq and return
             */
            return ;
        }
        if (hADC->blockStatus == BLOCK)
        {
            /* block only change machine status*/
            hADC->status = CPU_FINISH_STATUS;
        }
        else
        {
            if (hADC->pConvertData != NULL)
                /*cpu mode*/
                ADC_FifoConvertValue(pDevice, hADC->resolution,         \
                                     hADC->pConvertData, hADC->length);
            else
            {
                printf("hADC->pConvertData is NULL\n");
            }
            /* no block send convert finish event to usr */
            if (s_adcCallback)
            {
                ADC_Unlock(hADC);

                s_adcCallback(hADC, ADC_EVENT_FINISHED, 0);
            }
            else
            {
                ADC_Unlock(hADC);
            }

            hADC->status = IDLE_STATUS;
        }
    }
}

RET_CODE ADC_Init(ADC_Device_T *pADC, ADC_CommonConfig_T *pConfig, ADC_Handle_T *hADC)
{
    RET_CODE ret = RET_OK;

    if (!(pADC && pConfig && hADC))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    memset(hADC, 0, sizeof(ADC_Handle_T));

    s_pHandle = hADC;

    hADC->device = ADC_DEV;
    hADC->lock = UNLOCKED;
    hADC->pDMAHandle = &s_DMAHandle;
    s_adcCallback = pConfig->callback;
#if (defined(CHIP_GM6621))
    hADC->pestrigersrcfunc = pConfig->pestrigersrcfunc;
#endif
    hADC->continuousMode = pConfig->continuousMode;

    ADC_CLOCK_ENABLE();
    ADC_RESET();
#if defined(CHIP_GM6601)
    CONFIG_ATUO_POWEROFF(pADC, ENABLE);
#endif
    ADC_CONFIG_DAM_ENABLE(pADC, pConfig->enableDma);

    if (pConfig->enableDma == ENABLE)
    {
        hADC->expectFinishStatus = DMA_FINISH_STATUS;
    }
    else
    {
        hADC->expectFinishStatus = CPU_FINISH_STATUS;
    }

#if (defined(CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6611B))
    /* remember trigerSource*/
    hADC->trigerSource = pConfig->trigerSource;
    /* select triger source */
    if (pConfig->trigerSource != ADC_SW_TRIGER)
    {
        ADC_COFNIG_HW_TRIGER_SOUTCE(pADC, pConfig->trigerSource);
        /*enable hw triger mode*/
        ADC_COFNIG_HW_TRIGER_EN(pADC, ENABLE);
    }
    else
    {
        /*disable hw triger mode, means enable sw triger mode */
        ADC_COFNIG_HW_TRIGER_EN(pADC, DISABLE);
    }
#elif defined(CHIP_GM6621)
    /* remember trigerSource*/
    hADC->trigerSource = pConfig->trigerSource;
    hADC->trigerMode = pConfig->trigerMode;
    hADC->lowPowerEn = pConfig->lowPowerEn;
    hADC->enableDisconMode = pConfig->enableDisconMode;

    /*< adc low power mode enable */
    if (hADC->lowPowerEn == ENABLE)
    {
        CONFIG_LOW_POWERMODE_ENABLE(pADC, hADC->lowPowerEn);
    }
    /*< whether use discontinuous mode */
    if (hADC->enableDisconMode == ENABLE)
    {
        CONFIG_DISCONTIU_MODE_SET(pADC, ENABLE);
    }
    else
    {
        CONFIG_DISCONTIU_MODE_SET(pADC, DISABLE);
    }
    hADC->power_supply_sel = pConfig->power_supply_sel;
    if (hADC->power_supply_sel == VCCANA)
    {
        CONFIG_POWER_SUPPLY_SEL(pADC, 1);
    }
    else
    {
        CONFIG_POWER_SUPPLY_SEL(pADC, 0);
    }
    HW32_REG(0x40000048) |= 0x2F << 0; /*clock 7'bxxx + 1*/
#endif

    ADC_InterruptConfig(hADC);

    hADC->status = INIT_STATUS;

    ret = ADC_Start(hADC);
    if (ret != RET_OK)
    {
        ADC_PR_ERR("%s Err:%d ADC_Start \n", __FUNCTION__, ret);
    }

out:
    return ret;
}

static int ADC_Triger(ADC_Handle_T *hADC)
{

#if defined(CHIP_GM6621)
    PES_ChannelConfig_T  chCfg;
    memset(&chCfg, 0, sizeof(PES_ChannelConfig_T));
#endif

#if (defined(CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6611B))
    /*6601B */
    if (hADC->trigerSource == ADC_SW_TRIGER)
    {
        ADC_SW_TRIGGER(hADC->device);
    }
    else
    {
#if defined(CHIP_GM6611B)
        if (hADC->trigerSource != ADC_SW_TRIGER)
        {
            ADC_COFNIG_HW_TRIGER_SOUTCE(hADC->device, hADC->trigerSource);
            /*enable hw triger mode*/
            ADC_COFNIG_HW_TRIGER_EN(hADC->device, ENABLE);
        }

        HARDWARE_TRIGER_ADC(ADC_ACMP_TRIGER);
#endif
    }
#elif (defined(CHIP_GM6621))
    /* select triger source */
    if (hADC->trigerMode == ADC_HW_TRIGER)
    {

        ADC_COFNIG_HW_TRIGER_SOUTCE(hADC->device, hADC->trigerSource);
        /*enable hw triger mode*/
        ADC_COFNIG_HW_TRIGER_EN(hADC->device, ENABLE);
        if (hADC->pestrigersrcfunc != NULL)
        {
            hADC->pestrigersrcfunc(hADC);
        }
    }
    else
    {
        /*sw triger*/
        if (hADC->trigerMode == ADC_SW_TRIGER)
        {
            /*disable hw triger mode, means enable sw triger mode */
            ADC_COFNIG_HW_TRIGER_EN(hADC->device, DISABLE);
            ADC_SW_TRIGGER(hADC->device);
        }
    }
#elif (defined(CHIP_GM6601) || defined(CHIP_GM6611))
    /*6601 and 6611 only support sw triger*/
    ADC_SW_TRIGGER(hADC->device);
#endif
    return RET_OK;
}

static RET_CODE ADC_SelectChannel(ADC_Device_T *pDevice, uint8_t *pSequenceChannel, uint8_t length)
{
    uint8_t i = 0 ;


    if (!((IO_BITS_GET(pDevice->CTRL, ADC_CTRL_WM) == ADC_SINGLE_MODE << ADC_CTRL_WM_POS)))
    {
        ADC_PR_ERR("Error Work Mode to Set channel,please set work mode first\n");
        return RET_ERROR ;
    }

    /* set sample channel depth */
    SET_CHANNEL_FIFO_DEPTH(pDevice, length);
#if (defined(CHIP_GM6611) || defined(CHIP_GM6621) || defined(CHIP_GM6611B))
    /* Disable channel FIFO */
    if (IO_BITS_GET(pDevice->IE, ADC_IE_EOCE) && (length == 0))
    {
        ADC_SET_CHANNEL_INDEX(pDevice, 0);
    }
#endif
    /* choose sample channel */
    for (i = 0; i < length; i++)
    {
        ADC_SET_CHANNEL_INDEX(pDevice, pSequenceChannel[i]);
    }

    return RET_OK;
}

#if defined(CHIP_GM6621)
RET_CODE ADC_SelectExternChannel(ADC_Handle_T *hADC, uint8_t *pExternSequenceChannel, uint8_t length)
{
    uint8_t i = 0 ;
    unsigned int externchannel = 0;
    ADC_Device_T *pDevice = hADC->device;

    for (i = 0; i < length; i++)
    {
        ADC_SET_CHANNEL_INDEX(pDevice, pExternSequenceChannel[i]);
        externchannel |= 1 << pExternSequenceChannel[i];
        ADC_SET_EXCHEN_CHANNEL_INDEX(pDevice, externchannel);
    }

    return RET_OK;
}
#endif

static uint32_t ADC_ConvertToResolutionValue(ADC_Resolution_T resolution, uint32_t data)
{
#if 0//(!defined(CHIP_GM6621))
    if (resolution == ADC_RESOLUTION_10BIT)
    {
        /*ADC_DRST MSB is Left Aligned.we Need adjust to Right Aligned*/
        data = data >> 2;
    }
    else if (resolution == ADC_RESOLUTION_8BIT)
    {
        /* ADC_DRST MSB is Left Aligned.we Need adjust to Right Aligned */
        data = data >> 4;
    }
#endif

    return data;
}


/*cpu read fifo convert*/
static void ADC_FifoConvertValue(ADC_Device_T *pDevice, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length)
{
    uint8_t i = 0 ;
    uint32_t val = 0;

    for (i = 0; i < length; i++)
    {
        /*only low 12bit vaild data*/
        val = (pDevice->DRST & 0xfff);
        pConvertData[i] = ADC_ConvertToResolutionValue(resolution, val);
    }
}

/*dma read buf convert*/
static void ADC_BufConvertValue(ADC_Device_T *pDevice, ADC_Resolution_T resolution, uint32_t *pConvertData, uint8_t length)
{
    uint8_t i = 0 ;

    for (i = 0; i < length; i++)
    {
        pConvertData[i] = ADC_ConvertToResolutionValue(resolution, pConvertData[i]);
    }
}

static RET_CODE ADC_DmaConfig(ADC_Handle_T *hADC, uint32_t *pConvertData, uint8_t length)
{
    RET_CODE ret = RET_OK;
    DMA_Config_T dmaConfig;
    DMA_Config_T *pDMAConfig  = &dmaConfig;
    ADC_Device_T *pDevice     = hADC->device;

    /* Data fifo have one data will triger dma to transfer */
    ADC_DFIFO_PREFULL_NUM(pDevice, ADC_DFIFO_PRE_FULL_NUM_1);

    memset(pDMAConfig, 0, sizeof(DMA_Config_T));
    pDMAConfig->callback              = ADC_DmaCallback;
    pDMAConfig->destIncrement         = DMA_ADDR_INCREMENT;
    pDMAConfig->destHandshake         = DMA_SOFTWARE_HANDSHAKE;
    pDMAConfig->destWidth             = DMA_WIDTH_32BIT;
    pDMAConfig->srcIncrement          = DMA_ADDR_CONSTANT;
    pDMAConfig->srcHandshake          = DMA_HARDWARE_HANDSHAKE;
    pDMAConfig->srcWidth              = DMA_WIDTH_32BIT;
    pDMAConfig->direction             = DMA_PERIPH_TO_MEMORY;
#if defined(CHIP_GM6621)
    dmaConfig.requestType = DMA_REQUEST_ADC_RX;
    dmaConfig.chnPriority = DMA_CHN_PRIORITY_1;
    dmaConfig.srcReload = dmaConfig.destReload = DMA_NORMAL_MODE;
    dmaConfig.srcGather = DMA_GATHER_DISABLE;
    dmaConfig.srcGatherInterval = 0;
    dmaConfig.srcGatherCount = 0;
    dmaConfig.destScatter = DMA_SCATTER_DISABLE;
    dmaConfig.destScatterInterval = 0;
    dmaConfig.destScatterCount = 0;
#else
    pDMAConfig->srcHwHSChannel        = DMA_ADC_RX_CHANNEL;
#endif
    DMA_Init(DMA0, pDMAConfig, hADC->pDMAHandle);
    LINK_DMAHDL_TO_PARENTHDL(hADC, pDMAHandle, s_DMAHandle);

    ret = DMA_Start(hADC->pDMAHandle, (uint32_t)(ADC_READ_FIFO_PHY_ADDRESS), \
                    (uint32_t)(pConvertData), length);
    return ret;
}


/**
 * 1: set workmode
 * 2: enable Analog input
 */
static RET_CODE ADC_Start(ADC_Handle_T *hADC)
{
    RET_CODE ret = RET_OK;

    if (ADC_TryLock(hADC) != RET_OK)
    {
        return RET_BUSY;
    }

    if (!((hADC->status == STOP_STATUS) || (hADC->status == INIT_STATUS)))
    {
        ADC_PR_ERR("ERR:usr call this Function not match Machinestatus\n");
        ret = RET_ERROR;
        goto out;
    }
    /* setwork mode */
    CONFIG_CONTINOUS_MODE(hADC->device, hADC->continuousMode);
#if   defined(CHIP_GM6601)
    /* enable analog intput */
    ADC_ANALOG_ENABLE(hADC->device);
#elif (defined(CHIP_GM6611) || defined(CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6621) ||  defined(CHIP_GM6611B))
    /* enabel adc */
    ADC_ENABLE(hADC->device);
#endif

    hADC->status = INIT_STATUS;

out:
    ADC_Unlock(hADC);
    return ret;
}


static RET_CODE ADC_Stop(ADC_Handle_T *hADC)
{
    RET_CODE ret = RET_OK;

    if (ADC_TryLock(hADC) != RET_OK)
    {
        return RET_BUSY;
    }

    if (hADC->status == STOP_STATUS)
    {
        ADC_PR_ERR("ERR:usr call this Function not match Machinestatus\n");
        ret = RET_ERROR;
        goto out;
    }

#if defined(CHIP_GM6601)
    /* disable analg input */
    ADC_ANALOG_DISABLE(hADC->device);
#elif (defined(CHIP_GM6611) || defined(CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6621) ||  defined(CHIP_GM6611B))
    /* disabel adc */
    ADC_DISABLE(hADC->device);
#endif
    /* set workmode to stop mode and clear all fifo */
    CONFIG_CONTINOUS_MODE(hADC->device, ADC_STOP_WORK_MODE);
    hADC->status = STOP_STATUS;

out:
    ADC_Unlock(hADC);
    return ret;
}

RET_CODE ADC_DeInit(ADC_Device_T *pADC, ADC_Handle_T *hADC)
{
    RET_CODE ret = RET_OK;
    ret = ADC_Stop(hADC);
    hADC->device = NULL ;
    hADC->callback = NULL;

    return ret;
}

RET_CODE ADC_ChannelConfig(ADC_Handle_T *hADC, ADC_ChannelConfig_T *pConfig)
{
    RET_CODE ret = RET_OK;
    ADC_Device_T *pDevice = hADC->device;

    if (ADC_TryLock(hADC) != RET_OK)
    {
        ret = RET_BUSY;
        goto out;
    }
    /**
     * remember channelIndex length and resolution for
     * reconfig those value when start ADC next time,
     * because those value will lose when last adc finish
     */
    hADC->pSequence[0] = pConfig->channelIndex;
    hADC->resolution = pConfig->resolution;
    hADC->length = 1;
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
    if (IO_BITS_GET(pDevice->IE, ADC_IE_EOCE))
    {
        /*Disabled CFIFO depth*/
        hADC->length = 0;
    }
#endif
#if defined(CHIP_GM6621)
    if (IO_BITS_GET(pDevice->IE, ADC_IE_EOCE) && (hADC->enableDisconMode != ENABLE))
    {
        hADC->length = 0;
    }
#endif
    /* sampleTime and resolution */
    ADC_CONFIG_RESOLUTION(pDevice, pConfig->resolution);
    ADC_CONFIG_SAMPLETIME(pDevice, pConfig->sampleTime);

#if (defined(CHIP_GM6611) || defined(CHIP_GM6601B) ||defined(CHIP_F6601C)|| defined (CHIP_GM6621) || defined(CHIP_GM6611B))
    ADC_CONFIG_DFIFOFW(pDevice, pConfig->DataFifoFullWrEn);
#if  defined(CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6611B)
    ADC_CONFIG_SAMPEDGE(pDevice, pConfig->sampleEdge);
#endif
#endif
    /* config channel fifo data and channel fifo length */
    ret = ADC_SelectChannel(pDevice, hADC->pSequence, hADC->length);
    if (ret != RET_OK)
    {
        ADC_PR_ERR("%s:Err:ADC_SelectChannel\n", __FUNCTION__);
    }
    ADC_Unlock(hADC);

out:
    return ret;
}

RET_CODE ADC_SequenceConfig(ADC_Handle_T *hADC, ADC_SequenceConfig_T *pConfig)
{
    RET_CODE ret = RET_OK;
    ADC_Device_T *pDevice = hADC->device;

    if (pConfig->length > SAMPLE_SEQUENCE_MAX_LENGTH)
    {
        ret = RET_INVPARAM;
        goto out;
    }
    /**
     * remember channelIndex length and resolution for
     * reconfig those value when start ADC next time,
     * because those value will lose when last adc finish
     */
    memcpy(hADC->pSequence, pConfig->pSequence, pConfig->length);
    hADC->resolution = pConfig->resolution;
    hADC->length = pConfig->length;
    /* sampleTime and resolution */
    ADC_CONFIG_RESOLUTION(pDevice, pConfig->resolution);
    ADC_CONFIG_SAMPLETIME(pDevice, pConfig->sampleTime);
#if (defined(CHIP_GM6611) || defined(CHIP_GM6601B)||defined(CHIP_F6601C) || defined (CHIP_GM6621) || defined(CHIP_GM6611B))
    ADC_CONFIG_DFIFOFW(pDevice, pConfig->DataFifoFullWrEn);
#if  defined(CHIP_GM6601B) ||defined(CHIP_F6601C)|| defined(CHIP_GM6611B)
    ADC_CONFIG_SAMPEDGE(pDevice, pConfig->sampleEdge);
#endif
#endif
    /* config channel fifo data and channel fifo length */
    ret = ADC_SelectChannel(pDevice, hADC->pSequence, hADC->length);
    if (ret != RET_OK)
    {
        ADC_PR_ERR("%s:Err:ADC_SelectChannel\n", __FUNCTION__);
    }

    ADC_Unlock(hADC);

out:
    return ret;
}

/*#define ACMP_TRIGER_ADC_FUNCTION*/
int GPIO_TRIGER_ADC(void)
{
#if defined(CHIP_GM6611B)
#if defined(GPIO_TRIGER_ADC_FUNCTION)
    static int cnt = 0;
    int pin_num = 0;
    int ret = 0;
    GPIO_PinConfig_T GPIO_InitStruct;

    if (cnt == 0)
    {
        cnt++;
        GPIOA->TRIGEN |= 1 << 1;
        GPIO_InitStruct.pin = GPIO_PIN_0;
        GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = 0;
        ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        GPIO_InitStruct.pin = GPIO_PIN_1;/*GPIOA1 triger adc,GPIOA0 connect GPIOA1 by wire*/
        GPIO_InitStruct.mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = 0;
        ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }
    else
    {
        GPIOA->ITSR &= ~(1 << 1);
        GPIOA->IPSR |= (1 << 1);
        GPIOA->TRIGEN &= ~(1 << 1);
        SysTick_DelayMS(100);
        GPIOA->TRIGEN |= 1 << 1;
    }

    pin_num = 0 + 0 * 16;/*GPIOA0 output*/

    GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_SET);
    SysTick_DelayMS(100);
    /*GPIO_PinWrite(GPIO_BIT_ADDR(pin_num), GPIO_PIN_RESET);*/
#endif
#endif

    return RET_OK;
}

int ACMP_TRIGER_ADC(void)
{
#if defined(CHIP_GM6611B)
#if defined(ACMP_TRIGER_ADC_FUNCTION)
    AcmpModeMultiIntFunc(0, 128);
#endif
#endif

    return RET_OK;
}

int RTC_TRIGER_ADC(void)
{
    return RET_OK;
}

#if defined(CHIP_GM6611B)
int HARDWARE_TRIGER_ADC(ADC_TrigerSource_T sel)
{

    if (sel == ADC_ACMP_TRIGER)
    {
        ACMP_TRIGER_ADC();
    }
    else if (sel == ADC_GPIO_TRIGER)
    {
        GPIO_TRIGER_ADC();
    }
    else if (sel == ADC_RTC2_TRIGER)
    {
        RTC_TRIGER_ADC();
    }
    else
    {

    }

    return RET_OK;
}
#endif

RET_CODE ADC_ReadConvertValue(ADC_Handle_T *hADC, uint32_t *pConvertData, uint32_t timeout)
{

    RET_CODE ret = RET_OK;
    uint32_t data, i;
    ADC_Device_T *pDevice = hADC->device;
    WaitTimer_T waitTimer;
    uint8_t length = hADC->length;

#if defined(CHIP_GM6621)
    if (hADC->enableDisconMode == ENABLE)
    {
        length = 1;
    }
#endif
    /* remember parameter to hADC for Irq Using */
    hADC->pConvertData = pConvertData;

    if (ADC_TryLock(hADC) != RET_OK)
    {
        ret = RET_BUSY;
        goto out;
    }

    if (timeout == TIMEOUT_NO_BLOCKING)
    {
        /* noblock */
        hADC->blockStatus = NOBLOCK;
    }
    else
    {
        /* block*/
        hADC->blockStatus = BLOCK;
    }

#if 0
#if  defined(CHIP_GM6621)
    ADC_OV_SAMPLE_RATIO(pDevice, 3);
    ADC_OV_SAMPLE_SHIFT(pDevice, 8);
    ADC_OV_SAMPLE_ENABLE(pDevice);
#endif
#endif

    if (ADC_IDLE_STATUS(pDevice, ADC_ST_BUSY))
    {
        ADC_Unlock(hADC);
        return RET_ONGOING;
    }
    else
    {
        for (i = 0; i < MAX_SEQUENCE_NUM; i++)
        {
            if (ADC_DFIFO_EMPTY_STATUS(pDevice, ADC_ST_DFIFO_EMPTY))
            {
                break;
            }
            data = pDevice->DRST;
        }
        hADC->status = IDLE_STATUS;
    }

    /* config dma */
    if (hADC->expectFinishStatus == DMA_FINISH_STATUS)
    {
        ret = ADC_DmaConfig(hADC, pConvertData, length);
        if (ret != RET_OK)
        {
            goto unlock;
        }
    }
    /**
        * 1: when set single mode: need set fifo,chanel config
        *    and sw triger every time to adc convert
        * 2: when set repeat mode : only need the fisrt time(machine
        *    state is init_status)to set fifo,channel config
        *    and sw triger to adc convert,unless stop adc.
        */
    if ((hADC->continuousMode == ADC_SINGLE_MODE))
    {
        /* set channels index to cfifo */
        ret = ADC_SelectChannel(pDevice, hADC->pSequence, hADC->length);
        if (ret != RET_OK)
        {
            goto unlock;
        }

        ret = ADC_Triger(hADC);
        if (ret != RET_OK)
        {
            return ret;
        }
    }
    else
    {
#if defined(CHIP_GM6621)
        if (hADC->enableDisconMode == ENABLE)
        {
            ret = ADC_Triger(hADC);

            if (ret != RET_OK)
            {
                return ret;
            }
        }
#endif
    }
    /* Initialize timeout structure */
    WaitTimerInit(&waitTimer, timeout, 1);

    /* read convert */
    if (hADC->blockStatus == BLOCK)
    {
        while (1)
        {
            /* Exit if adc convert finish */
            if (hADC->status == hADC->expectFinishStatus)
            {
                break;
            }
            /* Exit when timeout */
            if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
            {
                ret = RET_TIMEOUT;
                goto unlock;
            }
        }

        /* store convet value to pConvertData */
        if (hADC->expectFinishStatus == CPU_FINISH_STATUS)
        {
#if defined(CHIP_GM6621)
            if (hADC->enableDisconMode == ENABLE)
            {
                length = 1;
            }
#endif
            /*cpu mode*/
            ADC_FifoConvertValue(pDevice, hADC->resolution, pConvertData, length);
        }
        else
        {
            /* dma mode*/
            ADC_BufConvertValue(pDevice, hADC->resolution, pConvertData, length);
        }

        hADC->status = IDLE_STATUS;
    }
    else
    {
        /**
            * noblock mode read
            * 1: after send callback should set hADC->status = IDLE_STATUS
            * 2: after send callback should unlock handlelock ,so not to unlock it here.
            */
        hADC->status = BUSY_STATUS;
        return RET_ONGOING;
    }

unlock:
    ADC_Unlock(hADC);
out:

    return ret;
}

RET_CODE ADC_CompareConfig(ADC_Handle_T *hADC, ADC_CompareConfig_T *pCompareConfig)
{
    RET_CODE ret = RET_OK;
    ADC_Device_T *pDevice = hADC->device;

    if (pCompareConfig->compareLowValue >=  pCompareConfig->compareHighValue)
    {
        ADC_PR_ERR("Err:Usr Set Err compareLowValue more than compareHighValue\n");
        ret = RET_INVPARAM;
        goto out;
    }

    if (ADC_TryLock(hADC) != RET_OK)
    {
        ret = RET_BUSY;
        goto out;
    }

    ADC_COMPARE_ENABLE(pDevice);
    ADC_SET_COMPARE_MODE(pDevice, pCompareConfig->compareMode);
    ADC_SET_COMPARE_VALUE(pDevice, pCompareConfig->compareLowValue,  \
                          pCompareConfig->compareHighValue);

    ADC_Unlock(hADC);

out:
    return ret;
}


RET_CODE ADC_CompareModeEnable(ADC_Handle_T *hADC)
{
    RET_CODE ret = RET_OK;
    ADC_Device_T *pDevice = hADC->device;

    if (ADC_TryLock(hADC) != RET_OK)
    {
        ret = RET_BUSY;
        goto out;
    }

    ADC_COMPARE_ENABLE(pDevice);

    ADC_Unlock(hADC);

out:
    return ret;
}

RET_CODE ADC_CompareModeDisable(ADC_Handle_T *hADC)
{
    RET_CODE ret = RET_OK;
    ADC_Device_T *pDevice = hADC->device;

    if (ADC_TryLock(hADC) != RET_OK)
    {
        ret = RET_BUSY;
        goto out;
    }

    ADC_COMPARE_DISABLE(pDevice);

    ADC_Unlock(hADC);

out:
    return ret;
}
