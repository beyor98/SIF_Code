/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lvd.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/08/22
 *
 * @brief        LVD APIs for application.
 *
 * @note
 *      2016/08/22, mark.lee, V1.0.0  Initial version.
 *      2017/05/19, yangy, V1.0.1     Modified LVD_Init to enable that VD1 and VD2 can work together.
 *      2019/04/24, yangy, V1.1.0     Catch interrupt on the driver layer.
 *      2021/08/26, terry_lee, V1.2.0 reconstruction of lvd driver code.
 */

#include <stdio.h>
#include <drv/dev/lvd.h>

LVD_Callback g_lvd_user_cb = NULL;

STATIC_ISR(LVD_IRQn, LVD_IRQHandler);

static void LVD_IRQHandler(void)
{
    int vd_idx = 0;

#if defined (CHIP_GM6611B) || defined (CHIP_GM6621) || defined (CHIP_F6621B)
    /* only vd2 has interrupt */
    if (IO_BITS_GET(LVD->VD2SR, LVD_CR_VOL2DT_INTFLG))
    {
        LVD_ClearInterruptFlag(LVD_VOLDT_2);
        vd_idx = 2;
    }
#else
    if (IO_BITS_GET(SYSDEV->VDSR, LVD_CR_VOL1DT_INTFLG))
    {
        LVD_ClearInterruptFlag(LVD_VOLDT_1);
        vd_idx = 1;
    }
    else if (IO_BITS_GET(SYSDEV->VDSR, LVD_CR_VOL2DT_INTFLG))
    {
        LVD_ClearInterruptFlag(LVD_VOLDT_2);
        vd_idx = 2;
    }
    else if (IO_BITS_GET(SYSDEV->VD3SR, LVD_CR_VOL3DT_INTFLG))
    {
        LVD_ClearInterruptFlag(LVD_VOLDT_3);
        vd_idx = 3;
    }
#endif

    if (g_lvd_user_cb)
    {
        g_lvd_user_cb(vd_idx);
    }
}

int LVD_GetVDXFlag(int vdIdx)
{
#if defined (CHIP_GM6601B) || defined (CHIP_F6601C)
    if (1 == vdIdx)
    {
        //0: Current VDD >= VD1LVL
        //1: Current VDD < VD1LVL
        return IO_BITS_GET(SYSDEV->VDSR, LVD_CR_VOL1DT_OUT);
    }
    else if (2 == vdIdx)
    {
        return IO_BITS_GET(SYSDEV->VDSR, LVD_CR_VOL2DT_OUT);
    }
    else if (3 == vdIdx)
    {
        return IO_BITS_GET(SYSDEV->VD3SR, LVD_CR_VOL3DT_OUT);
    }
#else
    if (1 == vdIdx)
    {
        return IO_BITS_GET(LVD->VD1SR, LVD_CR_VOL1DT_OUT);
    }
    else if (2 == vdIdx)
    {
        return IO_BITS_GET(LVD->VD2SR, LVD_CR_VOL2DT_OUT);
    }
    else if (3 == vdIdx)
    {
        return IO_BITS_GET(LVD->VD3SR, LVD_CR_VOL3DT_OUT);
    }
#endif
    
    return 0;
}


