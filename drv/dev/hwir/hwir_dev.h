/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          hwir_dev.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.0.0
 *
 * @date          2016/07/05
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/03/01, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/05, Mike.Zheng, V1.1.0
 *        Update coding style.
 *    2016/11/23, Mike.Zheng, V1.2.0
 *        Update For Release.
 */
#ifndef _HWIR_DEV_H_
#define _HWIR_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <common.h>
#include <drv/dev/clock.h>

//#define HWIR_DEBUG_PRINTF
//#define HWIR_ERR_PRINTF
//#define HWIR_WARNINR_PRINTF

#ifndef HWIR_DEBUG_PRINTF
    #define HWIR_PR_DEBUG(...)       do {} while (0)
#else
    #define HWIR_PR_DEBUG            printf
#endif
#ifndef HWIR_WARNINR_PRINTF
    #define HWIR_PR_WARNINR(...)     do {} while (0)
#else
    #define HWIR_PR_WARNINR          printf
#endif
#ifndef HWIR_ERR_PRINTF
    #define HWIR_PR_ERR(...)         do {} while (0)
#else
    #define HWIR_PR_ERR              printf
#endif

/* HWIR Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t CFG;                      /* HWIR contral register:offset: 0x0 */
    __IO uint32_t RESV0;                    /* HWIR reserver register:offset: 0x04 */
    __IO uint32_t IE;                       /* HWIR interrupt enable register:offset: 0x08 */
    __IO uint32_t IS;                       /* HWIR status register:offset: 0x0c */
    __IO uint32_t DECTHR[4];                /* HWIR decode thr register:offset: 0x10~0x1c */
    __IO uint32_t DECPOL[4];                /* HWIR polary register:offset: 0x20~0x2c */
    __IO uint32_t DECDONCA[4];              /* HWIR do not care bitmask register:offset: 0x30~0x3c */
    __IO uint32_t DECTS[16];                /* HWIR decode thr select register:offset: 0x40~0x7c */
    __IO uint32_t DECTDN;                   /* HWIR decode thr num register:offset: 0x80 */
    __IO uint32_t RESV1[30];                /* HWIR reserver register:offset: 0x84~0xec */
    __IO uint32_t VERNUM;                   /* HWIR version num register:offset: 0xfc */
}HWIR_Device_T;

#define HWIR_BASE                           (0x4000b000u)                   /* Base addresses of HWIR registers */
#define HWIR_DEV                            ((HWIR_Device_T *)HWIR_BASE)     /* HWIR device pointer */

#define HWIR_RESET_DELAYCNT                 0xff                             /* HWIR reset timedelay */

#define HWIR_CLOCK_GATE                     (1<<6)                           /* HWIR clock gate mask bit */

/*************** HWIR config register bitfiled **************************/
#define HWIR_CFG_NTHR_POS                   ((uint32_t) 24)                                            /* Threshold for noise detect pos */
#define HWIR_CFG_NTHR                       ((uint32_t) ((uint32_t)0xff << HWIR_CFG_NTHR_POS))         /* Threshold for noise detect filed */
#define HWIR_CFG_TOTHR_POS                  ((uint32_t) 16)                                            /* Threshold for timeout detect pos */
#define HWIR_CFG_TOTHR                      ((uint32_t) ((uint32_t)0xff << HWIR_CFG_TOTHR_POS))        /* Threshold for timeout detect filed */
#define HWIR_CFG_CDV_POS                    ((uint32_t) 8)                                             /* HWIR set clk div value pos */
#define HWIR_CFG_CDV                        ((uint32_t) ((uint32_t)0xff << HWIR_CFG_CDV_POS))          /* HWIR set clk div value filed */
#define HWIR_CFG_EN_POS                     ((uint32_t) 0)                                             /* HWIR decode key enable pos */
#define HWIR_CFG_EN                         ((uint32_t) ((uint32_t)1 << HWIR_CFG_EN_POS))              /* HWIR decode key enable filed */

/*************** HWIR Interrupt Enable Bitfiled *************************/
#define HWIR_DSIE_POS                       ((uint32_t) 0)                                             /* HWIR decode success interrupt enable pos */
#define HWIR_DSIE                           ((uint32_t) ((uint32_t)1 << HWIR_DSIE_POS))                /* HWIR decode success interrupt enable filed */

/*************** HWIR Status Bitfiled ***********************************/
#define HWIR_IS_DS_POS                      ((uint32_t) 0)                                             /* HWIR decode success interrupt status pos */
#define HWIR_IS_DS                          ((uint32_t) ((uint32_t)1 << HWIR_IS_DS_POS))               /* HWIR decode success interrupt status filed */

