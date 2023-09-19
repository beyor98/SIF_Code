#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/gpio.h>


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

void gpio_input_init(void)
{
		GPIO_PinConfig_T GPIO_InitStruct;
		memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

		GPIO_InitStruct.pin = GPIO_PIN_8;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}
