/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file          acmp.c
*
* @author        Mike.Zheng
*
* @version       1.1.0
*
* @date          2016/07/05
*
* @brief         Header file of wdog driver.
*
* @note
*    2016/03/01, Mike.Zheng, V1.0.0
*        Initial version.
*    2016/07/05, Mike.Zheng, V1.1.0
*        Update coding style.
*    2016/11/22, Mike.Zheng, V1.2.0
*        Update For Release.
*/

#include <drv/dev/acmp.h>
#if defined(CHIP_GM6621)
    #include <drv/dev/rtc.h>
    #include <drv/dev/pes.h>
#endif

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do {} while(0)
#else
    #define PR_DEBUG            printf
#endif
#if defined(CHIP_GM6621)
/*#define BVD_CHARGE_FUNCTION   1 */
#define BVD_PRE_VAL_ADDRESS        (0x20000000 + 0x1010)
#define BVD_CMP_CNT_ADDRESS        (0x20000000 + 0x1018)
#define BVD_CNT_ADDRESS            (0x20000000 + 0x101C)
#define BVD_CNT                          (5)
#define BVD_CMP_CNT                      (5)
#define BIT_CLEAR_3       0x3
#define BIT_set_2         0x2
#define BIT_set_1         0x1
#define ACMP_32KHZ_LIRC   (30)
#define RTC_32KHZ         (6)
#define ACMP_RTC_32KHZ_LIRC (3)
struct acmp_save_data
{
    unsigned int bvd_pre_val;
    unsigned int bvd_cmp_cnt;
    unsigned int bvd_cnt;
};

typedef struct
{
    unsigned int bvd_pre_val_addr;
    unsigned int bvd_cmp_cnt_addr;
    unsigned int bvd_cnt_addr;
} ACMP_Save_Addr_ParamT;

void ACMP_BV_Config(uint32_t bvFilterThr, uint8_t levelnum, uint8_t *pIputDavLevelValue);
#endif
static void ACMP_IrqHandler(void);
#if !defined(CHIP_GM6621)
    static void ACMP_InterruptConfig(ACMP_Handle_T *pHandle, ACMP_Config_T *pConfig);
#endif
STATIC_ISR(ACMP_IRQn, ACMP_IrqHandler);

static ACMP_Handle_T *s_pHandle = NULL;
ACMP_Callback_T s_acmpCallback = NULL;

/*AC0_NSEL VREFB(0.8V);DAC reference voltage source is VCC = {2.56v,2.438v,2.327v,2.226v} ;code =   64 * (0.8/vcc)  =  51.2/vcc */

uint8_t sbvsetLevelValue0[4] = {20, 21, 22, 23}; /*0XF*/
uint8_t sbvsetLevelValue1[4] = {0, 21, 22, 23}; /*0XE*/
uint8_t sbvsetLevelValue2[4] = {0, 0, 22, 23}; /*0XC*/
uint8_t sbvsetLevelValue3[4] = {0, 0, 0, 23}; /*0X8*/
uint8_t sbvsetLevelValue4[4] = {0, 0, 0, 0}; /*0X0*/

