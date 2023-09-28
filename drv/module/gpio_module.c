#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/gpio.h>


#define LOW                 0
#define HIGH                1

STATIC_ISR(GPIO0_IRQn, GPIOA_IRQHandler);


void gpio_output_init(GPIO_Device_T *pGPIO, uint32_t Pin)
{
		GPIO_PinConfig_T GPIO_InitStruct;
		memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

		GPIO_InitStruct.pin = Pin;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(pGPIO, &GPIO_InitStruct);
}

void gpio_input_init(GPIO_Device_T *pGPIO, uint32_t Pin)
{
		GPIO_PinConfig_T GPIO_InitStruct;
		memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

		GPIO_InitStruct.pin = Pin;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(pGPIO, &GPIO_InitStruct);
}

void gpio_aip16400_init(void)
{
		GPIO_PinConfig_T GPIO_InitStruct;
		memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

		GPIO_InitStruct.pin = GPIO_PIN_7 | GPIO_PIN_9;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
	
		GPIO_PinWrite(GPIOA7, GPIO_PIN_SET);
		GPIO_PinWrite(GPIOA9, GPIO_PIN_SET);
}


RET_CODE gpio_interrupt_falling_init(GPIO_Device_T *pGPIO, uint32_t Pin)
{
		GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));

    /* Configure GPIOA8 as external interrupt lines pin */
    GPIO_InitStruct.pin = Pin;
    GPIO_InitStruct.mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.pull = GPIO_PULLUP;
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

    printf("enter GPIOA_IRQHandler function!\n");
}

