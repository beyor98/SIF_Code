 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          hwir.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2016/07/05
 *
 * @brief         Header file of hwir driver.
 *
 * @note
 *    2016/03/01, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/05, Mike.Zheng, V1.1.0
 *        Update coding style.
 */
#include <drv/dev/hwir.h>

HWIR_Callback_T s_hwirCallback = NULL;
static HWIR_Handle_T *s_hHWIR = NULL;
static HWIR_DevConfig_T s_DevCfg;/* remember ir wave cfg */
static void HWIR_IrqHandler(void);
static RET_CODE HWIR_CommonConfig(HWIR_Handle_T *hHWIR);
static void HWIR_SetDecWavebitMask(HWIR_Handle_T *hHWIR);
static void HWIR_CfgDecMatchThr(HWIR_Handle_T *hHWIR);
static RET_CODE HWIR_ConfigProctolWave(HWIR_Handle_T *hHWIR,
                                       HWIR_KeyDesc_T *pkeyDesc);
static void HWIR_ConfigFMProctolWave(HWIR_Handle_T *hHWIR,
                                     HWIR_KeyDesc_T *pkeyDesc);
static void HWIR_ConfigFNProctolWave(HWIR_Handle_T *hHWIR,
                                     HWIR_KeyDesc_T *pkeyDesc);
static void HWIR_ConfigFXLogicBit(HWIR_Handle_T *hHWIR,uint32_t Logic_Polary,
                                  uint8_t *pWavePos);
static void HWIR_CfgWaveBit(HWIR_Handle_T *hHWIR,uint8_t *pWavePos,
                            HWIR_Polarity_T wavePolary,uint8_t waveThrLevel);
static void HWIR_DevConfig(HWIR_DevConfig_T *pDevCfg, HWIR_Protocol_T prctocol);

STATIC_ISR(HWIR_IRQn, HWIR_IrqHandler);

static void HWIR_IrqHandler(void)
{
    uint32_t status = 0;
    HWIR_Device_T *pDev = s_hHWIR->device;

    status = pDev->IS;
    if (status & HWIR_IS_DS)
    {
	    HWIR_CLEAR_INTERRUPT(pDev, HWIR_IS_DS);
        if (s_hwirCallback)
        {
            s_hwirCallback(s_hHWIR, HWIR_DECODE_SUCCESS, 0);
        }
    }
    else
    {
        HWIR_PR_ERR("Error HWIR IRQ Generate\n");
    }
}

static RET_CODE HWIR_CommonConfig(HWIR_Handle_T *hHWIR)
{
    RET_CODE ret = RET_OK ;
    HWIR_Protocol_T prctocol = hHWIR->prctocol;
    HWIR_DevConfig_T *pDevCfg = hHWIR->pDevCfg;
    HWIR_Device_T *pDev = hHWIR->device;

    if ((prctocol >= PROTOCOL_F0) && (prctocol <= PROTOCOL_FM))
    {
        HWIR_DevConfig(pDevCfg, prctocol);
    }
    else
    {
        ret = RET_INVPARAM;
        HWIR_PR_ERR("\rerr ir_prctocol:%d %s\n",prctocol,__FUNCTION__);
        goto out;
    }
    /* set 1us as measue uint */
    HWIR_SET_DIV_MEASURE(pDev, pDevCfg->div);
    /* set noise threshold */
    HWIR_SET_NOISE_THR(pDev, pDevCfg->noiseThr);
    /* set timeout threshold */
    HWIR_SET_TIMEOUT_THR(pDev, pDevCfg->timeoutThr);
    /* set Threshold levle num for HWIR Decode match*/
    HWIR_SET_DECODE_THR(pDev, pDevCfg->decMatchThrNum);
    /* set decode bitnum */
    HWIR_SET_DECODE_BITNUM(pDev, pDevCfg->decWaveBitNum);
    /* config wave match threshold */
    HWIR_CfgDecMatchThr(hHWIR);
    /* Mask not care wave bit */
    HWIR_SetDecWavebitMask(hHWIR);
out:
    return ret;
}