int LVD_Init(const LVD_Config_T *pConfig1, const LVD_Config_T *pConfig2,
             const LVD_Config_T *pConfig3)
{
    uint32_t ctrl1, ctrl2, ctrl3;
    uint32_t mask1, mask2, mask3;
    ctrl1 = ctrl2 = ctrl3 = 0;
    mask1 = mask2 = mask3 =0;

    if (!(pConfig1 || pConfig2 || pConfig3))
    {
        printf("Error:all param is NULL\n");
        return -1;
    }

#if defined (CHIP_GM6601B) || defined (CHIP_F6601C)
    if (pConfig1 && pConfig1->int_call)
    {
        g_lvd_user_cb = pConfig1->int_call;
    }
    else if (pConfig2 && pConfig2->int_call)
    {
        g_lvd_user_cb = pConfig2->int_call;
    }
    else if (pConfig3 && pConfig3->int_call)
    {
        g_lvd_user_cb = pConfig3->int_call;
    }
#else    
    if (pConfig2 && pConfig2->int_call)
    {
        g_lvd_user_cb = pConfig2->int_call;
    }
#endif
    
    if (pConfig1)
    {
        if (pConfig1->reset)
        {
            /* busywait to avoid restart again and again util rasing the voltage. */
            while (LVD_GetVDXFlag(1)) {;}

            mask1 = LVD_CR_VOLDT1_RSTEN;
            ctrl1 = LVD_CR_VOLDT1_RSTEN;
        }
 
#if !(defined (CHIP_F6621B))
        /* Sets timing threshold for filter the glitch of voltage detector */
        ctrl1 |= LVD_CR_TRH1_FIELD(pConfig1->threshold);
#endif
        /* Sets timing threshold for filter the glitch of voltage detector */
        ctrl1 |= LVD_CR_DT1LEVEL_FIELD(pConfig1->detectionLevel);

        /* Enable voltage detector operation */
        ctrl1 |= ((pConfig1->enable) ? LVD_CR_VOLDT1_EN : 0);

        /* Enable Voltage detection reset chip */
#if defined (CHIP_F6621B)
        mask1 |= LVD_CR_VOLDT1_EN | LVD_CR_DT1LEVEL ;
#elif defined (CHIP_G6601B) || defined (CHIP_F6601C)
        /* Enable voltage detector interrupt */
        ctrl1 |= ((pConfig1->int_en) ? LVD_CR_VOLDT1_INTEN : 0);
        mask1 |= LVD_CR_TRH1 | LVD_CR_DT1LEVEL | LVD_CR_VOLDT1_EN | LVD_CR_VOLDT1_INTEN;
#else
        mask1 |= LVD_CR_TRH1 | LVD_CR_DT1LEVEL | LVD_CR_VOLDT1_EN;
#endif
    }
    
    if (pConfig2)
    {
#if defined (CHIP_GM6601B) || defined (CHIP_F6601C)
        if (pConfig2->reset)
        {
            while (LVD_GetVDXFlag(2)) {;}

            mask2 = LVD_CR_VOLDT2_RSTEN;
            ctrl2 = LVD_CR_VOLDT2_RSTEN;
        }
#endif
        /* Sets timing threshold for filter the glitch of voltage detector */
        ctrl2 |= LVD_CR_TRH2_FIELD(pConfig2->threshold);

        /* Sets timing threshold for filter the glitch of voltage detector */
        ctrl2 |= LVD_CR_DT2LEVEL_FIELD(pConfig2->detectionLevel);
#if defined (CHIP_F6621B)
        /*sets  Prescaler of the voltage detector 2's filter clock */
        ctrl2 |= LVD_FILTER_CLK_PSC2_FIELD(pConfig2->filterclk);
#endif
        /* Enable voltage detector operation */
        ctrl2 |= ((pConfig2->enable) ? LVD_CR_VOLDT2_EN : 0);

        /* Enable voltage detector interrupt */
        ctrl2 |= ((pConfig2->int_en) ? LVD_CR_VOLDT2_INTEN : 0);

        /* Enable Voltage detection reset chip */
        mask2 |= LVD_CR_TRH2 | LVD_CR_DT2LEVEL | LVD_CR_VOLDT2_EN | LVD_CR_VOLDT2_INTEN;
    }

#if defined(CHIP_GM6621) || defined(CHIP_GM6611B) || defined(CHIP_F6621B)
    if (pConfig1)
    {
        /* Setup fields of Voltage Detector Control Register */
        IO_BITMASK_SET(LVD->VD1CR, mask1, ctrl1);
    }
    if (pConfig2)
    {
        /* Setup fields of Voltage Detector Control Register */
        IO_BITMASK_SET(LVD->VD2CR, mask2, ctrl2);
    }
#else
    if (pConfig1 || pConfig2)
    {
#if defined (CHIP_GM6601B)
        IO_BITMASK_SET(SYSDEV->VDCR, mask1 | mask2, ctrl1 | ctrl2);
#elif defined (CHIP_F6601C)
        //LVD register just can write for 1 time! can not clear and then write it!
        SYSDEV->VDCR = ctrl1 | ctrl2;
#endif
    }
#endif
    

    if (pConfig3)
    {
        if (pConfig3->reset)
        {
            /* busywait to avoid restart again and again util rasing the voltage. */
            while (LVD_GetVDXFlag(3)) {;}

            mask3 = LVD_CR_VOLDT3_RSTEN;
            ctrl3 = LVD_CR_VOLDT3_RSTEN;
        }

#if !(defined (CHIP_F6621B))
        /* Sets timing threshold for filter the glitch of voltage detector */
        ctrl3 |= LVD_CR_TRH3_FIELD(pConfig3->threshold);
#endif
        /* Sets timing threshold for filter the glitch of voltage detector */
        ctrl3 |= LVD_CR_DT3LEVEL_FIELD(pConfig3->detectionLevel);

        /* Enable voltage detector operation */
        ctrl3 |= ((pConfig3->enable) ? LVD_CR_VOLDT3_EN : 0);

#if defined (CHIP_GM6601B) || defined (CHIP_F6601C)
        /* Enable voltage detector interrupt */
        ctrl3 |= ((pConfig3->int_en) ? LVD_CR_VOLDT3_INTEN : 0);
#endif

#if defined (CHIP_F6621B)
        /* Enable Voltage detection reset chip */
        mask3 |= LVD_CR_DT3LEVEL | LVD_CR_VOLDT3_EN;
#elif defined (CHIP_GM6601B) || defined (CHIP_F6601C)
        /* Enable Voltage detection reset chip */
        mask3 |= LVD_CR_TRH3 | LVD_CR_DT3LEVEL | LVD_CR_VOLDT3_EN | LVD_CR_VOLDT3_INTEN;
#else
        mask3 |= LVD_CR_TRH3 | LVD_CR_DT3LEVEL | LVD_CR_VOLDT3_EN;
#endif

#if defined (CHIP_GM6601B)
        IO_BITMASK_SET(SYSDEV->VD3CR, mask3, ctrl3);
#elif defined (CHIP_F6601C)
        SYSDEV->VD3CR = ctrl3;  //clear and then write will fail
#else
        IO_BITMASK_SET(LVD->VD3CR, mask3, ctrl3);
#endif
    }

    return 0;
}


