/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          rtc.h
 *
 * @author        yangy
 *
 * @version       1.0.0
 *
 * @date          2017/04/19
 *
 * @brief         RTC Header For GM6601/6601B/6611/6621
 *
 * @note
 *    2017/04/19, yangy, V1.0.0
 *        Initial version.
 *    2017/05/12, yangy, V1.1.0
 *        Compatible with GM6601/6611/6621
 */

#ifndef _RTC_DEV_H_
#define _RTC_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <common.h>

#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_F6601C)|| defined (CHIP_GM6611) || defined (CHIP_GM6611B)
#define RTC_CLK                 (32768)

#elif defined (CHIP_GM6621)
/*#define RTC_CLK                 (8000000)*/
#define RTC_CLK                   (32768)

#endif

#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))

/* RTC Controller Register Layout Type Definition */
typedef struct
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611) || defined (CHIP_GM6611B) || defined (CHIP_F6601C)
    __IO uint32_t TM0_CTRL;         /**< RTC Timer 0 Control Register,              offset: 0x0         */
    __IO uint32_t RESV;             /**< RTC Reserved Register,                     offset: 0x4         */
    __IO uint32_t TM0_SI;           /**< RTC Timer 0 Init Second Register,          offset: 0x8         */
    __IO uint32_t TM0_MS;           /**< RTC Timer 0 Init Millisecond Register,     offset: 0xc         */
    __IO uint32_t TM0_SA;           /**< RTC Timer 0 Alarm Second Register,         offset: 0x10        */
    __IO uint32_t TM0_MSA;          /**< RTC Timer 0 Alarm Millisecond Register,    offset: 0x14        */
    __IO uint32_t TM0_SC;           /**< RTC Timer 0 Current Second Register,       offset: 0x18        */
    __IO uint32_t TM0_MSC;          /**< RTC Timer 0 Current Millisecond Register,  offset: 0x1c        */
    __IO uint32_t TM_INTST;         /**< RTC Interrupt Status Register,             offset: 0x20        */
#if defined (CHIP_GM6611B) || defined (CHIP_F6601C)
    __IO uint32_t TM0_TICK;         /**< Timer0 Tick value Register            offset: 0x24        */
#else
    __IO uint32_t RESV00;           /**< RTC Reserved Space,                                0x24        */
#endif
    __IO uint32_t RESV0[53];        /**< RTC Reserved Space,                                0x28~0x0F8  */
    __IO uint32_t TM_VER;           /**< RTC Version Register,                      offset: 0x0FC       */
    __IO uint32_t RESV30[192];      /**< RTC Reserved Space,                                0x100~0x3FC */
    __IO uint32_t TM1_CTRL;         /**< RTC Timer 1 Control Register,              offset: 0x400       */
    __IO uint32_t TM1_CMP;          /**< RTC Timer 1 Compare Register,              offset: 0x404       */
    __IO uint32_t TM1_CNT;          /**< RTC Timer 1 Count Register,                offset: 0x408       */
#if defined (CHIP_GM6611B)
    __IO uint32_t RESV01[61];       /**< RTC Reserved Space,                          0x40C~0x4FC       */
    __IO uint32_t TM1B_CTRL;        /**< RTC Timer 1 Control Register,              offset: 0x500       */
    __IO uint32_t TM1B_CMP;         /**< RTC Timer 1 Compare Register,              offset: 0x504       */
    __IO uint32_t TM1B_CNT;         /**< RTC Timer 1 Count Register,                offset: 0x508       */
    __IO uint32_t RESV02[61];       /**< RTC Reserved Space,                        0x50C~0x5FC       */
    __IO uint32_t TM1C_CTRL;        /**< RTC Timer 1 Control Register,              offset: 0x600       */
    __IO uint32_t TM1C_CMP;         /**< RTC Timer 1 Compare Register,              offset: 0x604       */
    __IO uint32_t TM1C_CNT;         /**< RTC Timer 1 Count Register,                offset: 0x608       */
    __IO uint32_t RESV1[125];       /**< RTC Reserved Space,                        0x60C~0x7FC        */
#else
    __IO uint32_t RESV1[253];       /**< RTC Reserved Space,                        0x40c~0x7FC */
