/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          adc_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2016/07/21
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/04/25, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/21, Mike.Zheng, V1.1.0
 *        Update coding style.
 *    2016/11/22, Mike.Zheng, V1.2.0
 *        Update For Release.
 */

#ifndef _ADC_DEV_H_
#define _ADC_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <drv/dev/dma.h>
#include <drv/dev/clock.h>

#ifndef ADC_DEBUG_PRINTF
#define ADC_PR_DEBUG(...)      do {} while (0)
#else
#define ADC_PR_DEBUG           printf
#endif

#ifndef ADC_ERR_PRINTF
#define ADC_PR_ERR(...)        do {} while (0)
#else
#define ADC_PR_ERR             printf
#endif

/* ADC control register layout type definition */
typedef struct
{
    __IO uint32_t CTRL;                       /* ADC control register:offset: 0x0 */
    __IO uint32_t TRIG;                       /* ADC triger register:offset: 0x4 */
    __IO uint32_t ST;                         /* ADC interrupt status register:offset: 0x8 */
    __IO uint32_t CHNSEL;                     /* ADC sample channel select register:offset: 0xc */
    __IO uint32_t DRST;                       /* ADC convert data result register:offset: 0x10 */
    __IO uint32_t COMPH;                      /* ADC compare highvalue register:offset: 0x14 */
    __IO uint32_t COMPL;                      /* ADC compare lowvalue register:offset: 0x18 */
    __IO uint32_t IE;                         /* ADC interrupt enable register:offset: 0x1c */
    __IO uint32_t DMACTR;                     /* ADC DMA contral register:offset: 0x20 */
#if defined(CHIP_GM6611B)
    __IO uint32_t VER;                        /* ADC IP version register:offset: 0x24 */
#endif
#if defined (CHIP_GM6621)
    __IO uint32_t OVSAMPCTR;                  /* ADC oversample register:offset: 0x24 */
    __IO uint32_t HWTRIGCTR;                  /* ADC hardware trigger register:offset: 0x28 */
    __IO uint32_t EXCHEN;                     /* ADC external channel register :offset: 0x2c */
    __IO uint32_t VER;                        /* ADC IP version register:offset: 0x30 */
#endif
} ADC_Device_T;

#define ADC_BASE                              (0x4000c000u) /* Base addresses of ADC registers */
#define ADC_DEV                               ((ADC_Device_T *)ADC_BASE) /* ADC device pointer */

#define SAMPLE_SEQUENCE_MAX_LENGTH            8      /* Max sequence sample channel number */
#define ADC_RESET_DELAYCNT                    0xff   /* ADC reset timedelay */
#define UNLOCKED                              0x0    /* ADC lockstus : Unlock */
#define LOCKED                                0x1    /* ADC lockstus : Lock */
#define ADC_STOP_WORK_MODE                    0x0    /* ADC stop mode define */
#define ADC_READ_FIFO_OFFSET                  0x10   /* ADC read fifo offset To ADC reg base */
#define ADC_READ_FIFO_PHY_ADDRESS             (uint32_t)((uint32_t)ADC_DEV + ADC_READ_FIFO_OFFSET) /* ADC read fifo offset physical address */
#define ADC_CLOCK_GATE_MASK                   (1<<12)/* ADC clock gate maskbit */

/*************** ADC Contral Register Bitfiled ******************/
#if defined(CHIP_GM6621)
#define ADC_CTRL_BUFEN_POS                    ((uint32_t) 28) /* ADC input buffer enablepos */
#define ADC_CTRL_BUFEN                        ((uint32_t) ((uint32_t)1 << ADC_CTRL_BUFEN_POS))   /* ADC input buffer enable filed */
#endif
#define ADC_CTRL_CM_POS                       ((uint32_t) 25) /* ADC compare mode pos */
#define ADC_CTRL_CM                           ((uint32_t) ((uint32_t)3 << ADC_CTRL_CM_POS))     /* ADC compare mode filed */
#define ADC_CTRL_CE_POS                       ((uint32_t) 24) /* ADC auto compare enable pos */
#define ADC_CTRL_CE                           ((uint32_t) ((uint32_t)1 << ADC_CTRL_CE_POS))     /* ADC auto compare enable filed */
#define ADC_CTRL_FDTH_POS                     ((uint32_t) 20) /* ADC channel fifo depth pos */
#define ADC_CTRL_FDTH                         ((uint32_t) ((uint32_t)0xf << ADC_CTRL_FDTH_POS)) /* ADC channel fifo depth filed */
#define ADC_CTRL_WM_POS                       ((uint32_t) 16) /* ADC work mode pos */
#define ADC_CTRL_WM                           ((uint32_t) ((uint32_t)3 << ADC_CTRL_WM_POS))     /* ADC work mode filed */