#if defined(CHIP_GM6621)
uint32_t  lcd_display_bvd_value = 0xF;
int ACMP_battery_process(ACMP_AppMode_T appmode, ACMP_Save_Addr_ParamT addr)
{
    ACMP_Handle_T *pHandle = s_pHandle;
    volatile uint32_t irqAC0Status = pHandle->device->AC0CS;
    volatile uint32_t irqAC0En = pHandle->device->AC0CS;
    volatile uint32_t irqAC1Status = pHandle->device->AC1CS;
    volatile uint32_t irqAC1En = pHandle->device->AC1CS;
    volatile uint32_t irqBvEnStatus = pHandle->device->BVCS;
    volatile uint32_t irqBvEn = pHandle->device->BVCS;
    volatile unsigned int bvd_cfg_level = pHandle->device->BVL0;
    static uint32_t bvd_value = 0;
    volatile uint32_t bvd_pre_value = 0;
    volatile uint32_t  bvd_value_min = 0;
    static int bvd_cmp = 0;
    static int bvd_cmp_tmp = 0;
    static int  bvd_cnt = 0;
    static int  bvd_cnt_tmp = 0;
    static struct acmp_save_data bvd_save = {20, 0, 0};

    uint8_t bvd_cfg_val[4] = {0};
    bvd_cfg_val[0] = bvd_cfg_level & 0xFF;
    bvd_cfg_val[1] = (bvd_cfg_level & 0xFF00) >> 8;
    bvd_cfg_val[2] = (bvd_cfg_level & 0xFF0000) >> 16;;
    bvd_cfg_val[3] = (bvd_cfg_level & 0xFF000000) >> 24;

    if (appmode == APP_STANDBY_MODE)
    {
        bvd_pre_value = *((unsigned int volatile *)(addr.bvd_pre_val_addr));
    }
    else
    {
        bvd_pre_value = bvd_save.bvd_pre_val;
    }

    bvd_value = ACMP_GET_BV_SAMPLE_VALUE(pHandle->device);

    if (appmode == APP_STANDBY_MODE)
    {
        bvd_cnt_tmp = *((unsigned int volatile *)(addr.bvd_cnt_addr));
    }
    else
    {
        bvd_cnt_tmp = bvd_save.bvd_cnt;
    }
    if ((bvd_cnt_tmp >= 1) && (bvd_cnt_tmp < 8))
    {
        bvd_cnt = bvd_cnt_tmp;
    }

    if (appmode == APP_STANDBY_MODE)
    {
        bvd_cmp_tmp = *((unsigned int volatile *)(addr.bvd_cmp_cnt_addr));
    }
    else
    {
        bvd_cmp_tmp = bvd_save.bvd_cmp_cnt;
    }

    if ((bvd_cmp_tmp >= 1) && (bvd_cmp_tmp < 8))
    {
        bvd_cmp = bvd_cmp_tmp;
    }

    if (bvd_value != bvd_pre_value)
    {
        PR_DEBUG("enter value change\n");
        bvd_cmp++;
    }
    else
    {
        bvd_cnt++;
    }

    PR_DEBUG("read bvd_pre_value:0x%x, bvd_cnt = %d, bvd_cmp=%d, bvd_cfg_level=0x%x\r\n", bvd_pre_value, bvd_cnt, bvd_cmp, bvd_cfg_level);

    if (appmode == APP_STANDBY_MODE)
    {
        *((unsigned int volatile *)(addr.bvd_pre_val_addr))  =  bvd_value;
    }
    else
    {
        bvd_save.bvd_pre_val = bvd_value;
    }

    if (bvd_pre_value == 0)
    {
        bvd_value_min = bvd_value;
    }
    else
    {
        bvd_value_min = bvd_value < bvd_pre_value ? bvd_value : bvd_pre_value;
    }
    if (bvd_cmp >= BVD_CMP_CNT)
    {
        bvd_value_min = bvd_value < bvd_pre_value ? bvd_value : bvd_pre_value;
        if (bvd_value_min == 0xE)
        {
            PR_DEBUG("step 1_1\r\n");
            ACMP_BV_Config(2, 3, sbvsetLevelValue1);
        }
        else if (bvd_value_min == 0xC)
        {
            PR_DEBUG("step 1_2\r\n");
            ACMP_BV_Config(2, 3, sbvsetLevelValue2);
        }
        else if (bvd_value_min == 0x8)
        {
            PR_DEBUG("step 1_3\r\n");
            ACMP_BV_Config(2, 3, sbvsetLevelValue3);
        }
        else if (bvd_value_min == 0x0)
        {
            PR_DEBUG("step 1_4\r\n");
            ACMP_BV_Config(2, 3, sbvsetLevelValue4);
        }
        else
        {

        }
        bvd_cmp = 0;
        //bvd_cnt = 0;
    }
    else if ((bvd_cnt >= BVD_CNT))
    {
        if (bvd_value == 0xF)
        {
            ACMP_BV_Config(2, 3, sbvsetLevelValue1);
            PR_DEBUG("debug 2_0 pHandle->device->BVL0= 0x%x \n", pHandle->device->BVL0);
        }
        else if (bvd_value == 0xE)
        {
            if (bvd_cfg_val[0] == sbvsetLevelValue1[0])
            {
                ACMP_BV_Config(2, 3, sbvsetLevelValue2);
                PR_DEBUG("debug 2 _1 pHandle->device->BVL0= 0x%x \n", pHandle->device->BVL0);
            }
            else
            {
                PR_DEBUG("debug 2 _2 \n");
                ACMP_BV_Config(2, 3, sbvsetLevelValue1);
                PR_DEBUG("pHandle->device->BVL0= 0x%x \n", pHandle->device->BVL0);
            }
        }
        else if (bvd_value == 0xC)
        {
            if ((bvd_cfg_val[1])  == sbvsetLevelValue2[1])
            {
                ACMP_BV_Config(2, 3, sbvsetLevelValue3);
                PR_DEBUG("debug 3_1\n");
                PR_DEBUG("pHandle->device->BVL0= 0x%x \n", pHandle->device->BVL0);
            }
            else
            {
                ACMP_BV_Config(2, 3, sbvsetLevelValue2);
                PR_DEBUG("debug 3_2\n");
                PR_DEBUG("pHandle->device->BVL0= 0x%x \n", pHandle->device->BVL0);
            }
        }
        else if (bvd_value == 0x8)
        {
            if (bvd_cfg_val[2]  == sbvsetLevelValue3[2])
            {
                ACMP_BV_Config(2, 3, sbvsetLevelValue4);
            }
            else
            {
                ACMP_BV_Config(2, 3, sbvsetLevelValue3);
            }

            PR_DEBUG("debug 4 pHandle->device->BVL0= 0x%x \n", pHandle->device->BVL0);
        }
        else  if (bvd_value == 0x0)
        {
            ACMP_BV_Config(2, 3, sbvsetLevelValue4);
            PR_DEBUG("debug 5 pHandle->device->BVL0= 0x%x \n", pHandle->device->BVL0);
        }
        else
        {
            PR_DEBUG("warning bvd_value = 0x%x\r\n", bvd_value);
        }
        bvd_cnt = 0;
        //bvd_cmp = 0;
    }
    if (appmode == APP_STANDBY_MODE)
    {
        *((unsigned int volatile *)(addr.bvd_cmp_cnt_addr)) =  bvd_cmp;
        *((unsigned int volatile *)(addr.bvd_cnt_addr)) =  bvd_cnt;
    }
    else
    {
        bvd_save.bvd_cmp_cnt = bvd_cmp;
        bvd_save.bvd_cnt = bvd_cnt;
    }

    lcd_display_bvd_value = bvd_value_min;

    PR_DEBUG("BV IRQ Output:0x%x\n", bvd_value);
    PR_DEBUG("LCD display:0x%x\n", lcd_display_bvd_value);

    return lcd_display_bvd_value;
}

