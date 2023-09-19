/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file spi_transfer.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/19
 *
 * @contents Verification and example code of Cyclic redundancy check (CRC).
 *
 * @history
 *     Date            By            Comments               Ver.
 *  =========       =========        =========           =========
 *  2015/07/19      Wen Liu          Create                 1.0.0
 *  2020/11/25      Christal Xu      update                 1.1.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/spi.h>
#include <drv/dev/gpio.h>
/******************************************************************************
* Global variables
******************************************************************************/
static SPI_Handle_T g_spiHandle;

/******************************************************************************
* Constants and macros
******************************************************************************/
#define TRANSFER_SPI    SPI0
/******************************************************************************
* Local types
******************************************************************************/
typedef struct
{
    uint32_t pin;                  /**< Specify GPIO pins to be configured */
    GPIO_Device_T *device;         /**< GPIO device: 0x4 */
    uint32_t alternate;            /**< Alternate function */
} Device_Pinmux_T;

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/

/******************************************************************************
* Transfer format, different bits format transfer of data(from 0x00 to 0x07)
+-----------------------------------------------------------------------------------------------------------------------+
|           |   bytes   |   byte0   |   byte1   |   byte2   |   byte3   |   byte4   |   byte5   |   byte6   |   byte7   |
|           |-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
|     F     |   8bits   |   0x00    |   0x01    |   0x02    |   0x03    |   0x04    |   0x05    |   0x06    |   0x07    |
|     O     |-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
|     R     |   16bits  |   0x00    |   0x00    |   0x01    |   0x00    |   0x02    |   0x00    |   0x03    |   0x00    |
|     M     |-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
|     A     |   24bits  |   0x00    |   0x00    |   0x00    |   0x00    |   0x01    |   0x00    |   0x00    |   0x00    | need to be 32bit aligned
|     T     |-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
|           |   32bits  |   0x00    |   0x00    |   0x00    |   0x00    |   0x01    |   0x00    |   0x00    |   0x00    |
+-----------------------------------------------------------------------------------------------------------------------+
******************************************************************************/
#ifdef CHIP_GM6601
Device_Pinmux_T g_spi0PinMux[8] =
{
    {GPIO_PIN_11, GPIOA, GPIOA11_AF0_SPICSJ0}, {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
    {GPIO_PIN_13, GPIOA, GPIOA13_AF0_SPICSJ1}, {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
    {GPIO_PIN_1,  GPIOB, GPIOB1_AF0_SPICSJ0},  {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
    {GPIO_PIN_2,  GPIOB, GPIOB2_AF0_SPICSJ1},  {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
};
#endif

#ifdef CHIP_GM6601B
//Device_Pinmux_T g_spi0PinMux[8] = {
//{GPIO_PIN_11, GPIOA, GPIOA11_AF0_SPICSJ0}, {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
//{GPIO_PIN_13, GPIOA, GPIOA13_AF0_SPICSJ1}, {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
//{GPIO_PIN_1,  GPIOB, GPIOB1_AF0_SPICSJ0},  {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
//{GPIO_PIN_2,  GPIOB, GPIOB2_AF0_SPICSJ1},  {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
//};
Device_Pinmux_T g_spi0PinMux[4] =
{
    {GPIO_PIN_11, GPIOA, GPIOA11_AF0_SPICSJ0},  {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
    {GPIO_PIN_0, GPIOB, GPIOB0_AF0_SPIMOSI}, {GPIO_PIN_15,  GPIOA, GPIOA15_AF0_SPIMISO},
};
Device_Pinmux_T g_spi1PinMux[4] =
{
    {GPIO_PIN_11, GPIOA, GPIOA11_AF0_SPICSJ0},  {GPIO_PIN_14, GPIOA, GPIOA14_AF0_SPICLK},
    {GPIO_PIN_0, GPIOB, GPIOB0_AF0_SPIMOSI}, {GPIO_PIN_15,  GPIOA, GPIOA15_AF0_SPIMISO},
};
#endif

#ifdef CHIP_GM6611
Device_Pinmux_T g_spi0PinMux[4] =
{
    {GPIO_PIN_10, GPIOA, GPIOA10_AF1_SPI0SS0},  {GPIO_PIN_11, GPIOA, GPIOA11_AF1_SPI0SCK},
    {GPIO_PIN_12, GPIOA, GPIOA12_AF1_SPI0MOSI}, {GPIO_PIN_9,  GPIOA, GPIOA9_AF1_SPI0MISO},
};

Device_Pinmux_T g_spi1PinMux[4] =
{
    {GPIO_PIN_11, GPIOE, GPIOE11_AF1_SPI1SS0},  {GPIO_PIN_12, GPIOE, GPIOE12_AF1_SPI1SCK},
    {GPIO_PIN_13, GPIOE, GPIOE13_AF1_SPI1MOSI}, {GPIO_PIN_10,  GPIOE, GPIOE10_AF1_SPI1MISO},
};
#endif

#ifdef CHIP_GM6611B
Device_Pinmux_T g_spi0PinMux[4] =
{
    {GPIO_PIN_10, GPIOA, GPIOA10_AF1_SPI0SS0},  {GPIO_PIN_11, GPIOA, GPIOA11_AF1_SPI0SCK},
    {GPIO_PIN_12, GPIOA, GPIOA12_AF1_SPI0MOSI}, {GPIO_PIN_9,  GPIOA, GPIOA9_AF1_SPI0MISO},
};

Device_Pinmux_T g_spi1PinMux[4] =
{
    {GPIO_PIN_11, GPIOE, GPIOE11_AF1_SPI1SS0},  {GPIO_PIN_12, GPIOE, GPIOE12_AF1_SPI1SCK},
    {GPIO_PIN_13, GPIOE, GPIOE13_AF1_SPI1MOSI}, {GPIO_PIN_10,  GPIOE, GPIOE10_AF1_SPI1MISO},
};
#endif

#ifdef CHIP_GM6621
static Device_Pinmux_T g_spi0PinMux[4] =
{
    {GPIO_PIN_8, GPIOA, GPIOA8_AF2_SPI0SS0},  {GPIO_PIN_9, GPIOA, GPIOA9_AF2_SPI0SCK},
    {GPIO_PIN_10, GPIOA, GPIOA10_AF2_SPI0MISO}, {GPIO_PIN_11,  GPIOA, GPIOA11_AF2_SPI0MOSI},
};

static Device_Pinmux_T g_spi1PinMux[4] =
{
    {GPIO_PIN_6, GPIOB, GPIOB6_AF2_SPI1SS0},  {GPIO_PIN_14, GPIOA, GPIOA14_AF2_SPI1SCK},
    {GPIO_PIN_6, GPIOD, GPIOD6_AF2_SPI1MOSI}, {GPIO_PIN_15,  GPIOA, GPIOA15_AF2_SPI1MISO},
};
#endif

#ifdef CHIP_F6621B
Device_Pinmux_T g_spi0PinMux[4] =
{
    {GPIO_PIN_15, GPIOA, GPIOA15_AF5_SPI0SSJ},  {GPIO_PIN_0, GPIOB, GPIOB0_AF5_SPI0SCK},
    {GPIO_PIN_2, GPIOB, GPIOB2_AF5_SPI0MOSI}, {GPIO_PIN_1,  GPIOB, GPIOB1_AF5_SPI0MISO},
};

Device_Pinmux_T g_spi1PinMux[4] =
{
    {GPIO_PIN_7, GPIOA, GPIOA7_AF5_SPI1SSJ},  {GPIO_PIN_8, GPIOA, GPIOA8_AF5_SPI1SCK},
    {GPIO_PIN_10, GPIOA, GPIOA10_AF5_SPI1MOSI}, {GPIO_PIN_9,  GPIOA, GPIOA9_AF5_SPI1MISO},
};
#endif

__attribute__((aligned(4))) volatile uint8_t s_txData[0x30]; /**< User spi transmission buffer, 4bytes aligned */
__attribute__((aligned(4))) volatile uint8_t s_rxData[0x30]; /**< User spi reception buffer, 4bytes aligned */

/******************************************************************************
* Local functions
******************************************************************************/

/******************************************************************************
* Global functions
******************************************************************************/

static void SPI_InterruptHandler(SPI_Handle_T *hSPI, uint8_t event, unsigned long param)
{
    //printf("spi interrupt\n");
}

/********************************************************************/
static int SPIPinMuxConfig(SPI_Device_T *pSPI)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    RET_CODE ret = RET_OK;

    if (pSPI == SPI0)
    {
        /* SPI0 CS GPIO Configuration */
        GPIO_InitStruct.pin = g_spi0PinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi0PinMux[0].alternate;

        ret = GPIO_Init(g_spi0PinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        /* SPI0 CLK GPIO Configuration */
        GPIO_InitStruct.pin = g_spi0PinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi0PinMux[1].alternate;
        ret = GPIO_Init(g_spi0PinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        /* SPI0 MOSI GPIO Configuration */
        GPIO_InitStruct.pin = g_spi0PinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi0PinMux[2].alternate;
        ret = GPIO_Init(g_spi0PinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        /* SPI0 MISO GPIO Configuration */
        GPIO_InitStruct.pin = g_spi0PinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi0PinMux[3].alternate;
        ret = GPIO_Init(g_spi0PinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }
    else
    {
        /* SPI0 CS GPIO Configuration */
        GPIO_InitStruct.pin = g_spi1PinMux[0].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi1PinMux[0].alternate;

        ret = GPIO_Init(g_spi1PinMux[0].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        /* SPI0 CLK GPIO Configuration */
        GPIO_InitStruct.pin = g_spi1PinMux[1].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi1PinMux[1].alternate;
        ret = GPIO_Init(g_spi1PinMux[1].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        /* SPI0 MOSI GPIO Configuration */
        GPIO_InitStruct.pin = g_spi1PinMux[2].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi1PinMux[2].alternate;
        ret = GPIO_Init(g_spi1PinMux[2].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }

        /* SPI0 MISO GPIO Configuration */
        GPIO_InitStruct.pin = g_spi1PinMux[3].pin;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_NOPULL;
        GPIO_InitStruct.alternate = g_spi1PinMux[3].alternate;
        ret = GPIO_Init(g_spi1PinMux[3].device, &GPIO_InitStruct);
        if (ret != RET_OK)
        {
            return RET_ERROR;
        }
    }
    return ret;
}

void spi_master_config(SPI_Handle_T *hSPI)
{
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));

    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);
    /* Get spi default configuration */
    spiConfig.mode = SPI_MASTER_MODE;
    /* MOTO mode configuration */
    spiConfig.format = SPI_FORMAT_MOTOLORA;
    /* config polarity */
    spiConfig.polarity = SPI_CLOCK_POLARITY_LOW;
    /* config phase */
    spiConfig.phase = SPI_CLOCK_PHASE_MIDDLE;
    /* LSB or MSB configuration */
    spiConfig.bitOrder = SPI_BIT_ORDER_MSB;
    /* baudrate configuration */
    spiConfig.baudRate = 200000;
    /* spi callback function configuration */
    spiConfig.callback = SPI_InterruptHandler;

    SPI_Init(TRANSFER_SPI, &spiConfig, hSPI);

    /* Initialize spi device with changeable configuration about data transferring,
    such as fixed frames, duplex mode, dma usage, fifo threshhold, continuous
    data frames, etc. */

    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_DATA_FRAME);
    /* Data frame width */
    spiTransferConfig.frameConfig.dataBits = 8;
    /* Transfer mode configuration, only can use tx_only, rx_only, half_duplex(master mode) full_duplex */
    spiTransferConfig.duplex = SPI_FULL_DUPLEX;
    /* Tx fifo threshold value, from 1 to 2 */
    spiTransferConfig.txWatermark = 2;
    /* Rx fifo threshold value, from 1 to 4 */
    spiTransferConfig.rxWatermark = 4;
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = FALSE;  //there is only one dma channel, can't both enable tx and rx dma
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = TRUE;

    SPI_TransferConfig(hSPI, &spiTransferConfig);

    /* Enable interrupts, when enable tx and rx DMA, DMA_INT must be enabled*/
    //SPI_EnableInterrupts(hSPI, (SPI_RX_INT | SPI_TX_INT));
}

int main(void)
{
    uint32_t i;
    SPI_Handle_T *spiHandle;

    /* Initialize uart device for printf port */
    DebugConsoleInit(115200);
    /* Init system timer */
    SysTick_Init();

    printf("Sample code of spi driver \n");

    /* Pinmux setting */
    SPIPinMuxConfig(TRANSFER_SPI);
    /* Enable global interrupt handling */
    enable_interrupt;

#if defined(CHIP_F6621B)
    /* clock enable and reset */
    SPI1_RESET();
    SPI1_CLOCK_ENABLE();
#endif
    spiHandle = &g_spiHandle;

    spi_master_config(spiHandle);

    /* Transmitted data is here */
    for (i = 0; i < 32; i++)
    {
        s_txData[i] = i + 16;
    }

    while (1)
    {
        /* full duplex transfer in TIMEOUT_NO_BLOCKING mode */
        SPI_TransmitReceive(spiHandle, (uint8_t *)&s_txData[0], (uint8_t *)&s_rxData[0], 32, 32, TIMEOUT_NO_BLOCKING);
        while (spiHandle->status != SPI_STATUS_IDLE);

        /* full duplex transfer in TIMEOUT_WAIT_FOREVER mode */
        //SPI_TransmitReceive(spiHandle, &s_txData[0], &s_rxData[0], 32, 32, TIMEOUT_WAIT_FOREVER);

        SysTick_DelayMS(1000);
    };

}