#if defined(CHIP_GM6621)
#define ADC_CTRL_DCONTEN_POS                  ((uint32_t) 12) /* ADC discontinuous mode enable pos */
#define ADC_CTRL_DCONTEN                      ((uint32_t) ((uint32_t)1 << ADC_CTRL_DCONTEN_POS))   /* ADC discontinuous mode enable filed */
#endif
#if defined(CHIP_GM6601B) ||defined(CHIP_F6601C)|| defined(CHIP_GM6611B)
#define ADC_CTRL_HTRIG_SEL_POS                ((uint32_t) 13) /* ADC hardware trigger source select */
#define ADC_CTRL_HTRIG_SEL                    ((uint32_t) ((uint32_t)7 << ADC_CTRL_HTRIG_SEL_POS))     /*  ADC hardware trigger source filed */
#define ADC_CTRL_HTRIG_EN_POS                 ((uint32_t) 12) /* ADC hardware trigger enable pos */
#define ADC_CTRL_HTRIG_EN                     ((uint32_t) ((uint32_t)1 << ADC_CTRL_HTRIG_EN_POS))      /*  ADC hardwaretrigger enable filed */
#endif

#if defined(CHIP_GM6601)

#define ADC_CTRL_AN_POS                       ((uint32_t) 8)  /*  ADC analog input enable */
#define ADC_CTRL_AN                           ((uint32_t) ((uint32_t)1 << ADC_CTRL_AN_POS))     /*  ADC analog input filed */
#define ADC_CTRL_RM_POS                       ((uint32_t) 6)  /* ADC resolution mode pos */
#define ADC_CTRL_RM                           ((uint32_t) ((uint32_t)3 << ADC_CTRL_RM_POS))     /*  ADC resolution mode filed */
#define ADC_CTRL_STIM_POS                     ((uint32_t) 4)  /* ADC simple time pos */
#define ADC_CTRL_STIM                         ((uint32_t) ((uint32_t)3 << ADC_CTRL_STIM_POS))   /* ADC simple time pos */
#define ADC_CTRL_AOFE_POS                     ((uint32_t) 0)  /* ADC auto power off enable pos */
#define ADC_CTRL_AOFE                         ((uint32_t) ((uint32_t)1 << ADC_CTRL_AOFE_POS))   /* ADC auto Power off enable filed */
#elif (defined (CHIP_GM6611) || defined (CHIP_GM6601B)||defined(CHIP_F6601C) ||  defined (CHIP_GM6621) || defined(CHIP_GM6611B))
#define ADC_CTRL_BNIT_POS                     ((uint32_t) 8)  /* ADC resolution mode */
#define ADC_CTRL_BNIT                         ((uint32_t) ((uint32_t)3 << ADC_CTRL_BNIT_POS))   /*  ADC resolution mode filed */
#if defined(CHIP_GM6621)
#define ADC_CTRL_PWSUPLY_POS                  ((uint32_t) 7)  /* ADC power supply  pos */
#define ADC_CTRL_PWSUPLY                      ((uint32_t) ((uint32_t)1 << ADC_CTRL_PWSUPLY_POS))/* ADC power supply pos filed */
#endif
#define ADC_CTRL_STIM_POS                     ((uint32_t) 4)  /* ADC simple time pos */
#define ADC_CTRL_STIM                         ((uint32_t) ((uint32_t)7 << ADC_CTRL_STIM_POS))   /* ADC simple time filed */

