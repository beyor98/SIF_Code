/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          wdog_dev.h
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
 *    2016/03/01,   Mike.Zheng,   V1.0.0
 *        Initial version.
 *    2016/06/29,   Mike.Zheng,   V1.1.0
 *        Update coding style.
 *    2016/11/22,   Mike.Zheng,   V1.2.0
 *        Update For Release.
 *    2017/03/15    kenny.cmt     V1.3.0
 *        Update For GM6611
 *    2017/05/30    kenny.cmt     V1.3.1
 *        Update For GM6601B
 *    2018/01/19    kenny.cmt     V2.0
 *        Update For GM6611B
 */
#ifndef _WDOG_DEV_H_
#define _WDOG_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include <common.h>
#include <drv/dev/clock.h>

#define WDOG_DEBUG_PRINTF
#define WDOG_WARNING_PRINTF
#define WDOG_ERR_PRINTF

#ifndef WDOG_DEBUG_PRINTF
#define WDOG_PR_DEBUG(...)      do {} while (0)
#else
#define WDOG_PR_DEBUG           printf
#endif

#ifndef WDOG_ERR_PRINTF
#define WDOG_PR_ERR(...)        do {} while (0)
#else
#define WDOG_PR_ERR             printf
#endif

#ifndef WDOG_WARNING_PRINTF
#define WDOG_PR_WARNINR(...)    do {} while (0)
#else
#define WDOG_PR_WARNINR         printf
#endif

/**
 * WDOG Control Register Layout Type Definition
 */
#if (defined(CHIP_GM6601))
typedef struct
{
    __IO uint32_t LD;                       /* WDOG Load Register:offset: 0x0 */
    __IO uint32_t VAL;                      /* WDOG Value Register:offset: 0x4 */
    __IO uint32_t CTRL;                     /* WDOG Control Register:offset 0x8 */
    __IO uint32_t IC;                       /* WDOG Clear Interrupt Register:offset: 0xc */
    __IO uint32_t RIS;                      /* WDOG Raw Interrupt Status Register:offset: 0x10 */
    __IO uint32_t MIS;                      /* WDOG Masked Interrupt Status Register:offset: 0x14 */
    __IO uint32_t RESV0[762];               /* WDOG Reserver Register:offset:offset: 0x18~0xbfc */
    __IO uint32_t KR;                       /* WDOG Lock Register Register:offset: 0xc000 */
    __IO uint32_t RESV1[191];               /* WDOG Reserver Register:offset: 0xc004~0xefc */
    __IO uint32_t ITC;                      /* WDOG Integration Test Control Register:offset: 0xf00 */
    __IO uint32_t ITO;                      /* WDOG IIntegration Test Output Set Register:offset: 0xf04 */
} WDOG_Device_T;
#endif

#if ((defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621)))
typedef struct
{
    __IO uint32_t LD;                       /* WDOG Load Register:offset: 0x0 */
    __IO uint32_t VAL;                      /* WDOG Value Register:offset: 0x4 */
    __IO uint32_t CTRL;                     /* WDOG Control Register:offset 0x8 */
    __IO uint32_t IC;                       /* WDOG Clear Interrupt Register:offset: 0xc */
    __IO uint32_t RIS;                      /* WDOG Raw Interrupt Status Register:offset: 0x10 */
    __IO uint32_t MIS;                      /* WDOG Masked Interrupt Status Register:offset: 0x14 */
    __IO uint32_t WL;                       /* WDOG Window Load Register:offset:0x18*/
    __IO uint32_t RESV0[761];               /* WDOG Reserver Register:offset:0x1c~0xbfc */
    __IO uint32_t KR;                       /* WDOG Lock Register :offset: 0xc00 */
    __IO uint32_t RESV1[191];               /* WDOG Reserver Register:offset: 0xc04~0xefc */
    __IO uint32_t TMR;                      /* WDOG Integration Test Control Register:offset: 0xf00 */
    __IO uint32_t TMOR;                     /* WDOG IIntegration Test Output Set Register:offset: 0xf04 */
    __IO uint32_t RESV2[61];                /* WDOG Reserver Register:offset:0xf04~0xffc */
    __IO uint32_t ID;                       /* WDOG Version ID Register:offset:0xFFC*/
} WDOG_Device_T;
#endif