#endif
    __IO uint32_t TM2_CTRL;         /**< RTC Timer 2 Control Register,              offset: 0x800       */
    __IO uint32_t TM2_LOAD;         /**< RTC Timer 2 Reload Value Register,         offset: 0x804       */
    __IO uint32_t TM2_CNT;          /**< RTC Timer 2 Counter Register,              offset: 0x808       */
    __IO uint32_t RESV2[253];       /**< RTC Reserved Space,                        0x804~0xBFC */
#endif
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    __IO uint32_t TM0;              /**< RTC Timer Register 0,                      offset: 0x0 (6611B off + 0xC00) */
    __IO uint32_t TM1;              /**< RTC Timer Register 1,                      offset: 0x4         */
    __IO uint32_t CTRL;             /**< RTC Control Register,                      offset: 0x8         */
    __IO uint32_t ALARM;            /**< RTC Alarm Register,                        offset: 0xC         */
    __IO uint32_t ALRMASK;          /**< RTC Alarm Mask Register,                   offset: 0x10        */
    __IO uint32_t TICKVAL;          /**< RTC Tick Value Register,                   offset: 0x14        */
    __IO uint32_t SUBSEC;           /**< RTC Sub Second Register,                   offset: 0x18        */
    __IO uint32_t TIMSTAMP0;        /**< RTC Time Stamp Register 0,                 offset: 0x1C        */
    __IO uint32_t TIMSTAMP1;        /**< RTC Time Stamp Register 1,                 offset: 0x20        */
    __IO uint32_t DCALCTL;          /**< RTC Calibration Register,                  offset: 0x24        */
    __IO uint32_t FLAG;             /**< RTC Flag Register,                         offset: 0x28        */
    __IO uint32_t INTEN;            /**< RTC Interrupt Mask Register,               offset: 0x2C        */
    __IO uint32_t INTCLR;           /**< RTC Interrupt Clear Register,              offset: 0x30        */
    __IO uint32_t TAMPER1;          /**< RTC Tamper1 Control Register,              offset: 0x34        */
    __IO uint32_t TAMPER2;          /**< RTC Tamper2 Control Register,              offset: 0x38        */
    __IO uint32_t RESV00;           /**< RTC Reserved Space,                                0x3C        */
    __IO uint32_t PROTECT;          /**< RTC Protect Register,                      offset: 0x40        */
    __IO uint32_t ALASSEC;          /**< RTC Alarm of Subsecond Register,           offset: 0x44        */
    __IO uint32_t WAKEUPT0;         /**< RTC Wakeup Timer Register0,                offset: 0x48        */
    __IO uint32_t WAKEUPT1;         /**< RTC Wakeup Timer Register1,                offset: 0x4C        */
    __IO uint32_t PES;              /**< RTC Event System Control Register,         offset: 0x50        */
    __IO uint32_t SPECIAL;          /**< RTC Special Register,                      offset: 0x54        */
    __IO uint32_t RESV3[41];        /**< RTC Reserved Space,                                0x58~0xF0   */
    __IO uint32_t VER;              /**< RTC Version Register,                      offset: 0xFC        */
#endif
} RTC_Device_T;

#define RTC_BASE                (0x40009000u)
#define RTC                     ((RTC_Device_T *)RTC_BASE)

#if defined CHIP_F6601C
#define RTC0_BASE_START                (0x40009000u)
#define RTC0_BASE_END                  (0x400090FCu)
#define RTC1_BASE_START                (0x40009400u)
#define RTC1_BASE_END                  (0x40009408u)
#define RTC2_BASE_START                (0x40009800u)
#define RTC2_BASE_END                  (0x40009808u)
#endif
/*for get FCLK div*/
#define CLKSEL0                 (*((volatile uint32_t  *)(0x40000040u)))
#if defined (CHIP_GM6601B) || defined (CHIP_F6601C)
#define FLCK_DIV_POS            (5)
#define FLCK_DIV_MASK           (0xE0<<FLCK_DIV_POS)
#else
#define FLCK_DIV_POS            (4)
#define FLCK_DIV_MASK           (0xFF<<FLCK_DIV_POS)
#endif

#define FLCK_SRC_POS            (0)
#define FLCK_SRC_MASK           (0x7<<FLCK_SRC_POS)