#if defined(CHIP_GM6621)
#define ADC_CTRL_LPEN_POS                     ((uint32_t) 3)  /* ADC low power mode enable pos */
#define ADC_CTRL_LPEN                         ((uint32_t) ((uint32_t)1 << ADC_CTRL_LPEN_POS))   /* ADC low power mode enable filed */
#endif

#define ADC_CTRL_DFFWEN_POS                   ((uint32_t) 2)  /* ADC data fifo write enable when it's full pos */
#define ADC_CTRL_DFFWEN                       ((uint32_t) ((uint32_t)1 << ADC_CTRL_DFFWEN_POS))   /* ADC data fifo write enable when it's full filed */
#if  defined(CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6611B)
#define ADC_CTRL_EOCSMP_POS                   ((uint32_t) 1)  /* ADC end of convert plus signal sample pos */
#define ADC_CTRL_EOCSMP                       ((uint32_t) ((uint32_t)1 << ADC_CTRL_EOCSMP_POS))   /* ADC end of convert plus signal sample filed */
#endif
#define ADC_CTRL_EN_POS                       ((uint32_t) 0)  /* ADC enable pos */
#define ADC_CTRL_EN                           ((uint32_t) ((uint32_t)1 << ADC_CTRL_EN_POS))       /* ADC enable filed */
#endif

/*************** ADC triger register bitfiled ****************************/
#define ADC_TRIG_CT_POS                       ((uint32_t) 0) /* ADC convert trigger pos */
#define ADC_TRIG_CT                           ((uint32_t) ((uint32_t)1 << ADC_TRIG_CT_POS))     /* ADC convert trigger filed */

/*************** ADC interrupt status register bitfiled ******************/
#define ADC_ST_CMI_POS                        ((uint32_t) 28) /* ADC compare match interrupt status pos */
#define ADC_ST_CMI                            ((uint32_t) ((uint32_t)1 << ADC_ST_CMI_POS))      /* ADC compare match interrupt status filed */
#define ADC_ST_DOFI_POS                       ((uint32_t) 24) /* ADC Data Convert Overflow Interrupt Status Pos */
#define ADC_ST_DOFI                           ((uint32_t) ((uint32_t)1 << ADC_ST_DOFI_POS))     /* ADC data convert overflow interrupt status filed */


#define ADC_ST_DOPSI_POS                       ((uint32_t) 20) /* ADC DFIFO prefull interrupt status pos */
#define ADC_ST_DOPSI                           ((uint32_t) ((uint32_t)1 << ADC_ST_DOPSI_POS))     /* ADC DFIFO prefull interrupt status pos*/

#define ADC_ST_DOFSI_POS                       ((uint32_t) 16) /* ADC DFIFO full interrupt status pos */
#define ADC_ST_DOFSI                           ((uint32_t) ((uint32_t)1 << ADC_ST_DOFSI_POS))     /* ADC DFIFO full interrupt status pos*/

#define ADC_ST_EOSI_POS                       ((uint32_t) 12) /* ADC end of sequence convert interrupt status pos */
#define ADC_ST_EOSI                           ((uint32_t) ((uint32_t)1 << ADC_ST_EOSI_POS))     /* ADC end of sequence convert interrupt status filed */

#define ADC_ST_EOCE_POS                       ((uint32_t) 8)  /* ADC end of  convert interrupt enable pos */
#define ADC_ST_EOCE                           ((uint32_t) ((uint32_t)1 << ADC_ST_EOCE_POS))

#if defined(CHIP_GM6601B) ||defined(CHIP_F6601C)|| defined(CHIP_GM6611B) || defined(CHIP_GM6621)
#define ADC_ST_DFIFO_EMPTY_POS                ((uint32_t) 4)  /* ADC data FIFO empty pos */
#define ADC_ST_DFIFO_EMPTY                    ((uint32_t) ((uint32_t)1 << ADC_ST_DFIFO_EMPTY_POS))