#endif

static void ACMP_IrqHandler(void)
{
    ACMP_Handle_T *pHandle = s_pHandle;

#if !defined(CHIP_GM6621)
    /*clear irq*/
    ACMP_CLEAR_INTERRUPT(pHandle->device);
#endif

#if defined(CHIP_GM6621)
    volatile uint32_t irqAC0Status = pHandle->device->AC0CS;
    volatile uint32_t irqAC0En = pHandle->device->AC0CS;
    volatile uint32_t irqAC1Status = pHandle->device->AC1CS;
    volatile uint32_t irqAC1En = pHandle->device->AC1CS;
    volatile uint32_t irqBvEnStatus = pHandle->device->BVCS;
    volatile uint32_t irqBvEn = pHandle->device->BVCS;
    static uint32_t acm0 = 0;
    static uint32_t acm1 = 0;
    ACMP_Save_Addr_ParamT data_save_addr;

    if ((irqAC0En & ACMP_AC0CS_IE) && (irqAC0Status & ACMP_AC0CS_IS))
    {
        IO_BITS_SET(pHandle->device->AC0CS, ACMP_AC0CS_IS);
        PR_DEBUG("ACMP0 CMP IRQ Acm0cnt :%d\n", ++acm0);
        PR_DEBUG("IRQ output:%d\n", ACMP_AC0_COMPARE_RESULT(pHandle->device));
    }
    else if ((irqAC1En & ACMP_AC1CS_IE) && (irqAC1Status & ACMP_AC1CS_IS))
    {
        IO_BITS_SET(pHandle->device->AC1CS, ACMP_AC1CS_IS);
        PR_DEBUG("ACMP1 CMP IRQ Acm1cnt :%d\n", ++acm1);
        PR_DEBUG("IRQ output:%d\n", ACMP_AC1_COMPARE_RESULT(pHandle->device));
    }
    else if ((irqBvEn & ACMP_BVCS_IE) && (irqBvEnStatus & ACMP_BVCS_IS))
    {
        IO_BITS_SET(pHandle->device->BVCS, ACMP_BVCS_IS);

        data_save_addr.bvd_cmp_cnt_addr = BVD_CMP_CNT_ADDRESS;
        data_save_addr.bvd_cnt_addr = BVD_CNT_ADDRESS;
        data_save_addr.bvd_pre_val_addr = BVD_PRE_VAL_ADDRESS;
        ACMP_battery_process(APP_NORMAL_MODE, data_save_addr);
    }
    else
    {

    }
#endif

    if (s_acmpCallback)
    {
        s_acmpCallback(pHandle, ACMP_MATCH_EVENT, 0);
    }
}

