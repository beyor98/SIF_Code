#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/gpio.h>

STATIC_ISR(GPIO0_IRQn, GPIOA_IRQHandler);


void gpio_output_init(GPIO_Device_T *pGPIO, uint32_t pin)
{
		GPIO_PinConfig_T GPIO_InitStruct;
		memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

		GPIO_InitStruct.pin = pin;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(pGPIO, &GPIO_InitStruct);
}

void gpio_input_init(GPIO_Device_T *pGPIO, uint32_t pin)
{
		GPIO_PinConfig_T GPIO_InitStruct;
		memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

		GPIO_InitStruct.pin = pin;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(pGPIO, &GPIO_InitStruct);
}

RET_CODE gpio_interrupt_falling_init(GPIO_Device_T *pGPIO, uint32_t pin)
{
		GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /* Configure GPIOA2 as external interrupt lines pin */
    GPIO_InitStruct.pin = pin;
    GPIO_InitStruct.mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    ret = GPIO_Init(pGPIO, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;;
}



static void GPIOA_IRQHandler(void)
{
    if (GPIO_EXTI_GET_IT(GPIOA, GPIO_PIN_8))
    {
        GPIO_EXTI_CLEAR_IT(GPIOA, GPIO_PIN_8);
    }

    /* Toggle GPIOD9 output */
    printf("enter GPIOA_IRQHandler function!\n");
}