#if (defined(CHIP_GM6601B)) || defined(CHIP_F6601C)
typedef struct
{
    __IO uint32_t LD;                       /* WDOG Load Register:offset: 0x0 */
    __IO uint32_t VAL;                      /* WDOG Value Register:offset: 0x4 */
    __IO uint32_t CTRL;                     /* WDOG Control Register:offset 0x8 */
    __IO uint32_t IC;                       /* WDOG Clear Interrupt Register:offset: 0xc */
    __IO uint32_t RIS;                      /* WDOG Raw Interrupt Status Register:offset: 0x10 */
    __IO uint32_t MIS;                      /* WDOG Masked Interrupt Status Register:offset: 0x14 */
    __IO uint32_t WL;                       /* WDOG Window Load Register:offset:0x18*/
    __IO uint32_t RESV0[761];               /* WDOG Reserver Register:offset:0x1c~0xbfc */
    __IO uint32_t KR;                       /* WDOG Lock Register :offset: 0xc00 */
    __IO uint32_t RESV1[191];               /* WDOG Reserver Register:offset: 0xc04~0xefc */
    __IO uint32_t TMR;                      /* WDOG Integration Test Control Register:offset: 0xf00 */
    __IO uint32_t TMOR;                     /* WDOG IIntegration Test Output Set Register:offset: 0xf04 */
    __IO uint32_t RESV2[61];                /* WDOG Reserver Register:offset:0xf04~0xffc */
    __IO uint32_t ID;                       /* WDOG Version ID Register:offset:0xFFC*/
} WDOG_Device_T;
#endif

#define WDOG_BASE                           (0x40008000u)                /* Base Addresses of WDOG Peripheral Control Registers */
#define WDOG_DEV                            ((WDOG_Device_T *)WDOG_BASE) /* WDOG Device Pointer */

#define IMMEDIATELY_RESERT_CNT              1                            /* Set This Cnt To Wdog Cnt, Wdog Will Reset Chip Immediately */

/**
 * @defgroup WDOG_control_reg_bit
 * @{
 */
#if (defined(CHIP_GM6601))
#define WDOG_CTRL_RE_POS                    ((uint32_t) 1)                                                /* WDOG Reset Enable Pos */
#define WDOG_CTRL_RE                        ((uint32_t) ((uint32_t)0x01 << WDOG_CTRL_RE_POS))             /* WDOG Reset Filed */
#define WDOG_CTRL_IE_POS                    ((uint32_t) 0)                                                /* WDOG Irq Enanle Pos */
#define WDOG_CTRL_IE                        ((uint32_t) ((uint32_t)0x01 << WDOG_CTRL_IE_POS))             /* WDOG Irq Enable Filed */
#endif
/**
 * @}
 */

#if (defined(CHIP_GM6601B)) || defined(CHIP_F6601C)

/**
 * @defgroup WDOG_control_reg_bit
 * @{
 */
#define WDOG_CTRL_IE_POS                    ((uint32_t) 31)                                               /* WDOG Timer module enable pos */
#define WDOG_CTRL_IE                        ((uint32_t) ((uint32_t)0x01 << WDOG_CTRL_IE_POS))             /* WDOG Timer module enalbe bitmask */
#define WDOG_CNT_OUT_WINDOW_FLAG_POS        ((uint32_t) 2)
#define WDOG_CNT_OUT_WINDOW_OUTSIDE         ((uint32_t) ((uint32_t)0x01 << WDOG_CNT_OUT_WINDOW_FLAG_POS)) /* WDOG counter value outside window */
#define WDOG_CNT_OUT_WINDOW_INSIDE          ((uint32_t) ((uint32_t)0x00 << WDOG_CNT_OUT_WINDOW_FLAG_POS)) /* WDOG counter value inside window */
#define WDOG_WINDOW_EN_POS                  ((uint32_t) 1)                                                /* WDOG enable window mode pos */
#define WDOG_WINDOW_EN                      ((uint32_t) ((uint32_t)0x01 << WDOG_WINDOW_EN_POS))           /* WDOG enable window mode bitmask */
#define WDOG_INT_EN_POS                     ((uint32_t) 0)                                                /* WDOG enable the early warning interrupt pos */
#define WDOG_INT_EN                         ((uint32_t) ((uint32_t)0x01 << WDOG_INT_EN_POS))              /* WDOG enable the early warning interrupt bitmask */
/**
 * @}
 */