#define TM_INT_CLEAR            (0x1<<9)
#define TM_IE                   (0x1<<8)
#define TM_EN                   (0x1<<0)

#define MASK_ALL                (0xFFFFFFFF)
#define SECOND_MASK             (0xFFFFFFFF)
#define MILLISECOND_MASK        (0x3FF)
#define INTST_MASK              (0x7)

#define COUNTER_DIV_MASK        (0x3<<4)
#define COUNTER_DIV_POS         (4)

#define BASETIMER_BASE            (0x40036000u)
#define BASETIMER_CR              (HW32_REG(BASETIMER_BASE+0x0))
#define BASETIMER_LR              (HW32_REG(BASETIMER_BASE+0x4))
#define BASETIMER_CNT             (HW32_REG(BASETIMER_BASE+0x8))
#define BASETIMER_RST_REG         (HW32_REG(0x40000020))
#define BASETIMER_RST             (1<<21)
#define BASETIMER_IE              (1<<8)
#define BASETIMER_EN              (1<<0)
#define BASETIMER_INT             (1<<9)

#if defined (CHIP_GM6611B)
#define TIMER0_PRESCALE_MASK    (0xFFFF0000)
#define TIMER0_PRESCALE_POS     (16)
#define TIMER0_TICKVALUE_MASK   (0x000003FF)
#define TIMER0_TICKVALUE_POS    (0)
#else
#if defined (CHIP_F6601C)
#define TIMER0_MS_TICK_MASK     (0xFFFF0000)
#define TIMER0_MS_TICK_POS      (16)
#define TIMER0_TICKVALUE_MASK   (0x000003FF)
#define TIMER0_TICKVALUE_POS    (0)
#else
#define TIMER0_TICKVALUE_MASK   (0xFFFF0000)
#define TIMER0_TICKVALUE_POS    (16)
#endif
#endif


#define RTC_8M_CLK                 (8000000)
#define RTC_48M_CLK                (48000000)
#define RTC_32K_CLK                (32768)

#if defined (CHIP_GM6611B)
#define DEFAULT_PRESCALE_48_8M        (124)
#define DEFAULT_PRESCALE_32K       (63)
#endif

#if defined (CHIP_GM6621)
#define DEFAULT_PRESCALE_48M       (124)
#define DEFAULT_PRESCALE_32K_8M       (121)

#endif
/* time */
#define TM_YEAR_BASE    1900

#define EPOCH_YEAR      1970

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

/* True if the arithmetic type T is signed.  */
#define TYPE_SIGNED(t) (! ((t) 0 < (t) -1))

/* Shift A right by B bits portably, by dividing A by 2**B and
   truncating towards minus infinity.  A and B should be free of side
   effects, and B should be in the range 0 <= B <= INT_BITS - 2, where
   INT_BITS is the number of useful bits in an int.  GNU code can
   assume that INT_BITS is at least 32.

   ISO C99 says that A >> B is implementation-defined if A < 0.  Some
   implementations (e.g., UNICOS 9.0 on a Cray Y-MP EL) don't shift
   right in the usual way when A < 0, so SHR falls back on division if
   ordinary A >> B doesn't seem to be the usual signed shift.  */
#define SHR(a, b)                                               \
    ((-1 >> 1 == -1                                               \
      && (int) -1 >> 1 == -1                                 \
      && ((Time_T) -1 >> 1 == -1 || ! TYPE_SIGNED (Time_T)))      \
     ? (a) >> (b)                                                 \
     : (a) / (1 << (b)) - ((a) % (1 << (b)) < 0))

/* end of time */

typedef enum
{
    DATE_TIME_RTCTIME   = 0,
    DATE_TIME_TIMESTAMP = 1,
} DATE_TIME_TYPE;

/*----------------------------------------------------------
 * Protect
 *-----------------------------------------------------------*/
#define PROTECT_UNLOCK  ((uint32_t)0x1ACCE551)
#define PROTECT_LOCK    ((uint32_t)0x0)

/*----------------------------------------------------------
 * Bin Bcd Common
 *-----------------------------------------------------------*/
#define BIN_BCD_MODE        ((uint32_t)0x00000080)
#define BCD2BIN(x)          (((x) & 0x0f) + ((x) >> 4) * 10)
#define BIN2BCD(x)          ((((x) / 10) << 4) + (x) % 10)

