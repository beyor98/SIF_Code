 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          pes.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2017/05/19
 *
 * @brief         Header file of pes driver.
 *
 * @note
 *     2017/05/19, Mike.Zheng, V1.0.0
 *        Initial version.
 */

#include <drv/dev/gpio.h>
#include <drv/dev/pes.h>

#define PES_IO_MAX_NUM                  17  /* PES io max number */
#define PES_CHANNLE_NUM                 12  /* PES channel number */
#define PES_CAHNNEL_FREE                0   /* current PES channel is not using*/
#define PES_CAHNNEL_USING               1   /* current PES channel is using*/


#ifdef CHIP_GM6621
typedef struct
{
    uint32_t pin;                           /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;                  /**< GPIO device */
    uint32_t mode;                          /**< Specifies the operating mode for the selected pins */
    uint32_t pull;                          /**< Specifies the Pull-up or Pull-up X2 activation for the selected pins */
    uint32_t alternate;                     /**< Peripheral to be connected to the selected pins
                                                 This parameter can be a value of @ref GPIOEx_Alternate_function_selection */
} PES_IOPinmux_T;

static RET_CODE PES_TryLockPesChIndex(void);
static void PES_UnlockPesChIndex(void);
static void PES_CpuAsSlaveEventChannelSelect(PES_ChannelIndex_T chIndex);
static void PES_CpuAsSlaveRecevieEventEn(uint8_t en);
static void PES_IOAsMasterSendEventEn(PES_ChannelIndex_T chIndex, uint8_t en);
static RET_CODE PES_CheckIOInputPinAvaild(PES_ChannelIndex_T chIndex,PES_IOPin_T slaveOutputPin);
static RET_CODE PES_MasterConfig(PES_MasterSource_T masterSource,
                                          PES_MasterTrigerOutputMode_T *pmasterTrigerOutput,
                                          PES_ChannelIndex_T chIndex);
static void PES_ConfigCascadeEventChannel(PES_ChannelCascadeEnConfig_T *pcfg,
                                                           PES_ChannelIndex_T chIndex);
static void PES_ConfigEventChannelOutputInvertedEn(uint8_t invertedEn,
                                                                        PES_ChannelIndex_T chIndex);
static RET_CODE PES_SyncModeConfig(PES_TrigerSlaveSyncMode_T syncMode, PES_ChannelIndex_T chIndex);

static uint8_t pesChIndex[PES_CHANNLE_NUM]={ PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                             PES_CAHNNEL_FREE,
                                           };

static PES_IOPinmux_T s_pinmux[PES_IO_MAX_NUM] = {
    {GPIO_PIN_7 , GPIOC, GPIO_MODE_AF, GPIO_NOPULL, GPIOC7_AF5_PESCH0},
    {GPIO_PIN_6 , GPIOB, GPIO_MODE_AF, GPIO_NOPULL, GPIOB6_AF5_PESCH0},
    {GPIO_PIN_8 , GPIOB, GPIO_MODE_AF, GPIO_NOPULL, GPIOB8_AF5_PESCH0},
    {GPIO_PIN_12, GPIOC, GPIO_MODE_AF, GPIO_NOPULL, GPIOC12_AF5_PESCH1},
    {GPIO_PIN_14, GPIOB, GPIO_MODE_AF, GPIO_NOPULL, GPIOB14_AF5_PESCH1},
    {GPIO_PIN_13, GPIOC, GPIO_MODE_AF, GPIO_NOPULL, GPIOC13_AF5_PESCH2},
    {GPIO_PIN_1 , GPIOC, GPIO_MODE_AF, GPIO_NOPULL, GPIOC1_AF5_PESCH2},
    {GPIO_PIN_3 , GPIOD, GPIO_MODE_AF, GPIO_NOPULL, GPIOD3_AF5_PESCH3},
    {GPIO_PIN_2 , GPIOC, GPIO_MODE_AF, GPIO_NOPULL, GPIOC2_AF5_PESCH3},
    {GPIO_PIN_0 , GPIOA, GPIO_MODE_AF, GPIO_NOPULL, GPIOA0_AF5_PESCH4},
    {GPIO_PIN_1 , GPIOA, GPIO_MODE_AF, GPIO_NOPULL, GPIOA1_AF5_PESCH5},
    {GPIO_PIN_5 , GPIOA, GPIO_MODE_AF, GPIO_NOPULL, GPIOA5_AF5_PESCH6},
    {GPIO_PIN_10, GPIOA, GPIO_MODE_AF, GPIO_NOPULL, GPIOA10_AF5_PESCH7},
    {GPIO_PIN_11, GPIOA, GPIO_MODE_AF, GPIO_NOPULL, GPIOA11_AF5_PESCH8},
    {GPIO_PIN_12, GPIOA, GPIO_MODE_AF, GPIO_NOPULL, GPIOA12_AF5_PESCH9},
    {GPIO_PIN_14, GPIOA, GPIO_MODE_AF, GPIO_NOPULL, GPIOA14_AF5_PESCH10},
    {GPIO_PIN_1 , GPIOB, GPIO_MODE_AF, GPIO_NOPULL, GPIOB1_AF5_PESCH11},
};

