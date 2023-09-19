/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          IRTX_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2017/04/05
 *
 * @brief         CMSIS Peripheral Access Layer for GM6621
 *
 * @note
 *                2017/04/05, Mike.Zheng, V1.0.0
 *                    Initial version.
 *                2018/02/28, Edward.Zhu, V1.1.0
 *                    Update coding style.
 */

#ifndef _IRTX_DEV_H_
#define _IRTX_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <drv/dev/clock.h>

#ifndef IRTX_DEBUG_PRINTF
#define IRTX_PR_DEBUG(...)      do {} while (0)
#else
#define IRTX_PR_DEBUG           printf
#endif

#ifndef IRTX_ERR_PRINTF
#define IRTX_PR_ERR(...)        do {} while (0)
#else
#define IRTX_PR_ERR             printf
#endif

/* IRTX Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CTRL0;                       /*  IRTX configuration register0:offset: 0x0 */
    __IO uint32_t CTRL1;                       /*  IRTX configuration register1:offset: 0x4 */
    __IO uint32_t CTRL2;                       /*  IRTX configuration register2:offset: 0x8 */
    __IO uint32_t CTRL3;                       /*  IRTX configuration register3:offset: 0xc */
    __IO uint32_t TM0;                         /*  IRTX time length register0:offset: 0x10 */
    __IO uint32_t TM1;                         /*  IRTX time length register1:offset: 0x14 */
    __IO uint32_t TM2;                         /*  IRTX time length register2:offset: 0x18 */
    __IO uint32_t TM3;                         /*  IRTX time length register3:offset: 0x1c */
    __IO uint32_t TM4;                         /*  IRTX time length register4:offset: 0x20 */
    __IO uint32_t FIFO;                        /*  IRTX FIFO data FIFO register:offset: 0x24 */
    __IO uint32_t IE;                          /*  IRTX interrupt mask register:offset: 0x28 */
    __IO uint32_t ST;                          /*  IRTX interrupt flag register:offset: 0x2C */
    __IO uint32_t INTCLR;                      /*  IRTX interrupt clear register:offset: 0x30 */
    __IO uint32_t FIFOST;                      /*  IRTX data FIFO status register:offset: 0x34 */
    __IO uint32_t PWMCTR;                      /*  IRTX PWM control register:offset: 0x38 */
    __IO uint32_t PES;                         /*  IRTX event system control register:offset: 0x3C */
    __IO uint32_t IRMAN;                       /*  IRTX manual mode control register:offset: 0x40 */
    __IO uint32_t RESV[47];                    /*  IRTX reserver register:offset: 0x44-0xF8*/
    __IO uint32_t XVER;                        /*  IRTX version register:offset: 0xFC */

} IRTX_Device_T;

/**
 * gree carrier config
 * dutyRatio:30%
 */
typedef struct
{
    uint32_t waveFrequency;     /* kHz */
    uint32_t srcFrequency;      /* Mhz */
    uint32_t dutyRatio;         /* gree carrier dutyRatio 30% */
} IRTX_CARRIER_CONFIG_T;

typedef enum
{
    NO_BIT_MASK                 = 0x0,    /* Not mask data bit */
    ALIGN_BIT_MASK,                       /* Align mask data bit */
    ALL_BIT_MASK                          /* All data bit mask */
} IRTX_DATA_BITMASK_T;

typedef enum
{
    HIGHEST_FIRST               = 0x0,    /* The length of the highest level data had been sent first */
    LOWEST_FIRST                          /* The length of the lowest level data had been sent first */
} IRTX_DATA_SEND_SEQ_T;

typedef enum
{
    NOARMAL_MODE               = 0x0,    /* Normal IR mode */
    MANUAL_MODE,                         /* Manual IR mode */
    TIME_MODE                            /* Timer mode */
} IRTX_WORK_MODE_T;

#define IRTX_BASE                                (0x40038000u) /* Base Addresses of IRTX Registers */
#define IRTX_DEV                                 ((IRTX_Device_T *)IRTX_BASE) /* IRTX Device Pointer */

#define IRTX_UNLOCKED                            0x0    /* IRTX Lockstus : Unlock */
#define IRTX_LOCKED                              0x1    /* IRTX Lockstus : Lock */

/*************** IRTX Configuration0 Register Bitfiled ******************/
#define IRTX_CTRL0_KST_POS                       ((uint32_t) 0) /* IRTX KST pos */
#define IRTX_CTRL0_KST                           ((uint32_t) ((uint32_t)1 << IRTX_CTRL0_KST_POS))     /* IRTX KST filed */

/*************** IRTX Configuration1 Register Bitfiled ******************/
#define IRTX_CTRL1_CWEN_POS                      ((uint32_t) 18) /* IRTX carrier waves output enable pos */
#define IRTX_CTRL1_CWEN                          ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_CWEN_POS))     /* IRTX carrier waves output enable filed */
#define IRTX_CTRL1_PHEADLCWEN_POS                ((uint32_t) 17) /* IRTX POL_HEADL carrier waves output pos */
#define IRTX_CTRL1_PHEADLCWEN                    ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_PHEADLCWEN_POS))     /* IRTX POL_HEADL carrier waves output filed */
#define IRTX_CTRL1_PHEADHCWEN_POS                ((uint32_t) 16) /* IRTX POL_HEADH carrier waves output pos */
#define IRTX_CTRL1_PHEADHCWEN                    ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_PHEADHCWEN_POS))     /* IRTX POL_HEADH carrier waves output filed */