#if !defined(CHIP_GM6621)
static void ACMP_InterruptConfig(ACMP_Handle_T *pHandle, ACMP_Config_T *pConfig)
{
    s_acmpCallback = pConfig->callback;

#if !defined(CHIP_GM6621)
    COMPARE_MATCH_TRIGGER(pHandle->device, pConfig->triggerMode);
    ACMP_INTERRUPT_ENABLE(pHandle->device);
    NVIC_EnableIRQ(ACMP_IRQn);
#endif
}
#endif

ACMP_CompareResult_T ACMP_GetCompareRestult(ACMP_Handle_T *pHandle)
{
    ACMP_CompareResult_T ret = ACMP_LOW;
#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_GM6601) || defined(CHIP_F6601C))
    /* get output compare level*/
    if (IO_BIT_GET(pHandle->device->CS, ACMP_CS_OPTVAL))
    {
        /*ositivePin voltage is more than negativePin voltage */
        ret = ACMP_HIHG;
    }
    else
    {
        /*positivePin voltage is no more than negativePin voltage */
        ret = ACMP_LOW;
    }
#endif
    return ret;
}

void ACMP_InitPowerMode()
{
#if defined (CHIP_GM6621)
#ifdef AC_LOWPOWER_32KHZ_ENABLE
    ACMP_LOWPOWER_ENABLE(s_pHandle->device);
#ifdef CHIP_GM6621
    AOPDEV->CSR2 &= ~(BIT_CLEAR_3 << ACMP_32KHZ_LIRC);
    AOPDEV->CSR2 |= (BIT_set_2 << ACMP_32KHZ_LIRC);
#else
    SYSDEV->CLK3 &= ~(BIT_set_1 << ACMP_RTC_32KHZ_LIRC);
    SYSDEV->CLK3 |= (BIT_set_1 << ACMP_RTC_32KHZ_LIRC);
    SYSDEV->CLK3 &= ~(BIT_CLEAR_3 << RTC_32KHZ);
#if (defined(OSC_EXTERNAL_CLOCK))
    SYSDEV->CLK3 |= (BIT_CLEAR_3 << RTC_32KHZ);
#else
    SYSDEV->CLK3 |= (BIT_set_2 << RTC_32KHZ);
#endif
#endif
#else
    ACMP_LOWPOWER_DISABLE(s_pHandle->device);
#endif
#endif

}

#if defined(CHIP_GM6621)
void ACMP_BV_Config(uint32_t bvFilterThr, uint8_t levelnum, uint8_t *pIputDavLevelValue)
{
    ACMP_Device_T *acmpDevice =  s_pHandle->device;
    /* config BV FILTER THR */
    ACMP_CONFIG_BV_FILTER_THR(acmpDevice, bvFilterThr);
    /* config 8 level mode */
    ACMP_BV_CONFIG_LEVEL(acmpDevice, levelnum);

    /*config input dac level value*/
    ACMP_CONFIG_BV_LEVEL0(acmpDevice, pIputDavLevelValue[0]);
    ACMP_CONFIG_BV_LEVEL1(acmpDevice, pIputDavLevelValue[1]);
    ACMP_CONFIG_BV_LEVEL2(acmpDevice, pIputDavLevelValue[2]);
    ACMP_CONFIG_BV_LEVEL3(acmpDevice, pIputDavLevelValue[3]);

    if (levelnum > 3)
    {
        ACMP_CONFIG_BV_LEVEL4(acmpDevice, pIputDavLevelValue[4]);
        ACMP_CONFIG_BV_LEVEL5(acmpDevice, pIputDavLevelValue[5]);
        ACMP_CONFIG_BV_LEVEL6(acmpDevice, pIputDavLevelValue[6]);
        ACMP_CONFIG_BV_LEVEL7(acmpDevice, pIputDavLevelValue[7]);
    }

    /*selcet pn mode: force dac p:dac n:vref*/
    ACMP_AC0_SEL_NEGATIVE_CHANNEL(acmpDevice, 0);
    ACMP_AC0_SEL_POSITIVE_CHANNEL(acmpDevice, 7);
}

void ACMP_BV_Interrupt_Config()
{
    ACMP_BV_IE_ENABLE(s_pHandle->device);
    /*nvic enable*/
    NVIC_EnableIRQ(ACMP_IRQn);
}

