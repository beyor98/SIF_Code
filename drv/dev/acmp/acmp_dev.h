/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          acmp_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2016/06/29
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/03/01, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/06/29, Mike.Zheng, V1.1.0
 *        Update coding style.
 *    2016/11/22, Mike.Zheng, V1.2.0
 *        Update For Release.
 */
#ifndef _ACMP_DEV_H_
#define _ACMP_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <drv/dev/clock.h>
#include <drv/dev/gpio.h>

/* #define ACMP_DEBUG_PRINTF */
#define ACMP_WARNING_PRINTF
#define ACMP_ERR_PRINTF

#ifndef ACMP_DEBUG_PRINTF
#define ACMP_PR_DEBUG(...)       do {} while (0)
#else
#define ACMP_PR_DEBUG            printf
#endif

#ifndef ACMP_ERR_PRINTF
#define ACMP_PR_ERR(...)         do {} while (0)
#else
#define ACMP_PR_ERR              printf
#endif

#ifndef ACMP_WARNING_PRINTF
#define ACMP_PR_WARNINR(...)     do {} while (0)
#else
#define ACMP_PR_WARNINR          printf
#endif
#if defined(CHIP_GM6611B) || defined(CHIP_F6601C)
#define ACMP_CLOCK_GATE_MASK                   (1<<13)/* ACMP clock gate maskbit */
#endif

#if defined(CHIP_GM6601)
/* ACMP register layout type definition */
typedef struct
{
    __IO uint32_t DACTR;                            /* DAC control register:offset: 0x00 */
    __IO uint32_t CS;                               /* ACMP control and status register:offset: 0x04 */
} ACMP_Device_T;

#elif (defined (CHIP_GM6601B)|| defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))

typedef struct
{
    __IO uint32_t DACTR;                            /* DAC control register:offset: 0x00 */
    __IO uint32_t CS;                               /* ACMP control and status register:offset: 0x04 */
    __IO uint32_t RESV0;                            /* Resever register:offset: 0x08 */
    __IO uint32_t FILTTHR;                          /* ACMP contral and status register:offset: 0x0c */

} ACMP_Device_T;

#elif defined (CHIP_GM6621)
/* ACMP register layout type definition */
typedef struct
{
    __IO uint32_t DACTR;                            /* DAC contral register:offset: 0x00 */
    __IO uint32_t RESV0[3];                         /* Resever register:offset: 0x04 ~0x0c*/
    __IO uint32_t ACCS;                             /* Analog comparator channel selection register:offset: 0x10 */
    __IO uint32_t ACSTM;                            /* Analog comparator setup Timing register:offset: 0x14 */
    __IO uint32_t RESV1[2];                         /* Resever register:offset: 0x14 ~0x1c*/
    __IO uint32_t AC0CS;                            /* Analog comparator 0 control & status register:offset: 0x20*/
    __IO uint32_t AC0OFS;                           /* Analog comparator 0 output filter Setting  register:offset: 0x24*/
    __IO uint32_t RESV3[2];                         /* Resever register:offset: 0x28 ~0x2c*/
    __IO uint32_t AC1CS;                            /* Analog comparator 1 control & status register:offset: 0x30 */
    __IO uint32_t AC1OFS;                           /* Analog comparator 1 output filter setting  register:offset: 0x34*/
    __IO uint32_t RESV4[2];                         /* Resever Register:offset: 0x38 ~0x3c*/
    __IO uint32_t BVCS;                             /* Battery voltage mode control & Status  Register:offset: 0x40*/
    __IO uint32_t BVL0;                             /* Battery voltage level 0~3  register:offset: 0x44*/
    __IO uint32_t BVL1;                             /* Battery voltage level 4~7  register:offset: 0x48*/
    __IO uint32_t RESV5[5];                         /* Resever register:offset: 0x4c ~0x5c*/
    __IO uint32_t LPCTR;                            /* Low power mode control register:offset: 0x60*/
    __IO uint32_t ECS;                              /* External event channel selection register:offset: 0x64*/
    __IO uint32_t RESV6[6];                         /* Resever register:offset: 0x68~ 0X7C*/
    __IO uint32_t ACMPVER;                          /* ACMP version:offset: 0x80*/

} ACMP_Device_T;
#endif

#define ACMP_BASE                                   (0x4000D000u) /* Base addresses of acmp peripheral registers */
#define ACMP_DEV                                    ((ACMP_Device_T *)ACMP_BASE) /* ACMP device pointer */

#if (defined(CHIP_GM6601) || defined(CHIP_GM6611) || defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
/*************** DAC contral register bitfiled ******************/

#define ACMP_DACTR_REFSEL_POS                       ((uint32_t) 16)   /* Select the input reference of DAC pos */
#define ACMP_DACTR_REFSEL                           ((uint32_t) ((uint32_t)0x01 << ACMP_DACTR_REFSEL_POS))/* Select the input reference of DAC filed */

#if defined(CHIP_F6601C)
#define ACMP_DACTR_DCV_POS                          ((uint32_t) 4)    /* DAC output level selection pos */
#define ACMP_DACTR_DCV                              ((uint32_t) ((uint32_t)0x3ff << ACMP_DACTR_DCV_POS))  /**
                                                                                                           * DAC output level selection filed
                                                                                                           * selects the output voltage using the given formula:
                                                                                                           * voutput=(Vin/64)x(DAC_VAL[5:0]+1)
                                                                                                           * Where the output range is from vin/64 to vin,
                                                                                                           * The step is vin/64
                                                                                                           */