#define IRTX_CTRL1_PFNINTERCWEN_POS              ((uint32_t) 15) /* IRTX POL_FNINTER carrier waves output pos */
#define IRTX_CTRL1_PFNINTERCWEN                  ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_PFNINTERCWEN_POS))     /* IRTX POL_FNINTER carrier waves output filed */
#define IRTX_CTRL1_PENDPOSCWEN_POS               ((uint32_t) 14) /* IRTX POL_POS carrier waves output pos */
#define IRTX_CTRL1_PENDPOSCWEN                   ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_PENDPOSCWEN_POS))     /* IRTX POL_POS carrier waves output filed */
#define IRTX_CTRL1_PINTERCWEN_POS                ((uint32_t) 13) /* IRTX POL_INTER carrier waves output pos */
#define IRTX_CTRL1_PINTERCWEN                    ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_PINTERCWEN_POS))     /* IRTX POL_INTER carrier waves output filed */
#define IRTX_CTRL1_PENDPRECWEN_POS               ((uint32_t) 12) /* IRTX POL_PRE carrier waves output pos */
#define IRTX_CTRL1_PENDPRECWEN                   ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_PENDPRECWEN_POS))     /* IRTX POL_PRE carrier waves output filed */
/* software dont care it*/
#define IRTX_CTRL1_STOPSEL_POS                   ((uint32_t) 11) /* IRTX carrier waves integritily output  pos */
#define IRTX_CTRL1_STOPSEL                       ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_STOPSEL_POS))     /* IRTX carrier waves integritily output filed */
/* software dont care it*/
#define IRTX_CTRL1_PHASE_POS                     ((uint32_t) 10) /* IRTX carrier signal phase pos */
#define IRTX_CTRL1_PHASE                         ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_PHASE_POS))     /* IRTX carrier signal phase filed */
#define IRTX_CTRL1_INTERFACE_POS                 ((uint32_t) 8) /* IRTX INTERFACE pos */
#define IRTX_CTRL1_INTERFACE                     ((uint32_t) ((uint32_t)3 << IRTX_CTRL1_INTERFACE_POS))     /* IRTX INTERFACE filed */
#define IRTX_CTRL1_INV_POS                       ((uint32_t) 7) /* IRTX INV pos */
#define IRTX_CTRL1_INV                           ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_INV_POS))     /* IRTX INV filed */
#define IRTX_CTRL1_CONTINUE_POS                  ((uint32_t) 6) /* IRTX continue sending next set of IR code or not pos */
#define IRTX_CTRL1_CONTINUE                      ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_CONTINUE_POS))     /* IRTX continue sending next set of IR code or not filed */
#define IRTX_CTRL1_LOWFIRST_POS                  ((uint32_t) 5) /* IRTX send the lowest or highest bit first Pos */
#define IRTX_CTRL1_LOWFIRST                      ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_LOWFIRST_POS))     /* IRTX send the lowest or highest bit first filed */
#define IRTX_CTRL1_DATO_POS                      ((uint32_t) 4) /* DATA0 control IR_OUT pin pos */
#define IRTX_CTRL1_DATO                          ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_DATO_POS))     /* DATA0 control IR_OUT pin filed */
#define IRTX_CTRL1_OEN_POS                       ((uint32_t) 3) /* IR_OUT pin is controlled by IRTX module or DATA0 pos */
#define IRTX_CTRL1_OEN                           ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_OEN_POS))     /* IR_OUT pin is controlled by IRTX module or DATA0 filed */
#define IRTX_CTRL1_MODE_POS                      ((uint32_t) 1) /* IRTX work mode select pos */
#define IRTX_CTRL1_MODE                          ((uint32_t) ((uint32_t)3 << IRTX_CTRL1_MODE_POS))     /* IRTX work mode select filed */
#define IRTX_CTRL1_EN_POS                        ((uint32_t) 0) /* IRTX module enable pos */
#define IRTX_CTRL1_EN                            ((uint32_t) ((uint32_t)1 << IRTX_CTRL1_EN_POS))     /* IRTX module enable filed */

/*************** IRTX Configuration2 Register Bitfiled ******************/
#define IRTX_CTRL2_FNINTEREN_POS                 ((uint32_t) 7) /* IRTX FNINTER stage enable pos */
#define IRTX_CTRL2_FNINTEREN                     ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_FNINTEREN_POS))    /* IRTX FNINTER stage enable filed */
#define IRTX_CTRL2_ENDPOSEN_POS                  ((uint32_t) 6) /* IRTX ENDPOS stage enable pos */
#define IRTX_CTRL2_ENDPOSEN                      ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_ENDPOSEN_POS))    /* IRTX ENDPOS stage enable filed */
#define IRTX_CTRL2_POSDATEN_POS                  ((uint32_t) 5) /* IRTX POSDAT stage enable pos */
#define IRTX_CTRL2_POSDATEN                      ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_POSDATEN_POS))    /* IRTX POSDAT stage enable filed */
#define IRTX_CTRL2_INTERVALEN_POS                ((uint32_t) 4) /* IRTX INTERVAL stage enable pos */
#define IRTX_CTRL2_INTERVALEN                    ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_INTERVALEN_POS))   /* IRTX INTERVAL stage enable filed */
#define IRTX_CTRL2_ENDPREEN_POS                  ((uint32_t) 3) /* IRTX ENDPRE stage enable pos */
#define IRTX_CTRL2_ENDPREEN                      ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_ENDPREEN_POS))    /* IRTX ENDPRE stage enable filed */
#define IRTX_CTRL2_PREDATEN_POS                  ((uint32_t) 2) /* IRTX PREDAT stage enable pos */
#define IRTX_CTRL2_PREDATEN                      ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_PREDATEN_POS))    /* IRTX PREDAT stage enable filed */
#define IRTX_CTRL2_HEADLEN_POS                   ((uint32_t) 1) /* IRTX HEADL stage enable pos */
#define IRTX_CTRL2_HEADLEN                       ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_HEADLEN_POS))    /* IRTX HEADL stage enable filed */
#define IRTX_CTRL2_HEADHEN_POS                   ((uint32_t) 0) /* IRTX HEADH stage enable pos */
#define IRTX_CTRL2_HEADHEN                       ((uint32_t) ((uint32_t)1 << IRTX_CTRL2_HEADHEN_POS))   /* IRTX HEADH stage enable filed */

/*************** IRTX Configuration3 Register Bitfiled ******************/
#define IRTX_CTRL3_INTERVALLENGTH_POS            ((uint32_t) 16) /* The length of interval low level pos */
#define IRTX_CTRL3_INTERVALLENGTH                ((uint32_t) ((uint32_t)0xffff << IRTX_CTRL3_INTERVALLENGTH_POS))    /* The length of interval low level filed */
/* REAL SEND NUM -1 */
#define IRTX_CTRL3_POSDATABITSNUM_POS            ((uint32_t) 8) /* The amount bits of second part data - 1 pos */
#define IRTX_CTRL3_POSDATABITSNUM                ((uint32_t) ((uint32_t)0xff << IRTX_CTRL3_POSDATABITSNUM_POS))    /* The amount bits of second part data - 1 filed */
/* REAL SEND NUM -1 */
#define IRTX_CTRL3_PREDATABITSNUM_POS            ((uint32_t) 0) /* The amount bits of the first part data - 1 pos */
#define IRTX_CTRL3_PREDATABITSNUM                ((uint32_t) ((uint32_t)0xff << IRTX_CTRL3_PREDATABITSNUM_POS))    /* The amount bits of the first part data - 1 filed */

/*************** IRTX TIME0  Register Bitfiled ******************/
#define IRTX_TIME0_HEADHLENGTH_POS               ((uint32_t) 16) /* The length of head code's high level pos */
#define IRTX_TIME0_HEADHLENGTH                   ((uint32_t) ((uint32_t)0xffff << IRTX_TIME0_HEADHLENGTH_POS))   /* The length of head code's high level pos */
#define IRTX_TIME0_HEADLLENGTH_POS               ((uint32_t) 0) /* The length of head code's low level pos */
#define IRTX_TIME0_HEADLLENGTH                   ((uint32_t) ((uint32_t)0xffff << IRTX_TIME0_HEADLLENGTH_POS))    /* The length of head code's low level filed */