static void ACMP_Interrupt_Config(ACMP_DevIndex_T DevIndex, ACMP_CompareMatchTrigger_T mode)
{

    if (DevIndex == ACMP_DEV_0)
    {

        /*config acmp0 cmp irq mode*/
        ACMP_AC0_IE_MODE_CONAFIG(s_pHandle->device, mode);
        /*enale acmp0 cmp irq*/
        ACMP_AC0_CMPIE_ENABLE(s_pHandle->device);

    }
    else
    {
        /*config acmp1 cmp irq mode*/
        ACMP_AC1_IE_MODE_CONAFIG(s_pHandle->device, mode);
        /*enale acmp1 cmp irq*/
        ACMP_AC1_CMPIE_ENABLE(s_pHandle->device);

    }

    /*nvic enable*/
    NVIC_EnableIRQ(ACMP_IRQn);
}

static void ACMP_ClearIrqInit()
{
    IO_BITS_SET(s_pHandle->device->AC0CS, ACMP_AC0CS_IS);
    IO_BITS_SET(s_pHandle->device->AC1CS, ACMP_AC1CS_IS);
    IO_BITS_SET(s_pHandle->device->BVCS, ACMP_BVCS_IS);
}


int ACMP_CompareMode(ACMP_Handle_T *pHandle, ACMP_DevIndex_T devIndex)
{
    RET_CODE ret = RET_OK;

    ACMP_ClearIrqInit();
    /*config AC PowerMode*/
    ACMP_InitPowerMode();
    /*
    ACMP0 --> XPA4  PIN_54(GM6621_64F)
    ACMP1 --> XPA5  PIN_55(GM6621_64F)
    ACMP2 --> XPA6  PIN_56(GM6621_64F)--> not use
    ACMP3 --> XPA7  PIN_57(GM6621_64F)
    ACMP4 --> XPA8  PIN_58(GM6621_64F)--> not use
    ACMP5 --> XPA9  PIN_59(GM6621_64F)

    ACMP6 --> XPC12  PIN_11(GM6621_64F)
    ACMP7 --> XPC13  PIN_13(GM6621_64F)
    ACMP8 --> XPC14  PIN_16(GM6621_64F)
    ACMP9 --> XPC15  PIN_18(GM6621_64F)
    ACMP10-> XPD0  PIN_20(GM6621_64F)
    ACMP11-> XPD9  PIN_29(GM6621_64F)
    ACMP12-> XPD10  PIN_30(GM6621_64F)

    ACMP0_OUT --> XPA10  PIN_61(GM6621_64F)
    ACMP1_OUT --> XPA11  PIN_63(GM6621_64F)
    */

    if (devIndex == ACMP_DEV_0)
    {
        /*channel select */
        ACMP_ENABLE_AINPUTCHL(s_pHandle->device, 1 << 0 | 1 << 3); /*XPA4,XPA7*/
        /*
        3'b000: XPA4
        3'b001: XPA8   --> error
        3'b010: XPA9
        3'b011: XPD9
        3'b100: 1/4 VREFA
        3'b101: 1/2 VREFA
        3'b110: VREFA
        3'b111: DAC_OUT
        */
        ACMP_AC0_SEL_POSITIVE_CHANNEL(s_pHandle->device, 0); /*XPA4*/
        /*
        AC0_NSEL: Analog Comparator 0 Negative Input Select
        3'b000: VREFB
        3'b001: XPA5
        3'b010: XPA7
        3'b011: XPC13
        3'b100: XPC14
        3'b101: XPC15
        3'b110: XPD0
        3'b111: XPD10
        */
        ACMP_AC0_SEL_NEGATIVE_CHANNEL(s_pHandle->device, 2); /*XPA7*/
        ACMP_AC0_HYSSEL(s_pHandle->device, 3);
    }
    else
    {
        /*channel select */
        ACMP_ENABLE_AINPUTCHL(s_pHandle->device, 1 << 3 | 1 << 5); /*XPA7,XPA9*/
        /*
        3'b000: DAC_OUT
        3'b001: XPA5
        3'b010: XPA7
        3'b011: XPC13
        3'b100: XPC14
        3'b101: XPC15
        3'b110: XPD0
        3'b111: XPD10
        */
        ACMP_AC1_SEL_POSITIVE_CHANNEL(s_pHandle->device, 2); //XPA7
        /*
        3'b000: XPA6
        3'b001: XPA8
        3'b010: XPA9
        3'b011: XPC12
        3'b100: 1/4VREFA
        3'b101: 1/2VREFA
        3'b110: 3/4VREFA
        3'b111: VREFA
        */
        ACMP_AC1_SEL_NEGATIVE_CHANNEL(s_pHandle->device, 2); //XPA9
        ACMP_AC1_HYSSEL(s_pHandle->device, 3);
    }

    /* config Analog Comparator Setup Timing time */
    ACMP_CONFIG_SETUP_TIME(s_pHandle->device, 0x1F0);
    /* config cmp output filter*/
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_CONFIG_AC0O_FILTER(s_pHandle->device, 0xFFFF);
    }
    else
    {
        ACMP_CONFIG_AC1O_FILTER(s_pHandle->device, 0xFFFF);
    }

    /* config output inversion */
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_AC0_OUTPUT_INVERSION_DISABLE(s_pHandle->device);
        /*ACMP_AC0_OUTPUT_INVERSION_ENBALE(gDevice);*/
    }
    else
    {
        ACMP_AC1_OUTPUT_INVERSION_DISABLE(s_pHandle->device);
    }

    ACMP_Interrupt_Config(devIndex, ANALOG_MATCH_FALLINGRISING);

    /* enable acmp modules*/
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_AC0_PIN_OUTPUT_ENABLE(s_pHandle->device);
        ACMP_AC0_ENABLE(s_pHandle->device);
    }
    else
    {
        ACMP_AC1_PIN_OUTPUT_ENABLE(s_pHandle->device);
        ACMP_AC1_ENABLE(s_pHandle->device);
    }

    return ret;

}

