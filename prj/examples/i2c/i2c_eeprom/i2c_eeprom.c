
/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file i2c_loop_example.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/19
 *
 * @contents Verification and example code of I2C loop.
 *
 * @history
 *  Date               By             Comments          Ver.
 *  =========       =========        =========        =========
 *  2016/07/19     Wizephen.Wang       Create           1.0.0
 **********************************************************************/
#include <common.h>
#include "stdio.h"
#include <drv/dev/uart.h>
#include <drv/dev/i2c.h>

/******************************************************************************
* Constants and macros
******************************************************************************/
#define I2C_MASTER_DEVICE       (I2C0)

#define I2C_MAX_DATALEN         17
#define EEPROM_DEVICE_CODE      0x05
/* Chip Enable (E2:E0 = 2'b 000) */
#define EEPROM_DEVICE_SELECT    0x00
/* EEPROM 24C02 Device Address */
#define EEPROM_DEVICE_ADDR      (EEPROM_DEVICE_CODE << 4 | EEPROM_DEVICE_SELECT)

/******************************************************************************
* Global variables
******************************************************************************/
I2C_Handle_T g_i2cMstHandle;
uint8_t g_rdbuf[I2C_MAX_DATALEN];

/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/

/******************************************************************************
* Local functions
******************************************************************************/
int main(void);
/******************************************************************************
* Global functions
******************************************************************************/

/** Configure GPIO for an available I2C device */
static void I2CX_PinMuxConfig(I2C_Device_T *pI2C)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    memset(&GPIO_InitStruct, 0, sizeof(GPIO_PinConfig_T));
    if (pI2C == I2C0)
    {
        /** GPIO Configuration for I2C0*/
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))
        GPIO_InitStruct.pin = GPIO_PIN_15;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOA15_AF0_I2C0SDA;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        GPIO_InitStruct.pin = GPIO_PIN_11;
        //GPIO_InitStruct.pin = GPIO_PIN_13;

        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOC11_AF0_I2C0SDA;//GPIOC13_AF1_I2C0SDA;
        //GPIO_InitStruct.alternate = GPIOC13_AF1_I2C0SDA;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6621)
        GPIO_InitStruct.pin = GPIO_PIN_3;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOD3_AF0_I2C0SDA;
        GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))
        GPIO_InitStruct.pin = GPIO_PIN_0;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOB0_AF0_I2C0SCL;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        GPIO_InitStruct.pin = GPIO_PIN_10;
        //GPIO_InitStruct.pin = GPIO_PIN_12;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOC10_AF0_I2C0SCL;
        //GPIO_InitStruct.alternate = GPIOC12_AF1_I2C0SCL;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6621)
        GPIO_InitStruct.pin = GPIO_PIN_2;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOD2_AF0_I2C0SCL;
        GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
    }

    if (pI2C == I2C1)
    {
        /** GPIO Configuration for I2C1*/
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C) )
        GPIO_InitStruct.pin = GPIO_PIN_10;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOA10_AF0_I2C1SDA;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        GPIO_InitStruct.pin = GPIO_PIN_6;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOD6_AF0_I2C1SDA;
        GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6621)
        GPIO_InitStruct.pin = GPIO_PIN_8;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOB8_AF0_I2C1SDA;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
#if (defined(CHIP_GM6601)  || defined(CHIP_GM6601B) || defined(CHIP_F6601C))
        GPIO_InitStruct.pin = GPIO_PIN_11;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOA11_AF0_I2C1SCL;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        GPIO_InitStruct.pin = GPIO_PIN_5;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOD5_AF0_I2C1SCL;
        GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6621)
        GPIO_InitStruct.pin = GPIO_PIN_7;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOB7_AF0_I2C1SCL;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
    }

    if (pI2C == I2C2)
    {
        /** GPIO Configuration for I2C2*/
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) )
        GPIO_InitStruct.pin = GPIO_PIN_4;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOD4_AF0_I2C2SDA;
        GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        GPIO_InitStruct.pin = GPIO_PIN_2;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOB2_AF1_I2C2SDA;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif

#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) )
        GPIO_InitStruct.pin = GPIO_PIN_5;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOD5_AF0_I2C2SCL;
        GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#if defined(CHIP_GM6611) || defined(CHIP_GM6611B)
        GPIO_InitStruct.pin = GPIO_PIN_1;
        GPIO_InitStruct.mode = GPIO_MODE_AF;
        GPIO_InitStruct.pull = GPIO_PULLUP;
        GPIO_InitStruct.alternate = GPIOB1_AF1_I2C2SCL;
        GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
    }
}

/** Write one byte data to EEPROM-24C02 */
static RET_CODE Write_DataToEeprom(I2C_Handle_T *hI2C, uint16_t devAddr, uint8_t memAddr,
                                   uint8_t data, int32_t timeout)
{
    RET_CODE error = RET_OK;
    uint8_t buf[2];

    buf[0] = memAddr;
    buf[1] = data;

    /*
     * Based on 24C02 I2C on BYTE WRITE protocol.
     * The memory address here is treated as only
     * one byte of data.
     */
    error = I2C_MasterTransmit(hI2C, devAddr, buf, 2, timeout);
    return error;
}

/** Read data out from EEPROM-24C02 */
static RET_CODE Read_DatafromEeprom(I2C_Handle_T *hI2C, uint16_t devAddr, uint8_t memAddr,
                                    uint8_t *pData, uint16_t size, int32_t timeout)
{
    RET_CODE error = RET_OK;

    /* Based on 24C02 I2C on SEQUENTIAL RANDOM READ protocol */
    error = I2C_MasterTransmit(hI2C, devAddr, &memAddr, 1, 1000);
    if (error != RET_OK)
    {
        return error;
    }
    SysTick_DelayMS(5);
    /* Read expected bytes of data out */
    error = I2C_MasterReceive(hI2C, devAddr, pData, size, 1000);
    return error;
}

int main(void)
{
    I2C_Config_T i2cMstConfig;
    uint8_t i = 0;
    uint8_t wrbuf[I2C_MAX_DATALEN];

    /* Pin Multiplex for I2C1 */
    I2CX_PinMuxConfig(I2C0);
    //I2CX_PinMuxConfig(I2C2);

    SysTick_Init();

    /* Configure I2C master for I2C1 */
    i2cMstConfig.addrMode     = I2C_7BIT_MODE;
    i2cMstConfig.callback     = NULL;
    i2cMstConfig.masterTxMode = I2C_TARGET_ADDRESS_MODE;
    i2cMstConfig.speedMode    = I2C_STANDARD_MODE;
    i2cMstConfig.baudrate     = 100;  /*in Kbps unit*/
    i2cMstConfig.workMode     = I2C_MASTER_MODE;
    i2cMstConfig.enableDMA    = FALSE;

    /* Initiailize I2C perihperal as master */
    if (I2C_Init(I2C_MASTER_DEVICE, &i2cMstConfig, &g_i2cMstHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    for (i = 0; i < I2C_MAX_DATALEN; i++)
    {
        wrbuf[i]  = 0xAA + i;

        /* Write data while memory address is increasing */
        if (Write_DataToEeprom(&g_i2cMstHandle, EEPROM_DEVICE_ADDR, i, wrbuf[i], 1000000) != RET_OK)
        {
            return RET_ERROR;
        }
        SysTick_DelayMS(5);
    }

    if (Read_DatafromEeprom(&g_i2cMstHandle, EEPROM_DEVICE_ADDR, 0, g_rdbuf, I2C_MAX_DATALEN, 1000000) != RET_OK)
    {
        return RET_ERROR;
    }

    for (i = 0; i < I2C_MAX_DATALEN; i++)
    {
        if (wrbuf[i] != g_rdbuf[i])
        {
            return RET_ERROR;
        }
    }

    while (1) {};
}