#define ADC_ST_IDLE_POS                      ((uint32_t)0)  /* ADC busy status pos */
#define ADC_ST_BUSY                          ((uint32_t) ((uint32_t)1 << ADC_ST_IDLE_POS))

#endif
/*************** ADCinterrupt enable register bitfiled ******************/
#define ADC_IE_CMIE_POS                       ((uint32_t) 20) /* ADC compare match interrupt enable pos */
#define ADC_IE_CMIE                           ((uint32_t) ((uint32_t)1 << ADC_IE_CMIE_POS))     /* ADC compare match interrupt enable filed */
#define ADC_IE_DOFIE_POS                      ((uint32_t) 16) /* ADC data convert overflow interruptenable pos */
#define ADC_IE_DOFIE                          ((uint32_t) ((uint32_t)1 << ADC_IE_DOFIE_POS))    /* ADC data convert overflow interruptenable filed */
#define ADC_IE_EOSE_POS                       ((uint32_t) 4)  /* ADC end of sequence convert interrupt enable pos */
#define ADC_IE_EOSE                           ((uint32_t) ((uint32_t)1 << ADC_IE_EOSE_POS))     /* ADC end of sequence convert interrupt enable filed */
#define ADC_IE_EOCE_POS                       ((uint32_t) 0)  /* ADC end of  convert interrupt enable pos */
#define ADC_IE_EOCE                           ((uint32_t) ((uint32_t)1 << ADC_IE_EOCE_POS))    /* ADC end of convert interrupt enable filed */

/*************** ADC DMA control register bitfiled ***********************/
#define ADC_DMACTR_DFPN_POS                   ((uint32_t) 12) /* ADC data fifo prefull num pos */
#define ADC_DMACTR_DFPN                       ((uint32_t) ((uint32_t)7 << ADC_DMACTR_DFPN_POS)) /* ADC data fifo prefull num filed */
#define ADC_DMACTR_DFDMAE_POS                 ((uint32_t) 4)  /* Data fifo can transmit data to DMA enable pos */
#define ADC_DMACTR_DFDMAE                     ((uint32_t) ((uint32_t)1 << ADC_DMACTR_DFDMAE_POS))/* Data fifo can transmit data to DMA enable filed */
#define ADC_DMACTR_CDMAEN_POS                 ((uint32_t) 0)  /* Channel  fifo can transmit data to DMA enable pos */
#define ADC_DMACTR_CDMAEN                     ((uint32_t) ((uint32_t)1 << ADC_DMACTR_CDMAEN_POS))/* channel  fifo can transmit data to DMA enable filed */

/*************** ADC oversample register bitfiled ************************/
#define ADC_OVSAPL_SFT_POS                   ((uint32_t) 8) /* ADC oversample shift pos */
#define ADC_OVSAPL_SFT                       ((uint32_t) ((uint32_t)0xf << ADC_OVSAPL_SFT_POS)) /* ADC oversample shift filed */
#define ADC_OVSAPL_RATIO_POS                 ((uint32_t) 4) /* ADC oversample ratio pos */
#define ADC_OVSAPL_RATIO                     ((uint32_t) ((uint32_t)7 << ADC_OVSAPL_RATIO_POS)) /* ADC oversample ratio filed */
#define ADC_OVSAPL_EN_POS                    ((uint32_t) 0) /* ADC oversample enable pos */
#define ADC_OVSAPL_EN                        ((uint32_t) ((uint32_t)1 << ADC_OVSAPL_EN_POS)) /* ADC oversample enable filed */

/*************** ADC Harware trigger Register Bitfiled *******************/
#define ADC_HWTRIG_SRCSEL_POS                ((uint32_t) 4) /* ADC hardware trigger source select pos */
#define ADC_HWTRIG_SRCSEL                    ((uint32_t) ((uint32_t)0xf << ADC_HWTRIG_SRCSEL_POS)) /* ADC hardware trigger source select filed */
#define ADC_HWTRIG_EN_POS                    ((uint32_t) 0) /* ADC hardware trigger enable pos */
#define ADC_HWTRIG_EN                        ((uint32_t) ((uint32_t)1 << ADC_HWTRIG_EN_POS)) /* ADC hardware trigger enable filed */

