#include <stdio.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/pes.h>
#include <drv/dev/adc.h>


int etm2_timerinit(void);
void sif_process(void);
void gpio_output_init(GPIO_Device_T *pGPIO, uint32_t pin);
void gpio_input_init(GPIO_Device_T *pGPIO, uint32_t pin);
int gpio_interrupt_falling_init(GPIO_Device_T *pGPIO, uint32_t pin);

int main(void)
{
		int ret;
		SysTick_Init();
		SysTick_DelayMS(2000);
	
		DebugConsoleInit(115200L);

		//ret = gpio_interrupt_falling_init(GPIOA, GPIO_PIN_8);
		gpio_input_init(GPIOC, GPIO_PIN_8);
	
		printf("enter main function!\n");
	
    while (1)
    {
			sif_process();
    }
}