#else
#define ACMP_DACTR_DCV_POS                          ((uint32_t) 8)    /* DAC output level selection pos */
#define ACMP_DACTR_DCV                              ((uint32_t) ((uint32_t)0x3f << ACMP_DACTR_DCV_POS))  /**
                                                                                                           * DAC output level selection filed
                                                                                                           * selects the output voltage using the given formula:
                                                                                                           * voutput=(Vin/1024)x(DAC_VAL[9:0]+1)
                                                                                                           * Where the output range is from vin/1024 to vin,
                                                                                                           * The step is vin/1024
                                                                                                           */

#endif
#define ACMP_DACTR_DACEN_POS                        ((uint32_t) 0)    /* DAC enable pos */
#define ACMP_DACTR_DACEN                            ((uint32_t) ((uint32_t)0x01 << ACMP_DACTR_DACEN_POS)) /* DAC enable filed */

/*************** ACMP contral and status register bitfiled ******************/
#define ACMP_CS_INT_POS                             ((uint32_t) 25)   /* ACMP interrupter flag pos */
#define ACMP_CS_INT                                 ((uint32_t) ((uint32_t)0x01 << ACMP_CS_INT_POS)) /* ACMP interrupter flag filed */

#define ACMP_CS_OPTVAL_POS                          ((uint32_t) 24)   /* ACMP output level(0 or 1) pos */
#define ACMP_CS_OPTVAL                              ((uint32_t) ((uint32_t)0x01 << ACMP_CS_OPTVAL_POS)) /* ACMP output level(0 or 1) filed */

#define ACMP_CS_INTEN_POS                           ((uint32_t) 19)   /* ACMP interrupter enable pos */
#define ACMP_CS_INTEN                               ((uint32_t) ((uint32_t)0x01 << ACMP_CS_INTEN_POS))  /* ACMP interrupter enable filed */

#define ACMP_CS_OPEN_POS                            ((uint32_t) 18)   /* ACMP output pin function enable pos */
#define ACMP_CS_OPEN                                ((uint32_t) ((uint32_t)0x01 << ACMP_CS_OPEN_POS))   /* ACMP output pin function enable pos*/

#define ACMP_CS_MOD_POS                             ((uint32_t) 16) /* ACMP Interrupt Trigger Mode pos */
#define ACMP_CS_MOD                                 ((uint32_t) ((uint32_t)0x03 << ACMP_CS_MOD_POS))    /* ACMP interrupt trigger mode filed */

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))
#define ACMP_CS_ADC_TRIGER_POS                      ((uint32_t) 13) /* ACMP triger ADC enbale pos */
#define ACMP_CS_ADC_TRIGER                          ((uint32_t) ((uint32_t)0x01 << ACMP_CS_ADC_TRIGER_POS))    /* ACMP triger ADC enbale  filed */
#define ACMP_CS_OUTINV_POS                          ((uint32_t) 12) /* ACMP output inversion pos */
#define ACMP_CS_OUTINV                              ((uint32_t) ((uint32_t)0x01 << ACMP_CS_OUTINV_POS))    /* ACMP Output inversion filed */
#endif

#define ACMP_CS_NPSEL_POS                           ((uint32_t) 10) /* Analog comparator negative input selection pos */
#define ACMP_CS_NPSEL                               ((uint32_t) ((uint32_t)0x03 << ACMP_CS_NPSEL_POS))  /* Analog comparator negative input selection filed */


#define ACMP_CS_PPSEL_POS                           ((uint32_t) 8)  /* Analog comparator positive input selection pos */
#define ACMP_CS_PPSEL                               ((uint32_t) ((uint32_t)0x03 << ACMP_CS_PPSEL_POS))  /* Analog comparator positive input selection pos */

#define ACMP_CS_EN_POS                              ((uint32_t) 0) /* ACMP modules enable pos */
#define ACMP_CS_EN                                  ((uint32_t) ((uint32_t)0x01 << ACMP_CS_EN_POS)) /* ACMP modules disable pos */

/**
 * @brief Enable one gpio pin as ACMP outputpin
 *
 * @param pACMP                 ACMP devive point
 *
 * @retval                      None
 */
#define OUTPUTPIN_ENABLE(pACMP)                     IO_BIT_SET(pACMP->CS, ACMP_CS_OPEN)

/**
 * @brief Disable one gpio pin as ACMP outputpin
 *
 * @param pACMP                 ACMP devive point
 * @param valule                enable or disable
 *
 * @retval                      None
 */
#define OUTPUTPIN_DISABLE(pACMP)                    IO_BIT_CLEAR(pACMP->CS, ACMP_CS_OPEN)

/**
 * @brief Set DAC refer voltiage
 *
 * @param pACMP                 ACMP devive point
 * @param valule                voltiage valule
 *
 * @retval                      None
 */
#define DAC_REFER_VOLTAGE(pACMP, valule)            IO_BITMASK_SET(pACMP->DACTR, ACMP_DACTR_REFSEL,     \
        (valule << ACMP_DACTR_REFSEL_POS))
/**
 * @brief Set DAC Convert Step Value
 *
 * @param pACMP                 ACMP devive point
 * @param valule                DAC convert step value
 *
 * @retval                      None
 */
#if (defined(CHIP_F6601C))
#define DAC_CONVERT_VALUE(pACMP, valule)            IO_BITMASK_SET(pACMP->DACTR, ACMP_DACTR_DCV,        \
        (valule % 1024) << ACMP_DACTR_DCV_POS)