/*----------------------------------------------------------
 * Bin Mode
 *-----------------------------------------------------------*/
#define BIN_YEAR_POS        (16)
#define BIN_YEAR_MASK       ((uint32_t)(0x3FFF<<BIN_YEAR_POS))

#define BIN_WEEK_POS        (13)
#define BIN_WEEK_MASK       ((uint32_t)(0x7<<BIN_WEEK_POS))

#define BIN_MONTH_POS       (8)
#define BIN_MONTH_MASK      ((uint32_t)(0x1F<<BIN_MONTH_POS))

#define BIN_DAY_POS         (0)
#define BIN_DAY_MASK        ((uint32_t)(0x3F<<BIN_DAY_POS))

// 1: PM
#define BIN_AMPM_POS        (22)
#define BIN_AMPM_MASK       ((uint32_t)(0x1<<BIN_AMPM_POS))

#define BIN_HOUR_POS        (16)
#define BIN_HOUR_MASK       ((uint32_t)(0x3F<<BIN_HOUR_POS))

#define BIN_MINUTE_POS      (8)
#define BIN_MINUTE_MASK     ((uint32_t)(0x7F<<BIN_MINUTE_POS))

#define BIN_SECOND_POS      (0)
#define BIN_SECOND_MASK     ((uint32_t)(0x7F<<BIN_SECOND_POS))

#define BIN_SUBSECOND_POS   (0)
#define BIN_SUBSECOND_MASK  ((uint32_t)(0xFFFF<<BIN_SUBSECOND_POS))

#define BIN_FORMAT12_POS    (0)
#define BIN_FORMAT12_MASK   ((uint32_t)(0x1<<BIN_FORMAT12_POS))

#define BIN_TIME(h, m, s, ampm) (((ampm)<<BIN_AMPM_POS)|((h)<<BIN_HOUR_POS)|((m)<<BIN_MINUTE_POS)|((s)<<BIN_SECOND_POS))
#define BIN_YEAR(y, m, d, w)    (((y)<<BIN_YEAR_POS)|((w)<<BIN_WEEK_POS)|((m)<<BIN_MONTH_POS)|((d)<<BIN_DAY_POS))

/*----------------------------------------------------------
 * Bcd Mode
 *-----------------------------------------------------------*/
#define BCD_H(x)                    (((x)&0xF0)>>4)
#define BCD_L(x)                    ((x)&0x0F)

#define BCD_YEAR(year_h, year_l)    (((BCD_H(year_h))<<12)|((BCD_L(year_h))<<8)|((BCD_H(year_l))<<4)|((BCD_L(year_l))<<0))
#define BCD_MONTH(month)            (((BCD_H(month))<<4)|((BCD_L(month))<<0))
#define BCD_DAY(day)                (((BCD_H(day))<<4)|((BCD_L(day))<<0))
#define BCD_WEEK(week)              ((BCD_L(week))<<0)
#define BCD_TIME(h,m,s,ampm)        (((BCD_L(ampm))<<22)|((BCD_H(h))<<20)|((BCD_L(h))<<16)|((BCD_H(m))<<12)|((BCD_L(m))<<8)|\
                                     ((BCD_H(s))<<4)|((BCD_L(s))<<0))
#define BCD_DATE(y,m,d,w)           ((y<<BIN_YEAR_POS)|(m<<BIN_MONTH_POS)|(d<<BIN_DAY_POS)|(w<<BIN_WEEK_POS))


#define BCD_YEAR_HIGH_GET_VALUE(v)  ((((v)&(0x3<<28))>>24)|(((v)&(0xF<<24))>>24))
#define BCD_YEAR_LOW_GET_VALUE(v)   ((((v)&(0xF<<20))>>16)|(((v)&(0xF<<16))>>16))
#define BCD_MONTH_GET_VALUE(v)      ((((v)&(0x1<<12))>>8)|(((v)&(0xF<<8))>>8))
#define BCD_DAY_GET_VALUE(v)        (((v)&(0x3<<4))|((v)&(0xF)))
#define BCD_WEEK_GET_VALUE(v)       (((v)&(0x7<<13))>>13)

