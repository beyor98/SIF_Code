/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          kbi.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2017/03/24 
 *    
 * @brief        KBI APIs for application. 
 * 
 * @note
 *    2016/03/24, mark.lee, V1.0.0
 *        Initial version. 
 */
 
#include <drv/dev/kbi.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#define KBI_NUMBER           ((uint8_t)8)

/*******************************************************************************
* Global variables
*******************************************************************************/


/*******************************************************************************
* Static variables
*******************************************************************************/


/*******************************************************************************
* Reference variables
*******************************************************************************/


/*******************************************************************************
* Local function prototypes
*******************************************************************************/

/*******************************************************************************
* Codes
*******************************************************************************/
RET_CODE KBI_Init(KBI_Device_T *pKBI, KBI_PinConfig_T *pConfig)
{
    uint8_t position;
    uint8_t ioPosition = 0x00;
    uint8_t ioCurrent = 0x00;

    if (!IS_KBI_PIN(pConfig->pin))
    {
        return RET_ERROR;
    }
    
    /* Configure the port pins */
    for (position = 0; position < KBI_NUMBER; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint8_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint8_t)(pConfig->pin) & ioPosition;

        if (ioCurrent == ioPosition)
        {
            if (pConfig->mode == KBI_MODE_IT_RISING)
            {
                pKBI->ES |= ioCurrent;
            }
            else
            {
                pKBI->ES &= ~ioCurrent;
            }

            pKBI->PE |= ioCurrent;
			
            NVIC_EnableIRQ(KBI_IRQn);
        }
    } 

    return RET_OK;
}

RET_CODE KBI_DeInit(KBI_Device_T *pKBI, uint8_t pin)
{
    uint8_t position;
    uint8_t ioPosition = 0x00;
    uint8_t ioCurrent = 0x00;

    if (!IS_KBI_PIN(pin))
    {
        return RET_ERROR;
    }
    
    /* Configure the port pins */
    for (position = 0; position < KBI_NUMBER; position++)
    {
        /* Get the IO position */
        ioPosition = ((uint32_t)0x01) << position;
        /* Get the current IO position */
        ioCurrent = (uint32_t)(pin) & ioPosition;

        if (ioCurrent == ioPosition)
        { 
            pKBI->PE &= ~ioCurrent;
        }
    } 

    return RET_OK;
}

