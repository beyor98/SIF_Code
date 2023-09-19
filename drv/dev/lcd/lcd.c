/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          lcd.c
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 *
 * @date          2016/08/22
 *
 * @brief        LCD APIs for application.
 *
 * @note
 *    2016/08/22, mark.lee, V1.0.0
 *        Initial version.
 */

#include <stdio.h>
#include <drv/dev/lcd.h>

/*******************************************************************************
* Constants and macros
*******************************************************************************/
#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
    #define MAX_LCD_RAM_NUM  13
#elif (defined(CHIP_GM6621))
    #define MAX_LCD_RAM_NUM  10
#endif

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
void LCD_Init(const LCD_Config_T *pConfig)
{
    /* sets LCD Clock prescaler */
    IO_BITMASK_SET(LCD->CR, LCD_CR_PSC_MASK, LCD_CR_PSC_FIELD(pConfig->prescaler));

    /* LCD Com slection */
    IO_BITMASK_SET(LCD->CR, LCD_CR_DUTY_MASK, LCD_CR_DUTY_FIELD(pConfig->duty));

    /* LCD Bias slection */
    IO_BITMASK_SET(LCD->CR, LCD_CR_BIAS_MASK, LCD_CR_BIAS_FIELD(pConfig->bias));

    /* sets LCD contrast */
    IO_BITMASK_SET(LCD->CR, LCD_CR_CONTRAST_MASK, LCD_CR_CONTRAST_FIELD(pConfig->contrast));

    /* Voltage source slection */
    if (pConfig->voltageSource == LCD_VoltageSource_External)
    {
        IO_BIT_SET(LCD->CR, LCD_CR_VSEL);
    }
    else
    {
        IO_BIT_CLEAR(LCD->CR, LCD_CR_VSEL);
    }

    /* sets LCD display mode */
    if (pConfig->displayMode == LCD_Display_Field)
    {
        IO_BIT_SET(LCD->CR, LCD_CR_DISMODE);
    }
    else
    {
        IO_BIT_CLEAR(LCD->CR, LCD_CR_DISMODE);
    }

    /* sets LCD Divider Network */
#if (defined(CHIP_GM6621))
    IO_BITMASK_SET(LCD->RS, 0x7f, 0x8);
#endif
}

void LCD_StartDisplay(void)
{
    IO_BIT_SET(LCD->CR, LCD_CR_LCDEN);
}

void LCD_StopDisplay(void)
{
    IO_BIT_CLEAR(LCD->CR, LCD_CR_LCDEN);
}

void LCD_EnablePin(uint8_t index, uint32_t data)
{
    if (index > 1)
    {
        return;
    }

    LCD->PER[index] |= data;
}

void LCD_DisablePin(uint8_t index, uint32_t data)
{
    if (index > 1)
    {
        return;
    }

    LCD->PER[index] &= ~data;
}

void LCD_SelectComPin(LCD_COM_T com, uint8_t pin)
{
    switch (com)
    {
        case LCD_COM_0:
            /* LCD com0 pin slection */
            IO_BITMASK_SET(LCD->CSR[0], LCD_CSR_COM0_SEL_MASK, LCD_CSR_COM0_SEL_FIELD(pin));
            break;
        case LCD_COM_1:
            /* LCD com1 pin slection */
            IO_BITMASK_SET(LCD->CSR[0], LCD_CSR_COM1_SEL_MASK, LCD_CSR_COM1_SEL_FIELD(pin));
            break;
        case LCD_COM_2:
            /* LCD com2 pin slection */
            IO_BITMASK_SET(LCD->CSR[0], LCD_CSR_COM2_SEL_MASK, LCD_CSR_COM2_SEL_FIELD(pin));
            break;
        case LCD_COM_3:
            /* LCD com3 pin slection */
            IO_BITMASK_SET(LCD->CSR[0], LCD_CSR_COM3_SEL_MASK, LCD_CSR_COM3_SEL_FIELD(pin));
            break;
        case LCD_COM_4:
            /* LCD com4 pin slection */
            IO_BITMASK_SET(LCD->CSR[1], LCD_CSR_COM0_SEL_MASK, LCD_CSR_COM0_SEL_FIELD(pin));
            break;
        case LCD_COM_5:
            /* LCD com5 pin slection */
            IO_BITMASK_SET(LCD->CSR[1], LCD_CSR_COM1_SEL_MASK, LCD_CSR_COM1_SEL_FIELD(pin));
            break;
        case LCD_COM_6:
            /* LCD com6 pin slection */
            IO_BITMASK_SET(LCD->CSR[1], LCD_CSR_COM2_SEL_MASK, LCD_CSR_COM2_SEL_FIELD(pin));
            break;
        case LCD_COM_7:
            /* LCD com7 pin slection */
            IO_BITMASK_SET(LCD->CSR[1], LCD_CSR_COM3_SEL_MASK, LCD_CSR_COM3_SEL_FIELD(pin));
            break;
        default:
            break;
    }
}

