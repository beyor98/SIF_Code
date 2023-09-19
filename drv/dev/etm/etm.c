/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          etm.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/06/22
 *
 * @brief        ETM peripheral APIs for application.
 *
 * @note
 *    2016/06/22, mark.lee, V1.0.0
 *        Initial version.
 *    2021/8/26, chenliwei, V1.0.0
 *        Initial version.
 */

#if !defined(CHIP_F6621B)
#include <drv/dev/etm.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
STATIC_ISR(ETM0_IRQn, ETM0_IRQHandler);
STATIC_ISR(ETM1_IRQn, ETM1_IRQHandler);
STATIC_ISR(ETM2_IRQn, ETM2_IRQHandler);

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    STATIC_ISR(ETM3_IRQn, ETM3_IRQHandler);
#endif

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    STATIC_ISR(ETM4_IRQn, ETM4_IRQHandler);
    STATIC_ISR(ETM5_IRQn, ETM5_IRQHandler);
#endif
/*******************************************************************************
* Global variables
*******************************************************************************/
uint32_t ctrl = 0;

/*******************************************************************************
* Static variables
*******************************************************************************/
/** @brief Pointers to ETM handles for each instance. */
static void *s_ETMHandle[MAX_ETM_NUM] = {NULL};

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
/** @brief Pointers to ETM bases for each instance. */
static ETM_Device_T *const s_ETMBases[] = {ETM0, ETM1, ETM2, ETM3, ETM4, ETM5};

/*! @brief Pointers to ETM IRQ number for each instance. */
const IRQn_Type s_ETMIrqs[] = {ETM0_IRQn, ETM1_IRQn, ETM2_IRQn, ETM3_IRQn, ETM4_IRQn, ETM5_IRQn};
#elif defined(CHIP_GM6611B)
/** @brief Pointers to ETM bases for each instance. */
static ETM_Device_T *const s_ETMBases[] = {ETM0, ETM1, ETM2, ETM3};

/*! @brief Pointers to ETM IRQ number for each instance. */
const IRQn_Type s_ETMIrqs[] = {ETM0_IRQn, ETM1_IRQn, ETM2_IRQn, ETM3_IRQn};
#elif defined(CHIP_GM6621)
/** @brief Pointers to ETM bases for each instance. */
static ETM_Device_T *const s_ETMBases[] = {ETM0, ETM1, ETM2};

/*! @brief Pointers to ETM IRQ number for each instance. */
const IRQn_Type s_ETMIrqs[] = {ETM0_IRQn, ETM1_IRQn, ETM2_IRQn};

static DMA_Handle_T s_DMAHandle;
#endif

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/
/**
 * @brief Get instance number for advanced timer module.
 *
 * @param pETM ETM peripheral base address.
 */
static uint32_t ETM_GetInstance(ETM_Device_T *pETM);

/**
 * @brief Wait time base counter stop before operate time base.
 *
 * @param pETM   ETM peripheral base address.
 * @param us   time out value.
 */
static RET_CODE ETM_WaitTbStop(ETM_Device_T *pETM, int16_t us);

/**
 * @brief ETM common interrupt handler.
 *
 * @param pETM   ETM peripheral base address.
 * @param pETMHandle  pointer to ETM_Handle_T structure which stores the transfer state
 */
static void ETM_CommonIRQHandler(ETM_Device_T *pETM, void *pETMHandle);

/*******************************************************************************
* Codes
*******************************************************************************/
static uint32_t ETM_GetInstance(ETM_Device_T *pETM)
{
    uint32_t instance;

    /* Find the instance index from base address mappings */
    for (instance = 0; instance < MAX_ETM_NUM; instance++)
    {
        if (s_ETMBases[instance] == pETM)
        {
            break;
        }
    }

    return instance;
}

static RET_CODE ETM_WaitTbStop(ETM_Device_T *pETM, int16_t us)
{
    while ((pETM->SR & ETM_SR_STOP) && (us--))
    {
        SysTick_DelayUS(1);
    }
    if (us <= 0)
    {
        return RET_TIMEOUT;
    }
    else
    {
        return RET_OK;
    }
}

