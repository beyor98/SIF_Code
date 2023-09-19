#include <stdio.h>
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/gpio.h>

#define UART_USERBUFFER_SIZE 128

#define OPEN_UART_RECEIVE     0
#define CLOSE_UART_RECEIVE    1
#define OPEN_UART_TRANSMIT    2
#define CLOSE_UART_TRANSMIT   3

/******************************************************************************
* Local types
******************************************************************************/
typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

uint8_t g_rxData[UART_USERBUFFER_SIZE];
uint8_t g_txData[UART_USERBUFFER_SIZE];

volatile uint32_t g_rxSize = 0;
volatile uint32_t g_txSize = 0;

#define BAUD_RATE 115200

/* UART_Handle_T  variable must be a global variable, if you use interrupt */
UART_Handle_T uartHandle;
UART_Handle_T uart1Handle;
UART_Handle_T uart2Handle;



/***** UART TEST CASE *****/
//#define Default_port_printf  //default port uart is UART0
//#define optional_uart_port_tx
//#define uart_interrupt_receive_from_PC
//#define uart_interrupt_transfer
//#define Uart_Interrupt_RX_Poll_tx
//#define Uart_Rx_Irq_Tx_Irq
//#define Uart_tx_and_rx_block
//#define Uart_set_mode_change_to_PC
#define Uart_HalfDuplex

RET_CODE UART_ConfigPinmux(UART_Device_T *pUART)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    Device_Pinmux_T s_uartPinMux[2];

    if (pUART == UART0)
    {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
        s_uartPinMux[0].pin = GPIO_PIN_6;
        s_uartPinMux[0].device = GPIOD;
        s_uartPinMux[0].alternate = GPIOD6_AF0_UART0TX;
        s_uartPinMux[1].pin = GPIO_PIN_7;
        s_uartPinMux[1].device = GPIOD;
        s_uartPinMux[1].alternate = GPIOD7_AF0_UART0RX;
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        s_uartPinMux[0].pin = GPIO_PIN_13;
        s_uartPinMux[0].device = GPIOC;
        s_uartPinMux[0].alternate = GPIOC13_AF0_UART0TX;
        s_uartPinMux[1].pin = GPIO_PIN_12;
        s_uartPinMux[1].device = GPIOC;
        s_uartPinMux[1].alternate = GPIOC12_AF0_UART0RX;
#endif
#if defined(CHIP_GM6621)
        s_uartPinMux[0].pin = GPIO_PIN_6;
        s_uartPinMux[0].device = GPIOA;
        s_uartPinMux[0].alternate = GPIOA6_AF0_UART0TX;
        s_uartPinMux[1].pin = GPIO_PIN_5;
        s_uartPinMux[1].device = GPIOA;
        s_uartPinMux[1].alternate = GPIOA5_AF0_UART0RX;
#endif
    }
    else if (pUART == UART1)
    {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
        s_uartPinMux[0].pin = GPIO_PIN_4;
        s_uartPinMux[0].device = GPIOD;
        s_uartPinMux[0].alternate = GPIOD4_AF0_UART1TX;
        s_uartPinMux[1].pin = GPIO_PIN_5;
        s_uartPinMux[1].device = GPIOD;
        s_uartPinMux[1].alternate = GPIOD5_AF0_UART1RX;
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        s_uartPinMux[0].pin = GPIO_PIN_14;
        s_uartPinMux[0].device = GPIOD;
        s_uartPinMux[0].alternate = GPIOD14_AF0_UART1TX;
        s_uartPinMux[1].pin = GPIO_PIN_13;
        s_uartPinMux[1].device = GPIOD;
        s_uartPinMux[1].alternate = GPIOD13_AF0_UART1RX;
#endif
#if defined(CHIP_GM6621)
        s_uartPinMux[0].pin = GPIO_PIN_15;
        s_uartPinMux[0].device = GPIOB;
        s_uartPinMux[0].alternate = GPIOB15_AF1_UART1TX;
        s_uartPinMux[1].pin = GPIO_PIN_0;
        s_uartPinMux[1].device = GPIOC;
        s_uartPinMux[1].alternate = GPIOC0_AF1_UART1RX;
#endif
    }
    else if (pUART == UART2)
    {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B)
        s_uartPinMux[0].pin = GPIO_PIN_10;
        s_uartPinMux[0].device = GPIOA;
        s_uartPinMux[0].alternate = GPIOA10_AF0_UART2TX;
        s_uartPinMux[1].pin = GPIO_PIN_11;
        s_uartPinMux[1].device = GPIOA;
        s_uartPinMux[1].alternate = GPIOA11_AF1_UART2RX;
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        s_uartPinMux[0].pin = GPIO_PIN_3;
        s_uartPinMux[0].device = GPIOA;
        s_uartPinMux[0].alternate = GPIOA3_AF1_UART2TX;
        s_uartPinMux[1].pin = GPIO_PIN_2;
        s_uartPinMux[1].device = GPIOA;
        s_uartPinMux[1].alternate = GPIOA2_AF1_UART2RX;