RET_CODE HWIR_Init(HWIR_Device_T *pDev, HWIR_Config_T *pConfig ,HWIR_Handle_T *hHWIR)
{
    RET_CODE ret = RET_OK;

    if (!(pDev && pConfig && hHWIR))
    {
        HWIR_PR_ERR("%s: input param error\n",__FUNCTION__);
        ret = RET_INVPARAM;
        goto out;
    }

    s_hHWIR = hHWIR;
    s_hHWIR->device = pDev;
    s_hHWIR->prctocol = pConfig->keyDesc.protocol;
    s_hHWIR->pDevCfg = &s_DevCfg;
    memset(&s_DevCfg, 0, sizeof(HWIR_DevConfig_T));

    /* disable HWIR modules */
    HWIR_Disable(hHWIR);
    /* enable nvic and install irq callbak */
    NVIC_EnableIRQ(HWIR_IRQn);
    s_hwirCallback = pConfig->callback;

    HWIR_RESET();
    HWIR_CLOCK_ENABLE();
    /* enable irq */
    HWIR_ENABLE_INTERRUPT(pDev, HWIR_DSIE);
    /* fill ir config struct for different protorl */
    ret = HWIR_CommonConfig(hHWIR);
    if (ret != RET_OK)
    {
        goto out;
    }
    /* config keycode wave to hwir reg and disable ir decode */
    ret = HWIR_ConfigProctolWave(hHWIR, &(pConfig->keyDesc));
    if (ret != RET_OK)
    {
        goto out;
    }
out:
    return ret;
}

static RET_CODE HWIR_ConfigProctolWave(HWIR_Handle_T *hHWIR, HWIR_KeyDesc_T *pkeyDesc)
{
    RET_CODE ret = RET_OK;
    HWIR_Protocol_T prctocol = pkeyDesc->protocol;

    switch (prctocol)
    {
    case PROTOCOL_F0:
    case PROTOCOL_F1:
    case PROTOCOL_F2:
    case PROTOCOL_F3:
    case PROTOCOL_F4:
         HWIR_ConfigFNProctolWave(hHWIR, pkeyDesc);
         break;

    case PROTOCOL_FM:
         HWIR_ConfigFMProctolWave(hHWIR, pkeyDesc);
         break;

    default:
         HWIR_PR_ERR("err ir_prctocol:%d %s\r\n",prctocol,__FUNCTION__);
         ret = RET_INVPARAM;
         break;
    }

    return ret;
}

static void HWIR_DevConfig(HWIR_DevConfig_T *pDevCfg, HWIR_Protocol_T prctocol)
{
    if ((PROTOCOL_F0 == prctocol) || \
        (PROTOCOL_F1 == prctocol) || \
        (PROTOCOL_F2 == prctocol) || \
        (PROTOCOL_F3 == prctocol) || \
        (PROTOCOL_F4 == prctocol))
    {
        pDevCfg->bitMask[0]   = HWIR_FN_MASK_DONCA0;
        pDevCfg->bitMask[1]   = HWIR_FN_MASK_DONCA1;
        pDevCfg->bitMask[2]   = HWIR_FN_MASK_DONCA2;
        pDevCfg->bitMask[3]   = HWIR_FN_MASK_DONCA3;

        pDevCfg->matchThr[0]  = HWIR_FX_MATCH_LEVEL0_THR;
        pDevCfg->matchThr[1]  = HWIR_FX_MATCH_LEVEL1_THR;
        pDevCfg->matchThr[2]  = HWIR_FX_MATCH_LEVEL2_THR;
        pDevCfg->matchThr[3]  = HWIR_FX_MATCH_LEVEL3_THR;
        pDevCfg->matchThr[4]  = HWIR_FX_MATCH_LEVEL4_THR;
        pDevCfg->matchThr[5]  = HWIR_FX_MATCH_LEVEL5_THR;
        pDevCfg->matchThr[6]  = HWIR_FX_MATCH_LEVEL6_THR;
        pDevCfg->matchThr[7]  = HWIR_FX_MATCH_LEVEL7_THR;

        pDevCfg->decWaveBitNum  = HWIR_FN_DEC_WAVE_BITNUM;
        pDevCfg->decMatchThrNum = HWIR_FX_DEC_THR_NUM;

        pDevCfg->div            = HWIR_FX_CLK_DIV_VALU;
        pDevCfg->noiseThr       = HWIR_FX_NOISE_THR;
        pDevCfg->timeoutThr     = HWIR_FN_TIMEOUT_THR;

    }
    else if (PROTOCOL_FM == prctocol)
    {
        pDevCfg->bitMask[0]  = HWIR_FM_MASK_DONCA0;
        pDevCfg->bitMask[1]  = HWIR_FM_MASK_DONCA1;
        pDevCfg->bitMask[2]  = HWIR_FM_MASK_DONCA2;
        pDevCfg->bitMask[3]  = HWIR_FM_MASK_DONCA3;

        pDevCfg->matchThr[0] = HWIR_FX_MATCH_LEVEL0_THR;
        pDevCfg->matchThr[1] = HWIR_FX_MATCH_LEVEL1_THR;
        pDevCfg->matchThr[2] = HWIR_FX_MATCH_LEVEL2_THR;
        pDevCfg->matchThr[3] = HWIR_FX_MATCH_LEVEL3_THR;
        pDevCfg->matchThr[4] = HWIR_FX_MATCH_LEVEL4_THR;
        pDevCfg->matchThr[5] = HWIR_FX_MATCH_LEVEL5_THR;
        pDevCfg->matchThr[6] = HWIR_FX_MATCH_LEVEL6_THR;
        pDevCfg->matchThr[7] = HWIR_FX_MATCH_LEVEL7_THR;

        pDevCfg->decWaveBitNum  = HWIR_FM_DEC_WAVE_BITNUM;
        pDevCfg->decMatchThrNum = HWIR_FX_DEC_THR_NUM;

        pDevCfg->div            = HWIR_FX_CLK_DIV_VALU;
        pDevCfg->noiseThr       = HWIR_FX_NOISE_THR;
        pDevCfg->timeoutThr     = HWIR_FM_TIMEOUT_THR;
    }
    else
    {
        /* Todo for more other gree ir prctocol */
        HWIR_PR_WARNINR("\r%s:%d Todo for more other gree ir_prctocol\n",
                           __FUNCTION__,__LINE__);
    }

}