static void ETM_CommonIRQHandler(ETM_Device_T *pETM, void *pETMHandle)
{

    ETM_Handle_T *pHandle = (ETM_Handle_T *)pETMHandle;
    uint32_t interruptMask = 0;
    uint32_t interruptFlags = 0;
    uint32_t event = 0;

    interruptMask = pETM->IER;

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    interruptFlags = pETM->SR;
#endif

#if defined(CHIP_GM6621)
    interruptFlags = pETM->ISR;
#endif

    if ((interruptMask & ETM_INT_CNT_START)  && (interruptFlags & ETM_TBSTINT_FLAG))
    {
        event |= ETM_TBSTINT_FLAG;
    }
    if ((interruptMask & ETM_INT_CNT_OVERFLOW) && (interruptFlags & ETM_TBOVFINT_FLAG))
    {
        event |= ETM_TBOVFINT_FLAG;
    }
    if ((interruptMask & ETM_INT_CNT_UNDERFLOW) && (interruptFlags & ETM_TBUDFINT_FLAG))
    {
        event |= ETM_TBUDFINT_FLAG;
    }
    if ((interruptMask & ETM_INT_CNT_MATCHED1) && (interruptFlags & ETM_TBD1MINT_FLAG))
    {
        event |= ETM_TBD1MINT_FLAG;
    }
    if ((interruptMask & ETM_INT_CNT_MATCHED2) && (interruptFlags & ETM_TBD2MINT_FLAG))
    {
        event |= ETM_TBD2MINT_FLAG;
    }
    if ((interruptMask & ETM_INT_EDGE_P1) && (interruptFlags & ETM_CH1PEINT_FLAG))
    {
        event |= ETM_CH1PEINT_FLAG;
    }
    if ((interruptMask & ETM_INT_EDGE_N1) && (interruptFlags & ETM_CH1NEINT_FLAG))
    {
        event |= ETM_CH1NEINT_FLAG;
    }
    if ((interruptMask & ETM_INT_EDGE_P2) && (interruptFlags & ETM_CH2PEINT_FLAG))
    {
        event |= ETM_CH2PEINT_FLAG;
    }
    if ((interruptMask & ETM_INT_EDGE_N2) && (interruptFlags & ETM_CH2NEINT_FLAG))
    {
        event |= ETM_CH2NEINT_FLAG;
    }
    if ((interruptMask & ETM_INT_SYNC) && (interruptFlags & ETM_SYNCINT_FLAG))
    {
        event |= ETM_SYNCINT_FLAG;
    }

#if defined(CHIP_GM6621)
    if ((interruptMask & ETM_INT_EDGE_P3) && (interruptFlags & ETM_CH3PEINT_FLAG))
    {
        event |= ETM_CH3PEINT_FLAG;
    }
    if ((interruptMask & ETM_INT_EDGE_N3) && (interruptFlags & ETM_CH3NEINT_FLAG))
    {
        event |= ETM_CH3NEINT_FLAG;
    }
    if ((interruptMask & ETM_INT_EDGE_P4) && (interruptFlags & ETM_CH4PEINT_FLAG))
    {
        event |= ETM_CH4PEINT_FLAG;
    }
    if ((interruptMask & ETM_INT_EDGE_N4) && (interruptFlags & ETM_CH4NEINT_FLAG))
    {
        event |= ETM_CH4NEINT_FLAG;
    }
#endif

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_GM6621) || defined(CHIP_F6601C))
    if ((interruptMask & ETM_INT_BREAKIN) && (interruptFlags & ETM_BREAKIN_FLAG))
    {
        event |= ETM_BREAKIN_FLAG;
    }
#endif

    /* Clear the interrupt flag. */
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    pETM->SR = event;
#endif

#if defined(CHIP_GM6621)
    pETM->ISR = event;
#endif

    /* Call the callback function after the function has completed. */
    if ((pHandle != NULL) && (pHandle->callback != NULL))
    {
        pHandle->callback(pETM, event, NULL);
    }
}

void ETM_SetPWMDuty(ETM_Device_T *pETM, uint8_t shadowMode, uint16_t duty1, uint16_t duty2)
{
    if (shadowMode == TRUE)
    {
        IO_BIT_SET(pETM->CR, ETM_CR_SHDEN);
    }
    else
    {
        IO_BIT_CLEAR(pETM->CR, ETM_CR_SHDEN);
    }

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    pETM->OCR = (duty2 << 16) | duty1;
#endif

#if defined(CHIP_GM6621)
    pETM->CMP1 = (duty2 << 16) | duty1;
#endif

}

#if defined(CHIP_GM6621)
void ETM_OCConfig(ETM_Device_T *pETM, uint8_t shadowMode, ETM_Channel_T channel, uint16_t pulse)
{
    if (shadowMode == TRUE)
    {
        IO_BIT_SET(pETM->CR, ETM_CR_SHDEN);
    }
    else
    {
        IO_BIT_CLEAR(pETM->CR, ETM_CR_SHDEN);
    }

    switch (channel)
    {
        case ETM_CHANNEL_1:
			pETM->CMP1 &= 0xFFFF0000;
            pETM->CMP1 |= pulse;
            break;
        case ETM_CHANNEL_2:
			pETM->CMP1 &= 0x0000FFFF;
            pETM->CMP1 |= (pulse << 16);
            break;
        case ETM_CHANNEL_3:
			pETM->CMP2 &= 0xFFFF0000;
            pETM->CMP2 |= pulse;
            break;
        case ETM_CHANNEL_4:
			pETM->CMP2 &= 0x0000FFFF;
            pETM->CMP2 |= (pulse << 16);
            break;
        default:
            break;
    }
}
#endif