#endif
#if defined(CHIP_GM6621)
        //here can not use PC6&PC7, it will disable SWD
        s_uartPinMux[0].pin = GPIO_PIN_6;
        s_uartPinMux[0].device = GPIOA;
        s_uartPinMux[0].alternate = GPIOA6_AF2_UART2TX;
        s_uartPinMux[1].pin = GPIO_PIN_7;
        s_uartPinMux[1].device = GPIOA;
        s_uartPinMux[1].alternate = GPIOA7_AF2_UART2RX;
#endif
    }

    GPIO_InitStruct.pin = s_uartPinMux[0].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_uartPinMux[0].alternate;
    ret = GPIO_Init(s_uartPinMux[0].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    GPIO_InitStruct.pin = s_uartPinMux[1].pin;
    GPIO_InitStruct.mode = GPIO_MODE_AF;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = s_uartPinMux[1].alternate;
    ret = GPIO_Init(s_uartPinMux[1].device, &GPIO_InitStruct);
    if (ret != RET_OK)
    {
        return RET_ERROR;
    }

    return ret;
}

#ifdef Default_port_printf
static void testcase_uart_printf(void)
{
    DebugConsoleInit(BAUD_RATE);

    printf("printf string with uart tx.\n");


}
#endif

#ifdef optional_uart_port_tx
static int testcase_uart_tx(UART_Device_T *pUART)
{
    uint8_t ch;
    UART_Config_T uartConfig;

    uartConfig.baudRate = BAUD_RATE;
    uartConfig.parity = UART_PARITY_EVEN;
    uartConfig.wordLength = UART_DATABITS_8;
    uartConfig.stopBits = UART_STOPBITS_1;
    uartConfig.callback = NULL;
    UART_Init(pUART, &uartConfig, &uartHandle);

    UART_PutChar(uartHandle.device, 'U');
    UART_PutChar(uartHandle.device, 'A');
    UART_PutChar(uartHandle.device, 'R');
    UART_PutChar(uartHandle.device, 'T');
    if (pUART == UART0)
    {
        ch = '0';
    }
    else if (pUART == UART1)
    {
        ch = '1';
    }
    else if (pUART == UART2)
    {
        ch = '2';
    }
    UART_PutChar(uartHandle.device, ch);
    UART_PutChar(uartHandle.device, ' ');
    UART_PutChar(uartHandle.device, 'T');
    UART_PutChar(uartHandle.device, 'X');
    UART_PutChar(uartHandle.device, ' ');
    UART_PutChar(uartHandle.device, 'T');
    UART_PutChar(uartHandle.device, 'E');
    UART_PutChar(uartHandle.device, 'S');
    UART_PutChar(uartHandle.device, 'T');
    UART_PutChar(uartHandle.device, ' ');
    UART_PutChar(uartHandle.device, 'O');
    UART_PutChar(uartHandle.device, 'K');
    UART_PutChar(uartHandle.device, '\n');

    return RET_OK;
}
#endif

static void SetUserBuffer(void)
{
    uint32_t i;
    g_rxSize = 0;
    g_txSize = 0;

    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        g_rxData[i] = 0xFF;
        g_txData[i] = (i & 0xFF);
    }
}

