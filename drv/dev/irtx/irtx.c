/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file          irtx.c
*
* @author        Mike.Zheng
*
* @version       1.1.0
*
* @date          2017/04/05
*
* @brief         Header file of irtx driver.
*
* @note
*                2017/04/05, Mike.Zheng, V1.0.0
*                    Initial version.
*                2018/02/28, Edward.Zhu, V1.1.0
*                    Update coding style.
*/

#include <drv/dev/irtx.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
STATIC_ISR(IRTX_IRQn, IRTX_IrqHandler);

/*******************************************************************************
* Global variables
*******************************************************************************/
IRTX_Callback_T s_irtxCallback = NULL;

/*******************************************************************************
* Static variables
*******************************************************************************/
static IRTX_Handle_T *s_pHandle = NULL;

/*******************************************************************************
* Local function prototypes
*******************************************************************************/
/**
 * @brief IRTX interrupt request handler.
 */
static void IRTX_IrqHandler(void);

/**
 * @brief IRTX unlock initial.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 */
static void IRTX_Unlock(IRTX_Handle_T *hIRTX);

/**
 * @brief IRTX lock initial.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 */
static void IRTX_LockInit(IRTX_Handle_T *hIRTX);

/**
 * @brief IRTX trylock initial.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 */
static RET_CODE IRTX_TryLock(IRTX_Handle_T *hIRTX);

/**
 * @brief IRTX interrupt basic configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 */
static void IRTX_InterruptConfig(IRTX_Handle_T *hIRTX);

/**
 * @brief IRTX carrier wave level basic configuration.
 *
 * @param pDevice    IRTX peripheral base address.
 */
void static IRTX_ConfigCarrierLevel(IRTX_Device_T *pDevice) ;

/**
 * @brief Set IRTX initial level.
 *
 * @param pDevice   IRTX peripheral base address.
 *
 * @param level     high or low.
 */
static void IRTX_SetInitLevel(IRTX_Device_T *pDevice, uint8_t level);

/**
 * @brief IRTX carrier wave output configuration.
 *
 * @param pDevice    IRTX peripheral base address.
 */
static void IRTX_ConfigCarrier(IRTX_Device_T *pDevice);

/**
 * @brief IRTX trigger mode configuration.
 *
 * @param pDevice    IRTX peripheral base address.
 *
 * @param mode       IRTX trigger mode
 */
static void IRTX_ConfigTriggerMode(IRTX_Device_T *pDevice, IRTX_TriggerMode_T mode);

/**
 * @brief IR signal output wave basic control initial.
 *
 * @param pDevice    IRTX peripheral base address.
 *
 * @param pConfig    Pointer to the configuration structure.
 */
static void IRTX_WaveBasicContrlInit(IRTX_Device_T *pDevice, IRTX_Config_T *pConfig);

/**
 * @brief Get Verify Code by referring Gree FN protol spec .
 *
 * @param data0     data0 code
 *
 * @param data1     data1 code
 *
 * @param cx        custome code
 */
static uint8_t IRTX_CalcFnCodeVerify(uint32_t data0, uint32_t data1, uint8_t cx);

/**
 * @brief IRTX FN code's level configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 *
 * @param pDataFormatFN    pointer to FN(F0 F1 F2 F3 F4 F5 F10) code protocol format.
 *
 * @param protocol     Gree Ir proticol type.
 *
 * @param index     Index of all uart constant table for irq, dma channel, ...
 */
static void IRTX_ConfigFNCode(IRTX_Handle_T *hIRTX, IRTX_FormatFN_T *pDataFormatFN,
                              IRTX_Protocol_T protocol, uint8_t index);

/**
 * @brief IRTX FM code's level configuration.
 *
 * @param hIRTX    pointer to IRTX_Handle_T structure.
 *
 * @param pDataFormatFM   pointer to FM code protocol format.
 *
 * @param index    Index of all uart constant table for irq, dma channel, ...
 */
static void IRTX_ConfigFMCode(IRTX_Handle_T *hIRTX, IRTX_FormatFM_T *pDataFormatFM, uint8_t index);

/**
 * @brief IRTX F0FM code configuration.
 *
 * @param hIRTX       pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc    pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0FMCode(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F1 code configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc   pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F1Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F1FM code configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc   pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F1FMCode(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F2 code configuration.
 *
 * @param hIRTX    pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc   pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F2Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F2F3 code configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc    pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F2F3Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F1F2 code configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc   pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F1F2Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F1F2F3 code configuration.
 *
 * @param hIRTX    pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc    pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F1F2F3Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F1F2F3F5 code configuration.
 *
 * @param hIRTX    pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc   pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F1F2F3F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F2F5 code configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc    pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F2F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F1F2F5 code configuration.
 *
 * @param hIRTX      pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc    pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F1F2F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX F0F2F3F5 code configuration.
 *
 * @param hIRTX     pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc   pointer to the keyval of FM/FN code wave.
 */
