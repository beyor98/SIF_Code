/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          led_demo.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/11/11 
 *    
 * @brief        light led demo.
 * 
 * @note
 *    2016/11/11, mark.lee, V1.0.0
 *        Initial version. 
 */
#include <stdio.h>
#include <drv/dev/gpio.h>


/*******************************************************************************
* Constants and macros
*******************************************************************************/

/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Static variables
*******************************************************************************/

/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/
int main(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    SysTick_Init();

    memset(&GPIO_InitStruct,0,sizeof(GPIO_PinConfig_T));

    /* Config GPIOD8 as output pin */
    GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if(ret != RET_OK)
    {
        return RET_ERROR;
    }

    /* Config GPIOD9 as output pin */
    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if(ret != RET_OK)
    {
        return RET_ERROR;
    }
		
    while(1)
    {
        SysTick_DelayMS(500);

        /* Sets GPIOD8/GPIOD9 output low level */
        GPIO_PinWrite(GPIOD8, GPIO_PIN_RESET);
        GPIO_PinWrite(GPIOD9, GPIO_PIN_RESET);
        SysTick_DelayMS(500);
        /* Sets GPIOD8/GPIOD9 output high level */
        GPIO_PinWrite(GPIOD8, GPIO_PIN_SET);
        GPIO_PinWrite(GPIOD9, GPIO_PIN_SET);
    }
}