void uart_receive_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    if (event == UART_EVENT_RX_CHAR)
    {
        g_rxData[g_rxSize++] = param;

        if (g_rxSize == UART_USERBUFFER_SIZE)
        {
            g_rxSize = 0;
        }

        //UART_PutChar(hUART->device, param);
    }
    else if (event == UART_EVENT_RX_ERROR)
    {
        printf("uart receive error.\n");
    }
}

#ifdef uart_interrupt_receive_from_PC
static int testcase_uart_rx_irq(UART_Device_T *pUART)
{
    UART_Config_T uartConfig;

    uartConfig.baudRate    = BAUD_RATE;
    uartConfig.parity      = UART_PARITY_EVEN;
    uartConfig.wordLength  = UART_DATABITS_8;
    uartConfig.stopBits    = UART_STOPBITS_1;
    uartConfig.callback    = uart_receive_callback;
    UART_Init(pUART, &uartConfig, &uartHandle);

    /* enable rx irq */
    UART_EnableInterrupts(&uartHandle, UART_RX_INT);

    return RET_OK;
}
#endif

void uart_transmit_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    if (event == UART_EVENT_TX_EMPTY)
    {
        if (g_txSize >= 1 && g_txSize < UART_USERBUFFER_SIZE)
        {
            UART_PutChar(hUART->device, g_txData[g_txSize++]);
        }
    }
    else if (event == UART_EVENT_TX_IDLE)
    {
        g_txSize = UART_USERBUFFER_SIZE;
    }
}

#ifdef uart_interrupt_transfer
static int testcase_uart_tx_irq(UART_Device_T *pUART)
{
    UART_Config_T uartConfig;
    UART_TransferStatus_T uartStatus;

    uartConfig.baudRate    = BAUD_RATE;
    uartConfig.parity      = UART_PARITY_EVEN;
    uartConfig.wordLength  = UART_DATABITS_8;
    uartConfig.stopBits    = UART_STOPBITS_1;
    uartConfig.callback    = uart_transmit_callback;
    UART_Init(pUART, &uartConfig, &uartHandle);

    /* Enable Tx irq, and then transmit first byte */
    UART_EnableInterrupts(&uartHandle, UART_TX_INT);

    g_txSize = 1;
    UART_PutChar(pUART, g_txData[0]);
    /* Wait for all data put into transmitter */
    while (g_txSize < UART_USERBUFFER_SIZE);
    /* Wait for transmitter idle */
    while (1)
    {
        UART_GetTransferStatus(&uartHandle, &uartStatus);
        if (uartStatus.lineStatus & UART_TRANSMITTER_IDLE)
        {
            UART_DisableInterrupts(&uartHandle, UART_TX_INT);
            break;
        }
    }
    return RET_OK;
}
#endif

#ifdef Uart_Interrupt_RX_Poll_tx
static int testcase_uart_rxtx_irq_poll(UART_Device_T *RX_UART, UART_Device_T *TX_UART)
{
    uint16_t i;
    UART_Config_T uart1Config;
    UART_Config_T uart2Config;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = uart_receive_callback;
    UART_Init(RX_UART, &uart1Config, &uart1Handle);

    /* 1, enable rx irq */
    UART_EnableInterrupts(&uart1Handle, UART_RX_INT);

    uart2Config.baudRate = BAUD_RATE;
    uart2Config.parity = UART_PARITY_EVEN;
    uart2Config.wordLength = UART_DATABITS_8;
    uart2Config.stopBits = UART_STOPBITS_1;
    uart2Config.callback = NULL;
    UART_Init(TX_UART, &uart2Config, &uart2Handle);

    /* 2, poll to transmit */
    while (g_txSize < UART_USERBUFFER_SIZE)
    {
        UART_PutChar(uart2Handle.device, g_txData[g_txSize++]);
    }

    SysTick_DelayMS(1000);

    /* 3, check the result */
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        if (g_rxData[i] != g_txData[i])
        {
            printf("testcase_uart_rxtx_irq_poll error.");
            return RET_ERROR;
        }
    }

    return RET_OK;
}
#endif