static void IRTX_ConfigF0F2F3F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/**
 * @brief IRTX module prepare sending uint wave.
 *
 * @param hIRTX    pointer to IRTX_Handle_T structure.
 *
 * @param uintIndex    uintIndex of all uart constant table for irq, dma channel, ...
 */
static void IRTX_PrepareSendUintWave(IRTX_Handle_T *hIRTX, int uintIndex);

/**
 * @brief IRTX module FM/FN code wave parament configuration.
 *
 * @param hIRTX pointer to IRTX_Handle_T structure.
 *
 * @param pKeyDesc pointer to the keyval of FM/FN code wave.
 */
static RET_CODE IRTX_WaveParamConfig(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc);

/*******************************************************************************
* Codes
*******************************************************************************/

static void IRTX_SetCLk()
{
    PeriphClk_Config_T clkConfig;
    memset(&clkConfig, 0, sizeof(PeriphClk_Config_T));
    clkConfig.periphClkMask = CLOCK_PERIPH_IRTX_MASK;

    /* select FCLK as IRTX's clock source */
    clkConfig.irtx_config_t.irtxSrcClock = 0;
    clkConfig.irtx_config_t.irtxDivider = (SYS_CLOCK / 1000000) - 1;

    PeriphClk_SetConfig(&clkConfig);
}

RET_CODE IRTX_Init(IRTX_Device_T *pIRTX, IRTX_Config_T *pConfig, IRTX_Handle_T *hIRTX)
{
    RET_CODE ret = RET_OK;

    if (!(pIRTX && pConfig && hIRTX))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    memset(hIRTX, 0, sizeof(IRTX_Handle_T));
    s_pHandle = hIRTX;
    hIRTX->device = IRTX_DEV;
    hIRTX->triggerMode = pConfig->triggerMode;
    s_irtxCallback = pConfig->callback;

    IRTX_RESET();
    IRTX_LockInit(hIRTX);
    IRTX_InterruptConfig(hIRTX);

    if (pConfig->workMode == IRTX_IR)
    {
        IRTX_WaveBasicContrlInit(pIRTX, pConfig);

    }
    /*enable irtx ,disable pwm and time*/
    IRTX_ENABLE(pIRTX);
out:
    return ret;

}

RET_CODE IRTX_SendKey(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc, uint32_t timeoutUs)
{
    RET_CODE ret = RET_OK;
    WaitTimer_T waitTimer;
    IRTX_TriggerMode_T triggerMode = hIRTX->triggerMode;

    if (IRTX_TryLock(hIRTX) != RET_OK)
    {
        ret = RET_BUSY;
        hIRTX->status = IRTX_BUSY;
        goto out;
    }
    /*
        * set status to idle befor send key,hIRTX->status
        * use to sync machine for block mode to send key
        */
    hIRTX->status = IRTX_IDLE;

    if ((timeoutUs == TIMEOUT_NO_BLOCKING) || (triggerMode != IRTX_SW_TRIGGER))
    {
        /*
         * select hw trigger mode, driver wil proess sendkey use
         * noblock mode
         */
        hIRTX->blockStatus = IRTX_NOBLOCK;
    }
    else
    {
        hIRTX->blockStatus = IRTX_BLOCK;
    }

    /* Initialize timeout structure */
    WaitTimerInit(&waitTimer, timeoutUs, 1);
    /* config the wave param according to usr keydes */

    IRTX_WaveParamConfig(hIRTX, pKeyDesc);
    /*
     * noblock mode only send 1st unit wave, the remain wave
     * will send at isr
     */
    IRTX_PrepareSendUintWave(hIRTX, 0);

    if (triggerMode == IRTX_SW_TRIGGER)
    {
        /*
         * only select sw mode need triger.
         * other mode will tiger by hw automatic
         */
        IRTX_SW_TRIGGER(hIRTX->device);
    }

    if (hIRTX->blockStatus == IRTX_BLOCK)
    {
        while (1)
        {
            /* Exit if adc convert finish */
            if (hIRTX->status == IRTX_SEND_FINISH)
            {
                break;
            }
            /* Exit when timeout */
            if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
            {
                ret = RET_TIMEOUT;
                IRTX_PR_ERR("%s: Timeout\n", __FUNCTION__);
                goto unlock;
            }
        }

    }
    else
    {
        /**
            * noblock mode read
            * 1: after send callback should set hIRTX->status = IDLE_STATUS
            * 2: after send callback should unlock handlelock ,so not to unlock it here.
            */
        hIRTX->status = IRTX_BUSY;
        return RET_ONGOING;
    }

unlock:
    IRTX_Unlock(hIRTX);
out:

    return ret;
}

static void IRTX_LockInit(IRTX_Handle_T *hIRTX)
{
    hIRTX->lock = IRTX_UNLOCKED;
}

static void IRTX_Unlock(IRTX_Handle_T *hIRTX)
{
    disable_interrupt;

    hIRTX->lock = IRTX_UNLOCKED;

    enable_interrupt;
}