#else
#define DAC_CONVERT_VALUE(pACMP, valule)            IO_BITMASK_SET(pACMP->DACTR, ACMP_DACTR_DCV,        \
        (valule % 64) << ACMP_DACTR_DCV_POS)
#endif
/**
 * @brief Enable DAC convert
 *
 * @param pACMP                 ACMP devive point
 * @retval                      None
 */
#define DAC_ENABLE(pACMP)                           IO_BITS_SET(pACMP->DACTR, ACMP_DACTR_DACEN)

/**
 * @brief Disable DAC convert
 *
 * @param pACMP                 ACMP devive point
 * @retval                      None
 */
#define DAC_DISABLE(pACMP)                          IO_BIT_CLEAR(pACMP->DACTR, ACMP_DACTR_DACEN)

/**
 * @brief Set pin to negative pin
 *
 * @param pACMP                 ACMP device point
 * @param pin                   Negative pin
 *
 * @retval                      None
 */

#if (defined(CHIP_GM6601B) || defined(CHIP_GM6611) || defined(CHIP_GM6611B) || defined(CHIP_F6601C))

#if defined(CHIP_GM6601B) || defined(CHIP_GM6611B) || defined(CHIP_F6601C)

/**
 * @brief Enable ACMP compare result inversion
 *
 * @param pACMP                 ACMP device point
 *
 * @retval                      None
 */
#define ACMP_OUTPUT_INVERSION_ENABLE(pACMP)         IO_BITS_SET(pACMP->CS, ACMP_CS_OUTINV)
/**
 * @brief Disable ACMP compare result inversion
 *
 * @param  pACMP                ACMP device point
 *
 * @retval                      None
 */
#define ACMP_OUTPUT_INVERSION_DISABLE(pACMP)        IO_BIT_CLEAR(pACMP->CS, ACMP_CS_OUTINV)
#endif

/**
 * @brief Config timing threshold value for the filter of analog comparator
 *
 * @param pACMP                 ACMP devive point
 * @param valule                Threshold value
 *
 * @retval                      None
 */
#define ACMP_CONFIG_FILTER_THR(pACMP, valule)       (pACMP->FILTTHR = valule)

/**
 * @brief Enable ACMP To triger ADC convert
 *
 * @param pACMP                 ACMP devive point
 * @retval                      None
 */
#define ACMP_TRIGER_ADC_ENABLE(pACMP)               IO_BITS_SET(pACMP->CS, ACMP_CS_ADC_TRIGER)

/**
 * @brief Disable ACMP to triger ADC convert
 *
 * @param pACMP                 ACMP devive point
 * @retval                      None
 */
#define ACMP_TRIGER_ADC_DISABLE(pACMP)              IO_BIT_CLEAR(pACMP->CS, ACMP_CS_ADC_TRIGER)

#endif

/*System reg config*/
#define ANA_IP_ASEL                                 0x400000a0  /* System IP asel reg address */

#define ACMP_ASEL_POS                               4 /*  ACMP anloag pin pos*/

#define ACMP_ASEL                                   ((uint32_t) ((uint32_t)0x07 << ACMP_ASEL_POS))/*  ACMP anloag pin filed*/

/**
 * @brief Config ACMP Anloag Pin As Input
 *
 * @retval                      None
 */
#define ACMP_AMA0_PIN_ENABLE()                      *((volatile uint32_t *)(ANA_IP_ASEL)) |= ACMP_ASEL

/**
 * @brief set pin to negative pin
 *
 * @param pACMP                ACMP devive point
 * @param pin                  positive pin
 *
 * @retval                     None
 */

#define SET_NEGATIVE_PIN(pACMP, pin)                IO_BITMASK_SET(pACMP->CS, ACMP_CS_NPSEL,            \
        (pin<<ACMP_CS_NPSEL_POS))
/**
 * @brief set pin to positive pin
 *
 * @param pACMP                ACMP devive point
 * @param pin                  Positive pin
 * @retval                     None
 */
#define SET_POSITIVE_PIN(pACMP, pin)                IO_BITMASK_SET(pACMP->CS, ACMP_CS_PPSEL,            \
        (pin << ACMP_CS_PPSEL_POS))
/**
 * @brief Set compare voltiage match trigger mode
 *
 * @param pACMP                ACMP devive point
 * @param triggerMode          Compare voltiage match trigger mode
 *
 * @retval                     None
 */
#define COMPARE_MATCH_TRIGGER(pACMP, triggerMode)   IO_BITMASK_SET(pACMP->CS, ACMP_CS_MOD,              \
        (triggerMode << ACMP_CS_MOD_POS))
/**
 * @brief ACMP interrupt enable
 *
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_INTERRUPT_ENABLE(pACMP)                IO_BITS_SET(pACMP->CS, ACMP_CS_INTEN)

/**
 * @brief ACMP interrupt disable
 *
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_INTERRUPT_DISABLE(pACMP)               IO_BIT_CLEAR(pACMP->CS, ACMP_CS_INTEN)

/**
 * @brief ACMP function enable
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_ENABLE(pACMP)                          IO_BITS_SET(pACMP->CS, ACMP_CS_EN)

/**
 * @brief ACMP function disable
 *
 * @param pACMP                ACMP devive point
 *
 * @retval                     None
 */
#define ACMP_DISABLE(pACMP)                         IO_BIT_CLEAR(pACMP->CS, ACMP_CS_EN)

/**
 * @brief Clear ACMP interrupt
 *
 * @param pACMP                ACMP devive point
 * @retval                     None
 */
#define ACMP_CLEAR_INTERRUPT(pACMP)                 IO_BITS_SET(pACMP->CS, ACMP_CS_INT)

