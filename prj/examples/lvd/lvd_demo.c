/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lvd_demo.c
 *
 * @author        yangy
 *
 * @version       1.0.0
 *
 * @date          2017/06/15
 *
 * @brief        Test Code for Low Voltage Detect on Board GM6611/6601B/GM6611B
 *
 *                  GM6611B:
 *                  common usage:
 *                      VD2 > VD1 > VD3
 *                  use VD2 to warning that a dangerous voltage occurs, then VD1 to reset the chip.
 *                  (vd2/vd1 - vcc)
 *                  (vd2/vd1 - vdd-core)
 * @note
 *    2017/06/15, yangy, V1.0.0
 *        Initial version.
 *    2019/04/25, yangy, V1.1.0
 *        Simplify demo code.
 *    2021/11/23, terrylee, V1.1.1
 *        add support for F6601C.
 */
//#include <stdio.h>
//#include <common.h>
#include <drv/dev/lvd.h>
#include <drv/dev/uart.h>
#include <drv/dev/gpio.h>

#define DBG_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif


/***************************************************************************************************************************
 *|        GM6601B        |        GM6611B        |        GM6621         |        F6621B         |        F6601C         |
 *|  VD1  |  VD2  |  VD3  |  VD1  |  VD2  |  VD3  |  VD1  |  VD2  |  VD3  |  VD1  |  VD2  |  VD3  |  VD1  |  VD2  |  VD3  |
 *|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|RST|INT|
 *| Y | Y | Y | Y | Y | Y | Y | N | N | Y | Y | N | Y | N | N | Y | Y | N | Y | N | N | Y | Y | N | Y | Y | Y | Y | Y | Y |
 ***************************************************************************************************************************/
#if defined (CHIP_GM6601B) || defined (CHIP_F6601C)
    #define LVD_VOL1DT_Level            LVD_DTVOL1_Level_3
    #define LVD_VOL2DT_Level            LVD_DTVOL1_Level_2
    #define LVD_VOL3DT_Level            LVD_DTVOL3_Level_2
#else
    #define LVD_VOL1DT_Level            LVD_DTVOL1_Level_6
    #define LVD_VOL2DT_Level            LVD_DTVOL1_Level_7
    #define LVD_VOL3DT_Level            LVD_DTVOL3_Level_3
#endif


static void LvdCallback(int vd_idx)
{
    PR_DEBUG("!!! Low Voltage Warning !!!(by vd%d)\n", vd_idx);
}

static void BasicInit()
{
    SysTick_Init();

#if (SYS_CLOCK == 48000000)
    DebugConsoleInit(115200); //8M-19200, 48M-115200
#else
    DebugConsoleInit(19200); //8M-19200, 48M-115200
#endif
    SysTick_DelayMS(500);
}

static void BasicConfigure(LVD_Config_T *pVD1, LVD_Config_T *pVD2, LVD_Config_T *pVD3)
{
    memset(pVD1, 0, sizeof(LVD_Config_T));
    memset(pVD2, 0, sizeof(LVD_Config_T));
    memset(pVD3, 0, sizeof(LVD_Config_T));

    pVD1->enable    = TRUE;
    pVD1->threshold = 0x1;
    pVD1->int_en    = TRUE;
    pVD1->int_call  = LvdCallback;
    pVD1->reset     = TRUE;

    *pVD2 = *pVD1;
    *pVD3 = *pVD1;

    /* Set VD1 */
    pVD1->detectionLevel = LVD_VOL1DT_Level;
    pVD1->reset = FALSE;

    /* Set VD2 */
    pVD2->detectionLevel = LVD_VOL2DT_Level;
    pVD2->reset = FALSE;

    /* Set VD3 */
    pVD3->detectionLevel = LVD_VOL3DT_Level;
    pVD3->reset = FALSE;
}


int main(void)
{
    LVD_Config_T lvdInfo1, lvdInfo2, lvdInfo3;

    BasicInit();
    PR_DEBUG("\nLvd Demo Start\n");

    BasicConfigure(&lvdInfo1, &lvdInfo2, &lvdInfo3);

    LVD_Init(&lvdInfo1, &lvdInfo2, &lvdInfo3);

    LVD_EnableInterrupt(LVD_VOLDT_1);
    LVD_EnableInterrupt(LVD_VOLDT_2);
    LVD_EnableInterrupt(LVD_VOLDT_3);

    SysTick_DelayMS(1000);

    while (1)
    {
        //Check VD* status should be set
        if (LVD_GetVDXFlag(1))
        {
            PR_DEBUG("Current VDD < VD1LVL\n");
        }
        if (LVD_GetVDXFlag(2))
        {
            PR_DEBUG("Current VDD < VD2LVL\n");
        }
        if (LVD_GetVDXFlag(3))
        {
            PR_DEBUG("Current VDD < VD3LVL\n");
        }

        SysTick_DelayMS(500);

        PR_DEBUG(".");
    }
}