static RET_CODE IRTX_TryLock(IRTX_Handle_T *hIRTX)
{
    RET_CODE ret = RET_OK;

    disable_interrupt;

    if (hIRTX->lock == IRTX_LOCKED)
    {
        ret =  RET_BUSY;
        goto out;
    }
    else
    {
        hIRTX->lock = IRTX_LOCKED;
    }

out:
    enable_interrupt;

    return ret;
}



static void IRTX_IrqHandler(void)
{
    IRTX_Handle_T *hIRTX = s_pHandle;
    IRTX_Device_T *pDevice = hIRTX->device;
    static uint8_t uintSendIndex = 1;
    static uint8_t uintDoneIndex = 0;
    uint8_t totalSegment = hIRTX->wave.validSegmentLength ;

    if (IO_BITS_GET(pDevice->ST, IRTX_ST_TIME) && \
            IO_BITS_GET(pDevice->IE, IRTX_IE_TIME))
    {
        IRTX_CLEAR_INTERRUPT(pDevice, IRTX_IE_TIME);
        if (s_irtxCallback)
        {
            /*except time cnt will send callback to usr*/
            s_irtxCallback(hIRTX, IRTX_EVEN_TIME, 0);
        }
    }
    else if (IO_BITS_GET(pDevice->ST, IRTX_ST_ENDPOS) && \
             IO_BITS_GET(pDevice->IE, IRTX_IE_ENDPOS))
    {
        IRTX_CLEAR_INTERRUPT(pDevice, IRTX_INTCLR_ENDPOS);
        if (uintSendIndex < totalSegment)
        {
            /* only send unit wave from 2st to (totalSegment-1)*/
            IRTX_PrepareSendUintWave(hIRTX, uintSendIndex++);
        }
    }

    /* only process endpos and done irq */
    else if (IO_BITS_GET(pDevice->ST, IRTX_ST_DONE) && \
             IO_BITS_GET(pDevice->IE, IRTX_IE_DONE))
    {

        uintDoneIndex++;

        IRTX_CLEAR_INTERRUPT(pDevice, IRTX_INTCLR_DONE);
        /*
              * when tx finish, reset segmentIndex to
              * prepare next time to send
              */
        if (uintDoneIndex == totalSegment)
        {
            if ((s_irtxCallback) && (hIRTX->blockStatus == IRTX_NOBLOCK))
            {
                /*
                        * noblock mode prevent user to call IRTX_SendKey at usr callback
                        * to generate dead lock,so must unlock first.
                        */
                uintDoneIndex = 0;
                uintSendIndex = 1;
                hIRTX->status = IRTX_SEND_FINISH;
                IRTX_Unlock(hIRTX);

                s_irtxCallback(hIRTX, IRTX_EVEN_SEND_SUCCESS, 0);
            }
            else
            {
                /*
                        * block mode not send callback event to usr
                        * only change status value to wakeup block
                        * mode to sendkey
                        */
                uintDoneIndex = 0;
                uintSendIndex = 1 ;
                hIRTX->status = IRTX_SEND_FINISH;
            }

        }
    }
    else
    {
        IRTX_PR_ERR("%s: Error Irq Status\n", __FUNCTION__);
    }
}

static void IRTX_InterruptConfig(IRTX_Handle_T *hIRTX)
{
    /* only enable done and posdataend irq */
    IRTX_INTERRUPT_ENABLE(hIRTX->device, (IRTX_IE_DONE | IRTX_IE_ENDPOS | IRTX_IE_TIME));
    NVIC_EnableIRQ(IRTX_IRQn);
}

static void IRTX_SetInitLevel(IRTX_Device_T *pDevice, uint8_t level)
{
    IRTX_ENABLE_OEN(pDevice);
    /* set data0 to low level */
    IRTX_CONFIG_DATALEVEL(pDevice, level);
}


/* config carrier wave */
static void IRTX_ConfigCarrier(IRTX_Device_T *pDevice)
{
    uint32_t prc = (1000000 / CARRYER_FREQUENCY) / (IRTX_SRC_FREQUENCY / 1000000) - 1;
    //  uint32_t pwc = (prc * 9 / 10)-1;
    uint32_t pwc = (prc * 30 / 100) - 1;
    pDevice->PWMCTR = ((pwc << IRTX_PWMCTR_PWC_POS) | (prc));

}

static void IRTX_ConfigTriggerMode(IRTX_Device_T *pDevice, IRTX_TriggerMode_T mode)
{
    if (mode == IRTX_SW_TRIGGER)
    {
        /* sw triger */
        IRTX_CONFIG_HW_TRIGGER_EN(pDevice, DISABLE);
    }
    else
    {
        /* irtx hw triger */
        IRTX_CONFIG_TRIGGER_EVENT(pDevice, mode);
        /*irtx selcet src master*/
        IRTX_CONFIG_HW_TRIGGER_EN(pDevice, ENABLE);
        /*config pes mast*/
        PES_EVENT_CONFIG();
    }
}