/**
 * Read ADC convert mode: block read or noblock read
 */
typedef enum
{
    NOBLOCK = 1,            /**< Noblock to read ADC convert value, function will return immediately */
    BLOCK,                  /**< Block to read ADC convert value, function will not return until convert finish */
} ADC_BLOCKSTATUS_T;

/**
 * ADC sample channel fifo depth
 */
typedef enum
{
    ADC_CFIFO_DEPTH_0                         = 0x0,   /**< ADC sample channel fifo depth Is 0 , means not use fifo */
    ADC_CFIFO_DEPTH_1,                                 /**< ADC sample channel fifo depth Is 1 */
    ADC_CFIFO_DEPTH_2,                                 /**< ADC sample channel fifo depth Is 2 */
    ADC_CFIFO_DEPTH_3,                                 /**< ADC sample channel fifo depth Is 3 */
    ADC_CFIFO_DEPTH_4,                                 /**< ADC sample channel fifo depth Is 4 */
    ADC_CFIFO_DEPTH_5,                                 /**< ADC sample channel fifo depth Is 5 */
    ADC_CFIFO_DEPTH_6,                                 /**< ADC sample channel fifo depth Is 6 */
    ADC_CFIFO_DEPTH_7,                                 /**< ADC sample channel fifo depth Is 7 */
    ADC_CFIFO_DEPTH_8                                  /**< ADC sample channel fifo depth Is 8 */
} ADC_ChannelFifoDepth_T;

/**
 * ADC Convert Data Fifo Prefull Num
 */
typedef enum
{
    ADC_DFIFO_PRE_FULL_NUM_1                  = 0x0,  /**< There is 1 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_2,                         /**< There is 2 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_3,                         /**< There is 3 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_4,                         /**< There is 4 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_5,                         /**< There is 5 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_6,                         /**< There is 6 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_7,                         /**< There is 7 data in data fifo generate prefull event */
    ADC_DFIFO_PRE_FULL_NUM_8                          /**< There is 8 data in data fifo generate prefull event */
} ADC_DfifoPrefullNum_T;

/**
 * ADC convert machine status
 */
typedef enum
{
    INIT_STATUS                                = 0x1, /**< ADC convert is init status: after call ADC_Start */
    IDLE_STATUS,                                      /**< ADC convert is idle status: last ADC convert is finish */
    CPU_FINISH_STATUS,                                /**< ADC convert is finish status: when not set dma to convert */
    DMA_FINISH_STATUS,                                /**< ADC convert is finish status: when set dma convert */
    BUSY_STATUS,  /**< ADC convert is finish status: others is using
                                                       * ADC Devices,can not get right to use ADC Device
                                                       */
    TIMEOUT_STATUS,  /**< ADC convert is Timeout status: curret does not convert
                                                       *  finsh during the set time
                                                       */
    DMA_ERROR_STATUS,                                 /**< ADC convert is DMA error status: DMA transfer data error */
    STOP_STATUS                                       /**< ADC convert is stop status, after all ADC_Stop */
} ADC_ConvertStatus_T;

/**
 * @brief Enable ADC interrupt.
 *
 * @param    pADC ADC devive point
 *
 * @retval   None
 */
#define ADC_INTERRUPT_ENABLE(pADC, mask)                       IO_BIT_SET(pADC->IE, mask)

/**
 * @brief Disable ADC interrupt.
 *
 * @param    pADC ADC devive point
 *
 * @retval   None
 */
#define ADC_INTERRUPT_DISABLE(pADC, mask)                      IO_BIT_CLEAR(pADC->IE, mask)


/**
 * @brief Config ADC convert mode singal or repart.
 *
 * @param     pADC ADC devive point
 * @param     value ADC convert mode
 *
 * @retval    None
 */