/*************** IRTX TIME1  Register Bitfiled ******************/
#define IRTX_TIME1_DATA0HLENGTH_POS              ((uint32_t) 16) /* The length of DATA0 code's high level pos */
#define IRTX_TIME1_DATA0HLENGTH                  ((uint32_t) ((uint32_t)0xffff << IRTX_TIME1_DATA0HLENGTH_POS))    /* The length of DATA0 code's low level filed */
#define IRTX_TIME1_DATA0LLENGTH_POS              ((uint32_t) 0) /* The length of DATA0 code's low level pos */
#define IRTX_TIME1_DATA0LLENGTH                  ((uint32_t) ((uint32_t)0xffff << IRTX_TIME1_DATA0LLENGTH_POS))

/*************** IRTX TIME2  Register Bitfiled ******************/
#define IRTX_TIME2_DATA1HLENGTH_POS              ((uint32_t) 16) /* The length of DATA1 code's high level pos */
#define IRTX_TIME2_DATA1HLENGTH                  ((uint32_t) ((uint32_t)0xffff << IRTX_TIME2_DATA1HLENGTH_POS))    /* The length of DATA1 code's high level filed */
#define IRTX_TIME2_DATA1LLENGTH_POS              ((uint32_t) 0) /* The length of DATA1 code's low level pos */
#define IRTX_TIME2_DATA1LLENGTH                  ((uint32_t) ((uint32_t)0xffff << IRTX_TIME2_DATA1LLENGTH_POS))    /* The length of DATA1 code's low level filed */

/*************** IRTX TIME3  Register Bitfiled ******************/
#define IRTX_TIME3_ENDPOSENGTH_POS               ((uint32_t) 16) /* The high level length of second part data's ending bit code pos */
#define IRTX_TIME3_ENDPOSENGTH                   ((uint32_t) ((uint32_t)0xffff << IRTX_TIME3_ENDPOSENGTH_POS))    /* The high level length of second part data's ending bit code filed */
#define IRTX_TIME3_ENDPREENGTH_POS               ((uint32_t) 0) /* The high level length of the first part data's ending bit code pos */
#define IRTX_TIME3_ENDPREENGTH                   ((uint32_t) ((uint32_t)0xffff << IRTX_TIME3_ENDPREENGTH_POS))    /* The high level length of the first part data's ending bit code filed */


/*************** IRTX TIME4  Register Bitfiled ******************/
#define IRTX_TIME4_PRESCALE_POS                  ((uint32_t) 24) /* Prescaler of the multi length between FNINTERVAL and INTERVAL pos */
#define IRTX_TIME4_PRESCALE                      ((uint32_t) ((uint32_t)0xff << IRTX_TIME4_PRESCALE_POS))    /* Prescaler of the multi length between FNINTERVAL and INTERVAL filed */
#define IRTX_TIME4_ENDPREENGTH_POS               ((uint32_t) 0) /* The interval low level length between two set of IR code pos */
#define IRTX_TIME4_ENDPREENGTH                   ((uint32_t) ((uint32_t)0xffff << IRTX_TIME4_ENDPREENGTH_POS))    /* The interval low level length between two set of IR code filed */

/***************IRTX Interrupt Mask Register Bitfiled ******************/
#define IRTX_IE_TIME_POS                         ((uint32_t) 13) /* Timer overflow interrupt enable pos */
#define IRTX_IE_TIME                             ((uint32_t) ((uint32_t)1 << IRTX_IE_TIME_POS)) /* Timer overflow interrupt enable filed */
#define IRTX_IE_FIFOTRIG_POS                     ((uint32_t) 12) /* FIFO entry level interrupt enable pos */
#define IRTX_IE_FIFOTRIG                         ((uint32_t) ((uint32_t)1 << IRTX_IE_FIFOTRIG_POS)) /* FIFO entry level interrupt enable filed */
#define IRTX_IE_DONE_POS                         ((uint32_t) 11) /* IR code had been finished interrupt enable pos */
#define IRTX_IE_DONE                             ((uint32_t) ((uint32_t)1 << IRTX_IE_DONE_POS)) /* IR code had been finished interrupt enable filed */
#define IRTX_IE_EMPTY_POS                        ((uint32_t) 10) /* Data FIFO empty interrupt enable pos */
#define IRTX_IE_EMPTY                            ((uint32_t) ((uint32_t)1 << IRTX_IE_EMPTY_POS)) /* Data FIFO empty interrupt enable filed */
#define IRTX_IE_FULL_POS                         ((uint32_t) 9) /* Data FIFO full interrupt enable pos */
#define IRTX_IE_FULL                             ((uint32_t) ((uint32_t)1 << IRTX_IE_FULL_POS)) /* Data FIFO full interrupt enable filed */
#define IRTX_IE_DATBIT_POS                       ((uint32_t) 8) /* A data bit had finished sending interrupt enable pos */
#define IRTX_IE_DATBIT                           ((uint32_t) ((uint32_t)1 << IRTX_IE_DATBIT_POS)) /* A data bit had finished sending interrupt enable filed */
#define IRTX_IE_FNINTER_POS                      ((uint32_t) 7) /* The FNINTER stage had finished interrupt enable pos */
#define IRTX_IE_FNINTER                          ((uint32_t) ((uint32_t)1 << IRTX_IE_FNINTER_POS)) /* The FNINTER stage had finished interrupt enable filed */
#define IRTX_IE_ENDPOS_POS                       ((uint32_t) 6) /* The ENDPOS stage had finished interrupt enable pos */
#define IRTX_IE_ENDPOS                           ((uint32_t) ((uint32_t)1 << IRTX_IE_ENDPOS_POS)) /* The ENDPOS stage had finished interrupt enable filed */
#define IRTX_IE_POSDAT_POS                       ((uint32_t) 5) /* The PREDAT stage had finished interrupt enable pos */
#define IRTX_IE_POSDAT                           ((uint32_t) ((uint32_t)1 << IRTX_IE_POSDAT_POS)) /* The PREDAT stage had finished interrupt enable filed */
#define IRTX_IE_INTER_POS                        ((uint32_t) 4) /* The INTER stage had finished interrupt enable pos */
#define IRTX_IE_INTER                            ((uint32_t) ((uint32_t)1 << IRTX_IE_INTER_POS)) /* The INTER stage had finished interrupt enable filed */
#define IRTX_IE_ENDPRE_POS                       ((uint32_t) 3) /* The ENDPRE stage had finished interrupt enable pos */
#define IRTX_IE_ENDPRE                           ((uint32_t) ((uint32_t)1 << IRTX_IE_ENDPRE_POS)) /* The ENDPRE stage had finished interrupt enable filed */
#define IRTX_IE_PREDAT_POS                       ((uint32_t) 2) /* The PREDAT stage had finished interrupt enable pos */
#define IRTX_IE_PREDAT                           ((uint32_t) ((uint32_t)1 << IRTX_IE_PREDAT_POS)) /* The PREDAT stage had finished interrupt enable filed */
#define IRTX_IE_HEADL_POS                        ((uint32_t) 1) /* The HEADL stage had finished interrupt enable pos */
#define IRTX_IE_HEADL                            ((uint32_t) ((uint32_t)1 << IRTX_IE_HEADL_POS)) /* The HEADL stage had finished interrupt enable filed */
#define IRTX_IE_HEADH_POS                        ((uint32_t) 0) /* The HEADH stage had finished interrupt enable pos */
#define IRTX_IE_HEADH                            ((uint32_t) ((uint32_t)1 << IRTX_IE_HEADH_POS)) /* The HEADH stage had finished interrupt enable filed */

