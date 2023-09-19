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
/*GM6601B only support SPI0*/
#ifdef CHIP_GM6601B
    #define TRANSFER_SPI    SPI0
    #define FLASH_SPI       SPI0
#else
    #define TRANSFER_SPI    SPI0
    #define FLASH_SPI       SPI0
#endif
#define SPI_TRANSFER    (1)
#define SPI_FLASH       (0)
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

#if defined CHIP_GM6601B ||defined CHIP_F6601C 
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

__attribute__((aligned(4))) volatile uint8_t s_txData[0x200]; /**< User spi transmission buffer, 4bytes aligned */
__attribute__((aligned(4))) volatile uint8_t s_rxData[0x200]; /**< User spi reception buffer, 4bytes aligned */

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
    /* TI mode or MOTO mode configuration */
    spiConfig.format = SPI_FORMAT_MOTOLORA;
    /* TI mode no need config polarity */
    spiConfig.polarity = SPI_CLOCK_POLARITY_LOW;
    /* TI mode no need config phase */
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
#ifdef CHIP_GM6621
    /* Tx fifo threshold value, from 1 to 2 */
    spiTransferConfig.txWatermark = 2;
    /* Rx fifo threshold value, from 1 to 4 */
    spiTransferConfig.rxWatermark = 4;
#else
    /* Tx fifo threshold value, from 1 to 8 */
    spiTransferConfig.txWatermark = 8;
    /* Rx fifo threshold value, from 1 to 8 */
    spiTransferConfig.rxWatermark = 4;
#endif
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = TRUE;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = TRUE;

    SPI_TransferConfig(hSPI, &spiTransferConfig);

    //SPI_EnableInterrupts(hSPI, (SPI_RX_INT | SPI_TX_INT));
}

void spi_slave_config(SPI_Handle_T *hSPI)
{
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));

    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_SLAVE_MODE);
    /* Get spi default configuration */
    spiConfig.mode = SPI_SLAVE_MODE;
    /* TI mode or MOTO mode configuration */
    spiConfig.format = SPI_FORMAT_MOTOLORA;
    /* TI mode no need config polarity */
    spiConfig.polarity = SPI_CLOCK_POLARITY_LOW;
    /* TI mode no need config phase */
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
#if defined(CHIP_GM6621)
    /* Tx fifo threshold value, from 1 to 2 */
    spiTransferConfig.txWatermark = 2;
    /* Rx fifo threshold value, from 1 to 4 */
    spiTransferConfig.rxWatermark = 4;
#else
    /* Tx fifo threshold value, from 1 to 8 */
    spiTransferConfig.txWatermark = 8;
    /* Rx fifo threshold value, from 1 to 8 */
    spiTransferConfig.rxWatermark = 8;
#endif
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = TRUE;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = TRUE;

    SPI_TransferConfig(hSPI, &spiTransferConfig);

    //SPI_EnableInterrupts(hSPI, (SPI_RX_INT | SPI_TX_INT));
}

void SPIGetFlashId(SPI_Handle_T *hSPI)
{
    uint8_t ret = RET_ERROR;
    uint8_t flash_id[4];
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));

    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);
    SPI_Init(FLASH_SPI, &spiConfig, hSPI);
    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_DATA_FRAME | SPI_MASK_COMMAND_FRAME);
    /* Command frame width */
    spiTransferConfig.frameConfig.commandBits = 8;
    /* Data frame width */
    spiTransferConfig.frameConfig.dataBits = 24;
    /* Command frame value */
    spiTransferConfig.frameConfig.command = 0x9F;
    /* Transfer mode configuration, only can use tx_only, rx_only and full_duplex */
    spiTransferConfig.duplex = SPI_HALF_DUPLEX;
    /* Tx fifo threshold value, from 1 to 2 */
    spiTransferConfig.txWatermark = 2;
    /* Rx fifo threshold value, from 1 to 4 */
    spiTransferConfig.rxWatermark = 4;
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = FALSE;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = FALSE;

    SPI_TransferConfig(hSPI, &spiTransferConfig);

    ret = SPI_TransmitReceive(hSPI, NULL, flash_id, 0, 1, TIMEOUT_WAIT_FOREVER);
    /* wait for transfer over, avoid spi been configured in next transfer while previous transfer hasn't finished */
    while (ret != RET_OK);
    printf("\n** Flash Id is 0x%x%x%x **\n", flash_id[2], flash_id[1], flash_id[0]);
}