/**
 * @defgroup WDOG_write_access_bit
 * @{
 */
#define WDOG_WRITE_ACCESS_POS               ((uint32_t) 1)                                                /* WDOG access reg lock pos */
#define WDOG_WRITE_ACCESS                   ((uint32_t) ((uint32_t)0xfffffffe << WDOG_WRITE_ACCESS_POS))  /* WDOG access reg lock bitmask */
#define WDOG_WRITE_ENABLE_STATUS_POS        ((uint32_t) 0)                                                /* WDOG read only to check reglock status pos */
#define WDOG_WRITE_ENABLE_STATUS            ((uint32_t) ((uint32_t)0xfffffffe << WDOG_WRITE_ENABLE_STATUS_POS)) /*WDOG read only to check */
/**
 * @}
 */

/**
 * @defgroup WDOG_test_mode_reg_bit
 * @{
 */
#define WDOG_TEST_MODE_EN_POS               ((uint32_t) 0)
#define WDOG_TEST_MODE_EN                   ((uint32_t) ((uint32_t)0X01 << WDOG_TEST_MODE_EN_POS))        /* WDOG test mode register,enable integration*/
/**
 * @}
 */

/**
 * @defgroup WDOG_test_mode_output_reg_bit
 * @{
 */
#define WDOG_TEST_INTVALUE_POS              ((uint32_t) 1)                                                /* WDOG integration test wdtInt pos */
#define WDOG_TEST_INTVALUE                  ((uint32_t) ((uint32_t)0x01 << WDOG_TEST_INTVALUE_POS))       /* WDOG Value output on WDOGINT when in Integration Test Mode */
#define WDOG_TEST_RESVALUE_POS              ((uint32_t) 0)                                                /* WDOG Integration Test WDOGRES value pos */
#define WDOG_TEST_RESVALUE                  ((uint32_t) ((uint32_t)0x01 << WDOG_TEST_RESVALUE_POS))       /* WDOG Value output on WDOGRES when in Integration Test Mode  */
/**
 * @}
 */

/**
 * @defgroup WDOG_work_mode
 * @{
 */
#define WDOG_NORMAL_MODE                    0                  /* WDOG work mode: normal */
#define WDOG_WINDOW_MODE                    1                  /* WDOG work mode: windows */
#define WDOG_NORMAL_EWI                     2                  /* WDOG work mode: normal and early warning interrupt */
#define WDOG_WINDOW_EWI                     3                  /* WDOG work mode: windows and early warning interrupt */
/**
 * @}
 */
#endif /*(defined(CHIP_GM6601))*/

#if ((defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621)))

/**
 * @defgroup WDOG_control_reg_bit
 * @{
 */
#define WDOG_CTRL_IE_POS                    ((uint32_t) 31)                                               /* WDOG Timer module enable pos */
#define WDOG_CTRL_IE                        ((uint32_t) ((uint32_t)0x01 << WDOG_CTRL_IE_POS))             /* WDOG Timer module enalbe bitmask */
#define WDOG_CNT_OUT_WINDOW_FLAG_POS        ((uint32_t) 2)
#define WDOG_CNT_OUT_WINDOW_OUTSIDE         ((uint32_t) ((uint32_t)0x01 << WDOG_CNT_OUT_WINDOW_FLAG_POS)) /* WDOG counter value outside window */
#define WDOG_CNT_OUT_WINDOW_INSIDE          ((uint32_t) ((uint32_t)0x00 << WDOG_CNT_OUT_WINDOW_FLAG_POS)) /* WDOG counter value inside window */
#define WDOG_WINDOW_EN_POS                  ((uint32_t) 1)                                                /* WDOG enable window mode pos */
#define WDOG_WINDOW_EN                      ((uint32_t) ((uint32_t)0x01 << WDOG_WINDOW_EN_POS))           /* WDOG enable window mode bitmask */
#define WDOG_INT_EN_POS                     ((uint32_t) 0)                                                /* WDOG enable the early warning interrupt pos */
#define WDOG_INT_EN                         ((uint32_t) ((uint32_t)0x01 << WDOG_INT_EN_POS))              /* WDOG enable the early warning interrupt bitmask */
/**
 * @}
 */