#define BCD_YEAR_HIGH_GET()         BCD_YEAR_HIGH_GET_VALUE(*RTC_TM1)
#define BCD_YEAR_LOW_GET()          BCD_YEAR_LOW_GET_VALUE(*RTC_TM1)
#define BCD_MONTH_GET()             BCD_MONTH_GET_VALUE(*RTC_TM1)
#define BCD_DAY_GET()               BCD_DAY_GET_VALUE(*RTC_TM1)
#define BCD_WEEK_GET()              BCD_WEEK_GET_VALUE(*RTC_TM1)

#define BCD_YEAR_HIGH(v)            BCD_YEAR_HIGH_GET_VALUE(v)
#define BCD_YEAR_LOW(v)             BCD_YEAR_LOW_GET_VALUE(v)


#define BCD_HOUR_GET_VALUE(v)       ((((v)&(0x3<<20))>>16)|(((v)&(0xF<<16))>>16))
#define BCD_MINUTE_GET_VALUE(v)     ((((v)&(0x7<<12))>>8)|(((v)&(0xF<<8))>>8))
#define BCD_SECOND_GET_VALUE(v)     ((((v)&(0x7<<4)))|(((v)&(0xF))))
#define BCD_AMPM_GET_VALUE(v)       (((v)&(0x1<<22))>22)

#define BCD_HOUR_GET()              BCD_HOUR_GET_VALUE(*RTC_TM0)
#define BCD_MINUTE_GET()            BCD_MINUTE_GET_VALUE(*RTC_TM0)
#define BCD_SECOND_GET()            BCD_SECOND_GET_VALUE(*RTC_TM0)
#define BCD_AMPM_GET()              BCD_AMPM_GET_VALUE(*RTC_TM0)

#define RTC_CLK_FREQ_32K            (32768)
#define RTC_CLK_FREQ_8M             (8000000)
#define RTC_CLK_FREQ_48M            (48000000)

/*----------------------------------------------------------
 * Tick Cycles
 *-----------------------------------------------------------*/
#if defined (CHIP_GM6611B)

/* tm0 tick max: 2^10 - 1 = 1023(0x3FF)
 * tm0 prescaler max: 2^32 - 1 = 0xFFFFFFFF
 */
/* tm1/tm2 tick max: 2^32 - 1 = 0xFFFFFFFF
 */
/* calendar tick max: 2^16 - 1 = 65535(0xFFFF)
 * calendar prescaler max: 2^7 - 1 = 127(0x3F)
 */
#if RTC_CLK == 32768
#define PRESCALE          (63)
#define PRESCALE_CALENDAR   (63)
#define TICKS_PER_1S      (RTC_CLK / (PRESCALE + 1)- 1)      /**< =511(0x1FF)*/
#define TICKS_PER_1S_CALENADR (RTC_CLK / 2 / (PRESCALE_CALENDAR + 1)- 1)  /**< =255(0xFF) */
#elif RTC_CLK == 48000000
#define PRESCALE          (46874)
#define PRESCALE_CALENDAR   (124)
#define TICKS_PER_1S      (RTC_CLK / (PRESCALE + 1) - 1)      /**< =1023(0x3FF) */
/*
 * The max tick value is 0xffff, so that use 16Mhz as the default frequency,
 * which will cause that 1s of RTC is equal to 1/3s of the actual time.
 * One 1s-cycle of the counters will consume 3s of the actual time.
 */
#define TICKS_PER_1S_CALENADR ((RTC_CLK/3) / 2 / (PRESCALE_CALENDAR + 1)- 1)  /**< =63999(0xF9FF) max value is 0xFFFF*/
#elif RTC_CLK == 8000000
#define PRESCALE          (7811)
#define PRESCALE_CALENDAR   (124)
#define TICKS_PER_1S      (RTC_CLK / (PRESCALE + 1) - 1)      /**< =1023(0x3FF) */
#define TICKS_PER_1S_CALENADR (RTC_CLK / 2 / (PRESCALE_CALENDAR + 1)- 1)  /**< =31999(0x7CFF) */
#endif

#elif defined (CHIP_GM6601) || defined (CHIP_GM6601B)
#define PRESCALE            (1)
#define TICKS_PER_1S        (RTC_CLK / 1000)

#else /* CHIP_GM6621 */
#if RTC_CLK == 48000000
/*
 * The max tick value is 0xffff, so that use 16Mhz as the default frequency,
 * which will cause that 1s of RTC is equal to 1/3s of the actual time.
 */
