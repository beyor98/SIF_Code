#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/gpio.h>

UART_Handle_T uartHandle;
UART_Handle_T uart1Handle;
UART_Handle_T uart2Handle;

#define UART_USERBUFFER_SIZE 32

uint8_t g_rxData[UART_USERBUFFER_SIZE];
uint8_t g_txData[UART_USERBUFFER_SIZE];

volatile uint32_t g_rxSize = 0;
volatile uint32_t g_txSize = UART_USERBUFFER_SIZE;
volatile uint32_t g_event = UART_EVENT_NONE;

//implements in printf.c
RET_CODE UartPinMuxConfig(UART_Device_T *pUART);

static void SetUserBuffer(void)
{
    uint32_t i;
    for (i = 0; i < UART_USERBUFFER_SIZE; i++)
    {
        g_rxData[i] = 0xFF;
        g_txData[i] = (i & 0xFF);
    }
}

void uart_receive_callback(UART_Handle_T *hUART, uint8_t event, unsigned long param)
{
    //No fifo mode
    if (event == UART_EVENT_RX_CHAR)
    {
        g_rxData[g_rxSize++] = param;

        //Fifo mode
    }
    else if (event == UART_EVENT_RX_WATERMARK || event == UART_EVENT_RXFIFO_TIMEOUT)
    {
        uint32_t rcvCount = (uint32_t)param;
        while (rcvCount--)
        {
            g_rxData[g_rxSize++] = UART_GetChar(hUART->device); //UART0-2
        }
    }
    else if (event == UART_EVENT_RX_OVER || event == UART_EVENT_RX_ERROR)
    {
        g_event = event;
    }
}

static int testcase_uart_printf(void)
{
    int i = 0;

    DebugConsoleInit(115200L);
    while (i++ < 5)
    {
        printf("printf string with uart tx, i = %d.\n", i);
        SysTick_DelayMS(500);
    }

    return RET_OK;
}

static int testcase_uart_tx(UART_Device_T *pUART)
{
    uint8_t ch;
    UART_Config_T uartConfig;
    /* Uart1 device configuration */
    uartConfig.baudRate = 115200L;
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

static int testcase_uart_txrx_irq(UART_Device_T *pUART1, UART_Device_T *pUART2)
{
    uint16_t i;
    uint16_t copiedSize;
    UART_Config_T uart1Config;
    UART_Config_T uart2Config;
    //UART_TransferConfig_T uartTransConfig;

    SetUserBuffer();

    uart1Config.baudRate = 115200L;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = NULL;
    UART_Init(pUART1, &uart1Config, &uart1Handle);

    //1, enable rx irq and prepare for receiving
    UART_EnableInterrupts(&uart1Handle, UART_RX_INT);
    UART_Receive(&uart1Handle, &g_rxData[0], g_txSize, &copiedSize, TIMEOUT_NO_BLOCKING);

    uart2Config.baudRate = 115200L;
    uart2Config.parity = UART_PARITY_EVEN;
    uart2Config.wordLength = UART_DATABITS_8;
    uart2Config.stopBits = UART_STOPBITS_1;
    uart2Config.callback = NULL;
    UART_Init(pUART2, &uart2Config, &uart2Handle);

#if 0 //config for FIFO or DMA
    uartTransConfig.enableFifo = TRUE;
    uartTransConfig.txWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uartTransConfig.rxWatermark = MAX_UART_TX_FIFO_SIZE / 2;
    uartTransConfig.enableRxDMA = FALSE;
    uartTransConfig.enableTxDMA = TRUE;  //6601B&6611 just has only 1 DMA channel
    UART_TransferConfig(uart2Handle, &uartTransConfig);
#endif

    //2, enable tx irq and start to transmit data
    UART_EnableInterrupts(&uart2Handle, UART_TX_INT);
    UART_Transmit(&uart2Handle, &g_txData[0], g_txSize, TIMEOUT_NO_BLOCKING);

    SysTick_DelayMS(500);

    //3, check the result
    for (i = 0; i < g_txSize; i++)
    {
        if (g_rxData[i] != g_txData[i])
        {
            return RET_ERROR;
        }
    }

    return RET_OK;
}

static int testcase_uart_txrx_poll(UART_Device_T *pUART1, UART_Device_T *pUART2)
{
    uint16_t i;
    uint16_t count = 0;
    UART_Config_T uart1Config;
    UART_Config_T uart2Config;

    SetUserBuffer();

    uart1Config.baudRate = 115200L;
    uart1Config.parity = UART_PARITY_EVEN;
    uart1Config.wordLength = UART_DATABITS_8;
    uart1Config.stopBits = UART_STOPBITS_1;
    uart1Config.callback = uart_receive_callback;
    UART_Init(pUART1, &uart1Config, &uart1Handle);

    //1, enable rx irq
    UART_EnableInterrupts(&uart1Handle, UART_RX_INT);

    uart2Config.baudRate = 115200L;
    uart2Config.parity = UART_PARITY_EVEN;
    uart2Config.wordLength = UART_DATABITS_8;
    uart2Config.stopBits = UART_STOPBITS_1;
    uart2Config.callback = NULL;
    UART_Init(pUART2, &uart2Config, &uart2Handle);

    //2, poll to transmit
    while (count < g_txSize)
    {
        UART_PutChar(uart2Handle.device, g_txData[count]);
        count++;
    }

    SysTick_DelayMS(100);

    //3, check the result
    for (i = 0; i < g_txSize; i++)
    {
        if (g_rxData[i] != g_txData[i])
        {
            return RET_ERROR;
        }
    }

    return RET_OK;
}

int main(void)
{
    int ret = RET_OK;

    SysTick_Init();
    SysTick_DelayMS(500);

    //need to connect PC COM
    //default use UART0 in printf.c
    testcase_uart_printf();


    UartPinMuxConfig(UART0);
    UartPinMuxConfig(UART1);
    UartPinMuxConfig(UART2);

    //need to connect PC COM
    testcase_uart_tx(UART0);

    //need to connect PC COM
    //testcase_uart_tx(UART1);

    //need to connect PC COM
    //testcase_uart_tx(UART2);

    /*
       need to use line to connect like this:
       UART1:tx --- UART2:rx
       UART1:rx --- UART2:tx
    */
    ret = testcase_uart_txrx_irq(UART1, UART2);
    if (ret == RET_OK)
    {
        printf("testcase_uart_txrx_irq OK!\n");
    }
    else
    {
        printf("testcase_uart_txrx_irq Failed!\n");
    }

    /*
       need to use line to connect like this:
       UART1:tx --- UART2:rx
       UART1:rx --- UART2:tx
    */
    ret = testcase_uart_txrx_poll(UART1, UART2);
    if (ret == RET_OK)
    {
        printf("testcase_uart_txrx_poll OK!\n");
    }
    else
    {
        printf("testcase_uart_txrx_poll Failed!\n");
    }

    while (1) {}
}