/**
* @defgroup WDOG_write_access_bit
* @{
*/
#define WDOG_WRITE_ACCESS_POS               ((uint32_t) 1)                                                /* WDOG access reg lock pos */
#define WDOG_WRITE_ACCESS                   ((uint32_t) ((uint32_t)0xfffffffe << WDOG_WRITE_ACCESS_POS))  /* WDOG access reg lock bitmask */
#define WDOG_WRITE_ENABLE_STATUS_POS        ((uint32_t) 0)                                                /* WDOG read only to check reglock status pos */
#define WDOG_WRITE_ENABLE_STATUS            ((uint32_t) ((uint32_t)0xfffffffe << WDOG_WRITE_ENABLE_STATUS_POS)) /*WDOG read only to check */
/**
 * @}
 */

/**
 * @defgroup WDOG_test_mode_reg_bit
 * @{
 */
#define WDOG_TEST_MODE_EN_POS               ((uint32_t) 0)
#define WDOG_TEST_MODE_EN                   ((uint32_t) ((uint32_t)0X01 << WDOG_TEST_MODE_EN_POS))        /* WDOG test mode register,enable integration*/
/**
 * @}
 */

/**
 * @defgroup WDOG_test_mode_output_reg_bit
 * @{
 */
#define WDOG_TEST_INTVALUE_POS              ((uint32_t) 1)                                                /* WDOG integration test wdtInt pos */
#define WDOG_TEST_INTVALUE                  ((uint32_t) ((uint32_t)0x01 << WDOG_TEST_INTVALUE_POS))       /* WDOG Value output on WDOGINT when in Integration Test Mode */
#define WDOG_TEST_RESVALUE_POS              ((uint32_t) 0)                                                /* WDOG Integration Test WDOGRES value pos */
#define WDOG_TEST_RESVALUE                  ((uint32_t) ((uint32_t)0x01 << WDOG_TEST_RESVALUE_POS))       /* WDOG Value output on WDOGRES when in Integration Test Mode  */
/**
 * @}
 */

/**
 * @defgroup WDOG_work_mode
 * @{
 */
#define WDOG_NORMAL_MODE                    0                  /* WDOG work mode: normal */
#define WDOG_WINDOW_MODE                    1                  /* WDOG work mode: windows */
#define WDOG_NORMAL_EWI                     2                  /* WDOG work mode: normal and early warning interrupt. Note!the fclk is same as wdtclk in CHIP_GM6621,the case is forbidden  */
#define WDOG_WINDOW_EWI                     3                  /* WDOG work mode: windows and early warning interrupt. Note!the fclk is same as wdtclk in CHIP_GM6621,the case is forbidden */
/**
 * @}
 */

#endif /*(defined(CHIP_GM6611))*/

#define WDOG_CLEAR_INT_MAGICNUM             0xadcddcba         /* Write This Maigcnum To Tntclr Reg To Clear Irq And Feed WDOG */
#define WDOG_UNLOCK_MAGIC_NUM               0x1acce551         /* Write This Maigcnum To WDOG Lock Reg To Unlock Write WDOG Reg */

#define MS_DIV                              1000               /* WDOG Ms Div Cnt */

/**
 * @defgroup CHIP_GM6601_WDOG_CLK_SRC
 * @{
 */
#if (defined(CHIP_GM6601))
#define FCLK_CLK                            SYS_CLOCK          /* WDOG Srcclk:FCLK_CLK Which Is Equal Sysclk */
#define OSC_LS_CLK                          32476              /* WDOG Srcclk:OSC_LS_CLK: 32.768KHz */
#define IRC_LS_CLK                          1000               /* WDOG Srcclk:IRC_LS_CLK: 1KHz */
#define PLL_SRC_CLK                         8000000            /* WDOG Srcclk:PLL_SRC_CLK: 8MHz */
#endif /*(defined(CHIP_GM6601))*/
/**
 * @}
 */