/**
 * @brief Resert ACMP IP
 *
 * @retval                     None
 */
#define ACMP_RESET(void)                                do {} while (0)

/**
 * @brief Enable ACMP clock
 *
 * @retval                     None
 */
#define ACMP_CLKEN(void)                                do {} while (0)


#elif defined (CHIP_GM6621)

/*************** DAC contral register bitfiled ******************/

#define ACMP_DACTR_DETE_POS                      ((uint32_t) 24)
#define ACMP_DACTR_DETE                          ((uint32_t) ((uint32_t)0x01 << ACMP_DACTR_DETE_POS))
#define ACMP_DACTR_REFSEL_POS                    ((uint32_t) 16)   /* Select the input reference of DAC pos */
#define ACMP_DACTR_REFSEL                        ((uint32_t) ((uint32_t)0x01 << ACMP_DACTR_REFSEL_POS)) /* Select the input reference of DAC filed */

#define ACMP_DACTR_DCV_POS                       ((uint32_t) 8)    /* DAC output level Selection POS */
#define ACMP_DACTR_DCV                           ((uint32_t) ((uint32_t)0x3f << ACMP_DACTR_DCV_POS))      /**
                                                                                                           * DAC output level selection filed
                                                                                                           * Selects the output voltage using the given formula:
                                                                                                           * voutput=(vin/64)x(DAC_VAL[5:0]+1)
                                                                                                           * Where the output range is from vin/64 to vin,
                                                                                                           * The step is vin/64
                                                                                                           */


#define ACMP_DACTR_DACEN_POS                     ((uint32_t) 0)    /* DAC enable pos */
#define ACMP_DACTR_DACEN                         ((uint32_t) ((uint32_t)0x01 << ACMP_DACTR_DACEN_POS)) /* DAC enable filed */

/*************** Analog comparator channel selection register bitfiled **********************/
#define ACMP_ACCS_ASEL_POS                       ((uint32_t) 0)
#define ACMP_ACCS_ASEL                           ((uint32_t) ((uint32_t)0x1fff << ACMP_ACCS_ASEL_POS))

/***************  Analog comparator setup timing register bitfiled *************************/
#define ACMP_ACSTM_TIM_POS                       ((uint32_t) 0)
#define ACMP_ACSTM_TIM                           ((uint32_t) ((uint32_t)0x1ff << ACMP_ACSTM_TIM_POS))


/***************  Analog comparator 0 control & status register bitfiled *******************/
#define ACMP_AC0CS_OVLD_POS                      ((uint32_t) 26)
#define ACMP_AC0CS_OVLD                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_OVLD_POS))
#define ACMP_AC0CS_IS_POS                        ((uint32_t) 25)
#define ACMP_AC0CS_IS                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_IS_POS))
#define ACMP_AC0CS_OV_POS                        ((uint32_t) 24)
#define ACMP_AC0CS_OV                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_OV_POS))
#define ACMP_AC0CS_ETE_POS                       ((uint32_t) 21)
#define ACMP_AC0CS_ETE                           ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_ETE_POS))
#define ACMP_AC0CS_SAMTRIG_POS                   ((uint32_t) 20)
#define ACMP_AC0CS_SAMTRIG                       ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_SAMTRIG_POS))
#define ACMP_AC0CS_IE_POS                        ((uint32_t) 19)
#define ACMP_AC0CS_IE                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_IE_POS))
#define ACMP_AC0CS_OPEN_POS                      ((uint32_t) 18)
#define ACMP_AC0CS_OPEN                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_OPEN_POS))
#define ACMP_AC0CS_IEMODE_POS                    ((uint32_t) 16)
#define ACMP_AC0CS_IEMODE                        ((uint32_t) ((uint32_t)0x03 << ACMP_AC0CS_IEMODE_POS))
#define ACMP_AC0CS_HYSSEL_POS                    ((uint32_t) 14)
#define ACMP_AC0CS_HYSSEL                        ((uint32_t) ((uint32_t)0x03 << ACMP_AC0CS_HYSSEL_POS))
#define ACMP_AC0CS_EOEN_POS                      ((uint32_t) 13)
#define ACMP_AC0CS_EOEN                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_EOEN_POS))
#define ACMP_AC0CS_OINV_POS                      ((uint32_t) 12)
#define ACMP_AC0CS_OINV                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_OINV_POS))
#define ACMP_AC0CS_NSEL_POS                      ((uint32_t) 8)
#define ACMP_AC0CS_NSEL                          ((uint32_t) ((uint32_t)0x07 << ACMP_AC0CS_NSEL_POS))
#define ACMP_AC0CS_PSEL_POS                      ((uint32_t) 4)
#define ACMP_AC0CS_PSEL                          ((uint32_t) ((uint32_t)0x07 << ACMP_AC0CS_PSEL_POS))
#define ACMP_AC0CS_EN_POS                        ((uint32_t) 0)
#define ACMP_AC0CS_EN                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC0CS_EN_POS))

/***************  Analog comparator 0 output filter setting  register bitfiled ******************/
#define ACMP_AC0OFS_FERTHR_POS                   ((uint32_t) 0)
#define ACMP_AC0OFS_FERTHR                       ((uint32_t) ((uint32_t)0xffff << ACMP_AC0OFS_FERTHR_POS))