/*************** HWIR Decode Threshold Number Bitfiled ******************/
#define HWIR_DECTDN_BITNUM_POS              ((uint32_t) 8)                                             /* How many level HWIR will decoder pos */
#define HWIR_DECTDN_BITNUM                  ((uint32_t) ((uint32_t)0x0ff << HWIR_DECTDN_BITNUM_POS))   /* How many level HWIR will decoder filed */
#define HWIR_DECTDN_THRNUM_POS              ((uint32_t) 0)                                             /* The bit about how many threshold length is used for HIRX decode pos */
#define HWIR_DECTDN_THRNUM                  ((uint32_t) ((uint32_t)0x0f << HWIR_DECTDN_THRNUM_POS))    /* How many threshold length is ued for HIRX decode filed */

/**
 * HWIR rceive wave polarity level
 */
typedef enum
{
    LEVEL_LOW                               = 0x00,     /**< HWIR receive wave polarity level is low */
    LEVEL_HIGH                                          /**< HWIR receive wave polarity level is high */
} HWIR_Polarity_T;

/**
 * HWIR match threshold level
 */
typedef enum
{
    HWIR_MATCH_LEVEL0                       = 0x00,     /**< The first match threshold level */
    HWIR_MATCH_LEVEL1                             ,     /**< The second match threshold level */
    HWIR_MATCH_LEVEL2                             ,     /**< The third match threshold level */
    HWIR_MATCH_LEVEL3                             ,     /**< The forth match threshold level */
    HWIR_MATCH_LEVEL4                             ,     /**< The fifth match threshold level */
    HWIR_MATCH_LEVEL5                             ,     /**< The sixth match threshold level */
    HWIR_MATCH_LEVEL6                             ,     /**< The seventh match threshold level */
    HWIR_MATCH_LEVEL7                                   /**< The eighth match threshold level */
} HWIR_MatchLevel_T;

/**
 * HWIR start bit match threshold level
 */
typedef struct
{
    HWIR_MatchLevel_T                       startBitLowThrLevel;    /**< Start low bit  match threshold level */
    HWIR_MatchLevel_T                       startBitHighThrLevel;   /**< Start high bit match threshold level */
}HWIR_StartBitConfig_T;

/**
 * HWIR logic data bit match threshold level
 */
typedef struct
{
    HWIR_MatchLevel_T                       dataBit1LowThrLevel;    /**< Logic1 data low match threshold level */
    HWIR_MatchLevel_T                       dataBit1HighThrLevel;   /**< Logic1 data high match threshold level */
    HWIR_MatchLevel_T                       dataBit0LowThrLevel;    /**< Logic0 data low match threshold level */
    HWIR_MatchLevel_T                       dataBit0HighThrLevel;   /**< Logic0 data high match threshold level */
}HWIR_DataBitConfig_T;

/**
 * HWIR common config struct
 */
typedef struct
{
    uint32_t                                bitMask[4];             /**< HWIR decode do not care bitmask */
    uint16_t                                matchThr[8];            /**< HWIR macth thr level */
    uint8_t                                 decWaveBitNum;          /**< HWIR decode bit Num */
    uint8_t                                 decMatchThrNum;         /**< HWIR macth threshold num */
    uint8_t                                 div;                    /**< HWIR div frequency ratio */
    uint8_t                                 noiseThr;               /**< HWIR noise threshold */
    uint8_t                                 timeoutThr;             /**< HWIR timeout threshold */
}HWIR_DevConfig_T;

#define HWIR_SOURCR_FRE_MHZ                             8           /* HWIR frequency */
#define HWIR_FX_CLK_DIV_VALU                            7           /* HWIR div frequency */

#define HWIR_FX_CONVERT_US_TO_THR(us)                   ((HWIR_SOURCR_FRE_MHZ * (us)) / (1 + HWIR_FX_CLK_DIV_VALU))  /* Fx threshold convewrt to threshold */

#define HWIR_WAVE_DEC_START_POS                         0            /* Decode start bit pos */


/**
 * Start gree fx protol feature marco
 * FN : F0 F1 F2 F3 F10
 * FM : FM
 * FX : FM FN
 */