/***************IRTX Interrupt Status Register Bitfiled ******************/
#define IRTX_ST_INTANY_POS                       ((uint32_t) 31) /* Whether interrupt had happened or not pos */
#define IRTX_ST_INTANY                           ((uint32_t) ((uint32_t)1 << IRTX_ST_INTANY_POS))    /* Whether interrupt had happened or not filed */
#define IRTX_ST_TIME_POS                         ((uint32_t) 13) /* Whether timer is overflow or not in timer mode pos */
#define IRTX_ST_TIME                             ((uint32_t) ((uint32_t)1 << IRTX_ST_TIME_POS)) /* Whether timer is overflow or not in timer mode filed */
#define IRTX_ST_FIFOTRIG_POS                     ((uint32_t) 12) /* The fifo entry level is under or above the user defined value pos */
#define IRTX_ST_FIFOTRIG                         ((uint32_t) ((uint32_t)1 << IRTX_ST_FIFOTRIG_POS)) /* The fifo entry level is under or above the user defined value filed */
#define IRTX_ST_DONE_POS                         ((uint32_t) 11) /* Whether IR code have been finished or not pos */
#define IRTX_ST_DONE                             ((uint32_t) ((uint32_t)1 << IRTX_ST_DONE_POS)) /* Whether IR code have been finished or not filed */
#define IRTX_ST_EMPTY_POS                        ((uint32_t) 10) /* Whether the data FIFO is empty or not pos */
#define IRTX_ST_EMPTY                            ((uint32_t) ((uint32_t)1 << IRTX_ST_EMPTY_POS)) /* Whether the data FIFO is empty or not filed */
#define IRTX_ST_FULL_POS                         ((uint32_t) 9) /* Whether the data FIFO is full or not pos */
#define IRTX_ST_FULL                             ((uint32_t) ((uint32_t)1 << IRTX_ST_FULL_POS)) /* Whether the data FIFO is full or not filed */
#define IRTX_ST_DATBIT_POS                       ((uint32_t) 8) /* Whether a data bit had finished sending or not pos */
#define IRTX_ST_DATBIT                           ((uint32_t) ((uint32_t)1 << IRTX_ST_DATBIT_POS)) /* Whether a data bit had finished sending or not filed */
#define IRTX_ST_FNINTER_POS                      ((uint32_t) 7) /* Whether the FNINTER stage had finished or not pos */
#define IRTX_ST_FNINTER                          ((uint32_t) ((uint32_t)1 << IRTX_ST_FNINTER_POS)) /* Whether the FNINTER stage had finished or not filed */
#define IRTX_ST_ENDPOS_POS                       ((uint32_t) 6) /* Whether the ENDPOS stage had finished or not pos */
#define IRTX_ST_ENDPOS                           ((uint32_t) ((uint32_t)1 << IRTX_ST_ENDPOS_POS)) /* Whether the ENDPOS stage had finished or not filed */
#define IRTX_ST_POSDAT_POS                       ((uint32_t) 5) /* Whether the POSDAT stage had finished or not pos */
#define IRTX_ST_POSDAT                           ((uint32_t) ((uint32_t)1 << IRTX_ST_POSDAT_POS)) /* Whether the POSDAT stage had finished or not filed */
#define IRTX_ST_INTER_POS                        ((uint32_t) 4) /* Whether the INTER stage had finished or not pos */
#define IRTX_ST_INTER                            ((uint32_t) ((uint32_t)1 << IRTX_ST_INTER_POS)) /* Whether the INTER stage had finished or not filed */
#define IRTX_ST_ENDPRE_POS                       ((uint32_t) 3) /* Whether the ENDPRE stage had finished or not pos */
#define IRTX_ST_ENDPRE                           ((uint32_t) ((uint32_t)1 << IRTX_ST_ENDPRE_POS)) /* Whether the ENDPRE stage had finished or not filed */
#define IRTX_ST_PREDAT_POS                       ((uint32_t) 2) /* Whether the PREDAT stage had finished or not pos */
#define IRTX_ST_PREDAT                           ((uint32_t) ((uint32_t)1 << IRTX_ST_PREDAT_POS)) /* Whether the PREDAT stage had finished or not filed */
#define IRTX_ST_HEADL_POS                        ((uint32_t) 1) /* Whether the HEADL stage had finished or not pos */
#define IRTX_ST_HEADL                            ((uint32_t) ((uint32_t)1 << IRTX_ST_HEADL_POS)) /* Whether the HEADL stage had finished or not filed */
#define IRTX_ST_HEADH_POS                        ((uint32_t) 0) /* Whether the HEADH stage had finished or not pos */
#define IRTX_ST_HEADH                            ((uint32_t) ((uint32_t)1 << IRTX_ST_HEADH_POS)) /* Whether the HEADH stage had finished or not filed */

/***************IRTX Clear Interrupt Status Register Bitfiled ******************/