static void HWIR_ConfigFXLogicBit(HWIR_Handle_T *hHWIR,
                                  uint32_t Logic_Polary, uint8_t *pWavePos)
{
    if (Logic_Polary == 0)
    {
        HWIR_CfgWaveBit(hHWIR, pWavePos, LEVEL_LOW, HWIR_FX_DATABIT0_LOW_MATCH_THRLEVEL);
        HWIR_CfgWaveBit(hHWIR, pWavePos, LEVEL_HIGH, HWIR_FX_DATABIT0_HIGH_MATCH_THRLEVEL);
    }
    else
    {
        HWIR_CfgWaveBit(hHWIR, pWavePos, LEVEL_LOW,HWIR_FX_DATABIT1_LOW_MATCH_THRLEVEL);
        HWIR_CfgWaveBit(hHWIR, pWavePos, LEVEL_HIGH,HWIR_FX_DATABIT1_HIGH_MATCH_THRLEVEL);
    }
}

static void HWIR_ConfigFNProctolWave(HWIR_Handle_T *hHWIR, HWIR_KeyDesc_T *pkeyDesc)
{
    uint8_t  i      = 0;
    uint8_t  temp   = 0;
    uint32_t data0  = 0;  /* bit[27:0] is vaild */
    uint32_t data1  = 0;  /* bit[27:0] is vaild */
    uint8_t  cx     = 0;  /* bit[6:0]  is vaild */
    uint8_t  verify = 0;  /* bit[3:0]  is vaild */

    HWIR_Protocol_T prctocol = pkeyDesc->protocol;
    uint8_t wavePos = HWIR_WAVE_DEC_START_POS;

    /* Get Cx Code */
    if (PROTOCOL_F0 == prctocol)
    {
        cx = HWIR_F0_CUSTOME_CODE;
    }
    else if (PROTOCOL_F1 == prctocol)
    {
        cx = HWIR_F1_CUSTOME_CODE;
    }
    else if (PROTOCOL_F2 == prctocol)
    {
        cx = HWIR_F2_CUSTOME_CODE;
    }
    else if (PROTOCOL_F3 == prctocol)
    {
        cx = HWIR_F3_CUSTOME_CODE;
    }
    else if (PROTOCOL_F4 == prctocol)
    {
        cx = HWIR_F4_CUSTOME_CODE;
    }
    /* Get Data Code */
    data0 = pkeyDesc->keyValue.keyValueFN.dataCode0;
    data1 = pkeyDesc->keyValue.keyValueFN.dataCode1;

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
    for (i = 1; i < 7; i += 2 )
    {
		temp = (data1 & (0xf << (4 * i))) >> (4 * i);
        verify += temp;
    }
    /* get the resulet of FN protol verify : Nibble17 4bit */
	verify &= 0xf;

    /* config start bit */
    HWIR_CfgWaveBit(hHWIR, &wavePos, LEVEL_LOW,HWIR_FN_STARTBIT_LOW_MATCH_THRLEVEL);
    HWIR_CfgWaveBit(hHWIR, &wavePos, LEVEL_HIGH,HWIR_FN_STARTBIT_HIGH_MATCH_THRLEVEL);
    /* config  1st data(28bit) LSB */
    for (i = 0; i < HWIR_FN_DATA_LOGICBIT_NUM ; i++)
    {
        HWIR_ConfigFXLogicBit(hHWIR, (data0 & (1 << i)) , &wavePos);
    }
    /* config Cx(7bit) LSB */
    for (i = 0; i < HWIR_FN_CX_LOGICBIT_NUM ; i++)
    {
        HWIR_ConfigFXLogicBit(hHWIR, (cx & (1 << i)), &wavePos);
    }
    /* config stop bit (only half bit) */
    HWIR_CfgWaveBit(hHWIR, &wavePos, LEVEL_LOW,HWIR_FX_STOP_MATCH_THRLEVEL);
    /* config 20MS logbit ( half bit ) */
    HWIR_CfgWaveBit(hHWIR, &wavePos, LEVEL_HIGH,HWIR_FN_DELAY20MS_MATCH_THRLEVEL);
     /* config 2st data(27bit) LSB,reach to HW Max dec bit */
    for (i = 0; i < (HWIR_FN_DATA_LOGICBIT_NUM - 1); i++)
    {
        HWIR_ConfigFXLogicBit(hHWIR, data1&(1 << i), &wavePos);
    }
}