void static IRTX_ConfigCarrierLevel(IRTX_Device_T *pDevice)
{
    /* headl level is low */
    IO_BIT_CLEAR(pDevice->CTRL1, IRTX_CTRL1_PHEADLCWEN);
    /* headh level is high */
    IO_BIT_SET(pDevice->CTRL1, IRTX_CTRL1_PHEADHCWEN);
    /* fninter level is low */
    IO_BIT_CLEAR(pDevice->CTRL1, IRTX_CTRL1_PFNINTERCWEN);
    /*  endpos level is high*/
    IO_BIT_SET(pDevice->CTRL1, IRTX_CTRL1_PENDPOSCWEN);
    /* disable inter */
    IO_BIT_CLEAR(pDevice->CTRL1, IRTX_CTRL1_PINTERCWEN);
    /*  endpre level is high*/
    IO_BIT_SET(pDevice->CTRL1, IRTX_CTRL1_PENDPRECWEN);
}

/* basic reg Init */
static void IRTX_WaveBasicContrlInit(IRTX_Device_T *pDevice, IRTX_Config_T *pConfig)
{
    /*set ir tx work frequency to 1mhz */
    IRTX_SetCLk();
    /* for debug mode should disable carrier output , real useful envrioment will enble this function */
    ITRX_CONFIG_OUTPUT_CARRIER_EN(pDevice, ENABLE);
    /*not send integrity wave */
    ITRX_CONFIG_WAVE_INTEGRITY(pDevice, DISABLE);
    /* config stage level */
    IRTX_ConfigCarrierLevel(pDevice);
    /* config carrier wave */
    IRTX_ConfigCarrier(pDevice);
    /* not mask data bit */
    IRTX_CONFIG_DATABIT_MASK(pDevice, NO_BIT_MASK);
    /* not intvert output level */
    IRTX_DIR_REVERSAL_EN(pDevice, DISABLE);
    /* set fifo lowest bit tansfer first */
    IRTX_CONFIG_SEND_FIRST(pDevice, LOWEST_FIRST);
    /* set init output level is low */
    IRTX_SetInitLevel(pDevice, FALSE);
    /* set work mode to normal IR mode  */
    if (pConfig->workMode == IRTX_IR)
    {
        IRTX_SET_WORKMODE(pDevice, NOARMAL_MODE);
    }
    /* set interval prescale */
    SET_INTERVAL_FNINTERVAL_PRESCALE(pDevice, INTERVAL_FNINTERVAL_PRESCALE);
    /* set ir trige mode */
    IRTX_ConfigTriggerMode(pDevice, pConfig->triggerMode);
    /* init to flush fifo */
    IRTX_FLUSH_FIFO(pDevice);

}

static uint8_t IRTX_CalcFnCodeVerify(uint32_t data0, uint32_t data1, uint8_t cx)
{
    uint8_t  i = 0;
    uint8_t  verify = 0; /* bit[3:0]  is vaild */
    uint8_t  temp = 0;
    /**
     * Get Verify Code: please refer Gree FN protol spec
     * calc Nibble1 Nibble3 Nibble5 Nibble7 :data0 code
     */
    for (i = 0; i < 7; i += 2)
    {
        temp = (data0 & (0xf << (4 * i))) >> (4 * i);
        verify += temp;
    }
    /* calc Nibble9 :custome code */
    temp = ((cx & 0x70) >> 4) | (1 << 3);
    verify += temp;

    /* calc Nibble11 Nibble13 Nibble15 :data1 code*/
    for (i = 1; i < 7; i += 2)
    {
        temp = (data1 & (0xf << (4 * i))) >> (4 * i);
        verify += temp;
    }
    /* get the resulet of FN protol verify : Nibble17 4bit */
    verify &= 0xf;

    return verify;
}