static void SPIFlashWaitFree(SPI_Handle_T *hSPI)
{
    uint8_t ret = RET_ERROR;
    uint8_t read_back = 0x01;
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));

    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);
    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_DATA_FRAME | SPI_MASK_COMMAND_FRAME);
    /* Command frame width */
    spiTransferConfig.frameConfig.commandBits = 8;
    /* Data frame width */
    spiTransferConfig.frameConfig.dataBits = 8;
    /* Command frame value */
    spiTransferConfig.frameConfig.command = 0x05;
    /* Transfer mode configuration, only can use tx_only, rx_only and full_duplex */
    spiTransferConfig.duplex = SPI_HALF_DUPLEX;
    /* Tx fifo threshold value, from 1 to 2 */
    spiTransferConfig.txWatermark = 2;
    /* Rx fifo threshold value, from 1 to 4 */
    spiTransferConfig.rxWatermark = 4;
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = FALSE;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = FALSE;

    SPI_TransferConfig(hSPI, &spiTransferConfig);

    while (read_back & 0x01)
    {
        ret = SPI_TransmitReceive(hSPI, NULL, &read_back, 0, 1, TIMEOUT_WAIT_FOREVER);
        /* wait for transfer over, avoid spi been configured in next transfer while previous transfer hasn't finished */
        while (ret != RET_OK);
    }
}

static void SPIFlashWriteEnable(SPI_Handle_T *hSPI, uint32_t en)
{
    uint8_t ret = RET_ERROR;
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));

    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);
    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_COMMAND_FRAME);
    /* Command frame width */
    spiTransferConfig.frameConfig.commandBits = 8;
    if (en)
    {
        /* Command frame value */
        spiTransferConfig.frameConfig.command = 0x06;
    }
    else
    {
        /* Command frame value */
        spiTransferConfig.frameConfig.command = 0x04;
    }
    /* Transfer mode configuration, only can use tx_only, rx_only and full_duplex */
    spiTransferConfig.duplex = SPI_TX_ONLY;
    /* Tx fifo threshold value, from 1 to 2 */
    spiTransferConfig.txWatermark = 2;
    /* Rx fifo threshold value, from 1 to 4 */
    spiTransferConfig.rxWatermark = 4;
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = FALSE;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = FALSE;

    SPI_TransferConfig(hSPI, &spiTransferConfig);

    ret = SPI_TransmitReceive(hSPI, NULL, NULL, 0, 0, TIMEOUT_WAIT_FOREVER);
    /* wait for transfer over, avoid spi been configured in next transfer while previous transfer hasn't finished */
    while (ret != RET_OK);
}

RET_CODE SPIFlashEraseSector(SPI_Handle_T *hSPI, uint32_t saddr)
{
    uint8_t ret = RET_ERROR;
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));

    /* Write Enable before any Write/Erase operation*/
    SPIFlashWriteEnable(hSPI, 1);

    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);
    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_ADDRESS_FRAME | SPI_MASK_COMMAND_FRAME);
    /* Command frame width */
    spiTransferConfig.frameConfig.commandBits = 8;
    /* Address frame width */
    spiTransferConfig.frameConfig.addressBits = 24;
    /* Command frame value */
    spiTransferConfig.frameConfig.command = 0xd8;
    /* Address frame value */
    spiTransferConfig.frameConfig.address = saddr;
    /* Transfer mode configuration, only can use tx_only, rx_only and full_duplex */
    spiTransferConfig.duplex = SPI_TX_ONLY;
    /* Tx fifo threshold value, from 1 to 2 */
    spiTransferConfig.txWatermark = 2;
    /* Rx fifo threshold value, from 1 to 4 */
    spiTransferConfig.rxWatermark = 4;
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = FALSE;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = FALSE;

    SPI_TransferConfig(hSPI, &spiTransferConfig);

    ret = SPI_TransmitReceive(hSPI, NULL, NULL, 0, 0, TIMEOUT_WAIT_FOREVER);
    /* wait for transfer over, avoid spi been configured in next transfer while previous transfer hasn't finished */
    while (ret != RET_OK);
    SPIFlashWaitFree(hSPI);
    return RET_OK;
}