#ifdef Uart_Rx_Irq_Tx_Irq
static int testcase_uart_rxtx_irq_irq(UART_Device_T *RX_UART, UART_Device_T *TX_UART)
{
    uint16_t i, tag;

    UART_Config_T uart1Config;
    UART_Config_T uart2Config;

    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = uart_receive_callback;
    UART_Init(RX_UART, &uart1Config, &uart1Handle);

    //1, enable rx irq
    UART_EnableInterrupts(&uart1Handle, UART_RX_INT);

    uart2Config.baudRate = BAUD_RATE;
    uart2Config.parity = UART_PARITY_EVEN;
    uart2Config.wordLength = UART_DATABITS_8;
    uart2Config.stopBits = UART_STOPBITS_1;
    uart2Config.callback = uart_transmit_callback;
    UART_Init(TX_UART, &uart2Config, &uart2Handle);

    /* 2, enable tx irq , and then transmit first byte */
    UART_EnableInterrupts(&uart2Handle, UART_TX_INT);
    g_txSize = 1;
    UART_PutChar(TX_UART, g_txData[0]);

    SysTick_DelayMS(1000);

    /* 4, check the result */
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        if (g_rxData[i] != g_txData[i])
        {
            tag = 1;
            break;
        }
        else
        {
            tag = 0;
        }
    }

    if (tag == 1)
    {
        printf("testcase_uart_rxtx_irq_irq failed.\n");
    }
    else
    {
        printf("testcase_uart_rxtx_irq_irq successful.\n");
    }

    return RET_OK;
}
#endif

#ifdef Uart_tx_and_rx_block
static int TestCase_Tx_Block_Rx_Block(UART_Device_T *Rx_UART, UART_Device_T *Tx_UART)
{
    UART_Config_T Uart_Rx_Config;
    UART_Config_T Uart_Tx_Config;

    UART_TransferConfig_T Rx_uart_TransConfig;
    UART_TransferConfig_T Tx_uart_TransConfig;

    uint16_t i, tag;
    uint16_t copiedSize;

    /* Uart device configuration */
    Uart_Rx_Config.baudRate = BAUD_RATE;
    Uart_Rx_Config.parity = UART_PARITY_EVEN;
    Uart_Rx_Config.wordLength = UART_DATABITS_8;
    Uart_Rx_Config.stopBits = UART_STOPBITS_1;
    Uart_Rx_Config.callback = NULL;
    UART_Init(Rx_UART, &Uart_Rx_Config, &uart1Handle);

    /* Data transfer configuration */
    Rx_uart_TransConfig.enableFifo = FALSE;
    Rx_uart_TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    Rx_uart_TransConfig.rxWatermark = MAX_UART_RX_FIFO_SIZE / 2;
    Rx_uart_TransConfig.enableRxDMA = FALSE;
    Rx_uart_TransConfig.enableTxDMA = FALSE;
    UART_TransferConfig(&uart1Handle, &Rx_uart_TransConfig);

    /* Enable RX Interrupt */
    UART_EnableInterrupts(&uart1Handle, UART_RX_INT);

    /* Initialize variable about recevied size */
    UART_Receive(&uart1Handle, &g_rxData[0], UART_USERBUFFER_SIZE, &copiedSize, TIMEOUT_NO_BLOCKING);

    Uart_Tx_Config.baudRate = BAUD_RATE;
    Uart_Tx_Config.parity = UART_PARITY_EVEN;
    Uart_Tx_Config.wordLength = UART_DATABITS_8;
    Uart_Tx_Config.stopBits = UART_STOPBITS_1;
    Uart_Tx_Config.callback = NULL;
    UART_Init(Tx_UART, &Uart_Tx_Config, &uart2Handle);

    /* Data transfer configuration */
    Tx_uart_TransConfig.enableFifo = FALSE;
    Tx_uart_TransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    Tx_uart_TransConfig.rxWatermark = MAX_UART_RX_FIFO_SIZE / 2;
    Tx_uart_TransConfig.enableRxDMA = FALSE;
    Tx_uart_TransConfig.enableTxDMA = FALSE;
    UART_TransferConfig(&uart2Handle, &Tx_uart_TransConfig);

    /* Enable TX interrupt */
    UART_EnableInterrupts(&uart2Handle, UART_TX_INT);

    UART_Transmit(&uart2Handle, &g_txData[0], UART_USERBUFFER_SIZE, TIMEOUT_NO_BLOCKING);

    SysTick_DelayMS(1000);

    //check the result
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        if (g_rxData[i] != g_txData[i])
        {
            tag = 1;
            return RET_ERROR;
        }
        else
        {
            tag = 0;
        }
    }

    if (tag == 1)
    {
        printf("TestCase_Tx_Block_Rx_Block failed.\n");
    }
    else
    {
        printf("TestCase_Tx_Block_Rx_Block successful.\n");
    }

    return RET_OK;

}
#endif

