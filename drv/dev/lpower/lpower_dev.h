/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          LPower_dev.h
 *
 * @author        sophia.wu
 *
 * @version       1.0.0
 *
 * @date          2019/04/16
 *
 * @brief         LPower device header file for GM6621
 *
 * @note
 *    2019/04/16, sophia.wu, V1.0.0
 *        Support 6621.
 */

#ifndef _LPower_DEV_H_
#define _LPower_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>


#if defined(CHIP_GM6601B)
#define LPOWER_PMC_PMU_ENABLE           (SYSDEV->CFGMISC |= SYS_CFGMISC_PMU_EN)


#elif defined(CHIP_GM6611B)
#define STPMD_STOPEN_MASK               (0x66CD0002)
#define STPMD_STOPDEEPEN_MASK           (0x66AD0001)
#define LPOWER_PMC_PMU_ENABLE           (SYSDEV->CFGCTL |= SYS_CFGMISC_PMU_EN)

#define LPOWER_STOPDEEP_PENDING_CLEAR   (SYSDEV->SRS |= SYS_SRS_STOPDEEP_PENDING)
#define LPOWER_STOP_PENDING_CLEAR       (SYSDEV->SRS |= SYS_SRS_STOP_PENDING)
#define LPOWER_STOP_FLAG_CLEAR          (SYSDEV->SRS |= SYS_SRS_STOP_PENDING)

#define LPOWER_STOPDEEP_FLAG            (SYSDEV->SRS & SYS_SRS_STOPDEEP_PENDING)
#define LPOWER_STOP_FLAG                (SYSDEV->SRS & SYS_SRS_STOP_PENDING)

#define LPOWER_GPIOWKUP_PENDING_CLEAR   (SYSDEV->SRS |= SYS_SRS_GPIOWKUP_PENDING)
#define LPOWER_GPIOWKUP_CFG             (SYSDEV->WSC |= 0x3)

#define LPOWER_STOPMODE_SET             (SYSDEV->STPMD |= STPMD_STOPEN_MASK)
#define LPOWER_STOPDEEPMODE_SET         (SYSDEV->STPMD |= STPMD_STOPDEEPEN_MASK)


#elif defined(CHIP_GM6621)
#define LPOWER_STOP_FLAG                (AOPDEV->SRS & SYS_AOP_SYS_STOPMODEFLAG_MASK)
#define LPOWER_STOP_FLAG_CLEAR          (AOPDEV->SRS |= SYS_AOP_SYS_STOPMODEFLAG_MASK)

#define LPOWER_D_LOAD_CLEAR             (AOPDEV->PMU &= ~SYS_AOP_PMU_LOADEN_MASK)

#define LPOWER_EFLASH_ENABLE            (AOPDEV->PMC |= SYS_AOP_PMC_EFLASHSTBYEN_MASK)
#define LPOWER_LCD_ENABLE               (AOPDEV->PMC |= SYS_AOP_PMC_LCDACCESSEN_MASK)

#define LPOWER_EFLASH_DISABLE           (AOPDEV->PMC &= ~SYS_AOP_PMC_EFLASHSTBYEN_MASK)
#define LPOWER_LCD_DISABLE              (AOPDEV->PMC &= ~SYS_AOP_PMC_LCDACCESSEN_MASK)

#define LPOWER_STANDBYMODE_CLEAR        (AOPDEV->PMC &= ~SYS_AOP_PMC_STBYEN_MASK)
#define LPOWER_STOPMODE_SET             (AOPDEV->PMC |= SYS_AOP_PMC_STOPEN_MASK | SYS_AOP_PMC_PMUEN_MASK)
#define LPOWER_LDO_HW_TRIM_SET          (AOPDEV->PMC |= SYS_AOP_PMC_LDOTRIMEN_MASK)
#define LPOWER_D_LOAD_ENABLE            (AOPDEV->PMU |= SYS_AOP_PMU_LOADEN_MASK)

#define LPOWER_PMC_PMU_ENABLE           (AOPDEV->PMC |= SYS_AOP_PMC_PMUEN_MASK)

#elif defined(CHIP_F6621B)
#define LPOWER_STANDBY_FLAG             (AOPDEV->LPCR[0] & SYS_AOP_SYS_STANDBYMODEFLAG_MASK)
#define LPOWER_STANDBY_FLAG_CLEAR       (AOPDEV->LPCR[0] |= SYS_AOP_SYS_STANDBYMODEFLAG_MASK)

#define LPOWER_STOP_FLAG                (SYSDEV->SRS & SYS_AOP_SYS_STOPMODEFLAG_MASK)
#define LPOWER_STOP_FLAG_CLEAR          (SYSDEV->SRS |= SYS_AOP_SYS_STOPMODEFLAG_MASK)

#define LPOWER_EFLASH_ENABLE            (SYSDEV->PWRMODE |= SYS_AOP_PMC_EFLASHSTBYEN_MASK)
#define LPOWER_LCD_ENABLE               (SYSDEV->PWRMODE |= SYS_AOP_PMC_LCDACCESSEN_MASK)

#define LPOWER_EFLASH_DISABLE           (SYSDEV->PWRMODE &= ~SYS_AOP_PMC_EFLASHSTBYEN_MASK)
#define LPOWER_LCD_DISABLE              (SYSDEV->PWRMODE &= ~SYS_AOP_PMC_LCDACCESSEN_MASK)

#define LPOWER_STANDBYMODE_CLEAR        (SYSDEV->PWRMODE &= ~SYS_AOP_PMC_STBYEN_MASK)
#define LPOWER_STANDBYMODE_SET          (SYSDEV->PWRMODE |= SYS_AOP_PMC_STBYEN_MASK)

#define LPOWER_STOPMODE_CLEAR           (SYSDEV->PWRMODE &= ~SYS_AOP_PMC_STOPEN_MASK)
#define LPOWER_STOPMODE_SET             (SYSDEV->PWRMODE |= SYS_AOP_PMC_STOPEN_MASK)
#define LPOWER_LDO_HW_TRIM_SET          (SYSDEV->PWRMODE |= SYS_AOP_PMC_LDOTRIMEN_MASK)

#endif



#if defined(CHIP_GM6621)||defined(CHIP_F6621B)
__STATIC_INLINE void StopMode_Enter(void)
{
    /*Set Stop mode & reset Standby mode*/
    LPOWER_STANDBYMODE_CLEAR;
    LPOWER_STOPMODE_SET;

    /*Enable Sleep Deep for Cortex-M0*/
    SCB->SCR |= (1UL << 2);

    /*Set LDO_HW_TRIM*/
    LPOWER_LDO_HW_TRIM_SET;
#if defined(CHIP_GM6621)
    /*Set D_LOAD_EN*/
    LPOWER_D_LOAD_ENABLE;
#endif
}

#if defined(CHIP_F6621B)
__STATIC_INLINE void StandbyMode_Enter(void)
{
    /*Set Standby mode & reset Stop mode */
    LPOWER_STOPMODE_CLEAR;
    LPOWER_STANDBYMODE_SET;

    /*Enable Sleep Deep for Cortex-M0*/
    SCB->SCR |= (1UL << 2);
}
#endif
#endif

#ifdef __cplusplus
}

#endif /*_LPower_DEV_H_*/

#endif