/* pes gloab var lock */
static uint8_t spesChIndexLock = PES_UNLOCKED;

static RET_CODE PES_TryLockPesChIndex(void)
{
    RET_CODE ret = RET_OK;

    disable_interrupt;

    if (spesChIndexLock == PES_LOCKED)
    {
        ret =  RET_BUSY;
        goto out;
    }
    else
    {
        spesChIndexLock = PES_LOCKED;
    }

out:
    enable_interrupt;

    return ret;
}

static void PES_UnlockPesChIndex(void)
{
    disable_interrupt;

    spesChIndexLock = PES_UNLOCKED;

    enable_interrupt;
}

static RET_CODE PES_IOPinmuxConfig(PES_IOPin_T pin)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    PES_IOPinmux_T *pPinmuxcfg = &s_pinmux[(uint8_t)pin];

    memset(&GPIO_InitStruct,0,sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin = pPinmuxcfg->pin;
    GPIO_InitStruct.mode = pPinmuxcfg->mode;
    GPIO_InitStruct.pull = pPinmuxcfg->pull;
    GPIO_InitStruct.alternate = pPinmuxcfg->alternate;

    ret = GPIO_Init(pPinmuxcfg->device, &GPIO_InitStruct);

    return ret;
}

PES_ChannelIndex_T PES_RequestEventChannel()
{
    PES_ChannelIndex_T chIndex = PES_INVAILD_CHANNEL;
    uint8_t i = 0;

    /* protect write gloab pesChIndex: need lock */
    if (PES_TryLockPesChIndex() != RET_OK)
    {
        chIndex = PES_INVAILD_CHANNEL;
        goto out;
    }

    for (i = 0; i < PES_CHANNLE_NUM; i++)
    {
        if (pesChIndex[i] == PES_CAHNNEL_FREE)
        {
            chIndex = (PES_ChannelIndex_T)i;
            /* get a vaild channel and set flag */
            pesChIndex[i] = PES_CAHNNEL_USING;
            break;
        }
    }

    PES_UnlockPesChIndex();
out:
    return chIndex;
}

RET_CODE PES_ReleaseEventChannel(PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    if (chIndex == PES_INVAILD_CHANNEL)
    {
        PES_PR_WARNING("Waring: Not Regist Callback Before\n");
    }

    /*protect write gloab pesChIndex: need lock*/
    if (PES_TryLockPesChIndex() != RET_OK)
    {
        ret =  RET_BUSY;
        goto out;
    }

    pesChIndex[chIndex] = PES_CAHNNEL_FREE;

    PES_UnlockPesChIndex();

out:
    return ret;

}