static void IRTX_ConfigFNCode(IRTX_Handle_T *hIRTX, IRTX_FormatFN_T *pDataFormatFN,
                              IRTX_Protocol_T protocol, uint8_t index)
{
    uint32_t data0 = pDataFormatFN->data0;  /* bit[27:0] is valid */
    uint32_t data1 = pDataFormatFN->data1;  /* bit[27:0] is valid */
    uint8_t  cx = 0;     /* bit[6:0]  is valid */
    uint8_t  verify = 0; /* bit[3:0]  is valid */

    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    hIRTX->wave.validSegmentLength = 1;
    /* config data fifo*/
    /* get cx for different fn protocol */
    if (IRTX_PROTOCOL_F0 == protocol)
    {
        cx = IRTX_F0_CUSTOME_CODE;
    }
    else if (IRTX_PROTOCOL_F1 == protocol)
    {
        cx = IRTX_F1_CUSTOME_CODE;
    }
    else if (IRTX_PROTOCOL_F2 == protocol)
    {
        cx = IRTX_F2_CUSTOME_CODE;
    }
    else if (IRTX_PROTOCOL_F3 == protocol)
    {
        cx = IRTX_F3_CUSTOME_CODE;
    }
    else if (IRTX_PROTOCOL_F4 == protocol)
    {
        cx = IRTX_F4_CUSTOME_CODE;
    }
    else if (IRTX_PROTOCOL_F5 == protocol)
    {
        cx = IRTX_F5_CUSTOME_CODE;
    }
    else if (IRTX_PROTOCOL_F10 == protocol)
    {
        cx = IRTX_F10_CUSTOME_CODE;
    }

    verify = IRTX_CalcFnCodeVerify(data0, data1, cx);
    /* fill 28bit data0 to 1st fifo [27:0]*/

    pUintWave[index].dataWave[0] = data0 & 0xfffffff;
    /* fill lowest 4bit for 1st fifo [31:28] */
    pUintWave[index].dataWave[0] |= ((cx & 0xf) << 28);
    /* fill higst 3bit for 2st fifo [2:0] */
    pUintWave[index].dataWave[1] = ((cx & 0x70) >> 4);
    /* fill 28bit data1 for 2st fifo [3:30] */
    pUintWave[index].dataWave[1] |= ((data1 & 0xfffffff) << 3);
    /* fill lowest 1bit for 2st fifo [31] */
    pUintWave[index].dataWave[1] |= ((verify & 0x01) << 31);
    /* fill higset 3bit for 3st fifo [2:0] */
    pUintWave[index].dataWave[2] = ((verify & 0xe) >> 1) ;

    /* config data bitnum*/
    pUintWave[index].preDatBitNum = FN_PRE_DATA_BIT_NUM;
    pUintWave[index].posDatBitNum = FN_POS_DATA_BIT_NUM;

    /*config length*/
    pUintWave[index].logicLWidth.hWidth     = FN_LOGIC0_HENGTH;
    pUintWave[index].logicLWidth.lWidth     = FN_LOGIC0_LENGTH;
    pUintWave[index].logicHWidth.hWidth     = FN_LOGIC1_HENGTH;
    pUintWave[index].logicHWidth.lWidth     = FN_LOGIC1_LENGTH;
    pUintWave[index].headHWidth             = FN_HEADH_LENGTH;
    pUintWave[index].headLWidth             = FN_LEADH_LENGTH;
    pUintWave[index].stopPreWidth           = FN_STOP_PRE_LENGTH;
    pUintWave[index].intervalWidth          = FN_INTER_LENGTH;
    pUintWave[index].stopPosWidth           = FN_STOP_POS_LENGTH;
    pUintWave[index].fninterWidth           = FN_FNINTER_LENGTH;

    /* config mask stage */
    pUintWave[index].stageMask.headHMask    = ENABLE;
    pUintWave[index].stageMask.headLMask    = ENABLE;
    pUintWave[index].stageMask.preDatMask   = ENABLE;
    pUintWave[index].stageMask.endPreMask   = ENABLE;
    pUintWave[index].stageMask.intervalMask = ENABLE;
    pUintWave[index].stageMask.posDatMask   = ENABLE;
    pUintWave[index].stageMask.endPosMask   = ENABLE;
    pUintWave[index].stageMask.fninterMask  = DISABLE;
    pUintWave[index].stageMask.autoTxMask   = DISABLE;
}

static void IRTX_ConfigFMCode(IRTX_Handle_T *hIRTX, IRTX_FormatFM_T *pDataFormatFM, uint8_t index)
{
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    uint8_t verify = 1; /* !!!Todo cala data verigy, dont have fm calc verify spec */
    hIRTX->wave.validSegmentLength = 1;

    pUintWave[index].dataWave[0]  = pDataFormatFM->data;
    /* !!!Todo cala data verigy, dont have fm calc verify sepc */
    pUintWave[index].dataWave[0]  |= verify << 8;

    /* config data*/
    pUintWave[index].preDatBitNum = FM_PRE_DATA_BIT_NUM;
    pUintWave[index].posDatBitNum = FM_POS_DATA_BIT_NUM;

    /*config length*/
    pUintWave[index].logicLWidth.hWidth     = FM_LOGIC0_HENGTH;
    pUintWave[index].logicLWidth.lWidth     = FM_LOGIC0_LENGTH;
    pUintWave[index].logicHWidth.hWidth     = FM_LOGIC1_HENGTH;
    pUintWave[index].logicHWidth.lWidth     = FM_LOGIC1_LENGTH;
    pUintWave[index].headHWidth             = FM_HEADH_LENGTH;
    pUintWave[index].headHWidth             = FM_HEADH_LENGTH;
    pUintWave[index].headLWidth             = FM_LEADH_LENGTH;
    pUintWave[index].stopPreWidth           = FM_STOP_PRE_LENGTH;
    pUintWave[index].intervalWidth          = FM_INTER_LENGTH;
    pUintWave[index].stopPosWidth           = FM_STOP_POS_LENGTH;
    pUintWave[index].stopPosWidth           = 1;
    pUintWave[index].fninterWidth           = FM_FNINTER_LENGTH;

    /* config mask stage */
    pUintWave[index].stageMask.headHMask    = ENABLE;
    pUintWave[index].stageMask.headLMask    = ENABLE;
    pUintWave[index].stageMask.preDatMask   = ENABLE;
    pUintWave[index].stageMask.endPreMask   = ENABLE;
    pUintWave[index].stageMask.intervalMask = DISABLE;
    pUintWave[index].stageMask.posDatMask   = DISABLE;
    pUintWave[index].stageMask.endPosMask   = DISABLE;
    pUintWave[index].stageMask.fninterMask  = DISABLE;
    pUintWave[index].stageMask.autoTxMask   = DISABLE;
}