static void HWIR_ConfigFMProctolWave(HWIR_Handle_T *hHWIR, HWIR_KeyDesc_T *pkeyDesc)
{
    uint8_t i        = 0;
    uint8_t verify   = 1; /* !!!Todo cala data verigy, dont have fm calc verify sepc */
    uint8_t wavePos  = HWIR_WAVE_DEC_START_POS ;
    uint8_t dataCode = pkeyDesc->keyValue.keyValueFM.dataCode;

    /* config start bit */
    HWIR_CfgWaveBit(hHWIR, &wavePos, LEVEL_LOW,HWIR_FM_STARTBIT_LOW_MATCH_THRLEVEL);
    HWIR_CfgWaveBit(hHWIR, &wavePos, LEVEL_HIGH,HWIR_FM_STARTBIT_HIGH_MATCH_THRLEVEL);

    /* config 1st data(8bit) LSB */
    for (i = 0; i < HWIR_FM_DATA_LOGICBIT_NUM; i++)
    {
        HWIR_ConfigFXLogicBit(hHWIR, (dataCode & (1 << i)), &wavePos);
    }
    /*config verify(8bit) LSB */
    for (i = 0; i < HWIR_FM_VERIFY_LOGICBIT_NUM ; i++)
    {
        HWIR_ConfigFXLogicBit(hHWIR, (verify & (1 << i)), &wavePos);
    }

    /*config stop logic bit(only half bit) */
    HWIR_CfgWaveBit(hHWIR, &wavePos, LEVEL_LOW,HWIR_FX_STOP_MATCH_THRLEVEL);
}

static void HWIR_CfgDecMatchThr(HWIR_Handle_T *hHWIR)
{
    uint8_t i = 0;
    HWIR_Device_T *pDev = hHWIR->device;
    HWIR_DevConfig_T *pDevCfg = hHWIR->pDevCfg;

    for (i = 0; i < ((pDevCfg->decMatchThrNum) >> 1 ); i++)
    {
        pDev->DECTHR[i] = pDevCfg->matchThr[2 * i] | (pDevCfg->matchThr[2 * i + 1] << 16);
    }

    if ((pDevCfg->decMatchThrNum) % 2)
    {
        IO_BITMASK_SET(pDev->DECTHR[i], 0xffff, (pDevCfg->matchThr[2 * i]) << 0);
    }
}

static void HWIR_CfgWaveBit(HWIR_Handle_T *hHWIR, uint8_t *pwavePos,
                            HWIR_Polarity_T wavePolary, uint8_t waveThrLevel)
{
    HWIR_Device_T *pDev = hHWIR->device;
    uint8_t thrIndexY   = (4 * (*pwavePos)) >> 5;
    uint8_t thrIndexX   = (4 * (*pwavePos)) % 32;
    uint8_t polIndexY   = (*pwavePos) >> 5;
    uint8_t polIndexX   = (*pwavePos) % 32;

    /* config databit wave threshold level*/
    IO_BITMASK_SET(pDev->DECTS[thrIndexY], (0xf << thrIndexX),    \
                  (waveThrLevel << thrIndexX));
    /* config databit wave polary */
    IO_BITMASK_SET(pDev->DECPOL[polIndexY], (1 << polIndexX),    \
                  (wavePolary << polIndexX));

    (*pwavePos)++;
}

static void HWIR_SetDecWavebitMask(HWIR_Handle_T *hHWIR)
{
    int i  = 0;
    HWIR_Device_T *pDev = hHWIR->device;
    HWIR_DevConfig_T *pDevCfg = hHWIR->pDevCfg;

    /* Mask All The Bit */
    for (i=0 ; i < 4 ; i++)
    {
        pDev->DECDONCA[i] = pDevCfg->bitMask[i];
    }
}

void HWIR_Enable(HWIR_Handle_T *hHWIR)
{
    HWIR_Device_T *pDev = hHWIR->device;
    IO_BIT_CLEAR(pDev->CFG, HWIR_CFG_EN);
    SysTick_DelayUS(1);
    IO_BITS_SET(pDev->CFG, HWIR_CFG_EN);
}

void HWIR_Disable(HWIR_Handle_T *hHWIR)
{
    HWIR_Device_T *pDev = hHWIR->device;
    IO_BIT_CLEAR(pDev->CFG, HWIR_CFG_EN);
}