uint16_t ETM_GetCaptureValue(ETM_Device_T *pETM, ETM_Channel_T channel)
{
    uint16_t cap_val = 0;

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    if (channel == ETM_CHANNEL_1)
    {
        cap_val = (pETM->CCR) & 0xFFFF;
    }
    else if (channel == ETM_CHANNEL_2)
    {
        cap_val = (pETM->CCR) >> 16;
    }
#endif

#if defined(CHIP_GM6621)
    switch (channel)
    {
        case ETM_CHANNEL_1:
            cap_val = (pETM->CAP1) & 0xFFFF;
            break;
        case ETM_CHANNEL_2:
            cap_val = (pETM->CAP2) & 0xFFFF;
            break;
        case ETM_CHANNEL_3:
            cap_val = (pETM->CAP3) & 0xFFFF;
            break;
        case ETM_CHANNEL_4:
            cap_val = (pETM->CAP4) & 0xFFFF;
            break;
        default:
            break;
    }
#endif

    return cap_val;
}

void ETM_SetTriggerMode(ETM_Device_T *pETM, ETM_PWMTriggerMode_T mode, uint8_t level)
{
#if defined(CHIP_GM6621)
    switch (mode)
    {
        case ETM_PWM_TRG_NONE:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKP, 0);
            break;
        case ETM_PWM_TRG_START:
        case ETM_PWM_TRG_START_STOP:
        case ETM_PWM_LEVEL_HDL:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKP,  \
                           (ETM_CR_BKIN_FIELD(1) | ETM_CR_BKP_FIELD(level)));
            break;
        default:
            break;
    }
#endif

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    switch (mode)
    {
        case ETM_PWM_TRG_NONE:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP, 0);
            break;
        case ETM_PWM_TRG_TCIN:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1) | ETM_CR_BKP_FIELD(level)));
            break;
        case ETM_PWM_TRG_ACMP:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        case ETM_PWM_TRG_LOCKUP:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        case ETM_PWM_TRG_ADC:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        case ETM_PWM_TRG_HOSC:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        default:
            break;
    }
#endif

#if (defined(CHIP_GM6611B))
    switch (mode)
    {
        case ETM_PWM_TRG_TCIN:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1) | ETM_CR_BKP_FIELD(level)));
            break;

        case ETM_PWM_TRG_ACMP:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        case ETM_PWM_TRG_ADC:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        case ETM_PWM_TRG_LOCKUP:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        case ETM_PWM_TRG_DEEPSTOP:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        case ETM_PWM_TRG_STOPMODE:
            IO_BITMASK_SET(pETM->CR, ETM_CR_BKIN | ETM_CR_BKSOURCE | ETM_CR_BKP,  \
                           (ETM_CR_BKSOURCE_FIELD(mode) | ETM_CR_BKIN_FIELD(1)));
            break;
        default:
            break;
    }
#endif
}

void ETM_ConfigSync(ETM_Device_T *pETM, ETM_CountMode_T countMode, ETM_PWMSyncConfig_T *pConfig)
{
    if (countMode == ETM_COUNT_UPDOWN)
    {
        /* Sets the counter direction */
        if (pConfig->countDir == TRUE)
        {
            pETM->CR |= ETM_CR_SCD;
        }
        else
        {
            pETM->CR &= ~ETM_CR_SCD;
        }

        /* Sets sync event direction */
        if (pConfig->eventDir == TRUE)
        {
            pETM->CR |= ETM_CR_SED;
        }
        else
        {
            pETM->CR &= ~ETM_CR_SED;
        }
    }

    /* Config sync load start cnt val */
    pETM->SYNCONF = ((pConfig->startCount << 16) | (pConfig->eventCount));

    if (pConfig->syncEn == TRUE)
    {
        IO_BIT_SET(pETM->CR, ETM_CR_SYNCEN);
    }
    else
    {
        IO_BIT_CLEAR(pETM->CR, ETM_CR_SYNCEN);
    }
}