#if (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
#define  IRC_LS_CLK                          30000              /* WDOG Srcclk:IRC_LS_CLK: 30KHz */
#endif /*(defined(CHIP_GM6611))*/

/**
 * @defgroup CHIP_GM6621_WDOG_CLK_SRC
 * @{
 */
#if (defined(CHIP_GM6621))
#define FCLK_CLK                            SYS_CLOCK          /* WDOG Srcclk:FCLK_CLK Which Is Equal Sysclk */
#define OSC_LS_CLK                          32476              /* WDOG Srcclk:OSC_LS_CLK: 32.768KHz */
#define IRC_LS_CLK                          32000              /* WDOG Srcclk:IRC_LS_CLK: 32KHz */
#define PLL_SRC_CLK                         8000000            /* WDOG Srcclk:PLL_SRC_CLK: 8MHz */
#endif /*(defined(CHIP_GM6621))*/
/**
 * @}
 */

/**
 * @defgroup CHIP_GM6601B_WDOG_CLK_SRC
 * @{
 */
#if (defined(CHIP_GM6601B)) || defined(CHIP_F6601C)
#define FCLK_CLK                            SYS_CLOCK          /* WDOG Srcclk:FCLK_CLK Which Is Equal Sysclk */
#if (defined(CHIP_F6601C))
#define OSC_LS_CLK                          32766              /* WDOG Srcclk:OSC_LS_CLK: 32.768KHz */
#else
#define OSC_LS_CLK                          32400
#endif
#define IRC_LS_CLK                          30000              /* WDOG Srcclk:IRC_LS_CLK: 30KHz */
#define PLL_SRC_CLK                         8000000            /* WDOG Srcclk:PLL_SRC_CLK: 8MHz */
#endif /*(defined(CHIP_GM6601B))*/

/**
 * @}
 */
#if (defined(CHIP_GM6611B) || defined(CHIP_GM6621))
#define WDOG_WAIT_WRITEDONE()               do{while(SYSDEV->DBGCTRL & SYS_DGB_WDOG_WRITEDONE);}while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
#define WDOG_UNLOCK(pWDOG)                  do{WDOG_WAIT_WRITEDONE(); pWDOG->KR = WDOG_UNLOCK_MAGIC_NUM;}while(0u) /* Ulock WDOG Reg,Acess WDOG Reg Is Vaild */
#define WDOG_LOCK(pWDOG)                    do{WDOG_WAIT_WRITEDONE(); pWDOG->KR = 0;}while(0u)                     /* Lock WDOG Reg,Access WDOG Reg Is Not Vaild */

#elif (defined(CHIP_F6601C))
#define WDOG_WAIT_WRITEDONE()               do{while(SYSDEV->DBGCTRL & SYS_RSV_WDOG_WRITEDONE);}while(0u);         /* Waiting for DBG_CFG_CTRL bit16 wdog write done flag */
#define WDOG_UNLOCK(pWDOG)                  do{WDOG_WAIT_WRITEDONE(); pWDOG->KR = WDOG_UNLOCK_MAGIC_NUM;}while(0u) /* Ulock WDOG Reg,Acess WDOG Reg Is Vaild */
#define WDOG_LOCK(pWDOG)                    do{WDOG_WAIT_WRITEDONE(); pWDOG->KR = 0;}while(0u)                     /* Lock WDOG Reg,Access WDOG Reg Is Not Vaild */

#else
#define WDOG_WAIT_WRITEDONE()               do{}while(0u);
#define WDOG_UNLOCK(pWDOG)                  (pWDOG->KR = WDOG_UNLOCK_MAGIC_NUM)  /* Ulock WDOG Reg,Acess WDOG Reg Is Vaild */
#define WDOG_LOCK(pWDOG)                    (pWDOG->KR = 0)    /* Lock WDOG Reg,Access WDOG Reg Is Not Vaild */
#endif /* (defined(CHIP_GM6611B) || defined(CHIP_GM6621)) */

#ifdef __cplusplus
}
#endif

#endif /*_WDOG_DEV_H_*/