int ACMP_SAM_TrigerMode(ACMP_Handle_T *pHandle,  ACMP_DevIndex_T devIndex)
{
    RET_CODE ret = RET_OK;
    ACMP_Device_T *acmpDevice;
    acmpDevice = pHandle->device;

    ACMP_ClearIrqInit();
    /*config AC PowerMode*/
    ACMP_InitPowerMode();

    if (devIndex == ACMP_DEV_0)
    {
        /*
        3'b000: XPA4
        3'b001: XPA8   --> error ,not acmp function
        3'b010: XPA9
        3'b011: XPD9
        3'b100: 1/4 VREFA
        3'b101: 1/2 VREFA
        3'b110: VREFA
        3'b111: DAC_OUT
        */
        ACMP_AC0_SEL_POSITIVE_CHANNEL(acmpDevice, 0); /*XPA4*/
        /*
        AC0_NSEL: Analog Comparator 0 Negative Input Select
        3'b000: VREFB
        3'b001: XPA5
        3'b010: XPA7
        3'b011: XPC13
        3'b100: XPC14
        3'b101: XPC15
        3'b110: XPD0
        3'b111: XPD10
        */
        ACMP_AC0_SEL_NEGATIVE_CHANNEL(acmpDevice, 2); /*XPA7*/
        ACMP_AC0_HYSSEL(acmpDevice, 1);
        ACMP_ENABLE_AINPUTCHL(s_pHandle->device, 1 << 0 | 1 << 3); /*XPA4,XPA7*/
    }
    else
    {
        /*
        3'b000: DAC_OUT
        3'b001: XPA5
        3'b010: XPA7
        3'b011: XPC13
        3'b100: XPC14
        3'b101: XPC15
        3'b110: XPD0
        3'b111: XPD10
        */
        ACMP_AC1_SEL_POSITIVE_CHANNEL(acmpDevice, 2); //XPA7
        /*
        3'b000: XPA6
        3'b001: XPA8
        3'b010: XPA9
        3'b011: XPC12
        3'b100: 1/4VREFA
        3'b101: 1/2VREFA
        3'b110: 3/4VREFA
        3'b111: VREFA
        */
        ACMP_AC1_SEL_NEGATIVE_CHANNEL(acmpDevice, 2); //XPA9
        ACMP_AC1_HYSSEL(acmpDevice, 3);
        ACMP_ENABLE_AINPUTCHL(acmpDevice, 1 << 3 | 1 << 5); /*XPA7,XPA9*/
    }

    /* config Analog Comparator Setup Timing time */
    /*ACMP_CONFIG_SETUP_TIME(acmpDevice, 0x10);*/


    /* config cmp output filer*/
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_CONFIG_AC0O_FILTER(acmpDevice, 0x0);
    }
    else
    {
        ACMP_CONFIG_AC1O_FILTER(acmpDevice, 0x0);
    }

    /* disable acmp modules*/
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_AC0_DISABLE(acmpDevice);
    }
    else
    {
        ACMP_AC1_DISABLE(acmpDevice);
    }

    /* config cpu triger enable*/
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_AC0_EVENT_TRIG_ENABLE(acmpDevice);
    }
    else
    {
        ACMP_AC1_EVENT_TRIG_ENABLE(acmpDevice);
    }

    ACMP_Interrupt_Config(devIndex, ANALOG_MATCH_FALLINGRISING);

    return ret;

}