#define IRTX_INTCLR_INTANY_POS                   ((uint32_t) 31) /* Clear interrupt flag pos */
#define IRTX_INTCLR_INTANY                       ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_INTANY_POS))    /* Clear interrupt filed */
#define IRTX_INTCLR_TIME_POS                     ((uint32_t) 13) /* Timer overflow interrupt flag clear pos */
#define IRTX_INTCLR_TIME                         ((uint32_t) ((uint32_t)1 << IRTX_ST_TIME_POS)) /* Timer overflow interrupt clear flag filed */
#define IRTX_INTCLR_FIFOTRIG_POS                 ((uint32_t) 12) /* FIFO entry level interrupt flag clear pos */
#define IRTX_INTCLR_FIFOTRIG                     ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_FIFOTRIG_POS)) /* FIFO entry level interrupt flag clear filed */
#define IRTX_INTCLR_DONE_POS                     ((uint32_t) 11) /* IR code finished interrupt flag clear pos */
#define IRTX_INTCLR_DONE                         ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_DONE_POS)) /* IR code finished interrupt flag clear filed */
#define IRTX_INTCLR_EMPTY_POS                    ((uint32_t) 10) /* Data FIFO empty interrupt flag clear pos */
#define IRTX_INTCLR_EMPTY                        ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_EMPTY_POS)) /* Data FIFO empty interrupt flag clear filed */
#define IRTX_INTCLR_FULL_POS                     ((uint32_t) 9) /* Data FIFO full interrupt flag clear pos */
#define IRTX_INTCLR_FULL                         ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_FULL_POS)) /* Data FIFO empty interrupt flag clear filed */
#define IRTX_INTCLR_DATBIT_POS                   ((uint32_t) 8) /* Data bit finished sending interrupt flag clear pos */
#define IRTX_INTCLR_DATBIT                       ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_DATBIT_POS)) /* Data bit finished sending interrupt flag clear filed */
#define IRTX_INTCLR_FNINTER_POS                  ((uint32_t) 7) /* FNINTER stage finished interrupt flag clear pos */
#define IRTX_INTCLR_FNINTER                      ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_FNINTER_POS)) /* FNINTER stage finished interrupt flag clear filed */
#define IRTX_INTCLR_ENDPOS_POS                   ((uint32_t) 6) /* ENDPOS stage finished interrupt flag clear pos */
#define IRTX_INTCLR_ENDPOS                       ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_ENDPOS_POS)) /* ENDPOS stage finished interrupt flag clear filed */
#define IRTX_INTCLR_POSDAT_POS                   ((uint32_t) 5) /* POSDAT stage finished interrupt flag clear pos */
#define IRTX_INTCLR_POSDAT                       ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_POSDAT_POS)) /* POSDAT stage finished interrupt flag clear filed */
#define IRTX_INTCLR_INTER_POS                    ((uint32_t) 4) /* INTER stage finished interrupt flag clear pos */
#define IRTX_INTCLR_INTER                        ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_INTER_POS)) /* INTER stage finished interrupt flag clear filed */
#define IRTX_INTCLR_ENDPRE_POS                   ((uint32_t) 3) /* ENDPRE stage finished interrupt flag clear pos */
#define IRTX_INTCLR_ENDPRE                       ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_ENDPRE_POS)) /* ENDPRE stage finished interrupt flag clear filed */
#define IRTX_INTCLR_PREDAT_POS                   ((uint32_t) 2) /* PREDAT stage finished interrupt flag clear pos */
#define IRTX_INTCLR_PREDAT                       ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_PREDAT_POS)) /* PREDAT stage finished interrupt flag clear filed */
#define IRTX_INTCLR_HEADL_POS                    ((uint32_t) 1) /* HEADL stage finished interrupt flag clear pos */
#define IRTX_INTCLR_HEADL                        ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_HEADL_POS)) /* HEADL stage finished interrupt flag clear filed */
#define IRTX_INTCLR_HEADH_POS                    ((uint32_t) 0) /* HEADH stage finished interrupt flag clear pos */
#define IRTX_INTCLR_HEADH                        ((uint32_t) ((uint32_t)1 << IRTX_INTCLR_HEADH_POS)) /* HEADH stage finished interrupt flag clear filed */

/***************IRTX Data FIFO Statue Register  Bitfiled ******************/
#define IRTX_FIFOST_TRIGLEVEL_POS                ((uint32_t) 6) /* When the data count remain in the FIFO below the TRIG_LEVEL,FIFOINT flag will be set pos */
#define IRTX_FIFOST_TRIGLEVEL                    ((uint32_t) ((uint32_t)7 << IRTX_FIFOST_TRIGLEVEL_POS))  /* When the data count remain in the FIFO below the TRIG_LEVEL,FIFOINT flag will be set filed */
#define IRTX_FIFOST_REMAINLEVEL_POS              ((uint32_t) 3) /* The data count remain in the FIFO pos */
#define IRTX_FIFOST_REMAINLEVEL                  ((uint32_t) ((uint32_t)7 << IRTX_FIFOST_REMAINLEVEL_POS))  /* The data count remain in the FIFO filed */
#define IRTX_FIFOST_FLUSH_POS                    ((uint32_t) 2) /* Write '1' to flush the data FIFO pos */
#define IRTX_FIFOST_FLUSH                        ((uint32_t) ((uint32_t)1 << IRTX_FIFOST_FLUSH_POS))  /* Write '1' to flush the data FIFO filed */
#define IRTX_FIFOST_EMPTY_POS                    ((uint32_t) 1) /* Whether the data FIFO is empty or not pos */
#define IRTX_FIFOST_EMPTY                        ((uint32_t) ((uint32_t)1 << IRTX_FIFOST_EMPTY_POS))  /* Whether the data FIFO is empty or not filed */
#define IRTX_FIFOST_FULL_POS                     ((uint32_t) 1) /* Whether the data FIFO is full or not pos */
#define IRTX_FIFOST_FULL                         ((uint32_t) ((uint32_t)1 << IRTX_FIFOST_FULL_POS))  /* Whether the data FIFO is full or not filed */

/***************IRTX PWM control Register Bitfiled ******************/
#define IRTX_PWMCTR_PWC_POS                      ((uint32_t) 8) /* Control the duty cycle of the carrier wave - 1 pos */
#define IRTX_PWMCTR_PWC                          ((uint32_t) ((uint32_t)0xff << IRTX_PWMCTR_PWC_POS))  /* Control the duty cycle of the carrier wave - 1 filed */
#define IRTX_PWMCTR_PRC_POS                      ((uint32_t) 0) /* Control the period of the carrier wave - 1 pos */
#define IRTX_PWMCTR_PRC                          ((uint32_t) ((uint32_t)0xff << IRTX_PWMCTR_PRC_POS)) /* Control the period of the carrier wave - 1 filed */

/***************IRTX Event System Control Register Bitfiled ******************/
#define IRTX_PES_RTCEVSEL_POS                    ((uint32_t) 4) /* The channel selection for issuing the event to IRTX pos */
#define IRTX_PES_RTCEVSEL                        ((uint32_t) ((uint32_t)0xf << IRTX_PES_RTCEVSEL_POS)) /* The channel selection for issuing the event to IRTX filed */

#define IRTX_PES_RTCEVEN_POS                     ((uint32_t) 0) /* An event is generated to the RTC when the channel selected by IRTXEV_SEL is high pos */
#define IRTX_PES_RTCEVEN                         ((uint32_t) ((uint32_t)1 << IRTX_PES_RTCEVEN_POS))  /* An event is generated to the RTC when the channel selected by IRTXEV_SEL is high filed */


/***************IRTX MANUL control Register Bitfiled ******************/
#define IRTX_MANUL_POL1ST_POS                    ((uint32_t) 31) /* Whether output the carrier waves or not at the first stage pos */
#define IRTX_MANUL_POL1ST                        ((uint32_t) ((uint32_t)1 << IRTX_MANUL_POL1ST_POS))  /* Whether output the carrier waves or not at the first stage filed */
#define IRTX_MANUL_LENGTH1ST_POS                 ((uint32_t) 17) /* The time length of stage 1 pos */
#define IRTX_MANUL_LENGTH1ST                     ((uint32_t) ((uint32_t)0x3fff << IRTX_MANUL_LENGTH1ST_POS))  /* The time length of stage 1 filed */
#define IRTX_MANUL_POL2ST_POS                    ((uint32_t) 16) /* Whether output the carrier waves or not at the second stage pos */
#define IRTX_MANUL_POL2ST                        ((uint32_t) ((uint32_t)1 << IRTX_MANUL_POL2ST_POS))  /* Whether output the carrier waves or not at the second stage filed */
#define IRTX_MANUL_LENGTH2ST_POS                 ((uint32_t) 0) /* The time length of stage 2 pos */
#define IRTX_MANUL_LENGTH2ST                     ((uint32_t) ((uint32_t)0xffff << IRTX_MANUL_LENGTH2ST_POS))  /* The time length of stage 1 filed */

