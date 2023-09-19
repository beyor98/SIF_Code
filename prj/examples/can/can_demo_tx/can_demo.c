/*====================================================================
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file can_demo.c
 *
 * @version 1.0.0
 *
 * @date 2016/09/05
 *
 * @contents Verification and example code of WDOG Output
 *
 * @history
 *     Date                  By            Comments        Ver
 *  ===========         ===========       =========      =========
 *  2016/09/05          Mike Zheng         Create         1.0.0
 **********************************************************************/

#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/can.h>


/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif
#ifdef ERR_PRINTF
    #undef ERR_PRINTF
#endif
#ifdef WARNING_PRINTF
    #undef WARNING_PRINTF
#endif

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)         do{}while(0)
#else
    #define PR_ERR              printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)     do{}while(0)
#else
    #define PR_WARNINR          printf
#endif

#define TIMEOUT                 1000000 /* timeout = 1s */

/******************************************************************************
* Local types
******************************************************************************/
static CAN_Handle_T s_Handle;

/**
 *GM6611B support 16 filer channels, user can
 *set different mask and code to decide which
 *can frame want to receive.please refer spec
 *for detail
 */
#if defined(CHIP_GM6611B)
static uint32_t s_filterCode[MAX_FILTER_NUM] = {
    0x0fffffff, 0x17ffffff, 0x1bffffff, 0x1dffffff,
    0x1effffff, 0x1f7fffff, 0x1fbfffff, 0x1fefffff,
    0x1ff7ffff, 0x1ffbffff, 0x1ffdffff, 0x1ffeffff,
    0x1fff7fff, 0x1fffbfff, 0x1fffdfff, 0xabcddecb
};
static uint32_t s_filterMask[MAX_FILTER_NUM] = {
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
    0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
};
#endif


/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
/******************************************************************************
* Local functions
******************************************************************************/
int main (void);
/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/

static RET_CODE CAN_ConfigPinmux(void)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_14;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = GPIOC14_AF0_CANRX;
    ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        PR_ERR("Err:CAN_ConfigPinmux\n");
        ret = RET_ERROR;
        goto out;
    }

    pGPIO = GPIOC;
    GPIO_InitStruct.pin  = GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = GPIOC15_AF0_CANTX;
    ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        PR_ERR("Err:CAN_ConfigPinmux\n");
        ret = RET_ERROR;
        goto out;
    }

out:
    return ret;
}

static void CAN_FilterInit(CAN_FilterConfig_T *pFilter)
{
#if defined(CHIP_GM6611B)

    uint8_t i= 0 ;

    for (i = 0; i < MAX_FILTER_NUM; i++)
    {
        pFilter->filterCode[i] = s_filterCode[i];
        pFilter->filterMask[i] = s_filterMask[i];
    }
#elif defined(CHIP_GM6611)
    pFilter->filterCode0 = 0x1405b317;
    pFilter->filterMask0 = 0xffffffff;
    pFilter->filterCode1 = 0x0005bb57;
    pFilter->filterMask1 = 0xffffffff;
#endif
}



int main (void)
{
    RET_CODE ret = RET_OK;
    CAN_Config_T config;
    CAN_FrameInfor_T txFrameInfor;
    uint8_t i = 0;

    SysTick_Init();
    /* init for uart printf log */
    DebugConsoleInit(115200);

    ret = CAN_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err CAN_ConfigPinmux\n",__FUNCTION__);
        goto out;
    }

    PR_DEBUG("Can Demo Test Start\n");

    memset(&config, 0, sizeof(CAN_Config_T));
    config.errWarnThreshold   = 120;
    config.workMode           = CAN_NORMAL_MODE;

    CAN_FilterInit(&config.filter);

     /* config baudRateSetting */
    config.baudRateAttribute.prescaler    = SYS_CLOCK;
    config.baudRateAttribute.baudRate     = 20000;        /* 20kbps */
    config.baudRateAttribute.sample       = CAN_SAM_3;
    config.baudRateAttribute.sjw          = CAN_SJW_1TQ;
    config.baudRateAttribute.bs1          = CAN_TSEG_7TQ;
    config.baudRateAttribute.bs2          = CAN_TSEG_4TQ;

    config.callback = NULL;

    ret = CAN_Init(CAN_DEV, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:CAN Init\n",__FUNCTION__);
        goto out;
    }

    /* config Tx Message And Sent To CAN NetWork */
    memset(&txFrameInfor, 0, sizeof(CAN_FrameInfor_T));
    do
    {
        for (i=0; i<8; i++)
        {
            txFrameInfor.data[i] = i;
        }
        txFrameInfor.dataLength  = 8;
        txFrameInfor.frameFormat = CAN_EXTEND_FRAME;
        txFrameInfor.frameType   = CAN_DATA_FRAME;
        txFrameInfor.id          = 0x01;

        ret = CAN_TransferMessage(&s_Handle, &txFrameInfor, TIMEOUT);
        if (ret != RET_OK)
        {
            PR_ERR("Err:%s:CAN TransferMessage\n", __FUNCTION__);
            goto out;
        }

        SysTick_DelayMS(1000);
    }while(1);

out:
    return ret;
}