void LVD_EnableInterrupt(LVD_VoltageDetection_T voltage)
{
    // ATTENTION: In fact, after LVD_Init(), we can not write lvd regitsters here!
    // LVD registers just can write only for 1 time!
    
#if defined(CHIP_GM6621) || defined(CHIP_GM6611B) || defined(CHIP_F6621B)
    if (voltage == LVD_VOLDT_2)
    {
        /* Enable voltage detector 2 interrupt */
        IO_BITS_SET(LVD->VD2CR, LVD_CR_VOLDT2_INTEN);
    }
#else
    if (voltage == LVD_VOLDT_1)
    {
        /* Enable voltage detector 1 interrupt */
        IO_BITS_SET(SYSDEV->VDCR, LVD_CR_VOLDT1_INTEN);
    }
    else if (voltage == LVD_VOLDT_2)
    {
        /* Enable voltage detector 2 interrupt */
        IO_BITS_SET(SYSDEV->VDCR, LVD_CR_VOLDT2_INTEN);
    }
    else if (voltage == LVD_VOLDT_3)
    {
        /* Enable voltage detector 3 interrupt */
        IO_BITS_SET(SYSDEV->VD3CR, LVD_CR_VOLDT3_INTEN);
    }
#endif

    NVIC_EnableIRQ(LVD_IRQn);
}


void LVD_DisableInterrupt(LVD_VoltageDetection_T voltage)
{
#if defined(CHIP_GM6621) || defined(CHIP_GM6611B) || defined(CHIP_F6621B)
    if (voltage == LVD_VOLDT_2)
    {
        /* Disable voltage detector 2 interrupt */
        IO_BITS_CLEAR(LVD->VD2CR, LVD_CR_VOLDT2_INTEN);
    }
#else
    if (voltage == LVD_VOLDT_1)
    {
        /* Disable voltage detector 1 interrupt */
        IO_BITS_CLEAR(SYSDEV->VDCR, LVD_CR_VOLDT1_INTEN);
    }
    else if (voltage == LVD_VOLDT_2)
    {
        /* Disable voltage detector 2 interrupt */
        IO_BITS_CLEAR(SYSDEV->VDCR, LVD_CR_VOLDT2_INTEN);
    }
    else if (voltage == LVD_VOLDT_3)
    {
        IO_BITS_CLEAR(SYSDEV->VD3CR, LVD_CR_VOLDT3_INTEN);
    }
#endif
}


void LVD_ClearInterruptFlag(LVD_VoltageDetection_T voltage)
{
#if defined(CHIP_GM6621) || defined(CHIP_GM6611B) || defined(CHIP_F6621B)
    if (voltage == LVD_VOLDT_2)
    {
        /* Clear voltage detector 2 interrupt flag */
        IO_BITS_SET(LVD->VD2SR, LVD_CR_VOL2DT_INTFLG);
    }
#else
    if (voltage == LVD_VOLDT_1)
    {
        /* Clear voltage detector 1 interrupt flag */
        IO_BITS_SET(SYSDEV->VDSR, LVD_CR_VOL1DT_INTFLG);
    }
    else if (voltage == LVD_VOLDT_2)
    {
        /* Clear voltage detector 2 interrupt flag */
        IO_BITS_SET(SYSDEV->VDSR, LVD_CR_VOL2DT_INTFLG);
    }
    else if (voltage == LVD_VOLDT_3)
    {
        IO_BITS_SET(SYSDEV->VD3SR, LVD_CR_VOL3DT_INTFLG);
    }
#endif
}


void LVD_SetOverCurrentProtection(uint8_t enable, LVD_OverCurrentThr_T threshold)
{
#if !( defined (CHIP_GM6621) || defined (CHIP_F6621B))
    if (enable == TRUE)
    {
        /* Enable Over current limit protection */
        IO_BITS_SET(SYSDEV->ACR, LVD_ACR_OCP_EN);
        
    #if defined(CHIP_F6601C)
        if (threshold == LVD_Current_80)
        {
            IO_BIT_CLEAR(SYSDEV->ACR, LVD_ACR_OCP_TRH);
        } 
        else if (threshold == LVD_Current_120)
        {
            IO_BIT_SET(SYSDEV->ACR, LVD_ACR_OCP_TRH);
        }
    #else
        IO_BITMASK_SET(SYSDEV->ACR, LVD_ACR_OCP_TRH, LVD_ACR_OCP_TRH_FIELD(threshold));
    #endif
    }
    else
    {
        /* Disable Over current limit protection */
        IO_BITS_CLEAR(SYSDEV->ACR, LVD_ACR_OCP_EN);
    }
#endif
}