/*********************************MARCO***************************************/
#define CARRYER_FREQUENCY                               38000   /* hz */
#define IRTX_SRC_FREQUENCY                              1000000    /* hz */

/*Be Carefull Bit 7 Will Not Send */
#define IRTX_F0_CUSTOME_CODE                            0xa5                /* F0 Portol Custome code */
#define IRTX_F1_CUSTOME_CODE                            0xa6                /* F1 Portol Custome code */
#define IRTX_F2_CUSTOME_CODE                            0xa7                /* F2 Portol Custome code */
#define IRTX_F3_CUSTOME_CODE                            0xa8                /* F3 Portol Custome code */
#define IRTX_F4_CUSTOME_CODE                            0xa9                /* F4 Portol Custome code */
#define IRTX_F5_CUSTOME_CODE                            0xaa                /* F5 Portol Custome code */
#define IRTX_F10_CUSTOME_CODE                           0xaf                /* F10 Portol Custome code */

#define MAXDATAUINT                                     4              /* Max data uint */
#define MAXSEGMENT                                      5              /* Max segment */
#define UINT_FIFO_LENGTH                                3              /* Uint FIFO length */
/*
 * set prescale for fninterval and interval:
 * unit 100us(prescale=99)
 * realtime = interval or fninterval *(prescale+1)
 */
#define INTERVAL_FNINTERVAL_PRESCALE                    99          /* The multi time length of FNINTER and INTERVAL value */

/* proctol stage length */
#define FN_HEADH_LENGTH                                 9000        /* FN code HEADH level length : us */
#define FN_LEADH_LENGTH                                 4500        /* FN code LEADH level length : us */
#define FN_STOP_PRE_LENGTH                              630         /* FN code STOP_PRE level length : us */
#define FN_STOP_POS_LENGTH                              630         /* FN code STOP_POS level length : us */

#define FN_LOGIC0_HENGTH                                630         /* FN code DATA0 high level length : us */
#define FN_LOGIC0_LENGTH                                560         /* FN code DATA0 low level length : us */
#define FN_LOGIC1_HENGTH                                630         /* FN code DATA1 high level length : us */
#define FN_LOGIC1_LENGTH                                1660        /* FN code DATA0 low level length : us */

#define FN_INTER_LENGTH                                 200         /* FN code INTER level length : us */
#define FN_FNINTER_LENGTH                               0           /* FN code FNINTER level length : us */

#define FN_PRE_DATA_BIT_NUM                             (35 - 1)   /* hw need to -1 */
#define FN_POS_DATA_BIT_NUM                             (32 - 1)   /* hw need to -1 */

#define FM_HEADH_LENGTH                                 6000       /* FM code HEADH level length : us */
#define FM_LEADH_LENGTH                                 3000       /* FM code LEADH level length : us */

#define FM_STOP_PRE_LENGTH                              630        /* FM code STOP_PRE level length : us */
#define FM_STOP_POS_LENGTH                              0          /* FM code STOP_POS level length : us */

#define FM_LOGIC0_HENGTH                                630        /* FM code DATA0 high level length : us */
#define FM_LOGIC0_LENGTH                                560        /* FM code DATA0 low level length : us */
#define FM_LOGIC1_HENGTH                                630        /* FM code DATA1 high level length : us */
#define FM_LOGIC1_LENGTH                                1660       /* FM code DATA0 low level length : us */

#define FM_INTER_LENGTH                                 0          /* FM code INTER level length : us */
#define FM_FNINTER_LENGTH                               0          /* FM code FNINTER level length : us */

#define FM_PRE_DATA_BIT_NUM                             (16-1)     /* hw need to -1 */
#define FM_POS_DATA_BIT_NUM                             0

#define IRTX_100MS_LENGTH                               1000       /* uint is 0.1ms*/
#define IRTX_40MS_LENGTH                                400        /* uint is 0.1ms*/

/**
 * IRTX transmit mode: Block or Noblock
 */
typedef enum
{
    IRTX_NOBLOCK                                 = 0x0,    /* Noblock to IRTX sending, function will return immediately */
    IRTX_BLOCK                                             /* Block to IRTX sending, function will not return until IR signal sending finish */
} IRTX_BlockStatus_T;

/**
 * IRTX module work status
 */
typedef enum
{
    IRTX_IDLE                                    = 0x0,    /* IRTX module is in idle status */
    IRTX_BUSY,                                             /* IRTX module is in busy status */
    IRTX_SEND_FINISH                                       /* IRTX had been finished sending */
} IRTX_TransferStatus_T;

/**
 * FM/FN code DATA0/1 high/low level
 */
typedef struct
{
    uint32_t    hWidth;                                    /* FM/FN code DATA0/1 high level */
    uint32_t    lWidth;                                    /* FM/FN code DATA0/1 low level */
} IRTX_LoigcLevelConfig_T;

/**
 * FM/FN code stage finished mask
 */
typedef struct
{
    uint32_t  headHMask       : 1;                        /* The HEADH stage finished mask */
    uint32_t  headLMask       : 1;                        /* The HEADL stage finished mask */
    uint32_t  preDatMask      : 1;                        /* The PREDAT stage finished mask */
    uint32_t  endPreMask      : 1;                        /* The ENDPRE stage finished mask */
    uint32_t  intervalMask    : 1;                        /* The INTERVAL stage finished mask */
    uint32_t  posDatMask      : 1;                        /* The POSDAT stage finished mask */
    uint32_t  endPosMask      : 1;                        /* The ENDPOS stage finished mask */
    uint32_t  fninterMask     : 1;                        /* The FNINTER stage finished mask */
    uint32_t  autoTxMask      : 1;                        /* Auto transmit mask */
} TRTX_WaveStageMask;

/**
 * The segment  length of FM/FN code
 */