#define HWIR_FX_NOISRE_US                               200         /* Fx portol noise time (uint us) */
#define HWIR_FM_TIMEOUT_US                              20000       /* Fm portol timeout time (uint us) */
#define HWIR_FN_TIMEOUT_US                              30000       /* Fn portol timeout time (uint us) */
#define HWIR_FN_DELAY20MS_US                            20000       /* Fn portol delay20ms time (uint us) */
#define HWIR_FN_STARTBIT_LOW_US                         9000        /* Fn portol start bit low time (uint us) */
#define HWIR_FM_STARTBIT_LOW_US                         6000        /* Fm portol start bit low time(uint us) */
#define HWIR_FN_STARTBIT_HIGH_US                        4500        /* Fn portol start bit high time(uint us) */
#define HWIR_FM_STARTBIT_HIGH_US                        3000        /* Fm portol start bit high time(uint us) */
#define HWIR_FX_DATABIT1_HIGH_US                        1660        /* Fx portol data logic1 high time(uint us) */
#define HWIR_FX_DATABIT1_LOW_US                         630         /* Fx portol data logic1 low time (uint us) */
#define HWIR_FX_STOP_US                                 630         /* Fx portol stop time(uint us) */
#define HWIR_FX_DATABIT0_LOW_US                         630         /* Fx portol data logic0 low time(uint us) */
#define HWIR_FX_DATABIT0_HIGH_US                        560         /* Fx portol data logic0 high time(uint us) */

#define HWIR_FX_NOISE_THR                               HWIR_FX_CONVERT_US_TO_THR(HWIR_FX_NOISRE_US)          /* Fx portol noise threshold */
#define HWIR_FM_TIMEOUT_THR                             (HWIR_FX_CONVERT_US_TO_THR(HWIR_FM_TIMEOUT_US) >> 7)  /* Fm portol timeout Threshold */
#define HWIR_FN_TIMEOUT_THR                             (HWIR_FX_CONVERT_US_TO_THR(HWIR_FN_TIMEOUT_US) >> 7)  /* Fn portol timeout Threshold */
#define HWIR_FN_DELAY20MS_THR                           HWIR_FX_CONVERT_US_TO_THR(HWIR_FN_DELAY20MS_US)       /* Fn portol delay20ms threshold */
#define HWIR_FN_STARTBIT_LOW_THR                        HWIR_FX_CONVERT_US_TO_THR(HWIR_FN_STARTBIT_LOW_US)    /* Fn portol start bit low threshold */
#define HWIR_FM_STARTBIT_LOW_THR                        HWIR_FX_CONVERT_US_TO_THR(HWIR_FM_STARTBIT_LOW_US)    /* Fm portol start bit low threshold */
#define HWIR_FN_STARTBIT_HIGH_THR                       HWIR_FX_CONVERT_US_TO_THR(HWIR_FN_STARTBIT_HIGH_US)   /* Fn portol start bit high threshold */
#define HWIR_FM_STARTBIT_HIGH_THR                       HWIR_FX_CONVERT_US_TO_THR(HWIR_FM_STARTBIT_HIGH_US)   /* Fm portol start bit high threshold */
#define HWIR_FX_DATABIT1_HIGH_THR                       HWIR_FX_CONVERT_US_TO_THR(HWIR_FX_DATABIT1_HIGH_US)   /* Fx portol data logic1 high threshold */
#define HWIR_FX_DATABIT1_LOW_THR                        HWIR_FX_CONVERT_US_TO_THR(HWIR_FX_DATABIT1_LOW_US)    /* Fx portol data logic1 low threshold */
#define HWIR_FX_STOP_THR                                HWIR_FX_CONVERT_US_TO_THR(HWIR_FX_STOP_US)            /* Fx portol stop threshold */
#define HWIR_FX_DATABIT0_LOW_THR                        HWIR_FX_CONVERT_US_TO_THR(HWIR_FX_DATABIT0_LOW_US)    /* Fx portol data logic0 low threshold */
#define HWIR_FX_DATABIT0_HIGH_THR                       HWIR_FX_CONVERT_US_TO_THR(HWIR_FX_DATABIT0_HIGH_US)   /* Fx portol data logic0 high threshold */