static void IRTX_ConfigF0FMCode(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_100ms_FM */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;
    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0FMCode.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth  = IRTX_100MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;

    /* config fm stage without fninter */
    IRTX_ConfigFMCode(hIRTX, &(pKeyDesc->KeyCode.F0FMCode.FMCode), 1);

    hIRTX->wave.validSegmentLength = 2;
}

static void IRTX_ConfigF0F1Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_100ms_F1 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth  = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f1 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1Code.F1Code), IRTX_PROTOCOL_F1, 1);

    hIRTX->wave.validSegmentLength = 2;
}

static void IRTX_ConfigF0F1FMCode(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F1_100ms_FM */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1FMCode.F0Code), IRTX_PROTOCOL_F0, 0);
    /* config f0 stage with fninter */
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f1 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1FMCode.F1Code), IRTX_PROTOCOL_F1, 1);
    pUintWave[1].fninterWidth           = IRTX_100MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;
    /* config fm stage without fninter */
    IRTX_ConfigFMCode(hIRTX, &(pKeyDesc->KeyCode.F0F1FMCode.FMCode), 2);

    hIRTX->wave.validSegmentLength = 3;
}

static void IRTX_ConfigF0F2Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F2 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f2 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2Code.F2Code), IRTX_PROTOCOL_F2, 1);

    hIRTX->wave.validSegmentLength = 2;
}

static void IRTX_ConfigF0F2F3Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F2_40ms_F3 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F3Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f2 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F3Code.F2Code), IRTX_PROTOCOL_F2, 1);
    pUintWave[1].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;
    /* config f3 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F3Code.F3Code), IRTX_PROTOCOL_F3, 2);

    hIRTX->wave.validSegmentLength = 3;
}

static void IRTX_ConfigF0F1F2Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F1_40ms_F2 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f1 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2Code.F1Code), IRTX_PROTOCOL_F1, 1);
    pUintWave[1].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;
    /* config f2 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2Code.F2Code), IRTX_PROTOCOL_F2, 2);

    hIRTX->wave.validSegmentLength = 3;
}

static void IRTX_ConfigF0F1F2F3Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F1_40ms_F2_40ms_F3 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f1 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3Code.F1Code), IRTX_PROTOCOL_F1, 1);
    pUintWave[1].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;
    /* config f2 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3Code.F2Code), IRTX_PROTOCOL_F2, 2);
    pUintWave[2].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[2].stageMask.fninterMask  = ENABLE;
    pUintWave[2].stageMask.autoTxMask   = ENABLE;
    /* config f3 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3Code.F3Code), IRTX_PROTOCOL_F3, 3);

    hIRTX->wave.validSegmentLength = 4;
}

static void IRTX_ConfigF0F1F2F3F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F1_40ms_F2_40ms_F3_40ms_F5 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3F5Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f1 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3F5Code.F1Code), IRTX_PROTOCOL_F1, 1);
    pUintWave[1].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;
    /* config f2 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3F5Code.F2Code), IRTX_PROTOCOL_F2, 2);
    pUintWave[2].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[2].stageMask.fninterMask  = ENABLE;
    pUintWave[2].stageMask.autoTxMask   = ENABLE;
    /* config f3 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3F5Code.F3Code), IRTX_PROTOCOL_F3, 3);
    pUintWave[3].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[3].stageMask.fninterMask  = ENABLE;
    pUintWave[3].stageMask.autoTxMask   = ENABLE;
    /* config f5 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F3F5Code.F5Code), IRTX_PROTOCOL_F5, 4);

    hIRTX->wave.validSegmentLength = 5;
}

static void IRTX_ConfigF0F2F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F2_40ms_F5 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F5Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f2 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F5Code.F2Code), IRTX_PROTOCOL_F2, 1);
    pUintWave[1].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;
    /* config f5 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F5Code.F5Code), IRTX_PROTOCOL_F5, 2);

    hIRTX->wave.validSegmentLength = 3;
}

static void IRTX_ConfigF0F1F2F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F1_40ms_F2_40ms_F5 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F5Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;

    /* config f1 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F5Code.F1Code), IRTX_PROTOCOL_F1, 1);
    pUintWave[1].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;

    /* config f2 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F5Code.F2Code), IRTX_PROTOCOL_F2, 2);
    pUintWave[2].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[2].stageMask.fninterMask  = ENABLE;
    pUintWave[2].stageMask.autoTxMask   = ENABLE;

    /* config f5 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F1F2F5Code.F5Code), IRTX_PROTOCOL_F5, 3);

    hIRTX->wave.validSegmentLength = 4;
}


static void IRTX_ConfigF0F2F3F5Code(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    /* wave: F0_40ms_F2_40ms_F3_40ms_F5 */
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;

    /* config f0 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F3F5Code.F0Code), IRTX_PROTOCOL_F0, 0);
    pUintWave[0].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[0].stageMask.fninterMask  = ENABLE;
    pUintWave[0].stageMask.autoTxMask   = ENABLE;
    /* config f2 stage with fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F3F5Code.F2Code), IRTX_PROTOCOL_F2, 1);
    pUintWave[1].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[1].stageMask.fninterMask  = ENABLE;
    pUintWave[1].stageMask.autoTxMask   = ENABLE;

    /* config f3 stage without fninter */
    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F3F5Code.F3Code), IRTX_PROTOCOL_F3, 2);
    pUintWave[2].fninterWidth           = IRTX_40MS_LENGTH;
    pUintWave[2].stageMask.fninterMask  = ENABLE;
    pUintWave[2].stageMask.autoTxMask   = ENABLE;

    IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0F2F3F5Code.F5Code), IRTX_PROTOCOL_F5, 3);

    hIRTX->wave.validSegmentLength = 4;
}