typedef struct
{
    uint32_t                preDatBitNum;            /* The amount bits of the first part data */
    uint32_t                posDatBitNum;            /* The amount bits of the second part data */
    uint32_t                headHWidth;              /* The length of head code's high level */
    uint32_t                headLWidth;              /* The length of head code's low level */
    uint32_t                stopPreWidth;            /* The high level length of the first part data's ending bit code */
    uint32_t                stopPosWidth;            /* The high level length of the second part data's ending bit code */
    uint32_t                intervalWidth;           /* The length of the interval low level */
    uint32_t                fninterWidth;            /* The interval low level length between two set of IR code */
    uint32_t                dataWave[MAXDATAUINT];   /* Max data uint */
    TRTX_WaveStageMask      stageMask;               /* The stage mask */
    IRTX_LoigcLevelConfig_T logicLWidth;             /*logic 0*/
    IRTX_LoigcLevelConfig_T logicHWidth;             /*logic 1*/
} IRTX_UintWaveConfig_T;

/**
 * IRTX wave configuration
 */
typedef struct
{
    uint8_t                 validSegmentLength;        /* The valid segment length */
    IRTX_UintWaveConfig_T   uintWave[MAXSEGMENT];      /* The uint wave of max segment */
} IRTX_WaveConfig_T;

/**
 * @brief Enable IRTX interrupt
 *
 * @param  pIRTX           IRTX devive point
 * @param  mask            FM/FN code stage finished mask
 *
 * @retval void
 */
#define IRTX_INTERRUPT_ENABLE(pIRTX, mask)                      IO_BIT_SET(pIRTX->IE, mask)

/**
 * @brief OEN bit control the IR_OUT
 *
 * @param  pIRTX           IRTX devive point
 *
 * @retval void
 */
#define IRTX_DISABLE_OEN(pIRTX)                                 IO_BIT_SET(pIRTX->CTRL1, IRTX_CTRL1_OEN)

/**
 * @brief OEN bit control the IR_OUT
 *
 * @param  pIRTX           IRTX devive point
 *
 * @retval void
 */
#define IRTX_ENABLE_OEN(pIRTX)                                  IO_BIT_CLEAR(pIRTX->CTRL1, IRTX_CTRL1_OEN)

/**
 * @brief Configure IRTX data level
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             data level
 *
 * @retval void
 */
#define IRTX_CONFIG_DATALEVEL(pIRTX, val)                       IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_DATO,\
        val << IRTX_CTRL1_DATO_POS)

/**
 * @brief Set IRTX work mode
 *
 * @param  pIRTX           IRTX devive point
 * @param  workMode        IRTX work mode
 *
 * @retval void
 */
#define IRTX_SET_WORKMODE(pIRTX, workMode)                      IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_MODE, \
        workMode << IRTX_CTRL1_MODE_POS)

/**
 * @brief Set the prescaler between interval and fninter
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             prescaler value
 *
 * @retval void
 */
#define SET_INTERVAL_FNINTERVAL_PRESCALE(pIRTX, value)          IO_BITMASK_SET(pIRTX->TM4, IRTX_TIME4_PRESCALE, \
        value << IRTX_TIME4_PRESCALE_POS)

/**
 * @brief Flush IRTX fifo
 *
 * @param  pIRTX           IRTX devive point
 *
 * @retval void
 */
#define IRTX_FLUSH_FIFO(pIRTX)                                  IO_BIT_SET(pIRTX->FIFOST, IRTX_FIFOST_FLUSH)

/**
 * @brief Set IRTX fifo
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             data FIFO
 *
 * @retval void
 */
#define IRTX_SET_FIFO(pIRTX, val)                               (pIRTX->FIFO = val)

/**
 * @brief Configure IRTX auto tx
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             anto tx
 *
 * @retval void
 */
#define IRTX_CONFIG_AUTO_TX(pIRTX, val)                         IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_CONTINUE, \
        val << IRTX_CTRL1_CONTINUE_POS)

/**
 * @brief configure the length of Time0 head level
 *
 * @param  pIRTX           IRTX devive point
 * @param  headH           Time0:the length of headH level
 * @param  headL           Time0:the length of headL level
 *
 * @retval void
 */
#define IRTX_CONFIG_HEAD(pIRTX, headH, headL)                   IO_BITMASK_SET(pIRTX->TM0,(IRTX_TIME0_HEADHLENGTH | IRTX_TIME0_HEADLLENGTH),\
        (((headH) << IRTX_TIME0_HEADHLENGTH_POS) |\
         ((headL) << IRTX_TIME0_HEADLLENGTH_POS)))

/**
 * @brief Configure the length of Time1 data0 level
 *
 * @param  pIRTX           IRTX devive point
 * @param  hWidth          Time1:the length of data0H level
 * @param  lWidth          Time1:the length of data0L level
 *
 * @retval void
 */
#define IRTX_CONFIG_LOGICL(pIRTX, hWidth, lWidth)               IO_BITMASK_SET(pIRTX->TM1,(IRTX_TIME1_DATA0HLENGTH | IRTX_TIME1_DATA0LLENGTH),\
        (((hWidth) << IRTX_TIME1_DATA0HLENGTH_POS) |\
         ((lWidth) << IRTX_TIME1_DATA0LLENGTH_POS)))

/**
 * @brief Configure the length of Time2 data1 level
 *
 * @param  pIRTX           IRTX devive point
 * @param  hWidth          Time2:the length of data1H level
 * @param  lWidth          Time2:the length of data1L level
 *
 * @retval void
 */
#define IRTX_CONFIG_LOGICH(pIRTX, hWidth, lWidth)               IO_BITMASK_SET(pIRTX->TM2,(IRTX_TIME2_DATA1HLENGTH | IRTX_TIME2_DATA1LLENGTH),\
        (((hWidth) << IRTX_TIME2_DATA1HLENGTH_POS) |\
         ((lWidth) << IRTX_TIME2_DATA1LLENGTH_POS)))

/**
 * @brief Configure the length of Time3 endpre/pos level
 *
 * @param  pIRTX           IRTX devive point
 * @param  preStop          Time3:the length of endpre  level
 * @param  posStop          Time3:the length of endpos level
 *
 * @retval void
 */
#define IRTX_CONFIG_END(pIRTX, preStop, posStop)                IO_BITMASK_SET(pIRTX->TM3,(IRTX_TIME3_ENDPOSENGTH | IRTX_TIME3_ENDPREENGTH),\
        (((preStop) << IRTX_TIME3_ENDPREENGTH_POS) |\
         ((posStop) << IRTX_TIME3_ENDPOSENGTH_POS)))

/**
 * @brief Configure the length of Time4 headH or headL level
 *
 * @param  pIRTX           IRTX devive point
 * @param  interval        Time4:the length of endpre level
 *
 * @retval void
 */
#define IRTX_CONFIG_FNINTER(pIRTX, interval)                    IO_BITMASK_SET(pIRTX->TM4, IRTX_TIME4_ENDPREENGTH, \
        interval << IRTX_TIME4_ENDPREENGTH_POS)

/**
 * @brief Configure the length of IRTX interval level
 *
 * @param  pIRTX           IRTX devive point
 * @param  fninter         set interval length
 *
 * @retval void
 */