void LCD_Write(uint32_t LCD_RAMRegisterIndex, uint32_t RAMRegisterMask, uint32_t data)
{
    if (LCD_RAMRegisterIndex >= MAX_LCD_RAM_NUM)
    {
        return;
    }

    IO_BITMASK_SET(LCD->DMR[LCD_RAMRegisterIndex], RAMRegisterMask, data);
}

void LCD_ClearDisplay(void)
{
    uint8_t i = 0;

    for (i = 0; i < MAX_LCD_RAM_NUM; i++)
    {
        LCD->DMR[i] = 0;
    }
}

#if (defined(CHIP_GM6621))
void LCD_BlinkConfig(uint8_t onOff, uint8_t blinkFrequency)
{
    if (onOff)
    {
        IO_BIT_SET(LCD->CR, LCD_CR_BLINKEN);
        IO_BITMASK_SET(LCD->CR, LCD_CR_BLINKRATE_MASK, LCD_CR_BLINKRATE_FIELD(blinkFrequency));
    }
    else
    {
        IO_BIT_CLEAR(LCD->CR, LCD_CR_BLINKEN);
    }
}

void LCD_RefreshConfig(uint8_t onOff, uint8_t loopCnt, uint32_t addr)
{
    if (onOff)
    {
        IO_BIT_SET(LCD->REFRESH, LCD_REFRESH_EN);
        IO_BITMASK_SET(LCD->REFRESH, 0xFF00, ((loopCnt << 8) & 0xFF00));
        IO_BITMASK_SET(LCD->REFRESH, 0xFFFF000, ((addr & 0x0000FFFF) << 16));
        IO_BIT_SET(AOPDEV->PMC, (0x1 << 6));
    }
    else
    {
        IO_BIT_CLEAR(LCD->REFRESH, LCD_REFRESH_EN);
        IO_BIT_CLEAR(AOPDEV->PMC, (0x1 << 6));
    }
}

RET_CODE LCD_ChargePumpConfig(uint8_t onOff, uint8_t prescaler, uint8_t vol)
{
    if (onOff)
    {
        if (IO_BITS_GET(LCD->CR, 0x00000100U) && (vol == 1))
        {
            return RET_ERROR;
        }

        IO_BITMASK_SET(LCD->CR, 0x0E000000, ((prescaler << 25) & 0x0E000000));
        IO_BITMASK_SET(LCD->CR, 0x00000008, ((vol << 3) & 0x00000008));
        IO_BITS_SET(LCD->CR, LCD_CR_CPBUFEN | LCD_CR_CPEN);
        IO_BITMASK_SET(LCD->RS, 0x7f, 0x0);
    }
    else
    {
        IO_BITS_CLEAR(LCD->CR, LCD_CR_CPBUFEN | LCD_CR_CPEN);
        IO_BITMASK_SET(LCD->RS, 0x7f, 0x8);
    }

    return RET_OK;
}

#endif