static void ACMP_DACVOLCampreMode(ACMP_Handle_T *pHandle,  ACMP_DevIndex_T devIndex)
{
    /*ACMP_DevIndex_T devIndex = ACMP_DEV_1;*/
    uint32_t setupTime = 0x100;
    uint32_t filterCnt = 0;
    ACMP_Device_T *acmpDevice;
    acmpDevice = pHandle->device;
    /*
    1> Configure DAC reference voltage from VCC,Enable DAC
    2> Configure DAC code based on the VCC voltage level needed to be detected
    3> Configure AC0/AC1 PSEL to DAC OUT
    4> Configure AC0/AC1 NSEL to VREF
    5> Disable AC0/AC1, Enable the trigger mode
    6> Clear the filter value of AC0/AC1
    7> Trigger AC0/AC1 by CPU/PES/RTC
    8> Check the AC0/AC1 interrupt status
    */
    /*1> Configur DAC reference voltage from VCC,Enable DAC*/
    ACMP_SET_REFER_VOLTAGE(acmpDevice, 0);
    ACMP_DAC_ENABLE(acmpDevice);

    /*2> Configur DAC code based on the VCC voltage level needed to be detected*/
    ACMP_DAC_CONVERT_VALUE(acmpDevice, 17);

    if (devIndex == ACMP_DEV_0)
    {
        /*selcet pn mode: force dac p:dac n:vref*/
        ACMP_AC0_SEL_NEGATIVE_CHANNEL(acmpDevice, 0);
        ACMP_AC0_SEL_POSITIVE_CHANNEL(acmpDevice, 7);

        /*5> Disable AC0/AC1, Enable the trigger mode*/
        ACMP_AC0_DISABLE(acmpDevice);
    }
    else
    {
        ACMP_AC1_SEL_NEGATIVE_CHANNEL(acmpDevice, 5);
        ACMP_AC1_SEL_POSITIVE_CHANNEL(acmpDevice, 0);
        /*5> Disable AC0/AC1, Enable the trigger mode*/
        ACMP_AC1_DISABLE(acmpDevice);
    }

    /* config Analog Comparator Setup Timing time */
    ACMP_CONFIG_SETUP_TIME(acmpDevice, setupTime);
    /* config cmp output filer*/
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_AC0_EVENT_TRIG_ENABLE(acmpDevice);
        ACMP_CONFIG_AC0O_FILTER(acmpDevice, filterCnt);
        ACMP_AC0_HYSSEL(acmpDevice, 3);
    }
    else
    {
        ACMP_AC1_EVENT_TRIG_ENABLE(acmpDevice);
        ACMP_CONFIG_AC1O_FILTER(acmpDevice, filterCnt);
        ACMP_AC1_HYSSEL(acmpDevice, 1);
    }
    if (devIndex == ACMP_DEV_0)
    {
        ACMP_Interrupt_Config(devIndex, ANALOG_MATCH_FALLING);
    }
    else
    {
        ACMP_Interrupt_Config(devIndex, ANALOG_MATCH_FALLING);
    }
}

#endif