#define PRESCALE            (124)
#define TICKS_PER_1S        ((RTC_CLK/3) / 2 / (PRESCALE + 1) - 1)
#else //not 48MHz
#define PRESCALE            (121)
#define TICKS_PER_1S        (RTC_CLK / 2 / (PRESCALE + 1) - 1)
#endif

#define PRESCALE_CALENDAR   (PRESCALE)
#define TICKS_PER_1S_CALENADR TICKS_PER_1S

#endif /* end of CHIP_GM6621 */

#define PRESCALE_POS        (16)
#define PRESCALE_MASK       ((uint32_t)(0x7F<<PRESCALE_POS))

#define TICKVAULE_POS       (0)
#define TICKVALUE_MASK      ((uint32_t)(0xFFFF<<TICKVAULE_POS))

/*----------------------------------------------------------
 * Interrupt
 *-----------------------------------------------------------*/
#define IRQ_PES_EN              (1<<17)
#define IRQ_YEAR_EN             (1<<16)
#define IRQ_MONTH_EN            (1<<15)
#define IRQ_DAY_EN              (1<<14)
#define IRQ_HOUR_EN             (1<<13)
#define IRQ_MINUTE_EN           (1<<12)
#define IRQ_SECOND_EN           (1<<11)
#define IRQ_WAKEUP_WP4F_EN      (1<<10) /**< 1000ms@RTCCLK  */
#define IRQ_WAKEUP_WP3F_EN      (1<<9)  /**< 62.5ms@RTCCLK  */
#define IRQ_WAKEUP_WP2F_EN      (1<<8)  /**< 125ms@RTCCLK   */
#define IRQ_WAKEUP_WP1F_EN      (1<<7)  /**< 250ms@RTCCLK   */
#define IRQ_WAKEUP_WP0F_EN      (1<<6)  /**< 500ms@RTCCLK   */
#define IRQ_TIMSTAMPOV_IE       (1<<5)  /**< time-stamp overflow event  */
#define IRQ_TIMSTAMP_IE         (1<<4)
#define IRQ_TAMPER2_IE          (1<<2)
#define IRQ_TAMPER1_IE          (1<<1)
#define IRQ_ALARM_IE            (1<<0)

#define IRQ_CLEAR_ALL_PENDING_FLAG_MASK     ((uint32_t)1<<31)

/*----------------------------------------------------------
 * Tamper
 *-----------------------------------------------------------*/
#define TAMPPUDIS_POS               (10)    /**< 0:precharge, enable internal pull-up */
#define TAMPPRCH_POS                (8)     /**< Pull-up duration: 2^x RTCLK cycles (0~3) */
#define TAMPFLT_POS                 (6)     /**< Filter count: 2^x consecutive samples (0~3) */
#define TAMPFRE_POS                 (3)     /**< Tamper input sample frequency: RTCLK/(2^x)  (0~7) */
#define TAMPTS                      (2)     /**< 1:save timestamp */
#define TAMPTRG                     (1)     /**< Trigger 1: high level */
#define TAMPE                       (0)     /**< 1:enable */

/**< tamper1 input is XPD5(IO 109), the default level is high. */
/**< tamper2 input is XPA4(IO  92), the default level is high. */
#define TAMPER_TRIGGER_LEVEL        (0) /**< low level to trigger */

/*----------------------------------------------------------
 * Alarm
 *-----------------------------------------------------------*/
#define ALARM_EN                    (1<<1)

#define ALARM_SUBSEC_ALLBIT_CHECK   (15)

#define ALARM_SUBSEC_AE_POS     (0x4)   /**< SUBSEC[14:x] are don't care, only SUBCSEC[x-1:0]are compared */
#define ALARM_DAY_AE_POS        (0x3)
#define ALARM_HOUR_AE_POS       (0x2)
#define ALARM_MIN_AE_POS        (0x1)
#define ALARM_SEC_AE_POS        (0x0)

#define ALARM_SUBSEC_AE_MASK    (0xF  << ALARM_SUBSEC_AE_POS)