#define CONFIG_CONTINOUS_MODE(pADC, value)                     IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_WM, value << ADC_CTRL_WM_POS)


#if defined(CHIP_GM6601)
/**
 * @brief Config ADC auto poweroff
 *
 * @param     pADC ADC devive point
 * @param     value enable or disbale
 *
 * @retval    None
 */
#define CONFIG_ATUO_POWEROFF(pADC,value)                       IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_AOFE, value << ADC_CTRL_AOFE_POS)
/**
 * @brief Enable ADC to receive analog data
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_ANALOG_ENABLE(pADC)                                IO_BIT_SET(pADC->CTRL, ADC_CTRL_AN)

/**
 * @brief Disable      ADC to receive analog data
 *
 * @param pADC         ADC devive point
 *
 * @retval None
 */
#define ADC_ANALOG_DISABLE(pADC)                               IO_BIT_CLEAR(pADC->CTRL, ADC_CTRL_AN)

/**
 * @brief     Config ADC convert resolution
 *
 * @param     pADC ADC devive point
 * @param     value ADC convert resolution
 *
 * @retval    None
 */
#define ADC_CONFIG_RESOLUTION(pADC, value)                     IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_RM, value << ADC_CTRL_RM_POS)

#elif (defined(CHIP_GM6611) || defined (CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6621) || defined(CHIP_GM6611B))

#if (defined(CHIP_GM6611) || defined (CHIP_GM6601B)||defined(CHIP_F6601C) || defined(CHIP_GM6611B))
/**
 * @brief    Select ADC hardware trigger source select
 *
 * @param    pADC ADC devive point
 * @param    Value ADC hardware trigger source
 *
 * @retval   None
 */
#define ADC_COFNIG_HW_TRIGER_SOUTCE(pADC, value)                IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_HTRIG_SEL,\
        value << ADC_CTRL_HTRIG_SEL_POS)
/**
 * @brief   Enable/Disable hw triger
 *
 * @param   pADC ADC devive point
 * @param   value Enable/Disable hw triger
 *
 * @retval  None
 */
#define ADC_COFNIG_HW_TRIGER_EN(pADC, value)                   IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_HTRIG_EN,\
        value << ADC_CTRL_HTRIG_EN_POS)
#endif

#if defined(CHIP_GM6621)
/**
 * @brief Config ADC Hardware trigger source
 *
 * @param   pADC ADC devive point
 * @param   value ADC convert resolution
 *
 * @retval  None
 */
#define ADC_COFNIG_HW_TRIGER_SOUTCE(pADC, value)               IO_BITMASK_SET(pADC->HWTRIGCTR, ADC_HWTRIG_SRCSEL,\
        value << ADC_HWTRIG_SRCSEL_POS)

/**
 * @brief Config ADC Hardware trigger enable,hardware and software trigger should one enable at the same time
 * when hardware trigger enable, software trigger will auto disable,
 * when hardware trigger disable, software trigger will auto enable.
 *
 * @param    pADC ADC devive point
 * @param    value ADC convert resolution
 *
 * @retval   None
 */
#define ADC_COFNIG_HW_TRIGER_EN(pADC, value)                    IO_BITMASK_SET(pADC->HWTRIGCTR, ADC_HWTRIG_EN,\
        value << ADC_HWTRIG_EN_POS)
/**
 * @brief Config ADC low power mode
 *
 * @param    pADC ADC devive point
 *
 * @retval   None
 */
#define CONFIG_LOW_POWERMODE_ENABLE(pADC, value)                IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_LPEN,\
        value << ADC_CTRL_LPEN_POS)
/**
 * @brief Config ADC power supply select
 *
 * @param    pADC ADC Devive Point
 * @param    value
 *  0: XAVREF is used for ADC VREFH
 *  1: VCCANA is used for ADC VREFH
 *
 * @retval None
 */
#define CONFIG_POWER_SUPPLY_SEL(pADC, value)           IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_PWSUPLY, value << ADC_CTRL_PWSUPLY_POS)