void ETM_SetSyncSource(ETM_SyncSource_T syncSource)
{
    ETM_Device_T *pETM = ETM0;
#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    IO_BITMASK_SET(SYSDEV->ESS, SYS_ESS_SEL_MASK, syncSource);
#elif (defined(CHIP_GM6611B) || defined(CHIP_GM6621))
    IO_BITMASK_SET(SYSDEV->DIPS, SYS_ESS_SEL_MASK, syncSource);
#endif
    if ((syncSource == ETM_SYNC_BY_SYS) || (syncSource == ETM_SYNC_BY_TIME0) || ((syncSource == ETM_SYNC_CHAINED) &&  \
                                            (pETM->CR & ETM_CR_SYNCEN)))
    {
        /* Wait for counter start */
        SysTick_DelayUS(32);
#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
        IO_BIT_SET(SYSDEV->ESS, SYS_ESS_EN);
#elif (defined(CHIP_GM6611B) || defined(CHIP_GM6621))
        IO_BIT_SET(SYSDEV->DIPS, SYS_ESS_EN);
#endif
    }
    else
    {
#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
        IO_BIT_CLEAR(SYSDEV->ESS, SYS_ESS_EN);
#elif (defined(CHIP_GM6611B) || defined(CHIP_GM6621))
        IO_BIT_CLEAR(SYSDEV->DIPS, SYS_ESS_EN);
#endif
    }
}

void ETM_SetSoftwareForceOutput(ETM_Device_T *pETM, ETM_Channel_T channel, ETM_LevelSetting_T level)
{
    uint32_t tmp = 0;

    tmp = pETM->FOCR;

    switch (channel)
    {
        case ETM_CHANNEL_1:
            tmp &= ~ETM_FOCR_CH1;
            tmp |= (level & ETM_FOCR_CH1);
            break;
        case ETM_CHANNEL_2:
            tmp &= ~ETM_FOCR_CH2;
            tmp |= (((uint32_t)(((uint32_t)(level)) << ETM_FOCR_CH2_POS)) & ETM_FOCR_CH2);
            break;
#if defined(CHIP_GM6621)
        case ETM_CHANNEL_3:
            tmp &= ~ETM_FOCR_CH3;
            tmp |= (((uint32_t)(((uint32_t)(level)) << ETM_FOCR_CH3_POS)) & ETM_FOCR_CH3);
            break;
        case ETM_CHANNEL_4:
            tmp &= ~ETM_FOCR_CH4;
            tmp |= (((uint32_t)(((uint32_t)(level)) << ETM_FOCR_CH4_POS)) & ETM_FOCR_CH4);
            break;
#endif
        default:
            break;
    }

    pETM->FOCR = tmp;
}

RET_CODE ETM_StartTimeBase(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    ret = ETM_WaitTbStop(pETM, US_TIMEOUT_VAL);
    if (ret != RET_OK)
    {
        return RET_TIMEOUT;
    }

    /* Start counter */
    IO_BIT_SET(pETM->CR, ETM_CR_TBEN);

    return RET_OK;
}

RET_CODE ETM_StopTimeBase(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    ret = ETM_WaitTbStop(pETM, US_TIMEOUT_VAL);
    if (ret != RET_OK)
    {
        return RET_TIMEOUT;
    }

    /* Stop counter */
    IO_BIT_CLEAR(pETM->CR, ETM_CR_TBEN);

    return RET_OK;
}

RET_CODE ETM_InitTimeBase(ETM_Device_T *pETM, const ETM_Config_T *pConfig, ETM_Handle_T *pHandle)
{
    RET_CODE ret = RET_OK;
    uint32_t instance = 0;
    uint32_t ctrl = 0;

    if ((pETM == NULL) || (pConfig == NULL))
    {
        return RET_INVPARAM;
    }

    instance = ETM_GetInstance(pETM);

    if (instance >= MAX_ETM_NUM)
    {
        return RET_ERROR;
    }

    if (pHandle != NULL)
    {
        /* Zero handle */
        memset(pHandle, 0, sizeof(*pHandle));

        /* Set callback */
        pHandle->pDevice = pETM;
        pHandle->callback = pConfig->callback;

        /* Save the context in global variables to support the double weak mechanism */
        s_ETMHandle[instance] = pHandle;
    }

    /* Disable time base counter */
    ret = ETM_StopTimeBase(pETM);
    if (ret != RET_OK)
    {
        s_ETMHandle[instance] = NULL;
        return RET_ERROR;
    }

    /* Enable nvic */
    NVIC_EnableIRQ(s_ETMIrqs[instance]);

    /* Select time base counter source */
    ctrl |= ((pConfig->extClock) ? ETM_CR_EXTCLK : 0);

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    /* Enable prescaler */
    ctrl |= (pConfig->prescalerEn) ? (ETM_CR_PSCEN | ETM_CR_PSCDIV_FIELD(pConfig->prescaler)) : 0;
#endif

    /* Set counter mode */
    ctrl |= ETM_CR_CD_FIELD(pConfig->countMode);

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    /* Setup fields of ETM control register */
    IO_BITMASK_SET(pETM->CR, ETM_CR_EXTCLK | ETM_CR_CD | \
                   ETM_CR_PSCEN | ETM_CR_PSCDIV, ctrl);

    /* Set period value */
    pETM->CCR = pConfig->period;
#endif

#if defined(CHIP_GM6621)
    /* Enable prescaler */
    if (pConfig->prescalerEn)
    {
        ctrl |= ETM_CR_PSCEN;
        IO_BITMASK_SET(pETM->CR1, ETM_CR_PSCDIV, ETM_CR_PSCDIV_FIELD(pConfig->prescaler));
    }

    /* Setup fields of ETM control register */
    IO_BITMASK_SET(pETM->CR, ETM_CR_EXTCLK | ETM_CR_CD | ETM_CR_PSCEN, ctrl);

    /* Set period value */
    pETM->ARR = (pConfig->repetitionCounter << 16) | pConfig->period;
#endif

    /* Set dead time */
    pETM->DTR = pConfig->deadTime;

    /* Disable trigger condition mode and Synchronization by default */
    pETM->CR &= ~(ETM_CR_BKIN | ETM_CR_BKP |  \
                  ETM_CR_SED | ETM_CR_SCD | ETM_CR_SYNCEN);
#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    IO_BITS_CLEAR(SYSDEV->ESS, SYS_ESS_EN | SYS_ESS_SEL_MASK);
#elif (defined(CHIP_GM6611B) || defined(CHIP_GM6621))
    IO_BITS_CLEAR(SYSDEV->DIPS, SYS_ESS_EN | SYS_ESS_SEL_MASK);
#endif
    return RET_OK;
}