RET_CODE SPIFlashWrite(SPI_Handle_T *hSPI, uint32_t addr, uint8_t *data, uint32_t size, uint8_t dma_en)
{
    uint8_t ret = RET_ERROR;
    uint32_t len;
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));

    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);

    SPI_Init(FLASH_SPI, &spiConfig, hSPI);
    while (size > 0)
    {
        /* PP start from any address in page and roll if increased address go beyond page boundary */
        len = (0x100 - (addr & 0xFF));
        len = (size >= len) ? len : size;
        /* Write Enable */
        SPIFlashWriteEnable(hSPI, 1);

        /* SPI transfers one fixed command frame and multiple data frames */
        spiTransferConfig.frameConfig.mask = (SPI_MASK_DATA_FRAME | SPI_MASK_COMMAND_FRAME | SPI_MASK_ADDRESS_FRAME);
        /* Command frame width */
        spiTransferConfig.frameConfig.commandBits = 8;
        /* Address frame width */
        spiTransferConfig.frameConfig.addressBits = 24;
        /* Data frame width */
        spiTransferConfig.frameConfig.dataBits = 8;
        /* Command frame value */
        spiTransferConfig.frameConfig.command = 0x02;
        /* Address frame value */
        spiTransferConfig.frameConfig.address = addr;

        /* Transfer mode configuration, only can use tx_only, rx_only and full_duplex */
        spiTransferConfig.duplex = SPI_TX_ONLY;
        /* Tx fifo threshold value, from 1 to 2 */
        spiTransferConfig.txWatermark = 2;
        /* Rx fifo threshold value, from 1 to 4 */
        spiTransferConfig.rxWatermark = 4;
        /* Tx use DMA enable */
        spiTransferConfig.enableRxDMA = dma_en;
        /* Rx use DMA enable */
        spiTransferConfig.enableTxDMA = dma_en;

        SPI_TransferConfig(hSPI, &spiTransferConfig);

        ret = SPI_TransmitReceive(hSPI, &data[0], NULL, len, 0, TIMEOUT_WAIT_FOREVER);
        while (ret != RET_OK);
        /* wait for transfer over, avoid spi been configured in next transfer while previous transfer hasn't finished */
        SPIFlashWaitFree(hSPI);

        /* Go to next page */
        addr += len;
        size -= len;
        data += len;
    }
    return RET_OK;
}

RET_CODE SPIFlashRead(SPI_Handle_T *hSPI, uint32_t addr, uint8_t *data, uint32_t len, uint8_t dma_en)
{
    uint8_t ret = RET_ERROR;
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));
    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);

    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_DATA_FRAME | SPI_MASK_COMMAND_FRAME | SPI_MASK_ADDRESS_FRAME);
    /* Command frame width */
    spiTransferConfig.frameConfig.commandBits = 8;
    /* Address frame width */
    spiTransferConfig.frameConfig.addressBits = 24;
    /* Data frame width */
    spiTransferConfig.frameConfig.dataBits = 8;
    /* Command frame value */
    spiTransferConfig.frameConfig.command = 0x03;
    /* Address frame value */
    spiTransferConfig.frameConfig.address = addr;
    /* Transfer mode configuration, only can use tx_only, rx_only and full_duplex */
    spiTransferConfig.duplex = SPI_HALF_DUPLEX;
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = dma_en;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = dma_en;
    SPI_TransferConfig(hSPI, &spiTransferConfig);

    ret = SPI_TransmitReceive(hSPI, NULL, &data[0], 0, len, TIMEOUT_WAIT_FOREVER);
    /* wait for transfer over, avoid spi been configured in next transfer while previous transfer hasn't finished */
    while (ret != RET_OK);

    SPIFlashWaitFree(hSPI);
    return RET_OK;
}

