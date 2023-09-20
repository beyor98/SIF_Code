/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          printf.c
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         Debug printf library.
 *
 * @note
 *    2015/09/08, wen.liu, V1.0.0
 *        Initial version.
 *    2016/11/08, wen.liu, V1.1.0
 *        Update for uasge with microlib.
 */
#include "common.h"
#include <drv/dev/uart.h>
#include <drv/dev/gpio.h>

#define PRINTF_PORT UART0
#define DEBUG_PRINTF 1

typedef struct
{
    uint32_t pin;                  /**< Specifies the GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;


/** Configure pinmux for an available UART device */
RET_CODE UartPinMuxConfig(UART_Device_T *pUART)
{
    RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    Device_Pinmux_T s_uartPinMux[2];

    if (pUART == UART0)
    {
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C)
        s_uartPinMux[0].pin = GPIO_PIN_6;
        s_uartPinMux[0].device = GPIOD;
        s_uartPinMux[0].alternate = GPIOD6_AF0_UART0TX;
        s_uartPinMux[1].pin = GPIO_PIN_7;
        s_uartPinMux[1].device = GPIOD;
        s_uartPinMux[1].alternate = GPIOD7_AF0_UART0TX;
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
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C)
        s_uartPinMux[0].pin = GPIO_PIN_4;
        s_uartPinMux[0].device = GPIOD;
        s_uartPinMux[0].alternate = GPIOD4_AF0_UART1TX;
        s_uartPinMux[1].pin = GPIO_PIN_5;
        s_uartPinMux[1].device = GPIOD;
        s_uartPinMux[1].alternate = GPIOD5_AF0_UART1TX;
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
#if defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C)
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

/* microlib printf library would callback fputc() */
int fputc(int ch, FILE *f)
{
#ifdef DEBUG_PRINTF
    /* Use single \n for new empty line */
    if ('\n' == ch)
    {
        UART_PutChar(PRINTF_PORT, '\r');
    }
    UART_PutChar(PRINTF_PORT, ch);
#endif

    return 1;
}


/** Init uart port */
void DebugConsoleInit(uint32_t baudrate)
{
#ifdef DEBUG_PRINTF
    UART_Config_T config;

    UartPinMuxConfig(PRINTF_PORT);

    config.baudRate = baudrate;
    config.parity = UART_PARITY_EVEN;
    config.wordLength = UART_DATABITS_8;
    config.stopBits = UART_STOPBITS_1;
    config.callback = NULL;

    UART_Init(PRINTF_PORT, &config, NULL);
#endif
}



