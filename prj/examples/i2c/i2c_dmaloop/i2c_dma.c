
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
 *	Date               By             Comments          Ver.
 *	=========		=========		 =========        =========
 *	2016/07/19	   Wizephen.Wang       Create           1.0.0
 **********************************************************************/
#include <common.h>
#include "stdio.h"
#include <drv/dev/uart.h>
#include <drv/dev/i2c.h>

/******************************************************************************
* Constants and macros
******************************************************************************/

#define I2C_MASTER_DEVICE       (I2C0)
#define I2C_SLAVE_DEVICE        (I2C1)

#define I2C_LOCAL_ADDRESS       (0x56)
#define I2C_TARGET_ADDRESS      I2C_LOCAL_ADDRESS

#define I2C_MAX_DATALEN         16
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

/******************************************************************************
* Global variables
******************************************************************************/
I2C_Handle_T g_i2cMstHandle;
I2C_Handle_T g_i2cSlvHandle;

static volatile uint32_t s_mstTransCmplt = 0;
static volatile uint32_t s_slvTransCmplt = 0;

static uint8_t s_wrBuf[I2C_MAX_DATALEN] = {0};
static uint8_t s_rdBuf[I2C_MAX_DATALEN] = {0};

static uint16_t s_dmabuffer[32] = {0};
static uint32_t s_dmaBufferLen  = I2C_MAX_DATALEN;

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
int main (void);
/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/
void i2cSlvCallback(struct I2C_Handle *hI2C, uint8_t event, unsigned long param)
{
    if(event == I2C_EVENT_TRANSFER_DONE)
    {
        s_slvTransCmplt = 1;
    }

    if(event == I2C_EVENT_TX_FIFO_OVERFLOW)
    {

    }

    if(event == I2C_EVENT_RX_FIFO_OVERFLOW)
    {

    }

    if(event == I2C_EVENT_RX_FIFO_UNDERFLOW)
    {

    }

    if(event == I2C_EVENT_ARB_LOST)
    {

    }

    if(event == I2C_EVENT_TERMINATED)
    {

    }

    if(event == I2C_EVENT_DMAERROR)
    {

    }

    if(event == I2C_EVNET_TRANSFER_ERROR)
    {

    }
}

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
#if (defined(CHIP_GM6601) || defined(CHIP_GM6601B) || defined(CHIP_F6601C))
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

int main (void)
{
    uint8_t i = 0;

    I2C_Config_T i2cMstConfig;
    I2C_Config_T i2cSlvConfig;
    RET_CODE retval;
    SysTick_Init();
    SysTick_DelayMS(1000);

    /* Pin Multiplex for I2C1, I2C2 */
    I2CX_PinMuxConfig(I2C0);
    I2CX_PinMuxConfig(I2C1);
	//WRITE_REG(*(volatile uint32_t *) 0x40000134, 0xC00);//pullup i2c0,when not connect to eeprom
    SysTick_Init();

    /* Configure I2C master */
    i2cMstConfig.addrMode     = I2C_7BIT_MODE;
    i2cMstConfig.callback     = NULL;
    i2cMstConfig.masterTxMode = I2C_TARGET_ADDRESS_MODE;
    i2cMstConfig.speedMode    = I2C_STANDARD_MODE;
    i2cMstConfig.baudrate     = 100;  /*in Kbps unit*/
    i2cMstConfig.workMode     = I2C_MASTER_MODE;
	//i2cMstConfig.enableDMA    = FALSE;
    i2cMstConfig.enableDMA    = TRUE;
    /*Ignore the right following two configurations, if DMA is not active*/
    i2cMstConfig.pdmaBuffer      = s_dmabuffer;
    i2cMstConfig.dmaBufferLength = s_dmaBufferLen;

    /* Initiailize I2C perihperal as master */
    if (I2C_Init(I2C_MASTER_DEVICE, &i2cMstConfig, &g_i2cMstHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    /*
     * Configure I2C slave, DMA is not available for slave now,
     * since it's already been used by I2C master
     */
    i2cSlvConfig.addrMode     = I2C_7BIT_MODE;
    i2cSlvConfig.callback     = i2cSlvCallback;
    i2cSlvConfig.masterTxMode = I2C_TARGET_ADDRESS_MODE;
    i2cSlvConfig.speedMode    = I2C_STANDARD_MODE;
    i2cSlvConfig.workMode     = I2C_SLAVE_MODE;
    /* The slave own address */
    i2cSlvConfig.localAddress = I2C_LOCAL_ADDRESS;

    /* Initiailize I2C perihperal as slave */
    if (I2C_Init(I2C_SLAVE_DEVICE, &i2cSlvConfig, &g_i2cSlvHandle) != RET_OK)
    {
        return RET_ERROR;
    }

    for (i = 0;i < I2C_MAX_DATALEN; i ++)
    {
        s_wrBuf[i] = i + 1;
        s_rdBuf[i] = 0;
    }

    for (i = 0;i < sizeof(s_wrBuf)/sizeof(uint8_t); i ++)
     {
         s_wrBuf[i] = i + 1;
         s_rdBuf[i] = 0;
     }

     /* I2C slave receives data through interrupt mode */
     retval = I2C_SlaveReceive(&g_i2cSlvHandle, s_rdBuf, sizeof(s_rdBuf)/sizeof(uint8_t), TIMEOUT_NO_BLOCKING);
     if(retval != RET_OK)
     {
         return RET_ERROR;
     }

     /* I2C mastre sends data through polling mode, DMA involves in data-transaction process */
     retval = I2C_MasterTransmit(&g_i2cMstHandle, I2C_TARGET_ADDRESS, s_wrBuf, sizeof(s_wrBuf)/sizeof(uint8_t), 1000000);
     if(retval != RET_OK)
     {
         return RET_ERROR;
     }

     /* Wait till data transaction completes */
     while(s_slvTransCmplt == 0) {};

     s_slvTransCmplt = 0;

     for(i = 0;i < sizeof(s_wrBuf)/sizeof(uint8_t); i ++)
     {
         if(s_rdBuf[i] != s_wrBuf[i])
         {
             return RET_ERROR;
         }
     }

     if (I2C_DeInit(&g_i2cMstHandle) != RET_OK)
     {
         return RET_ERROR;
     }

     if (I2C_DeInit(&g_i2cSlvHandle) != RET_OK)
     {
         return RET_ERROR;
     }

    while (1) {};
}
