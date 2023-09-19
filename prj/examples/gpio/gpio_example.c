#include <stdio.h>

#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/pes.h>
#include <drv/dev/adc.h>


int etm_timerinit(void);
void gpio_output_init(void);



#define PERIOD_5_2us   	120    //120*2/48000000 = 0.000005s = 0.005ms = 5us
#define CLK_8M      		 0
#define CLK_48M     		 1


ETM_Handle_T g_PWMHandle;
unsigned int  H_L_Level_time_cnt=0;
int start_H_L_Level_timming_flag=0; 
int i=0;

void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
//		if (start_H_L_Level_timming_flag==1)
//		{
//				H_L_Level_time_cnt++;     //高低电平维持时间计数变量
//				
//		}
	i++;
	if(i==100)
	{
		i=0;
		GPIO_PinToggle(GPIOA8);
	}
	
}

int etm_timerinit(void)
{
		RET_CODE ret = RET_OK;
		ETM_Config_T etmInfo;
		PeriphClk_Config_T config;
	
	
		/* Switch ETM IP Clock to 48MHZ */
    config.periphClkMask = CLOCK_PERIPH_ETM_MASK;
		config.etmClock = CLK_48M;

    ret = PeriphClk_SetConfig(&config);

		memset(&etmInfo, 0, sizeof(ETM_Config_T));
	
		etmInfo.extClock = FALSE;
    etmInfo.prescalerEn = TRUE;
    etmInfo.prescaler = ETM_PSC_DIV_2;		//分频 ETM_PSC_DIV_256
    etmInfo.countMode = ETM_COUNT_UP;
    etmInfo.deadTime = 0;
    etmInfo.period = PERIOD_5_2us;
		etmInfo.callback = ETM2Callback;
		
		ret = ETM_InitTimeBase(ETM2, &etmInfo, &g_PWMHandle);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }
		ETM_StartTimeBase(ETM2);
		ETM_EnableInterrupts(ETM2, ETM_INT_CNT_OVERFLOW);
		
		return ret;
}



void gpio_output_init(void)
{
		GPIO_PinConfig_T GPIO_InitStruct;
		memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

		GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}


int main(void)
{
		SysTick_Init();
		SysTick_DelayMS(2000);
	
		//DebugConsoleInit(115200L);

		gpio_output_init();
	
		etm_timerinit();
	
	
    while (1)
    {

    }
}