#define IRTX_CONFIG_INTERVAL(pIRTX, fninter)                    IO_BITMASK_SET(pIRTX->CTRL3, IRTX_CTRL3_INTERVALLENGTH, \
        fninter << IRTX_CTRL3_INTERVALLENGTH_POS)

/**
 * @brief Clear IRTX interrupt flag
 *
 * @param  pIRTX           IRTX devive point
 * @param  mask            FM/FN code stage finished interrupt mask
 *
 * @retval void
 */
#define IRTX_CLEAR_INTERRUPT(pIRTX, mask)                       (pIRTX->INTCLR = (mask))

/**
 * @brief Enable IRTX carrier wave output
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             enable or disable
 *
 * @retval void
 */
#define ITRX_CONFIG_OUTPUT_CARRIER_EN(pIRTX, val)                IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_CWEN, \
        (!val) << IRTX_CTRL1_CWEN_POS)

/**
 * @brief Whether send integrity wave or not
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             enable or disable
 *
 * @retval void
 */
#define ITRX_CONFIG_WAVE_INTEGRITY(pIRTX, val)                  IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_STOPSEL, \
        (val) << IRTX_CTRL1_STOPSEL_POS)

/**
 * @brief Configure IRTX trigger fifo length
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             enable or disable
 *
 * @retval void
 */
#define IRTX_CONFIG_TRIGGER_FIFOLENGTH(pIRTX, val)               IO_BITMASK_SET(pIRTX->FIFOST, IRTX_FIFOST_TRIGLEVEL, \
        (val+1) << IRTX_FIFOST_TRIGLEVEL_POS)

/**
 * @brief Software trigger IRTX module
 *
 * @param  pIRTX           IRTX devive point
 *
 * @retval void
 */
#define IRTX_SW_TRIGGER(pIRTX)                                  IO_BIT_SET(pIRTX->CTRL0, IRTX_CTRL0_KST);

/**
 * @brief Configure IRTX databit mask
 *
 * @param  pIRTX           IRTX devive point
 * @param  mode            mask data bit
 *
 * @retval void
 */
#define IRTX_CONFIG_DATABIT_MASK(pIRTX, mode)                   IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_INTERFACE, \
        mode << IRTX_CTRL1_INTERFACE_POS)

/**
 * @brief Whether intvert output level or not
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             enable or disable
 *
 * @retval void
 */
#define IRTX_DIR_REVERSAL_EN(pIRTX, val)                        IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_INV, \
        val << IRTX_CTRL1_INV_POS)

/**
 * @brief Set lowest or highest sending first
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             lowest first or highest first
 *
 * @retval void
 */
#define IRTX_CONFIG_SEND_FIRST(pIRTX, val)                      IO_BITMASK_SET(pIRTX->CTRL1, IRTX_CTRL1_LOWFIRST, \
        val << IRTX_CTRL1_LOWFIRST_POS)

/**
 * @brief Configure IRTX software or hardware trigger
 *
 * @param  pIRTX           IRTX devive point
 * @param  val             enable or disable
 *
 * @retval void
 */
#define IRTX_CONFIG_HW_TRIGGER_EN(pIRTX, val)                   IO_BITMASK_SET(pIRTX->PES, IRTX_PES_RTCEVEN, \
        val << IRTX_PES_RTCEVEN_POS)

/**
 * @brief IRTX hw triger
 *
 * @param  pIRTX           IRTX devive point
 * @param  event           trigger mode
 *
 * @retval void
 */
#define IRTX_CONFIG_TRIGGER_EVENT(pIRTX, event)                 IO_BITMASK_SET(pIRTX->PES, IRTX_PES_RTCEVSEL, \
        event << IRTX_PES_RTCEVSEL_POS)
/*
 * When the data count remain in the FIFO below the TRIG_LEVEL,
 * FIFOINT flag will be set.
 */

/**
* @brief Configure IRTX fifo trigger level
*
* @param  pIRTX           IRTX devive point
* @param  num             IRTX fifo trigger level
*
* @retval void
*/
#define IRTX_CONFIG_FIFO_TRIGGER_LEVEL(pIRTX, num)              IO_BITMASK_SET(pIRTX->FIFOST, IRTX_FIFOST_TRIGLEVEL, \
        num << IRTX_FIFOST_TRIGLEVEL_POS)

/**
 * @brief Get IRTX remain fifo
 *
 * @param  pIRTX           IRTX devive point
 *
 * @retval void
 */
#define IRTX_GET_REMAIN_FIFO(pIRTX)                             (IO_BITS_GET(pIRTX->IRTX_FIFOST_REMAINLEVEL, \
        IRTX_FIFOST_REMAINLEVEL) >> IRTX_FIFOST_REMAINLEVEL_POS)

/**
 * @brief Configure IRTX data bit number
 *
 * @param  pIRTX           IRTX devive point
 * @param  preDataBitNum   valid data bitnum
 * @param  posDataBitNum   valid data bitnum
 *
 * @retval void
 */
#define IRTX_CONFIG_DATABITNUM(pIRTX, preDataBitNum, posDataBitNum) \
    IO_BITMASK_SET(pIRTX->CTRL3,(IRTX_CTRL3_PREDATABITSNUM | IRTX_CTRL3_POSDATABITSNUM),\
                   (((preDataBitNum) << IRTX_CTRL3_PREDATABITSNUM_POS) |\
                    ((posDataBitNum) << IRTX_CTRL3_POSDATABITSNUM_POS)))

/**
 * @brief Enable IRTX module
 *
 * @param  pIRTX           IRTX devive point
 *
 * @retval void
 */
#define IRTX_ENABLE(pIRTX)                                      do{ /* enable output irtx mode */ IO_BIT_SET(pIRTX->CTRL1, IRTX_CTRL1_OEN); /* module enable */ IO_BIT_SET(pIRTX->CTRL1, IRTX_CTRL1_EN);  }while(0)

/**
 * @brief Disable IRTX module
 *
 * @param  pIRTX           IRTX devive point
 *
 * @retval void
 */
#define IRTX_DISABLE(pIRTX)                                     do{ /* module disable */ IO_BIT_CLEAR(pIRTX->CTRL1, IRTX_CTRL1_EN); /* disable output irtx mode */IO_BIT_CLEAR(pIRTX->CTRL1, IRTX_CTRL1_OEN); }while(0)

/**
 * @brief Config PES event
 * @retval None
 */
#define PES_EVENT_CONFIG()                                      do{}while(0)

/**
 * @brief Reset IRTX module
 * @retval None
 */
#define IRTX_RESET()                                            do{\
        uint32_t i =100;\
        *(volatile uint32_t *)(0x40000020) &=~(1<<20);\
        while(i--);\
        *(volatile uint32_t *)(0x40000020) |= (1<<20);\
    }while(0)    /* reset IRTX */

/**
 * @brief Enable ITRX Clock
 * @retval None
 */
#define IRTX_CLOCK_ENABLE()                                      do{}while(0)    /* enable IRTX clk */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _IRTX_DEV_H_ */