RET_CODE ACMP_Init(ACMP_Device_T *pACMP, ACMP_Config_T *pConfig, ACMP_Handle_T *pHandle)
{
    RET_CODE ret = RET_OK;
#if defined(CHIP_GM6621)
    ACMP_DevIndex_T devIndex = pConfig->devIndex;
    uint32_t setupTime = 0x100;
    ACMP_Device_T *acmpDevice;
    /*AC0_NSEL VREFB(0.8V);DAC reference voltage source is VREFA(1.5V). {0.3v,0.4v,0.6v,0.7v,0.9v,1.0v,1.2v,1.3v},1.5*13/64= 0.3V*/
    /*static uint8_t sbvInputLevelValue[9]={13,17,26,30,38,43,51,55}; */

    /*AC0_NSEL VREFB(0.8V);DAC reference voltage source is VCC = {2.56v,2.5v,2.3v,2.2v} ;code =   64 * (0.8/vcc)  =  51.2/vcc */
#endif

    if (!(pHandle && pACMP && pConfig))
    {
        ret = RET_INVPARAM;
        goto out;
    }

    pHandle->device = pACMP;
    /* config phandle */
    s_pHandle = pHandle;
#if defined(CHIP_GM6621)
    acmpDevice = pACMP;
#endif

    ACMP_RESET();
    ACMP_CLKEN();

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) ||defined(CHIP_GM6601) || defined(CHIP_F6601C))
    ACMP_DISABLE(pHandle->device);
    /*init acmp accord config*/
    if (pConfig->outputPinEnable == ENABLE)
    {
        OUTPUTPIN_ENABLE(pACMP);
    }
    else
    {
        OUTPUTPIN_DISABLE(pACMP);
    }

    if (pConfig->dacConfig.dacEnbale == ENABLE)
    {
				#if !(defined(CHIP_F6601C))
					DAC_REFER_VOLTAGE(pACMP, pConfig->dacConfig.referVoltage);
				#endif
        DAC_CONVERT_VALUE(pACMP, pConfig->dacConfig.dacValue);
        DAC_ENABLE(pACMP);
    }
    else
    {
        DAC_DISABLE(pACMP);
    }

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
#if defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C)
    if (pConfig->outputInversionEnable == ENABLE)
    {
        ACMP_OUTPUT_INVERSION_ENABLE(pACMP);
    }
    else
    {
        ACMP_OUTPUT_INVERSION_DISABLE(pACMP);
    }

    if (pConfig->trigerAdcEnable == ENABLE)
    {
        ACMP_TRIGER_ADC_ENABLE(pACMP);
    }
    else
    {
        ACMP_TRIGER_ADC_DISABLE(pACMP);
    }
#endif
    /*config ac output filer thr*/
    ACMP_CONFIG_FILTER_THR(pACMP, pConfig->filterThr);
#endif

    SET_NEGATIVE_PIN(pACMP, pConfig->negativePin);
    SET_POSITIVE_PIN(pACMP, pConfig->positivePin);
    ACMP_InitPowerMode();
    /*
     *config irq match trigger mode
     *enable acmp irq and nvic for acmp
     *regist acmp event callback
     */
    ACMP_InterruptConfig(pHandle, pConfig);
    ACMP_ENABLE(pHandle->device);
#elif defined(CHIP_GM6621)
    if ((pConfig->batteryEn ==  ENABLE) && ((pConfig->trigerSource == ACMP_TRI_SEL_NONE) || (pConfig->trigerSource == ACMP_TRI_SEL_RTC)))
    {
        if (devIndex == ACMP_DEV_0) /*voltage detect only support ACMP_DEV_0 */
        {
            /*1> Configur DAC reference voltage from VREFA,Enable DAC*/
            ACMP_SET_REFER_VOLTAGE(acmpDevice, 0);/*from vcc */
            /*config bv parameter*/
            if (pConfig->standby_flag != 1)
            {
                ACMP_BV_Config(2, 3, sbvsetLevelValue0);
            }
            /* config Analog Comparator Setup Timing time */
            ACMP_CONFIG_SETUP_TIME(acmpDevice, setupTime);
            /*selcet pn mode: force dac p:dac n:vref*/
            ACMP_AC0_SEL_NEGATIVE_CHANNEL(acmpDevice, 0);
            ACMP_AC0_SEL_POSITIVE_CHANNEL(acmpDevice, 7);

            /*5> Disable AC0/AC1, Enable the trigger mode*/
            ACMP_AC0_DISABLE(acmpDevice);
            /* config Analog Comparator 0 Hysteresis Selection*/
            ACMP_AC0_HYSSEL(acmpDevice, 0);

            if (pConfig->trigerSource == ACMP_TRI_SEL_NONE)
                /* disable pes and rtc triger*/
            {
                ACMP_BVD_EVENT_TRIG_DISABLE(acmpDevice);
            }
            else if (pConfig->trigerSource == ACMP_TRI_SEL_RTC)
            {
                ACMP_BVD_EVENT_TRIG_ENABLE(acmpDevice);
            }
            /*enable bv irq*/
            ACMP_BV_Interrupt_Config();
        }
    }
    else if ((pConfig->batteryEn ==  ENABLE) && (pConfig->trigerSource == ACMP_TRI_SEL_CPU))
    {
        ACMP_DACVOLCampreMode(pHandle, devIndex);
    }
    else if (pConfig->trigerSource == ACMP_TRI_SEL_CPU)
    {
        ACMP_SAM_TrigerMode(pHandle, devIndex);

    }
    else
    {
        ACMP_CompareMode(pHandle, devIndex);
    }
#endif

out:

    return ret;
}
