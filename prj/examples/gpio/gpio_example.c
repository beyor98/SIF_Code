#include <stdio.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/pes.h>
#include <drv/dev/adc.h>


int etm_timerinit(void);
void gpio_output_init(void);
void gpio_input_init(void);


#define CLK_8M      		 0
#define CLK_48M     		 1



int main(void)
{
		SysTick_Init();
		SysTick_DelayMS(2000);
	
		DebugConsoleInit(115200L);

		gpio_output_init();
	
		etm_timerinit();
	
    while (1)
    {

    }
}



