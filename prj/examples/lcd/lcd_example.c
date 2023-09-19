/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          lcd_example.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/03/23 
 *    
 * @brief        Sample code for lcd operation.
 * 
 * @note
 *    2017/03/23, mark.lee, V1.0.0
 *        Initial version. 
 */
#include <stdio.h>
#include <drv/dev/lcd.h>
#include <drv/dev/gpio.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
/*LCD charge pump Test*/	
#define TEST_CHARGE_PUMP

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
    LCD_Config_T pConfig;
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;
	
    SysTick_Init();

    memset(&GPIO_InitStruct,0,sizeof(GPIO_PinConfig_T));

#if (defined(CHIP_GM6621))
	  GPIO_InitStruct.pin = GPIO_PIN_15;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOA, &GPIO_InitStruct);
    if(ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_PinWrite(GPIOA15, GPIO_PIN_SET);

#if (defined(TEST_CHARGE_PUMP))
    GPIO_InitStruct.pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if(ret != RET_OK)
    {
        return RET_ERROR;
    }
#endif

#else
    GPIO_InitStruct.pin = GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOD, &GPIO_InitStruct);
    if(ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_PinWrite(GPIOD9, GPIO_PIN_SET);
#endif
		
    memset(&pConfig, 0, sizeof(LCD_Config_T));
    pConfig.bias = LCD_Bias_1_3;
    pConfig.contrast = LCD_Contrast_Level_2;
    pConfig.duty = LCD_Duty_1_4;
    pConfig.prescaler = LCD_Prescaler_4;
    pConfig.displayMode = LCD_Display_Line;
    pConfig.voltageSource = LCD_VoltageSource_Internal;

    LCD_Init(&pConfig);

#if (defined(CHIP_GM6621))
#if (defined(TEST_CHARGE_PUMP))
    ret = LCD_ChargePumpConfig(1, 0, 0);
    if (ret != RET_OK)
    {
        RET_ERROR;  
    }
#endif

    GPIO_InitStruct.pin = GPIO_PIN_All;
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOB, &GPIO_InitStruct);
    if(ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = GPIO_PIN_All & (~(GPIO_PIN_3|GPIO_PIN_6));
    GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.pull = GPIO_NOPULL;;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(GPIOC, &GPIO_InitStruct);
    if(ret != RET_OK)
    {
        return RET_ERROR;
    }	
    
    LCD_EnablePin(0, 0xffb7ffff);

#if defined(TEST_CHARGE_PUMP)
		LCD_EnablePin(1, 0xffffff00);
#else
		LCD_EnablePin(1, 0xffffffff);
#endif
		
	LCD_SelectComPin(LCD_COM_0, 0);
    LCD_SelectComPin(LCD_COM_1, 1);
    LCD_SelectComPin(LCD_COM_2, 2);
    LCD_SelectComPin(LCD_COM_3, 3);
#else
		LCD_EnablePin(0, 0xffffffff);
    LCD_EnablePin(1, 0xffffffff);
		
		LCD_SelectComPin(LCD_COM_0, 0);
    LCD_SelectComPin(LCD_COM_1, 1);
    LCD_SelectComPin(LCD_COM_2, 2);
    LCD_SelectComPin(LCD_COM_3, 3);
#endif
		
    LCD_ClearDisplay();

    LCD_Write(LCD_RAMRegister_1, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_2, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_3, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_4, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_5, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_6, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_7, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_8, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_9, 0xffffffff, 0xffffffff);

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
    LCD_Write(LCD_RAMRegister_10, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_11, 0xffffffff, 0xffffffff);
    LCD_Write(LCD_RAMRegister_12, 0xffffffff, 0xffffffff);
#endif

    LCD_StartDisplay();
    
    while (1)
    {
        LCD_StopDisplay();
        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_1, 0xffffffff, 0xffffffff);
        LCD_StartDisplay();
		
        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_2, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_3, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_4, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_5, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_6, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_7, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_8, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

         LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_9, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);
				
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_10, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_11, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);

        LCD_ClearDisplay();
        LCD_Write(LCD_RAMRegister_12, 0xffffffff, 0xffffffff);

        SysTick_DelayMS(1000);
#endif
    }
}