RET_CODE SPIFlashReadFast(SPI_Handle_T *hSPI, uint32_t addr, uint8_t *data, uint32_t len, uint8_t dma_en)
{
    uint8_t ret = RET_ERROR;
    SPI_Config_T spiConfig;
    SPI_TransferConfig_T spiTransferConfig;
    memset(&spiConfig, 0, sizeof(SPI_Config_T));
    memset(&spiTransferConfig, 0, sizeof(SPI_TransferConfig_T));
    /* Get spi default configuration */
    SPI_GetDefaultConfig(&spiConfig, &spiTransferConfig, SPI_MASTER_MODE);

    /* SPI transfers one fixed command frame and multiple data frames */
    spiTransferConfig.frameConfig.mask = (SPI_MASK_DATA_FRAME | SPI_MASK_COMMAND_FRAME | SPI_MASK_ADDRESS_FRAME | SPI_MASK_DUMMY_FRAME);
    /* Command frame width */
    spiTransferConfig.frameConfig.commandBits = 8;
    /* Address frame width */
    spiTransferConfig.frameConfig.addressBits = 24;
    /* Data frame width */
    spiTransferConfig.frameConfig.dataBits = 8;
    /* Dummy frame width */
    spiTransferConfig.frameConfig.dummyBits = 8;
    /* Command frame value */
    spiTransferConfig.frameConfig.command = 0x0B;
    /* Address frame value */
    spiTransferConfig.frameConfig.address = addr;
    /* Dummy frame value */
    spiTransferConfig.frameConfig.dummy = 0x00;
    /* Transfer mode configuration, only can use tx_only, rx_only and full_duplex */
    spiTransferConfig.duplex = SPI_HALF_DUPLEX;
    /* Tx use DMA enable */
    spiTransferConfig.enableRxDMA = dma_en;
    /* Rx use DMA enable */
    spiTransferConfig.enableTxDMA = dma_en;
    SPI_TransferConfig(hSPI, &spiTransferConfig);

    ret = SPI_TransmitReceive(hSPI, NULL, data, 0, len, TIMEOUT_WAIT_FOREVER);
    /* wait for transfer over, avoid spi been configured in next transfer while previous transfer hasn't finished */
    while (ret != RET_OK);

    SPIFlashWaitFree(hSPI);
    return RET_OK;
}

int main(void)
{
    uint32_t i;
    uint32_t flash_addr = 0x100;//0x9e8
    SPI_Handle_T *spiHandle;

    /* Initialize uart device for printf port */
    DebugConsoleInit(115200);
    /* Init system timer */
    SysTick_Init();

    printf("Sample code of spi driver \n");

    /* Pinmux setting */
#if SPI_TRANSFER
    SPIPinMuxConfig(TRANSFER_SPI);
#endif
#if SPI_FLASH
    SPIPinMuxConfig(FLASH_SPI);
#endif
    /* Enable global interrupt handling */
    enable_interrupt;

#if defined(CHIP_F6621B)
    /* clock enable and reset */
    SPI1_RESET();
    SPI1_CLOCK_ENABLE();
#endif

    spiHandle = &g_spiHandle;
#if SPI_TRANSFER
    spi_master_config(spiHandle);
    //spi_slave_config(spiHandle);
#endif
#if SPI_FLASH
    SPIGetFlashId(spiHandle);
#endif
    /* Transmitted data is here */
    for (i = 0; i < 32; i++)
    {
        s_txData[i] = i + 16;
    }

    while (1)
    {
#if SPI_TRANSFER
        /* full duplex transfer in TIMEOUT_NO_BLOCKING mode */
        SPI_TransmitReceive(spiHandle, (uint8_t *)&s_txData[0], (uint8_t *)&s_rxData[0], 32, 32, TIMEOUT_NO_BLOCKING);
        while (spiHandle->status != SPI_STATUS_IDLE);

        /* full duplex transfer in TIMEOUT_WAIT_FOREVER mode */
        //SPI_TransmitReceive(spiHandle, &s_txData[0], &s_rxData[0], 32, 32, TIMEOUT_WAIT_FOREVER);

        /* tx only transfer */
        //SPI_Transmit(spiHandle, &s_txData[0], 32, TIMEOUT_WAIT_FOREVER);
        /* rx only transfer */
        //SPI_Receive(spiHandle, &s_rxData[0], 32, TIMEOUT_WAIT_FOREVER);

        /* half duplex transfer in TIMEOUT_WAIT_FOREVER mode */
        //SPI_TransmitReceive(spiHandle, &s_txData[0], &s_rxData[0], 0, 1, TIMEOUT_WAIT_FOREVER);
#endif

#if SPI_FLASH
        /* flash erase, write and read */
        SPIFlashEraseSector(spiHandle, flash_addr);
        SPIFlashWrite(spiHandle, flash_addr, (uint8_t *)&s_txData[0], 32, 1);
        SPIFlashRead(spiHandle, flash_addr, (uint8_t *)&s_rxData[0], 32, 1);
        SPIFlashReadFast(spiHandle, flash_addr, (uint8_t *)&s_rxData[0], 32, 1);
#endif
        for (i = 0; i < 32; i++)
        {
            if (s_txData[i] != s_rxData[i])
            {
                printf("receive error!");
            }
        }
        printf("ok!");
        SysTick_DelayMS(1000);
    };
}