/***************  Analog comparator 1 control & status register *********************************/
#define ACMP_AC1CS_OVLD_POS                      ((uint32_t) 26)
#define ACMP_AC1CS_OVLD                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_OVLD_POS))
#define ACMP_AC1CS_IS_POS                        ((uint32_t) 25)
#define ACMP_AC1CS_IS                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_IS_POS))
#define ACMP_AC1CS_OV_POS                        ((uint32_t) 24)
#define ACMP_AC1CS_OV                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_OV_POS))
#define ACMP_AC1CS_ETE_POS                       ((uint32_t) 21)
#define ACMP_AC1CS_ETE                           ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_ETE_POS))
#define ACMP_AC1CS_SAMTRIG_POS                   ((uint32_t) 20)
#define ACMP_AC1CS_SAMTRIG                       ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_SAMTRIG_POS))
#define ACMP_AC1CS_IE_POS                        ((uint32_t) 19)
#define ACMP_AC1CS_IE                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_IE_POS))
#define ACMP_AC1CS_OPEN_POS                      ((uint32_t) 18)
#define ACMP_AC1CS_OPEN                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_OPEN_POS))
#define ACMP_AC1CS_IEMODE_POS                    ((uint32_t) 16)
#define ACMP_AC1CS_IEMODE                        ((uint32_t) ((uint32_t)0x03 << ACMP_AC1CS_IEMODE_POS))
#define ACMP_AC1CS_HYSSEL_POS                    ((uint32_t) 14)
#define ACMP_AC1CS_HYSSEL                        ((uint32_t) ((uint32_t)0x03 << ACMP_AC1CS_HYSSEL_POS))
#define ACMP_AC1CS_EOEN_POS                      ((uint32_t) 13)
#define ACMP_AC1CS_EOEN                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_EOEN_POS))
#define ACMP_AC1CS_OINV_POS                      ((uint32_t) 12)
#define ACMP_AC1CS_OINV                          ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_OINV_POS))
#define ACMP_AC1CS_NSEL_POS                      ((uint32_t) 8)
#define ACMP_AC1CS_NSEL                          ((uint32_t) ((uint32_t)0x07 << ACMP_AC1CS_NSEL_POS))
#define ACMP_AC1CS_PSEL_POS                      ((uint32_t) 4)
#define ACMP_AC1CS_PSEL                          ((uint32_t) ((uint32_t)0x07 << ACMP_AC1CS_PSEL_POS))
#define ACMP_AC1CS_EN_POS                        ((uint32_t) 0)
#define ACMP_AC1CS_EN                            ((uint32_t) ((uint32_t)0x01 << ACMP_AC1CS_EN_POS))

/***************  Analog comparator 1 output filter setting  register bitfiled *****************/
#define ACMP_AC1OFS_FERTHR_POS                   ((uint32_t) 0)
#define ACMP_AC1OFS_FERTHR                       ((uint32_t) ((uint32_t)0xffff << ACMP_AC1OFS_FERTHR_POS))

/***************  Battery voltage mode control & Status  register bitfiled *********************/
#define ACMP_BVCS_BVSAMDA_POS                    ((uint32_t) 24)
#define ACMP_BVCS_BVSAMDA                        ((uint32_t) ((uint32_t)0xff << ACMP_BVCS_BVSAMDA_POS))
#define ACMP_BVCS_SAMBUSY_POS                    ((uint32_t) 17)
#define ACMP_BVCS_SAMBUSY                        ((uint32_t) ((uint32_t)0x01 << ACMP_BVCS_SAMBUSY_POS))
#define ACMP_BVCS_IS_POS                         ((uint32_t) 16)
#define ACMP_BVCS_IS                             ((uint32_t) ((uint32_t)0x01 << ACMP_BVCS_IS_POS))
#define ACMP_BVCS_SAMTHR_POS                     ((uint32_t) 8)
#define ACMP_BVCS_SAMTHR                         ((uint32_t) ((uint32_t)0x1f << ACMP_BVCS_SAMTHR_POS))
#define ACMP_BVCS_BETE_POS                       ((uint32_t) 5)
#define ACMP_BVCS_BETE                           ((uint32_t) ((uint32_t)0x01 << ACMP_BVCS_BETE_POS))
#define ACMP_BVCS_IE_POS                         ((uint32_t) 4)
#define ACMP_BVCS_IE                             ((uint32_t) ((uint32_t)0x01 << ACMP_BVCS_IE_POS))
#define ACMP_BVCS_LEVELNUM_POS                   ((uint32_t) 1)
#define ACMP_BVCS_LEVELNUM                       ((uint32_t) ((uint32_t)0x07 << ACMP_BVCS_LEVELNUM_POS))
#define ACMP_BVCS_SAMTRIG_POS                    ((uint32_t) 0)
#define ACMP_BVCS_SAMTRIG                        ((uint32_t) ((uint32_t)0x01 << ACMP_BVCS_SAMTRIG_POS))

/***************  Battery voltage Level 0~3  register bitfiled ****************************/
#define ACMP_BVL0_BVL3_POS                       ((uint32_t) 24)
#define ACMP_BVL0_BVL3                           ((uint32_t) ((uint32_t)0xff << ACMP_BVL0_BVL3_POS))
#define ACMP_BVL0_BVL2_POS                       ((uint32_t) 16)
#define ACMP_BVL0_BVL2                           ((uint32_t) ((uint32_t)0xff << ACMP_BVL0_BVL2_POS))
#define ACMP_BVL0_BVL1_POS                       ((uint32_t) 8)
#define ACMP_BVL0_BVL1                           ((uint32_t) ((uint32_t)0xff << ACMP_BVL0_BVL1_POS))
#define ACMP_BVL0_BVL_POS                        ((uint32_t) 0)
#define ACMP_BVL0_BVL                            ((uint32_t) ((uint32_t)0xff << ACMP_BVL0_BVL_POS))