#define HWIR_FX_MATCH_LEVEL0_THR                        (HWIR_FX_DATABIT0_HIGH_THR >> 1)                                 /* Match fisrt level threshold value */
#define HWIR_FX_MATCH_LEVEL1_THR                        ((HWIR_FX_DATABIT0_HIGH_THR + HWIR_FX_DATABIT1_LOW_THR) >>1)     /* Match second level threshold value */
#define HWIR_FX_MATCH_LEVEL2_THR                        ((HWIR_FX_DATABIT1_LOW_THR  + HWIR_FX_DATABIT1_HIGH_THR ) >>1)   /* Match third level threshold value */
#define HWIR_FX_MATCH_LEVEL3_THR                        ((HWIR_FX_DATABIT1_HIGH_THR + HWIR_FM_STARTBIT_HIGH_THR ) >>1)   /* Match forth level threshold value */
#define HWIR_FX_MATCH_LEVEL4_THR                        ((HWIR_FM_STARTBIT_HIGH_THR + HWIR_FN_STARTBIT_HIGH_THR) >>1)    /* Match fifth level threshold value */
#define HWIR_FX_MATCH_LEVEL5_THR                        ((HWIR_FN_STARTBIT_HIGH_THR + HWIR_FM_STARTBIT_LOW_THR ) >>1)    /* Match sixth level threshold value */
#define HWIR_FX_MATCH_LEVEL6_THR                        ((HWIR_FM_STARTBIT_LOW_THR  + HWIR_FN_STARTBIT_LOW_THR) >>1)     /* Match seventh level threshold value */
#define HWIR_FX_MATCH_LEVEL7_THR                        ((HWIR_FN_STARTBIT_LOW_THR  + HWIR_FN_DELAY20MS_THR ) >>1)       /* Match eighth level threshold value */

#define HWIR_FN_DELAY20MS_MATCH_THRLEVEL                7               /* Fn portol delay20Ms correspond to the match threshold level */
#define HWIR_FN_STARTBIT_LOW_MATCH_THRLEVEL             6               /* Fn portol startbit low correspond to the match threshold level */
#define HWIR_FN_STARTBIT_HIGH_MATCH_THRLEVEL            4               /* Fn portol startbit high correspond to the match threshold level */
#define HWIR_FM_STARTBIT_LOW_MATCH_THRLEVEL             5               /* Fm portol startbit low correspond to the match threshold level */
#define HWIR_FM_STARTBIT_HIGH_MATCH_THRLEVEL            3               /* Fn portol startbit high correspond to the match threshold level */
#define HWIR_FX_DATABIT1_HIGH_MATCH_THRLEVEL            2               /* Fx portol data logic1 high correspond to the match threshold level */
#define HWIR_FX_DATABIT1_LOW_MATCH_THRLEVEL             1               /* Fx portol data logic1 low correspond to the match threshold level */
#define HWIR_FX_DATABIT0_LOW_MATCH_THRLEVEL             1               /* Fx portol data logic1 low correspond to the match threshold level */
#define HWIR_FX_DATABIT0_HIGH_MATCH_THRLEVEL            0               /* Fx portol data logic0 high correspond to the match threshold level */
#define HWIR_FX_STOP_MATCH_THRLEVEL                     1               /* Fx portol stop bit correspond to match threshold level */

#define HWIR_FN_MASK_DONCA0                             0x00            /* Fx portol bit0-31 must be decode */
#define HWIR_FN_MASK_DONCA1                             0x00            /* Fx portol bit32-63 must be decode */
#define HWIR_FN_MASK_DONCA2                             0x00            /* Fx portol bit64-95 must be decode */
#define HWIR_FN_MASK_DONCA3                             0x00            /* Fx portol bit96-127 must be decode */

#define HWIR_FM_MASK_DONCA0                             0x00            /* Fm portol bit0-31 must be decode */
#define HWIR_FM_MASK_DONCA1                             0xfffffff8     /* Fm portol bit32-34 must be decode,bit35-63 not to decode */
#define HWIR_FM_MASK_DONCA2                             0xffffffff     /* Fm portol bit64-95  not to decode */
#define HWIR_FM_MASK_DONCA3                             0xffffffff     /* Fm portol bit96-127 not to decode */

#define HWIR_FN_DEC_WAVE_BITNUM                         ((1 << 1) + (28 << 1) + (7 << 1) + 1 + 1 + (27 << 1)) /**
                                                                                                               * FM portol decode wavebit num,
                                                                                                               * for hw design only decode 128 bit
                                                                                                               */
#define HWIR_FM_DEC_WAVE_BITNUM                         ((1 << 1) +( 16 << 1 ) + 1)  /* Fm portol decode wavebit num */
#define HWIR_FX_DEC_THR_NUM                             8                            /* Fx portol decode threshold num */

#define HWIR_FN_DATA_LOGICBIT_NUM                       28                           /* Fn portol data logic bit num */
#define HWIR_FN_CX_LOGICBIT_NUM                         7                            /* Fn portol custome logic bit num */