/**
 * @brief Config ADC discontinueuous mode set
 *
 * @param     pADC ADC Devive Point
 * @param     value
 *   0: disable
 *   1: enable
 *
 * @retval None
 */
#define CONFIG_DISCONTIU_MODE_SET(pADC, value)         IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_DCONTEN, value << ADC_CTRL_DCONTEN_POS)

/**
 * @brief Config ADC over sample enable.
 *
 * @param     pADC ADC devive point
 * @param     value ADC convert resolution
 *
 * @retval    None
 */
#define ADC_OV_SAMPLE_ENABLE(pADC)                IO_BIT_SET(pADC->OVSAMPCTR, ADC_OVSAPL_EN)

/**
 * @brief Config ADC over sample ratio set
 *
 * @param     pADC ADC devive point
 * @param     value ADC sample ratio, 0-7
 *
 * @retval    None
 */
#define ADC_OV_SAMPLE_RATIO(pADC, value)          IO_BITMASK_SET(pADC->OVSAMPCTR, ADC_OVSAPL_RATIO, value << ADC_OVSAPL_RATIO_POS)

/**
 * @brief Config ADC over sample shift set
 *
 * @param     pADC ADC devive point
 * @param     value ADC shift bit, 0-8
 *
 * @retval    None
 */
#define ADC_OV_SAMPLE_SHIFT(pADC, value)          IO_BITMASK_SET(pADC->OVSAMPCTR, ADC_OVSAPL_SFT, value << ADC_OVSAPL_SFT_POS)

/**
 * @brief Get ADC convert value
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_GET_CONVERT_VALUE(pADC)               (pADC->DRST & 0xffff)

#endif

/**
 * @brief Disable ADC
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_DISABLE(pADC)                                      IO_BIT_CLEAR(pADC->CTRL, ADC_CTRL_EN)

/**
 * @brief Enable ADC
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_ENABLE(pADC)                                       IO_BIT_SET(pADC->CTRL, ADC_CTRL_EN)

/**
 * @brief Disable/Enable wite when data fifo full
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */

#define ADC_CONFIG_DFIFOFW(pADC,value)                          IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_DFFWEN, value << ADC_CTRL_DFFWEN_POS)

/**
 * @brief ADC end of convert plus signal sample
 *
 * @param    pADC ADC devive point
 *
 * @retval    None
 */
#if defined(CHIP_GM6601B) ||defined(CHIP_F6601C)|| defined(CHIP_GM6611B)
#define ADC_CONFIG_SAMPEDGE(pADC,value)                         IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_EOCSMP, value << ADC_CTRL_EOCSMP_POS)
#endif
/**
 * @brief config ADC convert resolution
 *
 * @param     pADC ADC devive point
 * @param     value ADC convert resolution
 *
 * @retval    None
 */
#define ADC_CONFIG_RESOLUTION(pADC, value)                     IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_BNIT, value << ADC_CTRL_BNIT_POS)
#endif

/**
 * @brief Triger ADC to start convert
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_SW_TRIGGER(pADC)                                   IO_BIT_SET(pADC->TRIG, ADC_TRIG_CT)

/**
 * @brief Set ADC convert channel fifo depth
 *
 * @param     pADC ADC devive point
 * @param     value the depth about the channel fifo
 *
 * @retval    None
 */
#define SET_CHANNEL_FIFO_DEPTH(pADC, value)                    IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_FDTH, value << ADC_CTRL_FDTH_POS)

/**
 * @brief Enable ADC convert result match function
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_COMPARE_ENABLE(pADC)                               IO_BIT_SET(pADC->CTRL, ADC_CTRL_CE)

/**
 * @brief Disable ADC convert result match function
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_COMPARE_DISABLE(pADC)                              IO_BIT_CLEAR(pADC->CTRL, ADC_CTRL_CE)

/**
 * @brief Set ADC convert samletime
 *
 * @param     pADC ADC devive point
 * @param     value ADC convert samletime
 *
 * @retval    None
 */
#define ADC_CONFIG_SAMPLETIME(pADC, value)                     IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_STIM, value << ADC_CTRL_STIM_POS)

