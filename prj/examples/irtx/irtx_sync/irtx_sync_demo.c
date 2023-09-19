/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file irtx_sync_demo.c
 *
 * @version 1.0.0
 *
 * @date 2017/04/07
 *
 * @contents Verification and example code of irtx sycn mode
 *
 * @history
 *     Date             By            Comments              Ver
 *  =========       =========        =========           =========
 *   2017/04/07     Mike Zheng        Create                1.0.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/irtx.h>

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

//#define DEBUG_PRINTF
//#define WARNING_PRINTF
//#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printf
#endif


#define IRTX_SEND_TIMEOUT     5000000    /* IRTX Timeout 5 s */

#define TEST_KEY_DATA0        0xaaaaaaaa
#define TEST_KEY_DATA1        0x55555555
/******************************************************************************
* Local types
******************************************************************************/
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device */
    uint32_t mode;                 /**< Specifies the operating mode for the selected pins */
    uint32_t pull;                 /**< Specifies the Pull-up or Pull-up X2 activation for the selected pins */
} Device_Pinmux_T;

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
static IRTX_Handle_T s_Handle;

/******************************************************************************
* Local functions
******************************************************************************/
int main(void);


/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/
static RET_CODE IRTX_ConfigPinmux(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;

    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /** IRTX output pin configuratiuon
      * XPA8  ------> IR_OUT
      * XPA7  ------> IR_OUT
      * XPA9  ------> IR_OUT
      * XPA10 ------> IR_OUT
      */

    GPIO_InitStruct.pin =  GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_PULLUP;
    GPIO_InitStruct.alternate = GPIOA8_AF0_IR;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

int main(void)
{
    RET_CODE ret = RET_OK;
    IRTX_Config_T config;
    IRTX_KeyDesc_T keyDesc;

    SysTick_Init();
    /* init for uart printf log */
    /* DebugConsoleInit(115200); */

    IRTX_ConfigPinmux();

    config.triggerMode = IRTX_SW_TRIGGER;
    config.callback   = NULL;
    config.workMode   = IRTX_IR;

    ret = IRTX_Init(IRTX_DEV, &config, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:IRTX_Init\n", __FUNCTION__);
        goto out;
    }

    /* config keydes to send */
    keyDesc.protocol = IRTX_PROTOCOL_F0;
    keyDesc.KeyCode.F0Code.data0 = TEST_KEY_DATA0;
    keyDesc.KeyCode.F0Code.data1 = TEST_KEY_DATA1;

    /* send key every 2 s*/
    ret = IRTX_SendKey(&s_Handle, &keyDesc, IRTX_SEND_TIMEOUT);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err:%d IRTX_SendKey\n", __FUNCTION__, ret);
        goto out;
    }

out:
    return ret;
}




