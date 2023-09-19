#include <stdio.h>
#include <drv/dev/gpio.h>

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printf
#endif

/*******************************************************************************
* Constants and macros
*******************************************************************************/
STATIC_ISR(GPIO0_IRQn, GPIOA_IRQHandler);
STATIC_ISR(GPIO1_IRQn, GPIOB_IRQHandler);
STATIC_ISR(GPIO2_IRQn, GPIOC_IRQHandler);
STATIC_ISR(GPIO3_IRQn, GPIOD_IRQHandler);

/*******************************************************************************
* Global variables
*******************************************************************************/

/*******************************************************************************
* Static variables
*******************************************************************************/
#define COUNT  10
/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/
static void GPIOA_IRQHandler(void)
{
    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);
    if (GPIO_EXTI_GET_IT(GPIOA, GPIO_PIN_3))
    {
        GPIO_EXTI_CLEAR_IT(GPIOA, GPIO_PIN_3);
    }
}

static void GPIOB_IRQHandler(void)
{
    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);
    if (GPIO_EXTI_GET_IT(GPIOB, GPIO_PIN_3))
    {
        GPIO_EXTI_CLEAR_IT(GPIOB, GPIO_PIN_3);
    }
}

static void GPIOC_IRQHandler(void)
{
    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);
    if (GPIO_EXTI_GET_IT(GPIOC, GPIO_PIN_3))
    {
        GPIO_EXTI_CLEAR_IT(GPIOC, GPIO_PIN_3);
    }
}

static void GPIOD_IRQHandler(void)
{
    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);
    if (GPIO_EXTI_GET_IT(GPIOD, GPIO_PIN_3))
    {
        GPIO_EXTI_CLEAR_IT(GPIOD, GPIO_PIN_3);
    }
}

int gpio_output_demo(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    int err_flag = 0;

    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;

    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_PinWrite(GPIOC2, GPIO_PIN_SET);
    if (GPIO_PinRead(GPIOC2) != GPIO_PIN_SET)
    {
        PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);

        err_flag = 1;
    }

    GPIO_PinWrite(GPIOC2, GPIO_PIN_RESET);
    if (GPIO_PinRead(GPIOC2) != GPIO_PIN_RESET)
    {
        PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
        err_flag = 1;
    }

    return err_flag;
}

int gpio_interrput_demo(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    int err_flag = 0;
    int i = 0;

    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;

    GPIO_Init(GPIOC, &GPIO_InitStruct);


    GPIO_InitStruct.pin =  GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;

    GPIO_Init(GPIOC, &GPIO_InitStruct);

    while (i++ < COUNT)
    {
        GPIO_PinWrite(GPIOC2, GPIO_PIN_RESET);
        SysTick_DelayMS(2000);
        GPIO_PinWrite(GPIOC2, GPIO_PIN_SET);
        SysTick_DelayMS(2000);
    }

    PR_DEBUG("exit Func:%s,  line=%d \n", __FUNCTION__, __LINE__);
    return err_flag;
}

int gpio_output_input_demo(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    int err_flag = 0;

    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);
    PR_DEBUG("GPIOC2 need connect GPIOC3 by wire \n");

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_2;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.pin =  GPIO_PIN_3;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_PinWrite(GPIOC2, GPIO_PIN_SET);
    if (GPIO_PinRead(GPIOC3) != GPIO_PIN_SET)
    {
        PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
        PR_DEBUG("GPIOC2 maybe not connect GPIOC3 by wire \n");

        err_flag = 1;
    }

    GPIO_PinWrite(GPIOC2, GPIO_PIN_RESET);
    if (GPIO_PinRead(GPIOC3) != GPIO_PIN_RESET)
    {
        PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
        PR_DEBUG("GPIOC2 maybe not connect GPIOC3 by wire \n");
        err_flag = 1;
    }

    return err_flag;

}

int gpio_filter_demo(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InputGlitchFilter_T  GPIO_InputGlitchFilter;
    int err_flag = RET_OK;

    PR_DEBUG("enter Func:%s,  line=%d \n", __FUNCTION__, __LINE__);

    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    GPIO_InitStruct.pin =  GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    memset(&GPIO_InputGlitchFilter, 0, sizeof(GPIO_InputGlitchFilter_T));

    GPIO_InitStruct.pin =  GPIO_PIN_1;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /**** Set glitch filter parameter,
    * filter time = (1/(SYS_CLOCK / FLT_CLK1_PSC_DIV_4K))*2
    * filter time = 1000us =  [1/(8000000 / 4000)]*2
    ****/
    GPIO_InputGlitchFilter.bypass = FALSE;
    /* selects glitch filter clock */
    GPIO_InputGlitchFilter.clock = PAD_FLT_CLK1;
    GPIO_InputGlitchFilter.prescaler = FLT_CLK1_PSC_DIV_4K;  //8MHz,1000us;
    GPIO_SetPinFilter(GPIOC1, &GPIO_InputGlitchFilter);

    PR_DEBUG("GPIOC0 must connect to GPIOC1 by wire.\n");

    GPIO_PinWrite(GPIOC0, GPIO_PIN_SET);

    SysTick_DelayUS(500);

    if (GPIO_PinRead(GPIOC1) != GPIO_PIN_SET)
    {
        PR_DEBUG("glitch filter 500us test successful\n");
    }
    else
    {
        PR_DEBUG("glitch filter test failed\n");
        PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
        err_flag = RET_ERROR;
    }

    GPIO_PinWrite(GPIOC0, GPIO_PIN_SET);

    SysTick_DelayUS(3000);

    if (GPIO_PinRead(GPIOC1) != GPIO_PIN_RESET)
    {
        PR_DEBUG("glitch filter 3000us test successful\n");
    }
    else
    {
        PR_DEBUG("glitch filter test failed\n");
        PR_ERR("Func:%s Failed, line=%d\n", __FUNCTION__, __LINE__);
        err_flag = RET_ERROR;
    }

    return err_flag;
}


int main(void)
{
    int select_demo = 3;

    /* init printf */
    SysTick_Init();
    DebugConsoleInit(19200L);

    SysTick_DelayMS(1000);
    printf("Gpio demo begin to test\n");

    if (select_demo == 0)
    {
        gpio_output_demo();
    }
    else if (select_demo == 1)
    {
        gpio_output_input_demo();
    }
    else if (select_demo == 2)
    {
        gpio_interrput_demo();
    }
    else if (select_demo == 3)
    {
        gpio_filter_demo();
    }
    else
    {
        /* you can add your other case */
    }

    printf("Gpio demo test finished\n");

    return 0;

    while (1);
}