/***************  Battery voltage Level 4~7  register bitfiled ****************************/
#define ACMP_BVL1_BVL7_POS                       ((uint32_t) 24)
#define ACMP_BVL1_BVL7                           ((uint32_t) ((uint32_t)0xff << ACMP_BVL1_BVL7_POS))
#define ACMP_BVL1_BVL6_POS                       ((uint32_t) 16)
#define ACMP_BVL1_BVL6                           ((uint32_t) ((uint32_t)0xff << ACMP_BVL1_BVL6_POS))
#define ACMP_BVL1_BVL5_POS                       ((uint32_t) 8)
#define ACMP_BVL1_BVL5                           ((uint32_t) ((uint32_t)0xff << ACMP_BVL1_BVL5_POS))
#define ACMP_BVL1_BVL4_POS                       ((uint32_t) 0)
#define ACMP_BVL1_BVL4                           ((uint32_t) ((uint32_t)0xff << ACMP_BVL1_BVL4_POS))


/***************  Low power mode control register bitfiled ******************************/
#define ACMP_LPCTR_EN_POS                        ((uint32_t) 0)
#define ACMP_LPCTR_EN                            ((uint32_t) ((uint32_t)0x01 << ACMP_LPCTR_EN_POS))

/***************  External event channel selection register bitfiled ********************/
#define ACMP_ECS_ECSEL_POS                       ((uint32_t) 0)
#define ACMP_ECS_ECSEL                           ((uint32_t) ((uint32_t)0xfff << ACMP_ECS_ECSEL_POS))

/*************** Marco:DAC contral register  ********************************************/

#define ACMP_DAC_EVENT_TRIG_ENABLE(pACMP)       IO_BIT_SET(pACMP->DACTR, ACMP_DACTR_DETE)
#define ACMP_DAC_EVENT_TRIG_DISABLE(pACMP)      IO_BIT_CLEAR(pACMP->DACTR, ACMP_DACTR_DETE)



#define ACMP_SET_REFER_VOLTAGE(pACMP, valule)   IO_BITMASK_SET(pACMP->DACTR, ACMP_DACTR_REFSEL,     \
        (valule << ACMP_DACTR_REFSEL_POS))

#define ACMP_DAC_CONVERT_VALUE(pACMP, valule)   IO_BITMASK_SET(pACMP->DACTR, ACMP_DACTR_DCV,        \
        (valule % 64) << ACMP_DACTR_DCV_POS)

#define ACMP_DAC_ENABLE(pACMP)                  IO_BITS_SET(pACMP->DACTR, ACMP_DACTR_DACEN)
#define ACMP_DAC_DISABLE(pACMP)                 IO_BIT_CLEAR(pACMP->DACTR, ACMP_DACTR_DACEN)


/***************  Marco:Analog comparator channel selection register bitfiled ***************/

#define ACMP_ENABLE_AINPUTCHL(pACMP, value)     IO_BITMASK_SET(pACMP->ACCS, ACMP_ACCS_ASEL,          \
        (value))
/***************  Marco: Analog comparator setup timing register bitfiled ******************/
#define ACMP_CONFIG_SETUP_TIME(pACMP, valule)   IO_BITMASK_SET(pACMP->ACSTM, ACMP_ACSTM_TIM,        \
        (valule) << ACMP_ACSTM_TIM_POS)

/*************** Marco:  Analog comparator 0 control & status register bitfiled **************/


#define ACMP_IS_AC0OUTVLD_READY(pACMP)           IO_BITS_GET(pACMP->AC0CS, ACMP_AC0CS_OVLD)

#define ACMP_AC0_COMPARE_RESULT(pACMP)           (!!IO_BITS_GET(pACMP->AC0CS, ACMP_AC0CS_OV))

#define ACMP_AC0_EVENT_TRIG_ENABLE(pACMP)        IO_BITS_SET(pACMP->AC0CS, ACMP_AC0CS_ETE)

#define ACMP_AC0_EVENT_TRIG_DISABLE(pACMP)       IO_BIT_CLEAR(pACMP->AC0CS, ACMP_AC0CS_ETE)

#define ACMP_AC0_SAM_TRIG(pACMP)                 IO_BITS_SET(pACMP->AC0CS, ACMP_AC0CS_SAMTRIG)

#define ACMP_AC0_CMPIE_ENABLE(pACMP)             IO_BITS_SET(pACMP->AC0CS, ACMP_AC0CS_IE)

#define ACMP_AC0_CMPIE_DISABLE(pACMP)            IO_BIT_CLEAR(pACMP->AC0CS, ACMP_AC0CS_IE)

#define ACMP_AC0_PIN_OUTPUT_ENABLE(pACMP)        IO_BITS_SET(pACMP->AC0CS, ACMP_AC0CS_OPEN)

#define ACMP_AC0_PIN_OUTPUT_DISABLE(pACMP)       IO_BIT_CLEAR(pACMP->AC0CS, ACMP_AC0CS_OPEN)

#define ACMP_AC0_IE_MODE_CONAFIG(pACMP, value)   IO_BITMASK_SET(pACMP->AC0CS, ACMP_AC0CS_IEMODE,          \
        (value) << ACMP_AC0CS_IEMODE_POS)


#define ACMP_AC0_HYSSEL(pACMP, value)            IO_BITMASK_SET(pACMP->AC0CS, ACMP_AC0CS_HYSSEL,          \
        (value) << ACMP_AC0CS_HYSSEL_POS)


