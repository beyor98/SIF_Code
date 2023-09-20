#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/etm.h>
#include <drv/dev/gpio.h>


#define PERIOD_500us   		12000	//12000*2/48000000 = 0.0005s = 0.5ms
#define CLK_8M      		 0
#define CLK_48M     		 1


ETM_Handle_T g_PWMHandle;

void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param);


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
    etmInfo.prescaler = ETM_PSC_DIV_2;		//·ÖÆµ ETM_PSC_DIV_256
    etmInfo.countMode = ETM_COUNT_UP;
    etmInfo.deadTime = 0;
    etmInfo.period = PERIOD_500us;
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