void uart_setmode_case_receive_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    if (event == UART_EVENT_RX_CHAR)
    {
        g_rxData[g_rxSize++] = param;  /* The datas is stored here */

        if (g_rxSize == UART_USERBUFFER_SIZE)
        {
            g_rxSize = 0;
        }
        /* printf the datas which are from PC */
        UART_PutChar(hUART->device, param);
    }
}

#ifdef Uart_set_mode_change_to_PC
static void TestCase_UART_Setmode(UART_Device_T *pUART)
{
    UART_Config_T uartConfig;

    uartConfig.baudRate = BAUD_RATE;
    uartConfig.parity = UART_PARITY_EVEN;
    uartConfig.wordLength = UART_DATABITS_8;
    uartConfig.stopBits = UART_STOPBITS_1;
    uartConfig.callback = uart_setmode_case_receive_callback;
    UART_Init(pUART, &uartConfig, &uartHandle);

    /* enable rx irq */
    UART_EnableInterrupts(&uartHandle, UART_RX_INT);

    printf("Default tx and rx mode are both allowed!\n");

    /* Default mode is UART_MODE_DUPLEX, Now close tx and rx. */
    UART_SetMode(&uartHandle, UART_MODE_DISABLE);
    printf("test whether you can send data to pc!\n");

    /* allow uart tx to send data to PC */
    UART_SetMode(&uartHandle, UART_MODE_TX);
    printf("tx is allowed, but rx is not allowed!\n");

    /* config uart to rx_mode but tx is allowed */
    UART_SetMode(&uartHandle, UART_MODE_RX);
    printf("rx is allowed, but tx is not allowed, data is stored in g_rxData[]\n");

    SysTick_DelayMS(10000);
    UART_SetMode(&uartHandle, UART_MODE_DUPLEX);
    printf("after ten second tx and rx both are allowed, data is stored in g_rxData[]\n");
}
#endif

#ifdef Uart_HalfDuplex
#define DUPLEX_USERBUFFER_SIZE 20
void uart_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    if (event == UART_EVENT_RX_CHAR)
    {
        if (g_rxSize < DUPLEX_USERBUFFER_SIZE)
        {
            g_rxData[g_rxSize++] = param;
        }
    }
    else if (event == UART_EVENT_RX_ERROR)
    {
        printf("uart receive error.\n");
    }
    else if (event == UART_EVENT_TX_EMPTY)
    {
        if (g_txSize >= 1 && g_txSize < DUPLEX_USERBUFFER_SIZE)
        {
            UART_PutChar(hUART->device, g_txData[g_txSize++]);
        }
    }
    else if (event == UART_EVENT_TX_IDLE)
    {
        g_txSize = DUPLEX_USERBUFFER_SIZE;
    }
}