static RET_CODE PES_SyncModeConfig(PES_TrigerSlaveSyncMode_T syncMode,
                                               PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    switch (syncMode)
    {
    case PES_SYNC_BYPASS:
    case PES_SYNC_RISING_EDGE:
    case PES_SYNC_FALLING_EDGE:
    case PES_SYNC_RISINGF_FALLING_EDGE:
         PES_RESYNC_ENABLE(PES_DEV, chIndex);
         PES_CONFIG_EVENT_OUTPUT_MODE(PES_DEV, chIndex, syncMode);
         break;

    case PES_ASYNC:
         PES_ASYNC_ENABEL(PES_DEV, chIndex);
         break;

    default:
        return RET_INVPARAM ;
    }

    return ret;
}

static RET_CODE PES_IOAsMasterSelIoPin(PES_ChannelIndex_T chIndex, PES_IOPin_T pin)
{
    RET_CODE ret = RET_OK;

    switch (pin)
    {
    case IO_PIN_XPC7:
    case IO_PIN_XPB8:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPC12:
    case IO_PIN_XPB14:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPC13:
    case IO_PIN_XPC1:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPD3:
    case IO_PIN_XPC2:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPA0:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPA1:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPA5:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPA10:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPA11:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPA12:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPA14:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

    case IO_PIN_XPB1:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, chIndex);
         break;

   default:
        return RET_INVPARAM ;
    }
    /*config pinmux*/
    ret = PES_IOPinmuxConfig(pin);

    return ret;

}

static RET_CODE PES_MasterConfig(PES_MasterSource_T masterSource,
                                          PES_MasterTrigerOutputMode_T *pmasterTrigerOutput,
                                          PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    /* selet master source*/
    PES_MASTER_SOURCE_SELECT(PES_DEV, chIndex, masterSource);

    switch (masterSource)
    {

    case PES_MASTER_CPU:
    case PES_MASTER_DAM:
    case PES_MASTER_RTCC:
    case PES_MASTER_BASETIMER:
         break;

    case PES_MASTER_IO:
         /*checkout master OutputPin and chIndex is availd*/
         ret = PES_CheckIOInputPinAvaild(chIndex, pmasterTrigerOutput->ioPinOutput);
         if (ret != RET_OK)
         {
            PES_PR_ERR("Err:PES_CheckIOInputPinAvaild\n");
            return RET_INVPARAM ;
         }
         PES_IOAsMasterSendEventEn(chIndex, TRUE);
         ret = PES_IOAsMasterSelIoPin(chIndex, pmasterTrigerOutput->ioPinOutput);
         break;

    case PES_MASTER_PESOUT:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->pesChannelOutput);
         break;

    case PES_MASTER_ETM20:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->etm20Output);
         break;

    case PES_MASTER_ACMP:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->acmpOutput);
         break;

    case PES_MASTER_ADC:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->adcOutput);
         break;

    case PES_MASTER_ETM21:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->etm21Output);
         break;

    case PES_MASTER_ETM22:
         PES_EVENT_SOURCE_SELECT(PES_DEV, chIndex, pmasterTrigerOutput->etm22Output);
         break;

    default:
         return RET_INVPARAM ;
    }

    return ret;
}

static void PES_ConfigCascadeEventChannel(PES_ChannelCascadeEnConfig_T *pcfg,
                                                           PES_ChannelIndex_T chIndex)
{
    if (pcfg->andNextChannelEnable == TRUE)
    {
        PES_AND_NEXT_CHANNEL_ENABLE(PES_DEV, chIndex);
    }
    else
    {
        PES_AND_NEXT_CHANNEL_DISABLE(PES_DEV, chIndex);
    }
    if (pcfg->orpreChannelEnable == TRUE)
    {
        PES_OR_PRE_CHANNEL_ENABLE(PES_DEV, chIndex);
    }
    else
    {
        PES_OR_PRE_CHANNEL_DISABLE(PES_DEV, chIndex);
    }
}

