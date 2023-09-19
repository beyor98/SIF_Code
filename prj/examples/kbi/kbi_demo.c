/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          kbi_test.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/03/23 
 *    
 * @brief        Sample code for kbi test case.
 * 
 * @note
 *    2017/03/23, mark.lee, V1.0.0
 *        Initial version. 
 */
#include <stdio.h>
#include <drv/dev/kbi.h>
#include <drv/dev/gpio.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
/*Basic  Test*/
//#define TEST_PIN KBI_PIN_0
#define TEST_PIN KBI_PIN_1
//#define TEST_PIN KBI_PIN_2
//#define TEST_PIN KBI_PIN_3
//#define TEST_PIN KBI_PIN_4
//#define TEST_PIN KBI_PIN_5
//#define TEST_PIN KBI_PIN_6
//#define TEST_PIN KBI_PIN_7

/* Interrup edge */
//#define KBI_IT_MODE KBI_MODE_IT_FALLING
#define KBI_IT_MODE KBI_MODE_IT_RISING


#define __CPU_HALT_DEBUG__    __asm ("bkpt 0")

STATIC_ISR(KBI_IRQn, KBI_IRQHandler);

#define PMU_EN()          ((*((volatile uint32_t *)(0x40039018))) |= (0x1<<0))
#define PMU_STOP()        ((*((volatile uint32_t *)(0x40039018))) |= (0x1<<0))
#define PMU_STANDBY()     ((*((volatile uint32_t *)(0x40039018))) |= (0x1<<0))
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
static void KBI_IRQHandler(void)
{
    if (KBI_GET_IT(KBI, TEST_PIN))
    {
        KBI_CLEAR_IT(KBI, TEST_PIN);

        if (KBI->SR & TEST_PIN)
        {
             __CPU_HALT_DEBUG__;       
        }
    }
}

//static void SleepMode(void)
//{
//    printf("Enter sleep mode \n");  
//    /* disable systick interrupt*/
//    (*((volatile uint32_t *)(0xE000E010U))) &= ~(1<<1);
//    SCB->SCR &= ~(1UL << 2);
//    PMU_EN();
//    SCB->SCR |= (1UL << 2); 
//    //SysTick_DelayMS(2);    
//    __WFE();// Enter sleep mode 
//    __WFE();// Enter sleep mode
// 
//    printf("wakeup cpu 1 \n");
//}

static void DeepSleepMode(void)
{
    printf("Enter deepsleep mode \n");    
    /* disable systick interrupt*/
    (*((volatile uint32_t *)(0xE000E010U))) &= ~(1<<1);
    SCB->SCR &= ~(1UL << 2);
    PMU_EN();
    SCB->SCR |= (1UL << 2); 
    //SysTick_DelayMS(2);
    __WFI();// Enter Deep sleep mode   
	
    printf("wakeup cpu 1 \n");
}

int main(void)
{
    RET_CODE ret = RET_OK;
    KBI_PinConfig_T KBI_InitStruct;
    GPIO_PinConfig_T GPIO_InitStruct;
	
    SysTick_Init();

    DebugConsoleInit(19200);
	
    KBI_InitStruct.pin = TEST_PIN;
    KBI_InitStruct.mode = KBI_IT_MODE;
    ret = KBI_Init(KBI, &KBI_InitStruct);

    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    KBI_SELECT_PIN(KBI, KBI_PIN_11, TRUE);
    
    /* Configure GPIOD5 pullup or pulldown function */
    memset(&GPIO_InitStruct,0,sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin = GPIO_PIN_5;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_PULLDOWN;//GPIO_PULLUP;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    memset(&GPIO_InitStruct,0,sizeof(GPIO_PinConfig_T));
    /* Configure GPIOD10 as output pin */
    GPIO_InitStruct.pin = GPIO_PIN_10;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
    
    /* Set GPIOD10 output high level */
    GPIO_PinWrite(GPIOD10, GPIO_PIN_SET);
    
    /* Set GPIOD10 output low level */
    //GPIO_PinWrite(GPIOD10, GPIO_PIN_RESET);
    
    KBI_ENABLE_IT(KBI);
	
    //SleepMode();
    DeepSleepMode();
		
    while (1)
    {
        printf("wakeup cpu 2 \n");
    }
}