static void IRTX_PrepareSendUintWave(IRTX_Handle_T *hIRTX, int uintIndex)
{
    uint8_t i = 0;
    uint32_t *pDataFifo = hIRTX->wave.uintWave[uintIndex].dataWave;
    IRTX_UintWaveConfig_T *pUintWave = hIRTX->wave.uintWave;
    IRTX_Device_T *pDevice = hIRTX->device;

    /* flush data fifo */
    IRTX_FLUSH_FIFO(pDevice);

    /* config data fifo */
    for (i = 0; i < UINT_FIFO_LENGTH; i++)
    {
        IRTX_SET_FIFO(pDevice, *pDataFifo++);
    }

    /* config valid data bitnum */
    IRTX_CONFIG_DATABITNUM(pDevice, pUintWave[uintIndex].preDatBitNum, pUintWave[uintIndex].posDatBitNum);
    /* config headh and headl length */
    IRTX_CONFIG_HEAD(pDevice, pUintWave[uintIndex].headHWidth, pUintWave[uintIndex].headLWidth);
    /* config logic 0 length */
    IRTX_CONFIG_LOGICL(pDevice, pUintWave[uintIndex].logicLWidth.hWidth, pUintWave[uintIndex].logicLWidth.lWidth);
    /* config logic 1 length */
    IRTX_CONFIG_LOGICH(pDevice, pUintWave[uintIndex].logicHWidth.hWidth, pUintWave[uintIndex].logicHWidth.lWidth);
    /* config end length */
    IRTX_CONFIG_END(pDevice, pUintWave[uintIndex].stopPreWidth, pUintWave[uintIndex].stopPosWidth);
    /* set interval length */
    IRTX_CONFIG_INTERVAL(pDevice, pUintWave[uintIndex].intervalWidth);
    /* set fininter length */
    IRTX_CONFIG_FNINTER(pDevice, pUintWave[uintIndex].fninterWidth);

    /* config mask stage */
    IO_BITMASK_SET(pDevice->CTRL2,                                 \
                   (IRTX_CTRL2_HEADHEN    | IRTX_CTRL2_HEADLEN     | \
                    IRTX_CTRL2_PREDATEN   | IRTX_CTRL2_ENDPREEN    | \
                    IRTX_CTRL2_INTERVALEN | IRTX_CTRL2_POSDATEN    | \
                    IRTX_CTRL2_ENDPOSEN   | IRTX_CTRL2_FNINTEREN), \
                   ((pUintWave[uintIndex].stageMask.headHMask    << IRTX_CTRL2_HEADHEN_POS)   | \
                    (pUintWave[uintIndex].stageMask.headLMask    << IRTX_CTRL2_HEADLEN_POS)   | \
                    (pUintWave[uintIndex].stageMask.preDatMask   << IRTX_CTRL2_PREDATEN_POS)  | \
                    (pUintWave[uintIndex].stageMask.endPreMask   << IRTX_CTRL2_ENDPREEN_POS)  | \
                    (pUintWave[uintIndex].stageMask.intervalMask << IRTX_CTRL2_INTERVALEN_POS) | \
                    (pUintWave[uintIndex].stageMask.posDatMask   << IRTX_CTRL2_POSDATEN_POS)  | \
                    (pUintWave[uintIndex].stageMask.endPosMask   << IRTX_CTRL2_ENDPOSEN_POS)  | \
                    (pUintWave[uintIndex].stageMask.fninterMask  << IRTX_CTRL2_FNINTEREN_POS)));

    /* config auto tx */
    IRTX_CONFIG_AUTO_TX(pDevice, pUintWave[uintIndex].stageMask.autoTxMask);
}