void ETM_SetPWMChannel(ETM_Device_T *pETM, ETM_Channel_T channel, ETM_PWMConfig_T *pConfig)
{
    switch (channel)
    {
        case ETM_CHANNEL_1:
            pETM->WFCR1 = (ETM_WFCR_CNTST_FIELD(pConfig->counterStart)  | ETM_WFCR_MD2_FIELD(pConfig->counterDown2)   | \
                           ETM_WFCR_MD1_FIELD(pConfig->counterDown1) | ETM_WFCR_MU2_FIELD(pConfig->counterUp2)  | \
                           ETM_WFCR_MU1_FIELD(pConfig->counterUp1) | ETM_WFCR_UN_FIELD(pConfig->underFlow)   |  \
                           ETM_WFCR_OV_FIELD(pConfig->overFlow) | pConfig->idleState);
            break;
        case ETM_CHANNEL_2:
            pETM->WFCR2 = (ETM_WFCR_CNTST_FIELD(pConfig->counterStart)  | ETM_WFCR_MD2_FIELD(pConfig->counterDown2)   | \
                           ETM_WFCR_MD1_FIELD(pConfig->counterDown1) | ETM_WFCR_MU2_FIELD(pConfig->counterUp2)  | \
                           ETM_WFCR_MU1_FIELD(pConfig->counterUp1) | ETM_WFCR_UN_FIELD(pConfig->underFlow)   |  \
                           ETM_WFCR_OV_FIELD(pConfig->overFlow) | pConfig->idleState);
            break;
#if defined(CHIP_GM6621)
        case ETM_CHANNEL_3:
            pETM->WFCR3 = (ETM_WFCR_CNTST_FIELD(pConfig->counterStart)  | ETM_WFCR_MD2_FIELD(pConfig->counterDown4)   | \
                           ETM_WFCR_MD1_FIELD(pConfig->counterDown3) | ETM_WFCR_MU2_FIELD(pConfig->counterUp4)  | \
                           ETM_WFCR_MU1_FIELD(pConfig->counterUp3) | ETM_WFCR_UN_FIELD(pConfig->underFlow)   |  \
                           ETM_WFCR_OV_FIELD(pConfig->overFlow) | pConfig->idleState);
            break;
        case ETM_CHANNEL_4:
            pETM->WFCR4 = (ETM_WFCR_CNTST_FIELD(pConfig->counterStart)  | ETM_WFCR_MD2_FIELD(pConfig->counterDown4)   | \
                           ETM_WFCR_MD1_FIELD(pConfig->counterDown3) | ETM_WFCR_MU2_FIELD(pConfig->counterUp4)  | \
                           ETM_WFCR_MU1_FIELD(pConfig->counterUp3) | ETM_WFCR_UN_FIELD(pConfig->underFlow)   |  \
                           ETM_WFCR_OV_FIELD(pConfig->overFlow) | pConfig->idleState);
            break;
#endif
        default:
            break;
    }
}