#define ALARM_WD_TEST           (0x1)
#define ALARM_WD_POS            (30)    /**< 0: the DAY is the day of month, 1: the DAY[3:0] is the week day */
#define ALARM_WD_MASK           (ALARM_WD_TEST  << ALARM_WD_POS)
#define ALARM_DAY_TEST          (0x3F)
#define ALARM_DAY_POS           (24)
#define ALARM_DAY_MASK          (ALARM_DAY_TEST << ALARM_DAY_POS)
#define ALARM_AMPM_POS          (22)
#define ALARM_AMPM_MASK         (0x1  << ALARM_AMPM_POS)
#define ALARM_HOUR_TEST         (0x3F)
#define ALARM_HOUR_POS          (16)
#define ALARM_HOUR_MASK         (ALARM_HOUR_TEST << ALARM_HOUR_POS)
#define ALARM_MIN_POS           (8)
#define ALARM_MIN_TEST          (0x3F)
#define ALARM_MIN_MASK          (ALARM_MIN_TEST << ALARM_MIN_POS)
#define ALARM_SEC_POS           (0)
#define ALARM_SEC_TEST          (0x3F)
#define ALARM_SEC_MASK          (ALARM_SEC_TEST << ALARM_SEC_POS)

/**< set RTC_OUT Output  */
/**< GM6621 FPGA - XPB5 - IO3 */
/**< GM6621 ES   - XPB8       */
#define RTCOUT1                 (HW32_REG(0x4000015C))
#define RTCOUT2                 (HW32_REG(0x40000144))

#define RTCOUT_COE_EN           (0x1<<6)
#define RTCOUT_COSEL_EN         (0x1<<5)

/**< 0x4000015C[3:0]=5;   0x40000144[17:16]=1; */

/**< Bin Mode */
#define ALARM_BIN_TIME(wd, day, h, m, s, ampm) (((wd)<<ALARM_WD_POS)|((day)<<ALARM_DAY_POS)|((ampm)<<ALARM_AMPM_POS)|\
        ((h)<<ALARM_HOUR_POS)|((m)<<ALARM_MIN_POS)|((s)<<ALARM_SEC_POS))
/**< Bcd Mode */
#define ALARM_BCD_TIME(wd, day, h, m, s, ampm) (((BCD_L(ampm))<<22)|((BCD_H(h))<<20)|((BCD_L(h))<<16)|\
        ((BCD_H(m))<<12)|((BCD_L(m))<<8)|((BCD_H(s))<<4)|\
        ((BCD_L(s))<<0)|((BCD_H(day))<<28)|((BCD_L(day))<<24)|((BCD_L(wd))<<30))

/*----------------------------------------------------------
 * Timestamp
 *-----------------------------------------------------------*/
#define TIMESTAMP_SUBSECOND_POS     (16)
#define TIMESTAMP_SUBSECOND_MASK    ((uint32_t)(((uint32_t)0xFFFF)<<TIMESTAMP_SUBSECOND_POS))

/*----------------------------------------------------------
 * Wakeup
 *-----------------------------------------------------------*/
#define MAX_WAKEUP_CNT          ((1<<16)-1) /**< 65535 */
#define ONCE_WAKEUP_TIME        ((MAX_WAKEUP_CNT*1000)/(RTCCLK/2))

#define TDIVSEL 0

#define WAKEUPINI_POS           (0)
#define WAKEUPINI_MASK          (0xFFFF<<WAKEUPINI_POS)



/*----------------------------------------------------------
 * Timer Hold
 *-----------------------------------------------------------*/
#define TIMER_HOLD_MASK         (1<<21)

/*----------------------------------------------------------
 * Safety
 *-----------------------------------------------------------*/
/* Ctrl */
#define CK_CHG              (0x1<<30)
#define WRDY_ERR            (0x1<<28)

#define EVSEL_EN            (0x1<<26)
#define WAKEUP_EN           (0x1<<25)
#define ACMP_TRIEN          (0x1<<24)

#define WRDY                (0x1<<22)
#define HOLD                (0x1<<21)
#define ACMP_SEL_MASK       (0x7<<17)
#define EV_SEL_MASK         (0xF<<13)

#define TDIVSEL_POS         (8)
#define TDIVSEL_MASK        (0x7<<TDIVSEL_POS)


#define LOCK                (0x1<<11)