/**
 * @brief Set ADC convert data fifo prefull num
 *
 * @param     pADC ADC devive point
 * @param     value ADC prefull num
 *
 * @retval    None
 */
#define ADC_DFIFO_PREFULL_NUM(pADC, value)                     IO_BITMASK_SET(pADC->DMACTR, ADC_DMACTR_DFPN, value << ADC_DMACTR_DFPN_POS)

/**
 * @brief Set ADC sampel channel num index
 *
 * @param     pADC ADC devive point
 * @param     channelindex sampel channel num index
 *
 * @retval    None
 */
#define ADC_SET_CHANNEL_INDEX(pADC, channelIndex)              (pADC->CHNSEL = channelIndex)

#if defined(CHIP_GM6601B) ||defined(CHIP_F6601C)|| defined(CHIP_GM6611B) || defined(CHIP_GM6621)
#define ADC_IDLE_STATUS(pADC,value)                             IO_BITS_GET(pADC->ST,value)
#define ADC_DFIFO_EMPTY_STATUS(pADC,value)                      IO_BITS_GET(pADC->ST,value)
#endif

#if defined(CHIP_GM6621)
#define ADC_SET_EXCHEN_CHANNEL_INDEX(pADC, externalchannelIndex)              (pADC->EXCHEN = externalchannelIndex)
#endif

/**
 * @brief clear ADC interrupt accordint to bitmask
 *
 * @param     pADC ADC devive point
 * @param     Mask interrupt mask bit
 *
 * @retval    None
 */
#define ADC_CLEAR_INTERRUPT(pADC, Mask)                        (pADC->ST = (Mask))

/**
 * @brief clear ADC interrupt, overflow, eos interrupt.
 *
 * @param     pADC ADC devive point
 *
 * @retval    None
 */
#define ADC_CLEAR_ALL_INTERRUPT(pADC)                          (pADC->ST = pADC->STATUS)

/**
 * @brief clear ADC interrupt, overflow, eos interrupt.
 *
 * @param    pADC ADC devive point
 *
 * @retval   None
 */
#define ADC_SET_COMPARE_MODE(pADC, value)                      IO_BITMASK_SET(pADC->CTRL, ADC_CTRL_CM, value << ADC_CTRL_CM_POS)


/**
 * @brief Config ADC convert wheather to use DMA or not
 *
 * @param     pADC ADC devive point
 * @param     value enable or disable DMA to transfer
 *
 * @retval    None
 */
#define ADC_CONFIG_DAM_ENABLE(pADC, value)                      do{                                                         \
        IO_BITMASK_SET(pADC->DMACTR, ADC_DMACTR_DFDMAE,         \
                       value << ADC_DMACTR_DFDMAE_POS);         \
        IO_BIT_CLEAR(pADC->DMACTR, ADC_DMACTR_CDMAEN);          \
    }while(0)

/**
 * @brief Set ADC convert match value
 *
 * @param      pADC ADC  devive point
 * @param      lowvalue  low convert match value
 * @param      highvalue high convert match value
 *
 * @retval     None
 */
#define ADC_SET_COMPARE_VALUE(pADC, lowValue, highValue)        do{                                                         \
        pADC->COMPL = lowValue;                                 \
        pADC->COMPH = highValue;                                \
    }while(0)
/**
 * @brief Resert ADC ip
 * @retval    None
 */
#define ADC_RESET()                                             do{                                                         \
        Clock_ResetPeripherals(ADC_RESET,                       \
                               Hold_ResetStatusTillStable,      \
                               ADC_RESET_DELAYCNT);             \
    }while(0)    /* Reset ADC ip */

/**
 * @brief Enable ADC clock
 * @retval None
 */
#define ADC_CLOCK_ENABLE()                                      do{                                                   \
        Clock_PeripheralClockEnable(ADC_CLOCK_GATE_MASK); \
    }while(0)    /* Enable ADC ip clk */


#ifdef __cplusplus
}
#endif

#endif /*_ADC_H_*/