static void PES_ConfigEventChannelOutputInvertedEn(uint8_t invertedEn,
                                                                        PES_ChannelIndex_T chIndex)
{
    if (invertedEn == TRUE)
    {
        PES_OUT_INVERT_ENABLE(PES_DEV, chIndex);
    }
    else
    {
        PES_OUT_INVERT_DISABLE(PES_DEV, chIndex);
    }
}

RET_CODE PES_ConfigEventChannel(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    /* config sync mode */
    ret = PES_SyncModeConfig(pchCfg->syncMode, chIndex);
    if (ret != RET_OK)
    {
        goto out;
    }
    /* config Master */
    ret = PES_MasterConfig(pchCfg->masterSrcConfig.masterSource,      \
                           &(pchCfg->masterSrcConfig.masterTrigerOutput),
                           chIndex);
    if (ret != RET_OK)
    {
        PES_PR_ERR("%s:Err:PES_MasterConfig\n",__FUNCTION__);
        goto out;
    }

    /* config cascade event channel */
    PES_ConfigCascadeEventChannel(&(pchCfg->cascadeConfig), chIndex);

    /* config envetoutput invert or not */
    PES_ConfigEventChannelOutputInvertedEn(pchCfg->channelOutputInverted, chIndex);

out:
    return ret;
}

/*cpu as master config*/
void PES_CupAsMasterGenerateTrigerEvent(PES_MasterCpuTriger_T trigerMode, PES_ChannelIndex_T chIndex)
{
    if ((trigerMode == PES_LOW_LEVEL) || (trigerMode == PES_HIGH_LEVEL))
    {
        PES_CONFIG_SW_LEVEL(PES_DEV, chIndex, trigerMode);
    }
    else if (trigerMode == PES_HIGH_LEVEL)
    {
        PES_SW_GENERATE_PLUSE(PES_DEV, chIndex);
    }
}

/*cpu as slave config*/
static void PES_CpuAsSlaveEventChannelSelect(PES_ChannelIndex_T chIndex)
{
    PES_TO_CPU_EVENT_CHANNEL(PES_DEV ,chIndex);
}

static void PES_CpuAsSlaveRecevieEventEn(uint8_t en)
{
    if (en)
    {
        PES_TO_CPU_EVENT_ENABLE(PES_DEV);
    }
    else
    {
        PES_TO_CPU_EVENT_DISABLE(PES_DEV);
    }
}

RET_CODE  PES_CPUAsSlaveConfig(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex)
{
    RET_CODE ret = RET_OK;

    /* config master */
    ret = PES_ConfigEventChannel(pchCfg, chIndex);
    if (ret != RET_OK)
    {
        goto out;
    }

    /*as slave select pes event channel */
    PES_CpuAsSlaveEventChannelSelect(chIndex);

    /* as slave enable receive master pes event */
    PES_CpuAsSlaveRecevieEventEn(TRUE);

out:
    return ret;
}


void PES_CupAsMasterGenerateSWTrigerEvent(PES_MasterCpuTriger_T trigerMode, PES_ChannelIndex_T chIndex)
{
    if (trigerMode == PES_PULSE)
    {
        PES_SW_GENERATE_PLUSE(PES_DEV, chIndex);
    }
    else
    {
        PES_CONFIG_SW_LEVEL(PES_DEV, chIndex, trigerMode);
    }
}

static void PES_IOAsMasterSendEventEn(PES_ChannelIndex_T chIndex, uint8_t en)
{
    /*secet io dir*/
    if (en)
    {
        PES_CONFIG_IO_DIR(PES_DEV, chIndex, IO_SEND_EVENT);
    }
    else
    {
        PES_CONFIG_IO_DIR(PES_DEV, chIndex, IO_RECEIVE_EVENT);
    }
}

static void PES_IOAsSlaveRecEventEn(PES_ChannelIndex_T chIndex, uint8_t en)
{
    if (en)
    {
        PES_CONFIG_IO_DIR(PES_DEV, chIndex, IO_RECEIVE_EVENT);
    }
    else
    {
        PES_CONFIG_IO_DIR(PES_DEV, chIndex, IO_SEND_EVENT);
    }
}