static void TestCase_UART_HalfDuplex(UART_Device_T *uart1_dev, UART_Device_T *uart2_dev)
{
    uint16_t i, tag;
    int  uart1_duplex_status, uart2_duplex_status;
    uint64_t startTime1, startTime2;
    UART_TransferStatus_T uart1Status;
    UART_TransferStatus_T uart2Status;
    UART_Config_T uart1Config;
    UART_Config_T uart2Config;

    /* Init user buffer */
    SetUserBuffer();

    /* Set pinmux and pin filter parameters for uart pins if it is necessary. */
	  /*
    UART_PinmuxConfig(RX_UART);
    UART_PinFilter(RX_UART);
    */
	
    /* Initialize uart device */
    uart1Config.baudRate = BAUD_RATE;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = uart_callback;
    UART_Init(uart1_dev, &uart1Config, &uart1Handle);
	
    uart2Config.baudRate = BAUD_RATE;
    uart2Config.parity = UART_PARITY_EVEN;
    uart2Config.wordLength = UART_DATABITS_8;
    uart2Config.stopBits = UART_STOPBITS_1;
    uart2Config.callback = uart_callback;
    UART_Init(uart2_dev, &uart2Config, &uart2Handle);
		
    uart1_duplex_status = 0;
    uart2_duplex_status = 0;
    g_rxSize = g_txSize = 0;
    
    /* Main loop */
    while (1)
    {
        /* Master device */
        if (OPEN_UART_RECEIVE == uart1_duplex_status) 
        {
            startTime1 = SysTick_GetSysTime();
            uart1_duplex_status = CLOSE_UART_RECEIVE;
            /* First of all, enable uart1 rx irq */	
            UART_SetMode(&uart1Handle, UART_MODE_RX);
            UART_EnableInterrupts(&uart1Handle, UART_RX_INT);
        }
        else if (CLOSE_UART_RECEIVE == uart1_duplex_status)
        {
            if ((SysTick_GetSysTime() - startTime1) >= 500)        /* Wait 500 milliseconds */
            {
                UART_DisableInterrupts(&uart1Handle, UART_RX_INT);
                
                /* Process received data */
                tag = 1;
                if (g_rxSize >= DUPLEX_USERBUFFER_SIZE)
                {
                    for (i = 0; i < DUPLEX_USERBUFFER_SIZE; i++)
                    {
                        if (g_rxData[i] != g_txData[i])
                        {
                            break;
                        }
                    }
                    if (i < DUPLEX_USERBUFFER_SIZE)
                    {
                        printf("testcase_uart_halfduplex master failed.\n");
                    }
                    else
                    {
                        tag = 0;
                        printf("testcase_uart_halfduplex master succeeded.\n");
                    }
                }
                /* Init user buffer again */
                for (i = 0; i < DUPLEX_USERBUFFER_SIZE; i++)
                {
                    g_rxData[i] = 0xFF;
                }
                g_rxSize = 0;
                g_txSize = 0;
                uart1_duplex_status = OPEN_UART_TRANSMIT;
            }
        }
        else if (OPEN_UART_TRANSMIT == uart1_duplex_status)
        {
            startTime1 = SysTick_GetSysTime();
            uart1_duplex_status = CLOSE_UART_TRANSMIT;
            
            UART_SetMode(&uart1Handle, UART_MODE_TX);
            UART_EnableInterrupts(&uart1Handle, UART_TX_INT);
            g_txSize = 1;
            UART_PutChar(uart1Handle.device, g_txData[0]);
        }
        else if (CLOSE_UART_TRANSMIT == uart1_duplex_status)
        {
            /* Wait transmitter idle */
            if (g_txSize >= DUPLEX_USERBUFFER_SIZE)
            {
                UART_GetTransferStatus(&uart1Handle, &uart1Status);
                if (uart1Status.lineStatus & UART_TRANSMITTER_IDLE)
                {
                    UART_DisableInterrupts(&uart1Handle, UART_TX_INT);
                    uart1_duplex_status = OPEN_UART_RECEIVE;
                }
                /* 
                if ((SysTick_GetSysTime() - startTime2) >= 500)
                {
                    UART_DisableInterrupts(&uart1Handle, UART_TX_INT);
                    uart1_duplex_status = 0;
                }
                */
            }
        }
        else
        {
            uart1_duplex_status = OPEN_UART_RECEIVE;
        }

        /* Slave device */
        if (OPEN_UART_RECEIVE == uart2_duplex_status) 
        {
            startTime2 = SysTick_GetSysTime();
            uart2_duplex_status = CLOSE_UART_RECEIVE;
            /* First of all, enable uart2 rx irq */	
            UART_SetMode(&uart2Handle, UART_MODE_RX);
            UART_EnableInterrupts(&uart2Handle, UART_RX_INT);
        }
        else if (CLOSE_UART_RECEIVE == uart2_duplex_status)
        {
            /* Slave device transmit only if it receives data from master */
            if (g_rxSize < 1)
            {
                startTime2 = SysTick_GetSysTime(); 
            }
            if (g_rxSize > 0 && (SysTick_GetSysTime() - startTime2) >= 400)  /* Wait 400 milliseconds, there is 100ms protection gap */
            {
                UART_DisableInterrupts(&uart2Handle, UART_RX_INT);

                /* Process received data */
                tag = 1;
                if (g_rxSize >= DUPLEX_USERBUFFER_SIZE)
                {
                    for (i = 0; i < DUPLEX_USERBUFFER_SIZE; i++)
                    {
                        if (g_rxData[i] != g_txData[i])
                        {
                            break;
                        }
                    }
                    if (i < DUPLEX_USERBUFFER_SIZE)
                    {
                        printf("testcase_uart_halfduplex slave  failed.\n");
                    }
                    else
                    {
                        tag = 0;
                        printf("testcase_uart_halfduplex slave succeeded.\n");
                    }
                }
                /* Init user buffer again */
                for (i = 0; i < DUPLEX_USERBUFFER_SIZE; i++)
                {
                    g_rxData[i] = 0xFF;
                }
                g_rxSize = 0;
                g_txSize = 0;
                /* Slave device transmit only if it receives data correctly */
                if (tag == 0)
                {
                    uart2_duplex_status = OPEN_UART_TRANSMIT;
                }
            }
        }
        else if (OPEN_UART_TRANSMIT == uart2_duplex_status)
        {
            startTime2 = SysTick_GetSysTime();
            uart2_duplex_status = CLOSE_UART_TRANSMIT;
            
            UART_SetMode(&uart2Handle, UART_MODE_TX);
            UART_EnableInterrupts(&uart2Handle, UART_TX_INT);
            g_txSize = 1;
            UART_PutChar(uart2Handle.device, g_txData[0]);
        }
        else if (CLOSE_UART_TRANSMIT == uart2_duplex_status)
        {
            /* Wait transmitter idle */
            if (g_txSize >= DUPLEX_USERBUFFER_SIZE)
            {
                UART_GetTransferStatus(&uart2Handle, &uart2Status);
                if (uart2Status.lineStatus & UART_TRANSMITTER_IDLE)
                {
                    UART_DisableInterrupts(&uart2Handle, UART_TX_INT);
                    uart2_duplex_status = 0;
                }
                /* 
                if ((SysTick_GetSysTime() - startTime2) >= 500)
                {
                    UART_DisableInterrupts(&uart2Handle, UART_TX_INT);
                    uart2_duplex_status = 0;
                }
               */
            }
        }
        else
        {
            uart2_duplex_status = OPEN_UART_RECEIVE;
        }

    } /* End of main loop */

}
#endif /* End of half duplex test case */

int main(void)
{
    SysTick_Init();
    SysTick_DelayMS(1000);

    UART_ConfigPinmux(UART0);
    UART_ConfigPinmux(UART1);
    UART_ConfigPinmux(UART2);

    DebugConsoleInit(BAUD_RATE);

    SetUserBuffer();

#ifdef Default_port_printf
    testcase_uart_printf();
#endif

#ifdef optional_uart_port_tx
    testcase_uart_tx(UART2);
#endif

#ifdef uart_interrupt_receive_from_PC
    testcase_uart_rx_irq(UART1);
#endif

#ifdef uart_interrupt_transfer
    testcase_uart_tx_irq(UART1);  //printf port can watch printf data
#endif

#ifdef Uart_Interrupt_RX_Poll_tx
    testcase_uart_rxtx_irq_poll(UART1, UART2);
#endif

#ifdef Uart_Rx_Irq_Tx_Irq
    testcase_uart_rxtx_irq_irq(UART1, UART2);
#endif

#ifdef Uart_tx_and_rx_block
    TestCase_Tx_Block_Rx_Block(UART1, UART2);
#endif

#ifdef Uart_HalfDuplex
    TestCase_UART_HalfDuplex(UART1, UART2);
#endif

    while (1);
}