RET_CODE ETM_StartPWM(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    /* Disable capture mode */
    IO_BIT_CLEAR(pETM->CR, ETM_CR_CAPEN);

    /* Start counter */
    ret = ETM_StartTimeBase(pETM);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

RET_CODE ETM_StopPWM(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    /* Stop counter */
    ret = ETM_StopTimeBase(pETM);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

RET_CODE ETM_InitCapture(ETM_Device_T *pETM, const ETM_Config_T *pConfig, ETM_Handle_T *pHandle)
{
    RET_CODE ret = RET_OK;
    uint32_t instance = 0;
    uint32_t ctrl = 0;

    if ((pETM == NULL) || (pConfig == NULL))
    {
        return RET_INVPARAM;
    }

    instance = ETM_GetInstance(pETM);

    if (instance >= MAX_ETM_NUM)
    {
        return RET_ERROR;
    }

    if (pHandle != NULL)
    {
        /* Zero handle */
        memset(pHandle, 0, sizeof(*pHandle));

        /* Set callback */
        pHandle->pDevice = pETM;
        pHandle->callback = pConfig->callback;

        /* Save the context in global variables to support the double weak mechanism */
        s_ETMHandle[instance] = pHandle;
    }

    /* Disable time base counter */
    ret = ETM_StopTimeBase(pETM);
    if (ret != RET_OK)
    {
        s_ETMHandle[instance] = NULL;
        return RET_ERROR;
    }

    /* Enable nvic */
    NVIC_EnableIRQ(s_ETMIrqs[instance]);

    /* Select time base counter source */
    ctrl |= ((pConfig->extClock) ? ETM_CR_EXTCLK : 0);

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
    /* Config prescaler */
    ctrl |= (pConfig->prescalerEn) ? (ETM_CR_PSCEN | ETM_CR_PSCDIV_FIELD(pConfig->prescaler)) : 0;

    /* Setup fields of ETM control register */
    IO_BITMASK_SET(pETM->CR, ETM_CR_EXTCLK | ETM_CR_PSCEN | ETM_CR_PSCDIV, ctrl);
#endif

#if defined(CHIP_GM6621)
    /* Config prescaler */
    if (pConfig->prescalerEn)
    {
        ctrl |= ETM_CR_PSCEN;
        IO_BITMASK_SET(pETM->CR1, ETM_CR_PSCDIV, ETM_CR_PSCDIV_FIELD(pConfig->prescaler));
    }

    /* Setup fields of ETM control register */
    IO_BITMASK_SET(pETM->CR, ETM_CR_EXTCLK | ETM_CR_PSCEN, ctrl);
#endif

    return RET_OK;
}

RET_CODE ETM_StartCapture(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    /* Enable capture mode */
    IO_BIT_SET(pETM->CR, ETM_CR_CAPEN);

    /* Start counter */
    ret = ETM_StartTimeBase(pETM);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

RET_CODE ETM_StopCapture(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    /* Stop counter */
    ret = ETM_StopTimeBase(pETM);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Disable capture mode */
    IO_BIT_CLEAR(pETM->CR, ETM_CR_CAPEN);

    return RET_OK;
}

#if defined(CHIP_GM6621)
static void ETM_DmaCallback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    ETM_Handle_T *pETMHandle = GET_PARENTHDL_FROM_DMAHDL(hDMA, ETM_Handle_T);

    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        if ((pETMHandle != NULL) && (pETMHandle->callback != NULL))
        {
            pETMHandle->callback(pETMHandle->pDevice, event, NULL);
        }
    }
}

RET_CODE ETM_StartCaptureByDMA(ETM_Handle_T *pETMHandle, ETM_Channel_T channel, uint32_t *pData, uint16_t length)
{
    ETM_Device_T *pETM = pETMHandle->pDevice;
    DMA_Config_T dmaConfig;
    DMA_Config_T *pDMAConfig  = &dmaConfig;
    RET_CODE ret = RET_OK;

    memset(pDMAConfig, 0, sizeof(DMA_Config_T));
    /* Enable capture mode */
    IO_BIT_SET(pETM->CR, ETM_CR_CAPEN);

    pETMHandle->hDMA = &s_DMAHandle;

    pDMAConfig->callback  = ETM_DmaCallback;

    if (pETM == ETM0)
    {
        switch (channel)
        {
            case ETM_CHANNEL_1:
                pDMAConfig->requestType = DMA_REQUEST_ETM2CH0_TX;
                break;
            case ETM_CHANNEL_2:
                pDMAConfig->requestType = DMA_REQUEST_ETM2CH1_TX;
                break;
            case ETM_CHANNEL_3:
                pDMAConfig->requestType = DMA_REQUEST_ETM2CH2_TX;
                break;
            case ETM_CHANNEL_4:
                pDMAConfig->requestType = DMA_REQUEST_ETM2CH3_TX;
                break;
            default:
                pDMAConfig->requestType = DMA_REQUEST_INVALID;
                break;
        }
    }
    else if (pETM == ETM1)
    {
        switch (channel)
        {
            case ETM_CHANNEL_1:
                pDMAConfig->requestType = DMA_REQUEST_ETM21CH0_TX;
                break;
            case ETM_CHANNEL_2:
                pDMAConfig->requestType = DMA_REQUEST_ETM21CH1_TX;
                break;
            default:
                pDMAConfig->requestType = DMA_REQUEST_INVALID;
                break;
        }
    }
    else if (pETM == ETM2)
    {
        switch (channel)
        {
            case ETM_CHANNEL_1:
                pDMAConfig->requestType = DMA_REQUEST_ETM22CH0_TX;
                break;
            case ETM_CHANNEL_2:
                pDMAConfig->requestType = DMA_REQUEST_ETM22CH1_TX;
                break;
            default:
                pDMAConfig->requestType = DMA_REQUEST_INVALID;
                break;
        }
    }
    else
    {
        pDMAConfig->requestType = DMA_REQUEST_INVALID;
    }

    pDMAConfig->direction = DMA_PERIPH_TO_MEMORY;
    pDMAConfig->srcHandshake  = DMA_HARDWARE_HANDSHAKE;
    pDMAConfig->destHandshake = DMA_SOFTWARE_HANDSHAKE;
    pDMAConfig->srcIncrement  = DMA_ADDR_CONSTANT;
    pDMAConfig->destIncrement = DMA_ADDR_INCREMENT;
    pDMAConfig->srcWidth  = DMA_WIDTH_32BIT;
    pDMAConfig->destWidth = DMA_WIDTH_32BIT;

    dmaConfig.chnPriority = DMA_CHN_PRIORITY_0;
    dmaConfig.srcReload = DMA_NORMAL_MODE;
    dmaConfig.destReload = DMA_NORMAL_MODE;
    dmaConfig.srcGather = DMA_GATHER_DISABLE;
    dmaConfig.srcGatherInterval = 0;
    dmaConfig.srcGatherCount = 0;
    dmaConfig.destScatter = DMA_SCATTER_DISABLE;
    dmaConfig.destScatterInterval = 0;
    dmaConfig.destScatterCount = 0;

    ret = DMA_Init(DMA0, pDMAConfig, pETMHandle->hDMA);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Associate DMA Handle with ETM handle */
    LINK_DMAHDL_TO_PARENTHDL(pETMHandle, hDMA, s_DMAHandle);

    /* Enable the Input Capture measurement on in DMA mode */
    switch (channel)
    {
        case ETM_CHANNEL_1:
            DMA_Start(pETMHandle->hDMA, (uint32_t) & (pETM->CAP1), \
                      (uint32_t)(pData), length);
            IO_BIT_SET(pETM->IER, ETM_DMA_IC1);
            break;
        case ETM_CHANNEL_2:
            DMA_Start(pETMHandle->hDMA, (uint32_t) & (pETM->CAP2), \
                      (uint32_t)(pData), length);
            IO_BIT_SET(pETM->IER, ETM_DMA_IC2);
            break;
        case ETM_CHANNEL_3:
            DMA_Start(pETMHandle->hDMA, (uint32_t) & (pETM->CAP3), \
                      (uint32_t)(pData), length);
            IO_BIT_SET(pETM->IER, ETM_DMA_IC3);
            break;
        case ETM_CHANNEL_4:
            DMA_Start(pETMHandle->hDMA, (uint32_t) & (pETM->CAP4), \
                      (uint32_t)(pData), length);
            IO_BIT_SET(pETM->IER, ETM_DMA_IC4);
            break;
        default:
            break;
    }

    /* Start counter */
    ret = ETM_StartTimeBase(pETM);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

RET_CODE ETM_StopCaptureByDMA(ETM_Handle_T *pETMHandle, ETM_Channel_T channel)
{
    ETM_Device_T *pETM = pETMHandle->pDevice;
    RET_CODE ret = RET_OK;

    /* Stop counter */
    ret = ETM_StopTimeBase(pETM);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Disable the Input Capture measurement on in DMA mode */
    switch (channel)
    {
        case ETM_CHANNEL_1:
            IO_BIT_CLEAR(pETM->IER, ETM_DMA_IC1);
            break;
        case ETM_CHANNEL_2:
            IO_BIT_CLEAR(pETM->IER, ETM_DMA_IC2);
            break;
        case ETM_CHANNEL_3:
            IO_BIT_CLEAR(pETM->IER, ETM_DMA_IC3);
            break;
        case ETM_CHANNEL_4:
            IO_BIT_CLEAR(pETM->IER, ETM_DMA_IC4);
            break;
        default:
            break;
    }

    /* Disable capture mode */
    IO_BIT_CLEAR(pETM->CR, ETM_CR_CAPEN);

    /* release DMA device */
    DMA_DeInit(pETMHandle->hDMA);

    return RET_OK;
}

RET_CODE ETM_InitOnePulse(ETM_Device_T *pETM, ETM_OnePulseConfig_T *pConfig, ETM_Handle_T *pHandle)
{
    RET_CODE ret = RET_OK;
    uint32_t instance = 0;

    if ((pETM == NULL) || (pConfig == NULL))
    {
        return RET_INVPARAM;
    }

    instance = ETM_GetInstance(pETM);

    if (instance >= MAX_ETM_NUM)
    {
        return RET_ERROR;
    }

    if (pHandle != NULL)
    {
        /* Zero handle */
        memset(pHandle, 0, sizeof(*pHandle));

        /* Set callback */
        pHandle->pDevice = pETM;
        pHandle->callback = pConfig->callback;

        /* Save the context in global variables to support the double weak mechanism */
        s_ETMHandle[instance] = pHandle;
    }

    /* Disable time base counter */
    ret = ETM_StopTimeBase(pETM);
    if (ret != RET_OK)
    {
        s_ETMHandle[instance] = NULL;
        return RET_ERROR;
    }

    /* Enable nvic */
    NVIC_EnableIRQ(s_ETMIrqs[instance]);

    /* Set period value */
    pETM->ARR = (pConfig->repetitionCounter << 16) | pConfig->period;

    /* Config duty */
    IO_BITMASK_SET(pETM->CMP1, 0xFFFF, pConfig->duty);

    if (pConfig->polarity == TRUE)
    {
        IO_BIT_SET(pETM->CR, (1 << 8));
    }
    else
    {
        IO_BIT_CLEAR(pETM->CR, (1 << 8));
    }

    /* Disable capture mode */
    IO_BIT_CLEAR(pETM->CR, ETM_CR_CAPEN);

    return RET_OK;
}


RET_CODE ETM_StartOnePulse(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    /* Enable capture mode */
    IO_BIT_SET(pETM->CR, ETM_CR_ONEPULSEEN);

    return ret;
}

RET_CODE ETM_StopOnePulse(ETM_Device_T *pETM)
{
    RET_CODE ret = RET_OK;

    /* Disable capture mode */
    IO_BIT_CLEAR(pETM->CR, ETM_CR_ONEPULSEEN);

    return ret;
}
#endif

void ETM_EnableInterrupts(ETM_Device_T *pETM, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pETM->IER;

    pETM->IER = interruptMask | mask;
}

void ETM_DisableInterrupts(ETM_Device_T *pETM, uint32_t mask)
{
    uint32_t interruptMask = 0;

    interruptMask = pETM->IER;

    pETM->IER = interruptMask & (~ mask);
}

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
void ETM_TriggerADC(ETM_Device_T *pETM, ETM_Channel_T channel,  uint8_t polarity)
{
    switch (channel)
    {
        case ETM_CHANNEL_1:
            IO_BITMASK_SET(pETM->TRIGEN, 0x3, polarity);
            break;
        case ETM_CHANNEL_2:
            IO_BITMASK_SET(pETM->TRIGEN, 0xc, (polarity << 2));
            break;
        default:
            break;
    }
}
#endif

void ETM0_IRQHandler(void)
{
    ETM_CommonIRQHandler(ETM0, s_ETMHandle[0]);
}

void ETM1_IRQHandler(void)
{
    ETM_CommonIRQHandler(ETM1, s_ETMHandle[1]);
}

void ETM2_IRQHandler(void)
{
    ETM_CommonIRQHandler(ETM2, s_ETMHandle[2]);
}

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
void ETM3_IRQHandler(void)
{
    ETM_CommonIRQHandler(ETM3, s_ETMHandle[3]);
}
#endif

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
void ETM4_IRQHandler(void)
{
    ETM_CommonIRQHandler(ETM4, s_ETMHandle[4]);
}

void ETM5_IRQHandler(void)
{
    ETM_CommonIRQHandler(ETM5, s_ETMHandle[5]);
}
#endif

#if defined(CHIP_GM6621)
void ETM_PESConfig(ETM_Device_T *pETM, uint8_t chIndex)
{
    pETM->PESCR = (1 << 16) | chIndex;
}

void ETM_asMasterConfig(ETM_Device_T *pETM)
{
    pETM->PESCR &= ~(1 << 16);
    pETM->PESCR |= (1 << 24);
}

void ETM_PESEVDisable(ETM_Device_T *pETM)
{
    pETM->PESCR &= ~(1 << 24);
}
#endif
#endif
