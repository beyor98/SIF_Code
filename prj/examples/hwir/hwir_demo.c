/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file hwir_demo.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/05
 *
 * @contents Verification and example code of Cyclic redundancy check (CRC).
 *
 * @history
 *	   Date			    By			     Comments			       Ver
 *	=========		=========		    =========			     =========
 *	2016/07/05		Mike Zheng		     Create			          1.0.0
 *  2016/11/22      Mike.Zheng           Update For Release       1.1.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/hwir.h>

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do {} while (0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do {} while (0)
#else
    #define PR_WARNINR         printf
#endif

/* For gree f0 protol feature key marco */
#define HWIR_F0_KEY_DATA0                       0x200900
#define HWIR_F0_KEY_DATA1                       0x2000


#define CHIP_ENTER_SLEEP_MODE()                 do {               \
                                                    __WFI();       \
                                                } while (0)

/******************************************************************************
* Local types
******************************************************************************/


/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
static HWIR_Handle_T s_Handle;
/******************************************************************************
* Local functions
******************************************************************************/
int main (void);
static RET_CODE HWIR_ConfigPinmux(void);
static void HWIR_Callback(HWIR_Handle_T *pHandle, HWIR_Event_T event, unsigned long param);

/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/

/******************************************************************************
* Marco Define
******************************************************************************/
#define SYSTICK_INTERRUPT_DISABLE()     do {SysTick->CTRL &=~ SysTick_CTRL_TICKINT_Msk;} while(0)


static void HWIR_Callback(HWIR_Handle_T *pHandle, HWIR_Event_T event, unsigned long param)
{
    if (event == HWIR_DECODE_SUCCESS)
    {
        PR_DEBUG("HWIR Decode Success\n");
    }
}

static RET_CODE HWIR_ConfigPinmux(void)
{
	RET_CODE ret = RET_OK;
	GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_Device_T *pGPIO = NULL;

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B))
    pGPIO                     = GPIOA;
	GPIO_InitStruct.pin       = GPIO_PIN_8;
	GPIO_InitStruct.mode      = GPIO_MODE_AF;
	GPIO_InitStruct.pull      = GPIO_NOPULL;
    GPIO_InitStruct.alternate = GPIOA8_AF0_IR;
#elif defined(CHIP_GM6611)
    pGPIO                     = GPIOB;
	GPIO_InitStruct.pin       = GPIO_PIN_0;
	GPIO_InitStruct.mode      = GPIO_MODE_AF;
	GPIO_InitStruct.pull      = GPIO_NOPULL;
    GPIO_InitStruct.alternate = GPIOB0_AF0_IR;
#endif
	ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
	if (ret != RET_OK)
	{
		PR_ERR("Err:HWIR_ConfigPinmux\n");
	}
    return ret;
}

int main (void)
{
    RET_CODE ret = RET_OK;
    HWIR_Config_T config;

    SysTick_Init();

    /* init for uart printf log */
    DebugConsoleInit(115200);

    HWIR_ConfigPinmux();

    config.callback                              = HWIR_Callback;
    config.keyDesc.protocol                      = PROTOCOL_F0;
    config.keyDesc.keyValue.keyValueFN.dataCode0 = HWIR_F0_KEY_DATA0;
    config.keyDesc.keyValue.keyValueFN.dataCode1 = HWIR_F0_KEY_DATA1;

    ret = HWIR_Init(HWIR_DEV, &config ,&s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("ERR:%d %s:%d\n",ret,__FUNCTION__,__LINE__);
        goto out;
    }

    /* enable ir to decode ir keycode when chip enter sleep mode */
    HWIR_Enable(&s_Handle);
    /* Disable SysTick To Stop From Systick interrupt wakeup chip*/
    SYSTICK_INTERRUPT_DISABLE();
    /* chip enter sleep until ir decode success and wakeup chip */
    CHIP_ENTER_SLEEP_MODE();

    PR_DEBUG("\rChip WakeUp From Sleep Mode By HWIR Interrupt\n");

out:
    return ret;
}