static RET_CODE IRTX_WaveParamConfig(IRTX_Handle_T *hIRTX, IRTX_KeyDesc_T *pKeyDesc)
{
    RET_CODE ret = RET_OK;

    switch (pKeyDesc->protocol)
    {
        case IRTX_PROTOCOL_F0:
        case IRTX_PROTOCOL_F1:
        case IRTX_PROTOCOL_F2:
        case IRTX_PROTOCOL_F3:
        case IRTX_PROTOCOL_F4:
        case IRTX_PROTOCOL_F5:
        case IRTX_PROTOCOL_F10:
            /* union: process F0Code means at the same time process
             * F1Code F2Code F3Code F4Code F5Codevalue, F10Codevalue
             * so only process F0Code is OK
             */
            IRTX_ConfigFNCode(hIRTX, &(pKeyDesc->KeyCode.F0Code), pKeyDesc->protocol, 0);
            break;

        case IRTX_PROTOCOL_FM:
            IRTX_ConfigFMCode(hIRTX, &(pKeyDesc->KeyCode.FMCode), 0);
            break;

        case IRTX_PROTOCOL_F0FM:
            IRTX_ConfigF0FMCode(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F1:
            IRTX_ConfigF0F1Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F1FM:
            IRTX_ConfigF0F1FMCode(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F2:
            IRTX_ConfigF0F2Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F2F3:
            IRTX_ConfigF0F2F3Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F1F2:
            IRTX_ConfigF0F1F2Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F1F2F3:
            IRTX_ConfigF0F1F2F3Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F2F5:
            IRTX_ConfigF0F2F5Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F1F2F5:
            IRTX_ConfigF0F1F2F5Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F2F3F5:
            IRTX_ConfigF0F2F3F5Code(hIRTX, pKeyDesc);
            break;

        case IRTX_PROTOCOL_F0F1F2F3F5:
            IRTX_ConfigF0F1F2F3F5Code(hIRTX, pKeyDesc);
            break;

        default:
            IRTX_PR_ERR("%s:%d Input protocol error\n", __FUNCTION__);
            ret = RET_INVPARAM;
            break;
    }

    return ret;
}

void IRTX_PwmConfig(IRTX_Handle_T *pIRTX, IRTX_PWMConfig_T *pConfig)
{
    IRTX_Device_T *pDevice = pIRTX->device;
    /* set loadval : headl must less then headh  */
    IRTX_CONFIG_HEAD(pDevice, (pConfig->period) & 0xffff, (pConfig->duty) & 0xffff);
    /*clear 16bit cnt*/
    pDevice->PWMCTR = 0;
    /* not have carrier */
    ITRX_CONFIG_OUTPUT_CARRIER_EN(pDevice, DISABLE);
    /* set ir start level  is low */
    IRTX_SetInitLevel(pDevice, FALSE);
    /* set irtx pin output enable */
    IRTX_DISABLE_OEN(pDevice);
    /* module disable */
    IO_BIT_CLEAR(pDevice->CTRL1, IRTX_CTRL1_EN);
}

void IRTX_TimeConfig(IRTX_Handle_T *pIRTX, uint32_t cnt)
{
    /* set loadval : headl must less then headh  */
    IRTX_CONFIG_HEAD(pIRTX->device, cnt & 0xffff, ((cnt & 0xffff) - 1));
    /*clear 16bit cnt*/
    pIRTX->device->PWMCTR = 0;
}

void IRTX_TimeEnable(IRTX_Handle_T *pIRTX)
{
    IO_BITMASK_SET(pIRTX->device->CTRL1, IRTX_CTRL1_MODE, IRTX_TIME << IRTX_CTRL1_MODE_POS);
}

void IRTX_TimeDisable(IRTX_Handle_T *pIRTX)
{
    /* set ir mode to disable time mode, cnt will no chage */
    IO_BITMASK_SET(pIRTX->device->CTRL1, IRTX_CTRL1_MODE, IRTX_IR << IRTX_CTRL1_MODE_POS);
}

void IRTX_PwmEnable(IRTX_Handle_T *pIRTX)
{
    /* becare  pwm and time is the same bit to control*/
    IO_BITMASK_SET(pIRTX->device->CTRL1, IRTX_CTRL1_MODE, IRTX_TIME << IRTX_CTRL1_MODE_POS);
}

void IRTX_PwmDisable(IRTX_Handle_T *pIRTX)
{
    /* set ir mode to disable time mode, pwm wave and time cnt will not chage */
    IO_BITMASK_SET(pIRTX->device->CTRL1, IRTX_CTRL1_MODE, IRTX_IR << IRTX_CTRL1_MODE_POS);
}