static RET_CODE PES_CheckIOOutputPinAvaild(PES_ChannelIndex_T chIndex,PES_IOPin_T slaveOutputPin)
{
    RET_CODE ret = RET_OK;

    switch (slaveOutputPin)
    {
    case IO_PIN_XPC7:
    case IO_PIN_XPB6:
         if (chIndex != PES_EVENT_CHANNEL0)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPC12:
    case IO_PIN_XPB14:
         if (chIndex != PES_EVENT_CHANNEL1)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPC13:
    case IO_PIN_XPC1:
         if (chIndex != PES_EVENT_CHANNEL2)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPD3:
    case IO_PIN_XPC2:
         if (chIndex != PES_EVENT_CHANNEL3)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA0:
         if (chIndex != PES_EVENT_CHANNEL4)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA1:
         if (chIndex != PES_EVENT_CHANNEL5)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA5:
         if (chIndex != PES_EVENT_CHANNEL6)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA10:
         if (chIndex != PES_EVENT_CHANNEL7)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA11:
         if (chIndex != PES_EVENT_CHANNEL8)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA12:
         if (chIndex != PES_EVENT_CHANNEL9)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA14:
         if (chIndex != PES_EVENT_CHANNEL10)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPB1:
         if (chIndex != PES_EVENT_CHANNEL11)
         {
            ret = RET_INVPARAM;
         }
         break;

    default:
         return RET_INVPARAM ;
    }

        return ret;
}


static RET_CODE PES_CheckIOInputPinAvaild(PES_ChannelIndex_T chIndex,PES_IOPin_T slaveOutputPin)
{
    RET_CODE ret = RET_OK;

    switch (slaveOutputPin)
    {
    case IO_PIN_XPC7:
    case IO_PIN_XPB8:
         if (chIndex != PES_EVENT_CHANNEL0)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPC12:
    case IO_PIN_XPB14:
         if (chIndex != PES_EVENT_CHANNEL1)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPC13:
    case IO_PIN_XPC1:
         if (chIndex != PES_EVENT_CHANNEL2)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPD3:
    case IO_PIN_XPC2:
         if (chIndex != PES_EVENT_CHANNEL3)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA0:
         if (chIndex != PES_EVENT_CHANNEL4)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA1:
         if (chIndex != PES_EVENT_CHANNEL5)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA5:
         if (chIndex != PES_EVENT_CHANNEL6)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA10:
         if (chIndex != PES_EVENT_CHANNEL7)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA11:
         if (chIndex != PES_EVENT_CHANNEL8)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA12:
         if (chIndex != PES_EVENT_CHANNEL9)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPA14:
         if (chIndex != PES_EVENT_CHANNEL10)
         {
            ret = RET_INVPARAM;
         }
         break;

    case IO_PIN_XPB1:
         if (chIndex != PES_EVENT_CHANNEL11){
            ret = RET_INVPARAM;
         }
         break;

    default:
         return RET_INVPARAM ;
    }

        return ret;
}


RET_CODE PES_IOAsSlaveConfig(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex, PES_IOPin_T slaveOutputPin)
{
    /*config master*/
    RET_CODE ret = RET_OK;

    /*checkout slave OutputPin and chIndex is availd*/
    ret = PES_CheckIOOutputPinAvaild(chIndex, slaveOutputPin);
    if (ret != RET_OK)
    {
        PES_PR_ERR("Err:PES_CheckIOOutputPinAvaild\n");
        goto out;
    }
    /* config io pinmux */
    ret = PES_IOPinmuxConfig(slaveOutputPin);
    if (ret != RET_OK)
    {
        goto out;
    }

    /* config master */
    ret = PES_ConfigEventChannel(pchCfg, chIndex);
    if (ret != RET_OK)
    {
        goto out;
    }

    /*as slave sel channel and enable receive event */
    PES_IOAsSlaveRecEventEn(chIndex, TRUE);

out:
    return ret;
}
#endif