#define ACMP_AC0_EVENT_OUTPUT_ENABLE(pACMP)      IO_BITS_SET(pACMP->AC0CS, ACMP_AC0CS_EOEN)

#define ACMP_AC0_EVENT_OUTPUT_DISABLE(pACMP)     IO_BIT_CLEAR(pACMP->AC0CS, ACMP_AC0CS_EOEN)

#define ACMP_AC0_OUTPUT_INVERSION_ENBALE(pACMP)  IO_BITS_SET(pACMP->AC0CS, ACMP_AC0CS_OINV)

#define ACMP_AC0_OUTPUT_INVERSION_DISABLE(pACMP) IO_BIT_CLEAR(pACMP->AC0CS, ACMP_AC0CS_OINV)


#define ACMP_AC0_SEL_NEGATIVE_CHANNEL(pACMP, value)  \
    IO_BITMASK_SET(pACMP->AC0CS, ACMP_AC0CS_NSEL,          \
                   (value) << ACMP_AC0CS_NSEL_POS)

#define ACMP_AC0_SEL_POSITIVE_CHANNEL(pACMP, value)  \
    IO_BITMASK_SET(pACMP->AC0CS, ACMP_AC0CS_PSEL,          \
                   (value) << ACMP_AC0CS_PSEL_POS)


#define ACMP_AC0_ENABLE(pACMP)                   IO_BITS_SET(pACMP->AC0CS, ACMP_AC0CS_EN)

#define ACMP_AC0_DISABLE(pACMP)                  IO_BIT_CLEAR(pACMP->AC0CS, ACMP_AC0CS_EN)

/***************Marco:  Analog comparator 0 output filter setting  register bitfiled ******************/

#define ACMP_CONFIG_AC0O_FILTER(pACMP, value)    IO_BITMASK_SET(pACMP->AC0OFS, ACMP_AC0OFS_FERTHR,          \
        (value) << ACMP_AC0OFS_FERTHR_POS)

/*************** Marco:  Analog comparator 1 control & status register bitfiled **********************/

#define ACMP_IS_AC1OUTVLD_READY(pACMP)           IO_BITS_GET(pACMP->AC1CS, ACMP_AC1CS_OVLD)

#define ACMP_AC1_COMPARE_RESULT(pACMP)           (!!IO_BITS_GET(pACMP->AC1CS, ACMP_AC1CS_OV))

#define ACMP_AC1_EVENT_TRIG_ENABLE(pACMP)        IO_BITS_SET(pACMP->AC1CS, ACMP_AC1CS_ETE)

#define ACMP_AC1_EVENT_TRIG_DISABLE(pACMP)       IO_BIT_CLEAR(pACMP->AC1CS, ACMP_AC1CS_ETE)

#define ACMP_AC1_SAM_TRIG(pACMP)                 IO_BITS_SET(pACMP->AC1CS, ACMP_AC1CS_SAMTRIG)

#define ACMP_AC1_CMPIE_ENABLE(pACMP)             IO_BITS_SET(pACMP->AC1CS, ACMP_AC1CS_IE)

#define ACMP_AC1_CMPIE_DISABLE(pACMP)            IO_BIT_CLEAR(pACMP->AC1CS, ACMP_AC1CS_IE)

#define ACMP_AC1_PIN_OUTPUT_ENABLE(pACMP)        IO_BITS_SET(pACMP->AC1CS, ACMP_AC1CS_OPEN)

#define ACMP_AC1_PIN_OUTPUT_DISABLE(pACMP)       IO_BIT_CLEAR(pACMP->AC1CS, ACMP_AC1CS_OPEN)

#define ACMP_AC1_IE_MODE_CONAFIG(pACMP, value)   IO_BITMASK_SET(pACMP->AC1CS, ACMP_AC1CS_IEMODE,          \
        (value) << ACMP_AC1CS_IEMODE_POS)
#define ACMP_AC1_HYSSEL(pACMP, value)            IO_BITMASK_SET(pACMP->AC1CS, ACMP_AC1CS_HYSSEL,          \
        (value) << ACMP_AC1CS_HYSSEL_POS)

#define ACMP_AC1_EVENT_OUTPUT_ENABLE(pACMP)       IO_BITS_SET(pACMP->AC1CS, ACMP_AC1CS_EOEN)

#define ACMP_AC1_EVENT_OUTPUT_DISABLE(pACMP)      IO_BIT_CLEAR(pACMP->AC1CS, ACMP_AC1CS_EOEN)

#define ACMP_AC1_OUTPUT_INVERSION_ENBALE(pACMP)   IO_BITS_SET(pACMP->AC1CS, ACMP_AC1CS_OINV)

#define ACMP_AC1_OUTPUT_INVERSION_DISABLE(pACMP)  IO_BIT_CLEAR(pACMP->AC1CS, ACMP_AC1CS_OINV)


#define ACMP_AC1_SEL_NEGATIVE_CHANNEL(pACMP, value)  IO_BITMASK_SET(pACMP->AC1CS, ACMP_AC1CS_NSEL,          \
        (value) << ACMP_AC1CS_NSEL_POS)

#define ACMP_AC1_SEL_POSITIVE_CHANNEL(pACMP, value)  IO_BITMASK_SET(pACMP->AC1CS, ACMP_AC1CS_PSEL,          \
        (value) << ACMP_AC1CS_PSEL_POS)

#define ACMP_AC1_ENABLE(pACMP)                    IO_BITS_SET(pACMP->AC1CS, ACMP_AC1CS_EN)