/* Be Carefull Bit 7 Will Not Send */
#define HWIR_F0_CUSTOME_CODE                            0xa5                         /* F0 portol custome code */
#define HWIR_F1_CUSTOME_CODE                            0xa6                         /* F1 portol custome code */
#define HWIR_F2_CUSTOME_CODE                            0xa7                         /* F2 portol custome code */
#define HWIR_F3_CUSTOME_CODE                            0xa8                         /* F3 portol custome code */
#define HWIR_F4_CUSTOME_CODE                            0xa9                         /* F4 portol custome code */

#define HWIR_FM_DATA_LOGICBIT_NUM                       8                            /* Fm portol data Logic bit num */
#define HWIR_FM_VERIFY_LOGICBIT_NUM                     8                            /* Fm portol verify bit num */



/**
 * @brief Set HWIR div frequency ratio.
 *
 * @param  pHWIR        HWIR devive point
 * @param  value        Div frequency ratio
 *
 * @retval void
 */
#define HWIR_SET_DIV_MEASURE(pHWIR,value)               IO_BITMASK_SET(pHWIR->CFG, HWIR_CFG_CDV,                 \
                                                                       value << HWIR_CFG_CDV_POS)
/**
 * @brief Set HWIR Decode Threshold
 *
 * @param  pHWIR        HWIR devive point
 * @param  value        Decode thr

 * @retval void
 */
#define HWIR_SET_DECODE_THR(pHWIR, value)               IO_BITMASK_SET(pHWIR->DECTDN, HWIR_DECTDN_THRNUM,        \
                                                                       value << HWIR_DECTDN_THRNUM_POS)
/**
 * @brief Set HWIR decode bit num
 *
 * @param  pHWIR        HWIR devive point
 * @param  value        Decode bit bum
 *
 * @retval void
 */
#define HWIR_SET_DECODE_BITNUM(pHWIR, value)            IO_BITMASK_SET(pHWIR->DECTDN, HWIR_DECTDN_BITNUM,        \
                                                                       value << HWIR_DECTDN_BITNUM_POS)
/**
 * @brief Set HWIR noise threshold
 *
 * @param  pHWIR        HWIR devive point
 * @param  value        Noise threshold
 *
 * @retval void
 */
#define HWIR_SET_NOISE_THR(pHWIR, value)                IO_BITMASK_SET(pHWIR->CFG, HWIR_CFG_NTHR, (value << HWIR_CFG_NTHR_POS))

/**
 * @brief Set HWIR Timeout Threshold
 *
 * @param  pHWIR        HWIR Devive Point
 * @param  value        Timeout threshold
 *
 * @retval void
 */
#define HWIR_SET_TIMEOUT_THR(pHWIR, value)              IO_BITMASK_SET(pHWIR->CFG, HWIR_CFG_TOTHR,                \
                                                                       (value << HWIR_CFG_TOTHR_POS))
/**
 * @brief Clear HWIR Intrrput
 *
 * @param  pHWIR        HWIR devive point
 * @param  value        Cleart intrrput bits
 *
 * @retval void
 */
#define HWIR_CLEAR_INTERRUPT(pHWIR, value)              (pHWIR->IS = value)

/**
 * @brief Enable HWIR intrrput
 *
 * @param  pHWIR        HWIR devive point
 * @param  value        Enable intrrput bits
 *
 * @retval void
 */
#define HWIR_ENABLE_INTERRUPT(pHWIR, value)             IO_BITS_SET(pHWIR->IE, value)

/**
 * @brief Disable HWIR Intrrput
 *
 * @param  pHWIR        HWIR devive point
 * @param  value        Disable intrrput bits
 *
 * @retval void
 */
#define HWIR_DISABLE_INTERRUPT(pHWIR, value)            IO_BIT_CLEAR(pHWIR->IE, value)

/**
 * @brief Resert HWIR module
 *
 * @param  void
 * @retval void
 */
#define HWIR_RESET()                                    do {                                                      \
                                                            Clock_ResetPeripherals(HWIR_RESET,                    \
                                                                                   Hold_ResetStatusTillStable,    \
                                                                                   HWIR_RESET_DELAYCNT);          \
                                                        } while (0)

/**
 * @brief Enable HWIR clock
 * @retval void
 */
#define HWIR_CLOCK_ENABLE()                             do {                                                      \
                                                            Clock_PeripheralClockEnable(HWIR_CLOCK_GATE);         \
                                                        } while (0)    /* Enable HWIR Module Clk */


#ifdef __cplusplus
}
#endif

#endif /*_HWIR_H_*/
































































