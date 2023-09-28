#include <stdio.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/pes.h>
#include <drv/dev/adc.h>


int etm1_timerinit(void);
int etm2_timerinit(void);
void sif_process(void);
void gpio_output_init(GPIO_Device_T *pGPIO, uint32_t Pin);
void gpio_input_init(GPIO_Device_T *pGPIO, uint32_t Pin);
int gpio_interrupt_falling_init(GPIO_Device_T *pGPIO, uint32_t Pin);


int main(void)
{
		SysTick_Init();
		SysTick_DelayMS(200);
	
		DebugConsoleInit(115200L);

		gpio_output_init(GPIOA, GPIO_PIN_8);
		gpio_input_init(GPIOC, GPIO_PIN_8);
	
		etm1_timerinit();
		etm2_timerinit();
		//gpio_interrupt_falling_init(GPIOA, GPIO_PIN_8);
		printf("enter main function!\n");
	
    while (1)
    {
			sif_process();
    }
}