/* Specail */
#define SAFE                (0x1<<18)
#define EVSEL_WEF           (0x1<<17)
#define ACMPTRI_WEF         (0x1<<16)
#define WAKEUP_WEF          (0x1<<15)
#define ALARM_WEF           (0x1<<14)
#define TAMP1_WEF           (0x1<<13)
#define TAMP2_WEF           (0x1<<12)

#define TICK_WEF            (0x1<<10)
#define PES_WEF             (0x1<<9)

#define SOFT_TST2           (0x1<<3)
#define SOFT_TST            (0x1<<2)

#define ACMP_SEL_WAKEUP4    (0x5<<17)
#define EV_SEL_WAKEUP4      (0xF<<13)
#define EV_SEL_ALARM        (0x4<<13)
#define EV_SEL_TAMPER1      (0x1<<13)
#define EV_SEL_TAMPER2      (0x2<<13)


void Timer0_Disable(void);
void Timer0_DisableInterrupt(void);
void Timer0_Enable(void);
void Timer0_EnableInterrupt(void);
uint32_t Timer0_GetMilliSecond(void);
uint32_t Timer0_GetSecond(void);
void Timer0_InitMilliSecond(uint32_t value);
void Timer0_InitSecond(uint32_t value);
void Timer0_SetAimMilliSecond(uint32_t value);
void Timer0_SetAimSecond(uint32_t value);
void Timer0_SetTickValue(int value);
void Timer1_Disable(void);
void Timer1_DisableInterrupt(void);
void Timer1_Enable(void);
void Timer1_EnableInterrupt(void);
void Timer1_SetCntValue(uint32_t value);
void Timer1_SetCmpValue(uint32_t value);
void Timer1_SetTickValue(uint32_t value);
#if defined (CHIP_GM6611B)
void Timer1B_SetCntValue(uint32_t value);
void Timer1B_SetCmpValue(uint32_t value);
void Timer1B_SetTickValue(uint32_t value);
uint32_t Timer1B_GetTickValue(void);
void Timer1B_EnableInterrupt(void);
void Timer1B_DisableInterrupt(void);
void Timer1B_Enable(void);
void Timer1B_Disable(void);
void Timer1B_SetPrescaler(uint32_t value);
void Timer1C_SetCntValue(uint32_t value);
void Timer1C_SetCmpValue(uint32_t value);
void Timer1C_SetTickValue(uint32_t value);
uint32_t Timer1C_GetTickValue(void);
void Timer1C_EnableInterrupt(void);
void Timer1C_DisableInterrupt(void);
void Timer1C_Enable(void);
void Timer1C_Disable(void);
void Timer1C_SetPrescaler(uint32_t value);
#endif

void Timer2_Disable(void);
void Timer2_DisableInterrupt(void);
void Timer2_Enable(void);
void Timer2_EnableInterrupt(void);
void Timer2_SetTickValue(uint32_t value);
void Timer2_SetLoadValue(uint32_t value);
void Timer2_SetCntValue(uint32_t value);
void Timer_DisableAll(void);
void RTC_Unlock(void);
void RTC_Lock(void);
uint32_t RTC_CanReadSafely(void);
void RTC_DumpTimeSafely(void);
void RTC_WaitClockReady(void);

#if defined (CHIP_GM6611B) || defined (CHIP_GM6621)
void RTC_DisableSecondIrq(void);
void RTC_EnableSecondIrq(void);
void RTC_DisableWakeupIrq(uint8_t wakeupIndex);
void RTC_EnableWakeupIrq(uint8_t wakeupIndex);
#endif

#if defined (CHIP_F6601C)
 uint32_t CHECK_IO_BIT_GET(volatile uint32_t *Reg, volatile uint32_t Bit);
 void CHECK_IO_BIT_SET(volatile uint32_t *Reg, volatile uint32_t Bit);
 void CHECK_IO_BIT_CLEAR(volatile uint32_t *Reg, volatile uint32_t Bit);
 int32_t CHECK_IO_BITS_GET(volatile uint32_t *Reg, volatile uint32_t Bits);
 void CHECK_IO_BITMASK_SET(volatile uint32_t *Reg, volatile uint32_t Bitmask, volatile uint32_t Value);
#endif

#ifdef __cplusplus
}
#endif


#endif //_RTC_DEV_H_