#define ACMP_AC1_DISABLE(pACMP)                   IO_BIT_CLEAR(pACMP->AC1CS, ACMP_AC1CS_EN)

/***************Marco:  Analog comparator 1 output filter setting  register bitfiled ******************/

#define ACMP_CONFIG_AC1O_FILTER(pACMP, value)     IO_BITMASK_SET(pACMP->AC1OFS, ACMP_AC1OFS_FERTHR,          \
        (value) << ACMP_AC1OFS_FERTHR_POS)

/***************Marco:  Battery voltage mode control & status  register bitfiled ******************/

#define ACMP_GET_BV_SAMPLE_VALUE(pACMP)           (IO_BITS_GET(pACMP->BVCS, ACMP_BVCS_BVSAMDA) \
        >> ACMP_BVCS_BVSAMDA_POS)


#define ACMP_IS_BV_SAMPLE_READY(pACMP)            !(IO_BITS_GET(pACMP->BVCS, ACMP_BVCS_SAMBUSY) \
        >> ACMP_BVCS_SAMBUSY_POS)

#define ACMP_CONFIG_BV_FILTER_THR(pACMP, value)   IO_BITMASK_SET_SPECAIL_ST(pACMP->BVCS, ACMP_BVCS_SAMTHR,          \
        (value) << ACMP_BVCS_SAMTHR_POS)

#define ACMP_BVD_EVENT_TRIG_ENABLE(pACMP)         IO_BITS_SET_SPECAIL_ST(pACMP->BVCS, ACMP_BVCS_BETE)

#define ACMP_BVD_EVENT_TRIG_DISABLE(pACMP)        IO_BIT_CLEAR(pACMP->BVCS, ACMP_BVCS_BETE)


#define ACMP_BV_IE_ENABLE(pACMP)                  IO_BITS_SET_SPECAIL_ST(pACMP->BVCS, ACMP_BVCS_IE)

#define ACMP_BV_IE_DISABLE(pACMP)                 IO_BIT_CLEAR(pACMP->BVCS, ACMP_BVCS_IE)


#define ACMP_BV_CONFIG_LEVEL(pACMP, value)        IO_BITMASK_SET_SPECAIL_ST(pACMP->BVCS, ACMP_BVCS_LEVELNUM,          \
        (value) << ACMP_BVCS_LEVELNUM_POS)

#define ACMP_BV_SAMPLE_TRIG(pACMP)                IO_BITS_SET(pACMP->BVCS, ACMP_BVCS_SAMTRIG)

/***************Marco:   Battery voltage Level 0~3  register bitfiled ******************/


#define ACMP_CONFIG_BV_LEVEL3(pACMP, value)       IO_BITMASK_SET(pACMP->BVL0, ACMP_BVL0_BVL3,          \
        (value) << ACMP_BVL0_BVL3_POS)


#define ACMP_CONFIG_BV_LEVEL2(pACMP, value)       IO_BITMASK_SET(pACMP->BVL0, ACMP_BVL0_BVL2,          \
        (value) << ACMP_BVL0_BVL2_POS)


#define ACMP_CONFIG_BV_LEVEL1(pACMP, value)       IO_BITMASK_SET(pACMP->BVL0, ACMP_BVL0_BVL1,          \
        (value) << ACMP_BVL0_BVL1_POS)

#define ACMP_CONFIG_BV_LEVEL0(pACMP, value)       IO_BITMASK_SET(pACMP->BVL0, ACMP_BVL0_BVL,          \
        (value) << ACMP_BVL0_BVL_POS)

#define ACMP_CONFIG_BV_LEVEL7(pACMP, value)       IO_BITMASK_SET(pACMP->BVL1, ACMP_BVL1_BVL7,          \
        (value) << ACMP_BVL1_BVL7_POS)

/***************Marco:   Battery voltage level 4~7  register bitfiled ******************/

#define ACMP_CONFIG_BV_LEVEL6(pACMP, value)       IO_BITMASK_SET(pACMP->BVL1, ACMP_BVL1_BVL6,          \
        (value) << ACMP_BVL1_BVL6_POS)

#define ACMP_CONFIG_BV_LEVEL5(pACMP, value)       IO_BITMASK_SET(pACMP->BVL1, ACMP_BVL1_BVL5,          \
        (value) << ACMP_BVL1_BVL5_POS)

#define ACMP_CONFIG_BV_LEVEL4(pACMP, value)       IO_BITMASK_SET(pACMP->BVL1, ACMP_BVL1_BVL4,          \
        (value) << ACMP_BVL1_BVL4_POS)

/***************Marco:Low power mode control register bitfiled **********************/

#define ACMP_LOWPOWER_ENABLE(pACMP)               IO_BITS_SET(pACMP->LPCTR, ACMP_LPCTR_EN)

#define ACMP_LOWPOWER_DISABLE(pACMP)              IO_BIT_CLEAR(pACMP->LPCTR, ACMP_LPCTR_EN)


/***************Marco: External event channel selection register bitfiled **********/

#define ACMP_SEL_PES_CHANNEL(pACMP, value)        IO_BITMASK_SET(pACMP->ECS,ACMP_ECS_ECSEL,             \
        (1<<value) << ACMP_ECS_ECSEL_POS)

/**
 * @brief Resert ACMP IP
 *
 * @retval               None
 */
#define ACMP_RESET()                                do {} while (0)

/**
 * @brief Enable ACMP clock
 *
 * @retval               None
 */
#define ACMP_CLKEN()                                do {} while (0)

#endif

#ifdef __cplusplus
}

#endif

#endif /*_ACMP_DEV_H_*/
