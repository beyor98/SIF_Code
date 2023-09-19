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
 * @brief         RTC Driver For GM6601/GM6601B/GM6611/GM6621.
 *
 * @note
 *    2017/04/19, yangy, V1.0.0
 *        Initial version.
 *    2017/05/12, yangy, V1.1.0
 *        Compatible with GM6601/6611/6621
 */

#include "rtc_dev.h"
#include <drv/dev/rtc.h>

/**************************** ***************************************************
* Constants and macros
*******************************************************************************/

#define DEBUG_MODE 0

#define DBG_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

typedef unsigned char bool;
typedef unsigned char TimerId_T;

#define True    1
#define False   0

typedef void (*TimerOper)();
typedef void (*TimerOperWithArg)(uint32_t value);
typedef uint32_t (*TimerOperWithReturn)(void);

typedef struct
{
    TimerOper enable;
    TimerOper disable;
    bool alarm_on;
    TimerOperWithArg set_aim_sec;
    TimerOperWithArg set_aim_msec;
    TimerOperWithReturn get_current_sec;
    TimerOperWithReturn get_current_msec;
    /* Indicate the cycle time of iterative expirations */
    Timer_T cycle_timer;
} RTC_Timer_T;

typedef struct
{
    TimerOper enable;
    TimerOper disable;
    TimerOperWithArg set_tick_value;
    TimerOperWithReturn get_tick_value;
    TimerOperWithArg set_prescaler;
    uint32_t freq;
} RTC_Counter_T;


const unsigned short int __mon_yday[2][13] =
{
    /* Normal years.  */
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    /* Leap years.  */
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};


#if (defined(CHIP_GM6601B)||defined(CHIP_F6601C))
static uint32_t rtc_clk_freq[4] =
{
    RTC_8M_CLK,
    RTC_32K_CLK,
    RTC_32K_CLK,
    RTC_48M_CLK,
};

static uint32_t precaler_value[4]=
{
    16,
    32,
    64,
    256
};


#elif defined (CHIP_GM6611B)
static uint32_t rtc_clk_freq[2] =
{
    RTC_48M_CLK,
    RTC_32K_CLK,
};
#elif defined (CHIP_GM6621)
static uint32_t rtc_clk_freq[6] =
{
    RTC_48M_CLK,
    RTC_32K_CLK,
    RTC_32K_CLK,
    RTC_8M_CLK,
    RTC_8M_CLK,
};
#endif



/* GMXX wday is 1~7 while the standard utctime wday is 0~6 */
#define ConvertToWday0(x)   ((x) = ((x) == 7 ? 0 : (x)))
#define ConvertToWday1(x)   ((x) = ((x) == 0 ? 7 : (x)))

#define SET_ERROR(val) (sTimeErrno = (val))

#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)||defined(CHIP_F6601C)
    #define COUNTER_CNT 2
#elif defined (CHIP_GM6611B)
    #define COUNTER_CNT 4
#elif defined (CHIP_GM6621)
    #define COUNTER_CNT 1
#endif

typedef struct
{
    uint32_t        rtc_clk_freq;
    uint32_t        prescaler;
    uint32_t        prescaler_calendar;
    uint32_t        ticks_per_1s;
    uint32_t        ticks_per_1s_calendar;
    UTCTime_T       utc_time;
    RTC_Timer_T     rtc_timer;
    RTC_Counter_T   rtc_counter[COUNTER_CNT];
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    /*--------------------------------------------------------------
     * 1 means date and time are stored in the registers in Bin mode
     * 0 means date and time are stored in the registers in BCD mode
     *--------------------------------------------------------------*/
    bool bin_mode;
    uint32_t wakeup_cycle[5];/* the unit: ms*/
    int current_wakeup_index;
    uint8_t tdivsel;
    bool locked;
    IRQn_Type calendar_irq;
#endif
    Timer_Callback alarm_handler;
    Timer_Callback timer_handler;
    Timer_Callback counter_handler;
} RTC_Driver_T;

/*******************************************************************************
* Global variables
*******************************************************************************/
/*******************************************************************************
* Static variables
*******************************************************************************/
static RTC_Driver_T sRtcDriverT;
/*******************************************************************************
* Local function prototypes
*******************************************************************************/

#if !defined (CHIP_GM6621)
    static void RTC_AimCall(void);
#endif
static uint32_t UtcTime_CalcWeekday(uint32_t y, uint32_t m, uint32_t d);

static void RTC_GlobalInit(RTC_Config_T *pConfig);

/*******************************************************************************
* Codes
*******************************************************************************/
#if defined (CHIP_F6601C)  
static void Check_SysReg(uint32_t  RTC_Addr)
{
    if ((RTC_Addr >= RTC0_BASE_START) && (RTC_Addr <= RTC0_BASE_END))
    {
        while (SYSDEV->DBGCTRL & SYS_RTC0_WRITE_DONE_FLAG) ;   
    }
    else if ((RTC_Addr >= RTC1_BASE_START) && (RTC_Addr <= RTC1_BASE_END))
    {
        while (SYSDEV->DBGCTRL & SYS_RTC1_WRITE_DONE_FLAG) ;
    }
    else if ((RTC_Addr >= RTC2_BASE_START) && (RTC_Addr <= RTC2_BASE_END))
    {
        while (SYSDEV->DBGCTRL & SYS_RTC2_WRITE_DONE_FLAG) ;
    }


}
 uint32_t CHECK_IO_BIT_GET(volatile uint32_t *Reg, volatile uint32_t Bit)
{
    Check_SysReg((uint32_t)Reg);
    return ((*Reg) & (Bit));
}
 void CHECK_IO_BIT_SET(volatile uint32_t *Reg, volatile uint32_t Bit)
{
    Check_SysReg((uint32_t)Reg);
    (*Reg) |= (Bit) ;
}
 void CHECK_IO_BIT_CLEAR(volatile uint32_t *Reg, volatile uint32_t Bit)
{
    Check_SysReg((uint32_t)Reg);
    *(Reg)&= ~(Bit);
}
 int32_t CHECK_IO_BITS_GET(volatile uint32_t *Reg, volatile uint32_t Bits)
{
    Check_SysReg((uint32_t)Reg);
    return ((*Reg) & (Bits));
}

 void CHECK_IO_BITMASK_SET(volatile uint32_t *Reg, volatile uint32_t Bitmask, volatile uint32_t Value)
{
    Check_SysReg((uint32_t)Reg);
    *(Reg) = *(Reg) & (~Bitmask);
    *(Reg) |= Value;
}

#endif


/* Compute the `UTCTime_T' representation of *T,
   offset OFFSET seconds east of UTC,
   and store year, yday, mon, mday, wday, hour, min, sec into *TP.
   Return nonzero if successful.  */
static int UtcTime_OfftimeCompute(const Time_T *t, long int offset, UTCTime_T *tp)
{
#define SECS_PER_HOUR   (60 * 60)
#define SECS_PER_DAY    (SECS_PER_HOUR * 24)
    Time_T days, rem, y;
    const unsigned short int *ip;

    days = *t / SECS_PER_DAY;
    rem = *t % SECS_PER_DAY;
    rem += offset;
    while (rem < 0)
    {
        rem += SECS_PER_DAY;
        --days;
    }
    while (rem >= SECS_PER_DAY)
    {
        rem -= SECS_PER_DAY;
        ++days;
    }
    tp->hour = rem / SECS_PER_HOUR;
    rem %= SECS_PER_HOUR;
    tp->min = rem / 60;
    tp->sec = rem % 60;
    /* January 1, 1970 was a Thursday.  */
    tp->wday = (4 + days) % 7;
    if (tp->wday < 0)
    {
        tp->wday += 7;
    }
    y = 1970;

#define DIV(a, b) ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

    while (days < 0 || days >= (isleap(y) ? 366 : 365))
    {
        /* Guess a corrected year, assuming 365 days per year.  */
        Time_T yg = y + days / 365 - (days % 365 < 0);

        /* Adjust DAYS and Y to match the guessed year.  */
        days -= ((yg - y) * 365
                 + LEAPS_THRU_END_OF(yg - 1)
                 - LEAPS_THRU_END_OF(y - 1));
        y = yg;
    }
    tp->year = y - 1900;
    if (tp->year != y - 1900)
    {
        /* The year cannot be represented due to overflow.  */
        SET_ERROR(EOVERFLOW);
        return 0;
    }
    tp->yday = days;
    ip = __mon_yday[isleap(y)];
    for (y = 11; days < (long int) ip[y]; --y)
    {
        continue;
    }
    days -= ip[y];
    tp->mon = y;
    tp->day = days + 1;
    return 1;
}

/* Return the `UTCTime_T' representation of *TIMER in the local timezone.
   Use local time if USE_LOCALTIME is nonzero, UTC otherwise.  */
static UTCTime_T *UtcTime_Convert(const Time_T *timer, UTCTime_T *tp)
{
    if (timer == 0)
    {
        SET_ERROR(EINVAL);
        return 0;
    }

    if (! UtcTime_OfftimeCompute(timer, 0, tp))
    {
        tp = 0;
    }

    tp->year += TM_YEAR_BASE;
    tp->mon  += 1;
    ConvertToWday1(tp->wday);

    return tp;
}

static Time_T UtcTime_DiffYDHMS(int year1, int yday1, int hour1, int min1, int sec1,
                                int year0, int yday0, int hour0, int min0, int sec0)
{

    /* Compute intervening leap days correctly even if year is negative.
       Take care to avoid integer overflow here.  */
    /*              year1   year0
     *              2017    1970
     * 4-years      504     492
     * 100-years    20      19
     * 400-years    5       4
     * intervening_leap_days 12 (there are 12 leap years in 1970~2017)
     */
    int a4 = SHR(year1, 2) + SHR(TM_YEAR_BASE, 2) - !(year1 & 3);
    int b4 = SHR(year0, 2) + SHR(TM_YEAR_BASE, 2) - !(year0 & 3);
    int a100 = a4 / 25 - (a4 % 25 < 0);
    int b100 = b4 / 25 - (b4 % 25 < 0);
    int a400 = SHR(a100, 2);
    int b400 = SHR(b100, 2);
    int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);

    /* Compute the desired time in Time_T precision.  Overflow might
       occur here.  */
    Time_T tyear1 = year1;
    Time_T years = tyear1 - year0;
    Time_T days = 365 * years + yday1 - yday0 + intervening_leap_days;
    Time_T hours = 24 * days + hour1 - hour0;
    Time_T minutes = 60 * hours + min1 - min0;
    Time_T seconds = 60 * minutes + sec1 - sec0;
    return seconds;
}

/* Return 1 if YEAR + TM_YEAR_BASE is a leap year.  */
static int UtcTime_Leapyear(int year)
{
    /* Don't add YEAR to TM_YEAR_BASE, as that might overflow.
       Also, work even if YEAR is negative.  */
    return
                    ((year & 3) == 0
                     && (year % 100 != 0
                         || ((year / 100) & 3) == (- (TM_YEAR_BASE / 100) & 3)));
}

/* Convert *TP to a Time_T value */
static Time_T UtcTime_InternalMake(UTCTime_T *tp)
{
    Time_T secondFrom1970;

    /* Time requested.  Copy it in case CONVERT modifies *TP; this can
       occur if TP is localtime's returned value and CONVERT is localtime.  */
    int sec = tp->sec;
    int min = tp->min;
    int hour = tp->hour;
    int mday = tp->day;
    int mon = tp->mon;
    int year_requested = tp->year;

    /* Ensure that mon is in range, and set year accordingly.  */
    int mon_remainder = mon % 12;
    int negative_mon_remainder = mon_remainder < 0;
    int mon_years = mon / 12 - negative_mon_remainder;
    int lyear_requested = year_requested;
    int year = lyear_requested + mon_years;

    /* Calculate day of year from year, month, and day of month.
       The result need not be in range.  */
    int mon_yday = ((__mon_yday[UtcTime_Leapyear(year)]
                     [mon_remainder + 12 * negative_mon_remainder])
                    - 1);
    int lmday = mday;
    int yday = mon_yday + lmday;

    /* to = seconds from 1970/01/01 00:00:00 to y/m/d h:m:s */
    secondFrom1970 = UtcTime_DiffYDHMS(year, yday, hour, min, sec,
                                       EPOCH_YEAR - TM_YEAR_BASE, 0, 0, 0, 0);
    return secondFrom1970;
}

/**
* @brief Get the weekday on the day y/m/d
* @param y - year
* @param m - month
* @param d - day
* @return weekday from 1 to 7
* @note
*/
static uint32_t UtcTime_CalcWeekday(uint32_t y, uint32_t m, uint32_t d)
{
    uint32_t week = 0;

    if (m == 1 || m == 2)
    {
        m += 12;
        y--;
    }

    week = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;

    return (week + 1); /* from 1 to 7 */
}


#if (DEBUG_MODE)
/* Print some arguments to help to analysis */
static void RTC_PrintArgs(void)
{
    PR_DEBUG("--------------------------------\n");
    PR_DEBUG("-        RTC_PrintArgs         -\n");
    PR_DEBUG("--------------------------------\n");
#if defined (CHIP_GM6601)
    PR_DEBUG("Platform:%s\n", "CHIP_GM6601");
#elif defined (CHIP_GM6601B)
    PR_DEBUG("Platform:%s\n", "CHIP_GM6601B");
#elif defined(CHIP_F6601C)
    PR_DEBUG("Platform:%s\n", "CHIP_F6601C");
#elif defined (CHIP_GM6611)
    PR_DEBUG("Platform:%s\n", "CHIP_GM6611");
#elif defined (CHIP_GM6611B)
    PR_DEBUG("Platform:%s\n", "CHIP_GM6611B");
#elif defined (CHIP_GM6621)
    PR_DEBUG("Platform:%s\n", "CHIP_GM6621");
#endif
    PR_DEBUG("RTC clock:%dHz\n", sRtcDriverT.rtc_clk_freq);
    PR_DEBUG("prescaler:%d\n", PRESCALE);
    PR_DEBUG("ticks_per_second:%d\n", TICKS_PER_1S);
    PR_DEBUG("--------------------------------\n\n");
}
#endif /* #if (DEBUG_MODE) */


#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)

void RTC_WaitReady(void);


/* Ought to unlock the register firstly before almost all writing about rtc registers.
    Use the variable `sRtcDriverT.locked` to avoid multiple call. */

void RTC_Lock(void)
{
    if (!sRtcDriverT.locked)
    {
        IO_BITMASK_SET(RTC->PROTECT, MASK_ALL, PROTECT_LOCK);
        sRtcDriverT.locked = True;
    }
}

void RTC_Unlock(void)
{
    if (sRtcDriverT.locked)
    {
        IO_BITMASK_SET(RTC->PROTECT, MASK_ALL, PROTECT_UNLOCK);
        sRtcDriverT.locked = False;
    }
}

/* RTC trigger acmp use wakeup4 in default */
void RTC_TriggerACMPEnable(void)
{
    RTC_Unlock();

    IO_BIT_CLEAR(RTC->CTRL, ACMP_TRIEN);
    while (!(IO_BIT_GET(RTC->SPECIAL, ACMPTRI_WEF)));
    IO_BITMASK_SET(RTC->CTRL, ACMP_SEL_MASK, ACMP_SEL_WAKEUP4);
    IO_BIT_SET(RTC->CTRL, ACMP_TRIEN);

    RTC_Lock();
}

void RTC_TriggerACMPDisable(void)
{
    RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, ACMP_TRIEN);
    RTC_Lock();
}

/* RTC trigger adc use alarm pes source in default */
void RTC_AlarmPESEnable(void)
{
    RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, EVSEL_EN);
    while (!(IO_BIT_GET(RTC->SPECIAL, EVSEL_WEF)));
    IO_BITMASK_SET(RTC->CTRL, EV_SEL_MASK, EV_SEL_ALARM);
    IO_BIT_SET(RTC->CTRL, EVSEL_EN);

    RTC_Lock();
}

/**
* @brief Enable Tamper irq
* @param
* @return
* @note
*/
void RTCIrq_Tamper_Enable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    RTC_Unlock();
    RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, MASK_ALL, IRQ_TAMPER1_IE | IRQ_TAMPER2_IE);
    RTC_Lock();

    NVIC_EnableIRQ(RTC_IRQn);
}


/**
* @brief Disable Tamper irq
* @param
* @return
* @note
*/
void RTCIrq_Tamper_Disable(void)
{

    NVIC_ClearPendingIRQ(RTC_IRQn);

    /*Interrupt enable*/
    RTC_Unlock();
    RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN, IRQ_TAMPER1_IE | IRQ_TAMPER2_IE);
    RTC_Lock();

    NVIC_DisableIRQ(RTC_IRQn);
}


/**
* @brief Two tampers test with different samples
* @param
* @return 0
* @note
*/
int RTCTamper_Enable()
{
    RTCIrq_Tamper_Enable();

    RTC_Unlock();

    /* tampflt :  2 ^ 0 consecutive samples,  more sensitive */
    /* tampfre :  input sample RTCLK/(2^1) */
    IO_BITMASK_SET(RTC->TAMPER1, MASK_ALL, ((0 << TAMPPUDIS_POS) | (3 << TAMPPRCH_POS) | (3 << TAMPFLT_POS) |
                                            (1 << TAMPFRE_POS) | (1 << TAMPTS) | (TAMPER_TRIGGER_LEVEL << TAMPTRG) | (1 << TAMPE)));
    /* tampflt :  2 ^ 3 consecutive samples */
    /* tampfre :  input sample RTCLK/(2^1) */
    IO_BITMASK_SET(RTC->TAMPER2, MASK_ALL, ((0 << TAMPPUDIS_POS) | (3 << TAMPPRCH_POS) | (0 << TAMPFLT_POS) |
                                            (1 << TAMPFRE_POS) | (1 << TAMPTS) | (TAMPER_TRIGGER_LEVEL << TAMPTRG) | (1 << TAMPE)));
    RTC_Lock();
    //RTCIrq_Tamper_Disable();
    return 0;
}

/* RTC trigger adc use tamper pes source in default */
void RTC_TamperPESEnable(int tamper_index)
{
    RTCTamper_Enable();
    RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, EVSEL_EN);
    while (!(IO_BIT_GET(RTC->SPECIAL, EVSEL_WEF)));
    if (tamper_index == 1)
    {
        IO_BITMASK_SET(RTC->CTRL, EV_SEL_MASK, EV_SEL_TAMPER1);
    }
    else
    {
        IO_BITMASK_SET(RTC->CTRL, EV_SEL_MASK, EV_SEL_TAMPER2);
    }
    IO_BIT_SET(RTC->CTRL, EVSEL_EN);

    RTC_Lock();
}


/* WRDY
 * 1:  means the RTCTM0/ RTCTM1/ RTCSUBSEC/RTCINTEN/RTCSPECIAL register
 *   have been ready to be wrote
 * 0:  means the RTCTM0/ RTCTM1/ RTCSUBSEC/ RTCINTEN/ RTCSPECIAL[3:0] register
 *   have been not ready to be wrote yet, and you can't write to these register
 */
uint32_t RTC_ReadyToWrite(void)
{
    return IO_BIT_GET(RTC->CTRL, WRDY);
}


/* When the RTCCLK is 32.768K,
 * you can read the current time register(RTCTM0/ RTCTM1) safely
 * if the SAFE flag is '1'
 */
uint32_t RTC_CanReadSafely(void)
{
    return IO_BIT_GET(RTC->SPECIAL, SAFE);
}

/* the RTCTM0 will be captured into the RTCTIMSTAMP0 register and
 * the { RTCSUBSEC[15:0], RTCTM1[15:0], not include year} will be captured into the RTCTIMSTAMP1 register
 * Note1 : When read this bit ,the value '0' will be returned
 * Note2: You can use this function to get the current time safely
 */
void RTC_DumpTimeSafely(void)
{
    int flag = (IO_BIT_GET(RTC->CTRL, LOCK) != 0);
    RTC_Unlock();
    RTC_WaitReady();
    IO_BIT_SET(RTC->SPECIAL, SOFT_TST);
    if (flag)
    {
        RTC_Lock();
    }
    RTC_WaitReady();
}

void RTC_WaitReady(void)
{
    uint32_t val = RTC_ReadyToWrite();
    while ((val & WRDY) == 0)
    {
        val = RTC_ReadyToWrite();
        ;
    }
}

void RTC_WaitClockReady(void)

{
    RTC_Unlock();
    IO_BIT_SET(RTC->CTRL, HOLD);
    while (!IO_BIT_GET(RTC->SPECIAL, TICK_WEF));
    IO_BIT_CLEAR(RTC->CTRL, HOLD);
    RTC_Lock();
}



/**
* @brief Set wakeup initial cnt,  WAKEUPCNT and WAKEUPINI
* @param
* @return
* @note The 16bit wakeup timer counter, when the WTR1-WAKEUPINI isn't 0 and
*   WTR0-WAKEUPCNT isn't 0, the counter will decrease 1 at the clock of RTC_CLK/2,
*   until it decrease to 0, then the wakeup interrupt will happen and
*   the WAKEUPINI will reload to the WAKEUPCNT, and repeat to decreasin.
*   Decrease frequency = RTC_CLK/2/(PRESCALE+1)/(2^(13-TDIVSEL)
*   RTCCTRL[10:8]
*   0 -> 2^13
*   1 -> 2^12
*   2 -> 2^11
*   ...
*   7 -> 2^6
*/
static void RTC_StartWakeup(uint32_t wakeup_cnt)
{
    RTC_Unlock();
    IO_BITMASK_SET(RTC->WAKEUPT0, 0xFFFF, wakeup_cnt);
    IO_BITMASK_SET(RTC->WAKEUPT1, 0xFFFF, wakeup_cnt);
    RTC_Lock();
}

static void RTC_StopWakeup(void)
{
    RTC_Unlock();
    IO_BITMASK_SET(RTC->WAKEUPT0, 0xFFFF, 0);
    IO_BITMASK_SET(RTC->WAKEUPT1, 0xFFFF, 0);
    RTC_Lock();
}

/* Disable the specific wakeup irq */
void RTC_DisableWakeupIrq(uint8_t wakeupIndex)
{

    /*Interrupt enable*/
    RTC_Unlock();
    RTC_WaitReady();
    /* IRQ_WAKEUP_WP4F_EN|IRQ_WAKEUP_WP3F_EN|IRQ_WAKEUP_WP2F_EN|IRQ_WAKEUP_WP1F_EN|IRQ_WAKEUP_WP0F_EN */
    IO_BIT_CLEAR(RTC->INTEN, 1 << (6 + wakeupIndex));
    RTC_Lock();

    NVIC_DisableIRQ(sRtcDriverT.calendar_irq);
}

/* Enable the specific wakeup irq */
void RTC_EnableWakeupIrq(uint8_t wakeupIndex)
{

    NVIC_ClearPendingIRQ(sRtcDriverT.calendar_irq);

    /*Interrupt enable*/
    RTC_Unlock();
    RTC_WaitReady();
    /* IRQ_WAKEUP_WP4F_EN|IRQ_WAKEUP_WP3F_EN|IRQ_WAKEUP_WP2F_EN|IRQ_WAKEUP_WP1F_EN|IRQ_WAKEUP_WP0F_EN */
    IO_BITMASK_SET(RTC->INTEN, MASK_ALL, 1 << (6 + wakeupIndex));
    RTC_Lock();

    NVIC_EnableIRQ(sRtcDriverT.calendar_irq);
}

/* Disable second irq */
void RTC_DisableSecondIrq()
{
    RTC_Unlock();
    RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN, IRQ_SECOND_EN);
    RTC_Lock();

    NVIC_DisableIRQ(sRtcDriverT.calendar_irq);
}

/* Enable second irq */
void RTC_EnableSecondIrq()
{

    NVIC_ClearPendingIRQ(sRtcDriverT.calendar_irq);

    /*Interrupt enable*/
    RTC_Unlock();
    RTC_WaitReady();
    IO_BITMASK_SET(RTC->INTEN, MASK_ALL, IRQ_SECOND_EN);
    RTC_Lock();

    NVIC_EnableIRQ(sRtcDriverT.calendar_irq);
}

/**
* @brief Set alarm with the format day(wd), hour:minute:second, pm, format12
* @param pm: 1-pm, 0-am or 24-hours format
*        format12: 1 - 12-hours format, 0 - 24-hours format
*        wd:  0 - the DAY is the day of month, 1 - the DAY[3:0] is the week day
* @return
* @note
*/
static void RTC_ConfigAlarm(uint8_t wd, uint32_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t pm, uint8_t format12)
{
    if (sRtcDriverT.bin_mode)
    {
        /* 24-hour format */
        if (!format12)
        {
            pm = 0;
        }
        IO_BITMASK_SET(RTC->ALARM, MASK_ALL, ALARM_BIN_TIME(wd, day, hour, minute, second, pm));
    }
    else
    {
        IO_BITMASK_SET(RTC->ALARM, MASK_ALL, ALARM_BCD_TIME(BIN2BCD(wd), BIN2BCD(day), BIN2BCD(hour), BIN2BCD(minute),
                       BIN2BCD(second), pm));
    }
    IO_BITMASK_SET(RTC->CTRL,  BIN_FORMAT12_MASK, format12 << BIN_FORMAT12_POS);
}

/* Enable alarm irq */
void RTC_EnableAlarmIrq(void)
{

    NVIC_ClearPendingIRQ(sRtcDriverT.calendar_irq);

    /*Interrupt enable*/
    RTC_Unlock();
    RTC_WaitReady();
    IO_BIT_SET(RTC->INTEN, IRQ_ALARM_IE);
    RTC_Lock();

    NVIC_EnableIRQ(sRtcDriverT.calendar_irq);
}

/* Disable alarm irq */
void RTC_DisableAlarmIrq(void)
{
    /*Interrupt enable*/
    RTC_Unlock();
    RTC_WaitReady();
    IO_BIT_CLEAR(RTC->INTEN, IRQ_ALARM_IE);
    RTC_Lock();

    NVIC_DisableIRQ(sRtcDriverT.calendar_irq);
}

/* Enbale the alarm */
static void RTC_EnableAlarm(void)
{
    RTC_Unlock();
    IO_BIT_SET(RTC->CTRL, ALARM_EN);
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, ((1 << ALARM_DAY_AE_POS) | (1 << ALARM_HOUR_AE_POS) | (1 << ALARM_MIN_AE_POS) | (1 << ALARM_SEC_AE_POS)));
    RTC_Lock();

    RTC_EnableAlarmIrq();
}

/* Disable the alarm */
static void RTC_DisableAlarm(void)
{
    RTC_DisableAlarmIrq();
    RTC_Unlock();
    IO_BIT_CLEAR(RTC->CTRL, ALARM_EN);
    IO_BITMASK_SET(RTC->ALRMASK, 0xFF, 0);
    RTC_Lock();
}

/**
* @brief Hold the timer.
* @param hold 1 - pause the tick
*             0 - resume the tick
* @return negative value: failed
* @note
*/
static void RTC_HoldTime(uint8_t hold)
{
    RTC_Unlock();
    if (hold)
    {
        IO_BIT_SET(RTC->CTRL, TIMER_HOLD_MASK);
    }
    else
    {
        IO_BIT_CLEAR(RTC->CTRL, TIMER_HOLD_MASK);
    }
    RTC_Lock();
}

/**
* @brief Set global storage mode
* @param 1: bin mode; 0: bcd mode
* @return
* @note
*/
static void RTC_SetBinMode(bool bin_mode)
{

    RTC_Unlock();
    if (bin_mode == True)
    {
        IO_BIT_CLEAR(RTC->CTRL, BIN_BCD_MODE);
    }
    else
    {
        IO_BIT_SET(RTC->CTRL, BIN_BCD_MODE);
    }
    RTC_Lock();

    sRtcDriverT.bin_mode = bin_mode;
}

/**
* @brief Set RTC time in bin mode or bcd mode
* @param pm: 1 - pm,  0 - am or 24 hours
*        format12: 0 - 24-hours format, 1 - 12-hours format
* @return
* @note
*/
static void Time_Set(uint8_t hour, uint8_t minute, uint8_t second, uint8_t pm, uint8_t format12)
{
    RTC_WaitReady();
    if (sRtcDriverT.bin_mode)
    {
        /* 24-hour format */
        if (!format12)
        {
            pm = 0;
        }
        IO_BITMASK_SET(RTC->TM0, 0xFFFFFFFF, BIN_TIME(hour, minute, second, pm));
    }
    else
    {
        IO_BITMASK_SET(RTC->TM0, 0xFFFFFFFF, BCD_TIME(BIN2BCD(hour), BIN2BCD(minute),
                       BIN2BCD(second), pm));
    }
    IO_BITMASK_SET(RTC->CTRL,   BIN_FORMAT12_MASK,  format12 << BIN_FORMAT12_POS);
    RTC_WaitReady();
    IO_BITMASK_SET(RTC->SUBSEC, BIN_SUBSECOND_MASK, (TICKS_PER_1S_CALENADR - 1) << BIN_SUBSECOND_POS);
    RTC_WaitReady();
}


/**
* @brief Set RTC date in bin mode or bcd mode
* @param
* @return
* @note
*/
static void Date_Set(uint32_t year, uint32_t month, uint32_t day, uint32_t week)
{
    RTC_WaitReady();
    if (sRtcDriverT.bin_mode)
    {
        IO_BITMASK_SET(RTC->TM1, MASK_ALL, BIN_YEAR(year, month, day, week));
    }
    else
    {
        uint32_t bcd_year_high = 0;
        uint32_t bcd_year_low  = 0;
        uint32_t bcd_month = 0;
        uint32_t bcd_day = 0;
        uint32_t bcd_week = 0;

        bcd_year_high = BIN2BCD(year / 100);
        bcd_year_low  = BIN2BCD(year % 100);
        bcd_month = BIN2BCD(month);
        bcd_day = BIN2BCD(day);
        bcd_week = BIN2BCD(week);

        IO_BITMASK_SET(RTC->TM1, MASK_ALL, BCD_DATE(BCD_YEAR(bcd_year_high, bcd_year_low), BCD_MONTH(bcd_month),
                       BCD_MONTH(bcd_day), BCD_WEEK(bcd_week)));
    }
}

/**
* @brief Get RTC date in bin mode or bcd mode
* @param date_time_type: DATE_TIME_TIMESTAMP/DATE_TIME_RTCTIME
* @return
* @note
*/
void Date_All_Get(UTCTime_T *pTime, DATE_TIME_TYPE date_time_type)
{
    uint32_t date_value = 0;
    if (date_time_type == DATE_TIME_TIMESTAMP)
    {
        date_value = IO_BITS_GET(RTC->TIMSTAMP1, MASK_ALL);
    }
    else
    {
        date_value = IO_BITS_GET(RTC->TM1, MASK_ALL);
    }
    if (sRtcDriverT.bin_mode)
    {
        pTime->year     = IO_BITS_GET(date_value, BIN_YEAR_MASK) >> BIN_YEAR_POS;
        pTime->mon      = IO_BITS_GET(date_value, BIN_MONTH_MASK) >> BIN_MONTH_POS;
        pTime->day      = IO_BITS_GET(date_value, BIN_DAY_MASK) >> BIN_DAY_POS;
        pTime->wday     = IO_BITS_GET(date_value, BIN_WEEK_MASK) >> BIN_WEEK_POS;
    }
    else
    {
        pTime->year     = BCD2BIN(BCD_YEAR_HIGH_GET_VALUE(date_value)) * 100 +
                          BCD2BIN(BCD_YEAR_LOW_GET_VALUE(date_value));
        pTime->mon      = BCD2BIN(BCD_MONTH_GET_VALUE(date_value));
        pTime->day      = BCD2BIN(BCD_DAY_GET_VALUE(date_value));
        pTime->wday     = BCD2BIN(BCD_WEEK_GET_VALUE(date_value));
    }
    if (date_time_type == DATE_TIME_TIMESTAMP)      /* timestamp has no year bits */
    {
        pTime->year = 0;
    }
}


/**
* @brief Get RTC time in bin mode or bcd mode
* @param date_time_type: DATE_TIME_TIMESTAMP/DATE_TIME_RTCTIME
* @return
* @note
*/
void Time_All_Get(UTCTime_T *pTime, DATE_TIME_TYPE date_time_type)
{
    uint32_t time_value = 0;

    if (date_time_type == DATE_TIME_TIMESTAMP)
    {
        time_value = IO_BITS_GET(RTC->TIMSTAMP0, MASK_ALL);
    }
    else
    {
        time_value = IO_BITS_GET(RTC->TM0, MASK_ALL);
    }
    if (sRtcDriverT.bin_mode)
    {
        pTime->hour     = IO_BITS_GET(time_value, BIN_HOUR_MASK) >> BIN_HOUR_POS;
        pTime->min      = IO_BITS_GET(time_value, BIN_MINUTE_MASK) >> BIN_MINUTE_POS;
        pTime->sec      = IO_BITS_GET(time_value, BIN_SECOND_MASK) >> BIN_SECOND_POS;
    }
    else
    {
        pTime->hour     = BCD2BIN(BCD_HOUR_GET_VALUE(time_value));
        pTime->min      = BCD2BIN(BCD_MINUTE_GET_VALUE(time_value));
        pTime->sec      = BCD2BIN(BCD_SECOND_GET_VALUE(time_value));
    }
}

/**
* @brief Get the timestamp about some concerned event
* @param
* @return
* @note
*/
void TimeStamp_Get(UTCTime_T *pTime)
{
    Date_All_Get(pTime, DATE_TIME_TIMESTAMP);
    Time_All_Get(pTime, DATE_TIME_TIMESTAMP);
}

/**
* @brief Get RTC year in bin mode or bcd mode
* @param
* @return the year from 1 to 4000
* @note
*/
uint32_t Year_Get(void)
{
    if (sRtcDriverT.bin_mode)
    {
        return IO_BITS_GET(RTC->TM1, BIN_YEAR_MASK) >> BIN_YEAR_POS;
    }
    else
    {
        return (BCD2BIN(BCD_YEAR_HIGH(RTC->TM1)) * 100 + BCD2BIN(BCD_YEAR_LOW(RTC->TM1)));
    }
}


/* Get RTC time */
void Time_Get(UTCTime_T *pTime)
{
    if ((RTC_CanReadSafely() & SAFE) != 0)
    {
        Date_All_Get(pTime, DATE_TIME_RTCTIME);
        Time_All_Get(pTime, DATE_TIME_RTCTIME);
    }
    else
    {
        RTC_DumpTimeSafely();
        TimeStamp_Get(pTime);
        pTime->year = Year_Get();
    }
}

/* Get the real frequency of Wakeup[x]  */
static void RTC_InitWakeupFreq(void)
{
    int i = 0;

    for (i = 0; i < 4; i++)
    {
        sRtcDriverT.wakeup_cycle[i] = 1000 * 2 * (PRESCALE_CALENDAR + 1) * (1 << (13 - i - sRtcDriverT.tdivsel)) / sRtcDriverT.rtc_clk_freq;
#if (DEBUG_MODE)
        PR_DEBUG("wakeup[%d] %dms\n", i, sRtcDriverT.wakeup_cycle[i]);
#endif
    }
    sRtcDriverT.wakeup_cycle[i] = 1000 * 2 * ((MAX_WAKEUP_CNT) + 1) / sRtcDriverT.rtc_clk_freq;
#if (DEBUG_MODE)
    PR_DEBUG("wakeup[%d] %dms\n", i, sRtcDriverT.wakeup_cycle[i]);
#endif
}

/**
* @brief Set tickval and prescale
* @param max value of prescale is 0x7F
* @param max value of tickvalue is 0xFFFF
* @return
* @note 1s can be generated by
*       RTCCLK / 2 / (PRE_SCALE+1) / (TICKVAL + 1),
*/
static void TickValue_Set(uint32_t prescale, uint32_t tickvalue)
{
    IO_BITMASK_SET(RTC->TICKVAL, PRESCALE_MASK,  prescale << PRESCALE_POS);
    IO_BITMASK_SET(RTC->TICKVAL, TICKVALUE_MASK, tickvalue << TICKVAULE_POS);
}

#if defined (CHIP_GM6621)
#if !defined (RTC_DISABLE_INNER_ISR)
STATIC_ISR(BaseTimer1_IRQn, BASETIMER_ISR);
static void BASETIMER_ISR()
{
    if (IO_BIT_GET(BASETIMER_CR, BASETIMER_INT))
    {
        IO_BIT_SET(BASETIMER_CR, BASETIMER_INT);
        if (sRtcDriverT.counter_handler)
        {
            sRtcDriverT.counter_handler(0);
        }
    }
}
#endif
/**
* @brief Enable BaseTimer irq
* @param
* @return
* @note
*/
static void BaseTimerIrq_Enable(void)
{
    NVIC_ClearPendingIRQ(BaseTimer1_IRQn);
    IO_BIT_SET(BASETIMER_CR, BASETIMER_IE);
    NVIC_EnableIRQ(BaseTimer1_IRQn);
}

static void BaseTimerIrq_Disable(void)
{
    IO_BIT_CLEAR(BASETIMER_CR, BASETIMER_IE);
    NVIC_DisableIRQ(BaseTimer1_IRQn);
}

static void BaseTimer_Enable(void)
{
    BaseTimerIrq_Enable();
    IO_BIT_SET(BASETIMER_CR, BASETIMER_EN);
}

static void BaseTimer_Disable(void)
{
    IO_BIT_CLEAR(BASETIMER_CR, BASETIMER_EN);
    BaseTimerIrq_Disable();
}

static void BaseTimer_SetTickValue(uint32_t value)
{
    IO_BITMASK_SET(BASETIMER_LR,  MASK_ALL, value);
    IO_BITMASK_SET(BASETIMER_CNT, MASK_ALL, value);
}

static uint32_t BaseTimer_GetTickValue(void)
{
    return IO_BITS_GET(BASETIMER_CNT, MASK_ALL);
}

static void BaseTimer_SetPrescaler(uint32_t value)
{

}
#endif //defined(CHIP_GM6621)
/* End of deined(CHIP_GM6621) || defined (CHIP_GM6611B) */
#endif

#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611) || defined (CHIP_GM6611B)||defined(CHIP_F6601C)



/* Start to tick timer 0 */
void Timer0_Enable(void)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BIT_SET(&RTC->TM0_CTRL, TM_EN);
    #else
    IO_BIT_SET(RTC->TM0_CTRL, TM_EN);
    #endif
    Timer0_EnableInterrupt();
}

/* Stop ticking timer 0 */
void Timer0_Disable(void)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BIT_CLEAR(&RTC->TM0_CTRL, TM_EN);
    #else
    IO_BIT_CLEAR(RTC->TM0_CTRL, TM_EN);
    #endif
    Timer0_DisableInterrupt();
}

/* Enable timer 0 interrupt */
void Timer0_EnableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);
    #if defined(CHIP_F6601C)
    CHECK_IO_BIT_SET(&RTC->TM0_CTRL, TM_INT_CLEAR);
    CHECK_IO_BIT_SET(&RTC->TM0_CTRL, TM_IE);
    #else
    IO_BIT_SET(RTC->TM0_CTRL, TM_INT_CLEAR);
    IO_BIT_SET(RTC->TM0_CTRL, TM_IE);
    #endif

    NVIC_EnableIRQ(RTC_IRQn);
}

/* Disable timer 0 interrupt */
void Timer0_DisableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);
    #if defined(CHIP_F6601C)
    CHECK_IO_BIT_SET(&RTC->TM0_CTRL, TM_INT_CLEAR);
    CHECK_IO_BIT_CLEAR(&RTC->TM0_CTRL, TM_IE);
    #else
    IO_BIT_SET(RTC->TM0_CTRL, TM_INT_CLEAR);
    IO_BIT_CLEAR(RTC->TM0_CTRL, TM_IE);
    #endif

    NVIC_DisableIRQ(RTC_IRQn);
}

/*
    platform GM6611B
        1 Hz = RTC0CLK/ (TM0_PRESCALE+1)/(TM0_TICK+1) on GM6611B
             = 32768 / (31 + 1) / (1023 + 1)
    else
        Set (1ms) tickvalue = RTC_CLK / 1000 - 1

    prescale max 2^16 = 65535   (for GM6611B)
    tickvalue max 2^10 = 1024
*/
void Timer0_SetTickValue(int value)
{
#if defined (CHIP_GM6611B)
    IO_BITMASK_SET(RTC->TM0_CTRL, TIMER0_PRESCALE_MASK, PRESCALE << TIMER0_PRESCALE_POS);
    IO_BITMASK_SET(RTC->TM0_TICK, TIMER0_TICKVALUE_MASK, value);
#else  
	#if defined (CHIP_F6601C)
	CHECK_IO_BITMASK_SET(&RTC->TM0_CTRL, TIMER0_MS_TICK_MASK, value << TIMER0_MS_TICK_POS);
    CHECK_IO_BITMASK_SET(&RTC->TM0_TICK, TIMER0_TICKVALUE_MASK, 1000-1);
	#else
	IO_BITMASK_SET(RTC->TM0_CTRL, TIMER0_TICKVALUE_MASK, value << TIMER0_TICKVALUE_POS);
    #endif
#endif
}

/* Set aim second. Reaching the aim second will cause the interrupt */
void Timer0_SetAimSecond(uint32_t value)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BITMASK_SET(&RTC->TM0_SA, SECOND_MASK, value);
    #else
    IO_BITMASK_SET(RTC->TM0_SA, SECOND_MASK, value);
    #endif
}

/* Set aim millisecond. Reaching the aim millisecond will cause the interrupt */
void Timer0_SetAimMilliSecond(uint32_t value)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BITMASK_SET(&RTC->TM0_MSA, MILLISECOND_MASK, value);
    #else
    IO_BITMASK_SET(RTC->TM0_MSA, MILLISECOND_MASK, value);
    #endif
}


/* Get the current second */
uint32_t Timer0_GetSecond(void)
{   
    #if defined (CHIP_F6601C)
    return CHECK_IO_BITS_GET(&(RTC->TM0_SC), SECOND_MASK);
    #else
    return IO_BITS_GET(RTC->TM0_SC, SECOND_MASK);
    #endif
}

/* Get the current millisecond */
uint32_t Timer0_GetMilliSecond(void)
{   
    #if defined(CHIP_F6601C)
    return CHECK_IO_BITS_GET(&RTC->TM0_MSC, MILLISECOND_MASK);
    #else
    return IO_BITS_GET(RTC->TM0_MSC, MILLISECOND_MASK);
    #endif
}

/**
* @brief set the initial second value of Timer0
*       1. it can be written only when TM0_EN = 0
*       2. TM0_SI/TM0_MSI -> TM0_SA/TM0_MSA, ==> TM0_INT=1
*       2^32s = 49710days
* @param
* @return
* @note
*/
void Timer0_InitSecond(uint32_t value)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BITMASK_SET(&RTC->TM0_SI, SECOND_MASK, value);
    #else
    IO_BITMASK_SET(RTC->TM0_SI, SECOND_MASK, value);
    #endif
}

/**
* @brief set the initial millisecond value of Timer0
*       1. it can be written only when TM0_EN = 0
*       2. TM0_SI/TM0_MSI -> TM0_SA/TM0_MSA, ==> TM0_INT=1
*       3. maximum value : 0x3E7    ==> 999ms
* @param
* @return
* @note
*/
void Timer0_InitMilliSecond(uint32_t value)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BITMASK_SET(&RTC->TM0_MS, MILLISECOND_MASK, value);
    #else
    IO_BITMASK_SET(RTC->TM0_MS, MILLISECOND_MASK, value);
    #endif
}

/**
* @brief Set current counter value.
* @param
* @return
* @note  Counts downwards at the frequency RTC_CLK
*/
void Timer1_SetCntValue(uint32_t value)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BITMASK_SET(&RTC->TM1_CNT, MASK_ALL, value);
    #else
    IO_BITMASK_SET(RTC->TM1_CNT, MASK_ALL, value);
    #endif
}

/**
* @brief Set load value.
* @param
* @return
* @note Counts downwards from this loaded value
*/
void Timer1_SetCmpValue(uint32_t value)
{
    #if defined(CHIP_F6601C)
    CHECK_IO_BITMASK_SET(&RTC->TM1_CMP, MASK_ALL, value);
    #else
    IO_BITMASK_SET(RTC->TM1_CMP, MASK_ALL, value);
    #endif
}

/* The counter increase 1 on one tick. */
void Timer1_SetTickValue(uint32_t value)
{
    Timer1_SetCntValue(0);
    Timer1_SetCmpValue(value);
}

uint32_t Timer1_GetTickValue(void)
{
    #if defined(CHIP_F6601C)
    return CHECK_IO_BITS_GET(&RTC->TM1_CNT, MASK_ALL);
    #else
    return IO_BITS_GET(RTC->TM1_CNT, MASK_ALL);
    #endif
}

void Timer1_SetPrescaler(uint32_t value)
{   
    #if defined(CHIP_F6601C)
    CHECK_IO_BITMASK_SET(&RTC->TM1_CTRL, COUNTER_DIV_MASK, value << COUNTER_DIV_POS);
    #else
    IO_BITMASK_SET(RTC->TM1_CTRL, COUNTER_DIV_MASK, value << COUNTER_DIV_POS);
    #endif
}

#if defined (CHIP_GM6611B)
void Timer1B_SetCntValue(uint32_t value)
{
    IO_BITMASK_SET(RTC->TM1B_CNT, MASK_ALL, value);
}

void Timer1B_SetCmpValue(uint32_t value)
{
    IO_BITMASK_SET(RTC->TM1B_CMP, MASK_ALL, value);
}

void Timer1B_SetTickValue(uint32_t value)
{
    Timer1B_SetCntValue(0);
    Timer1B_SetCmpValue(value);
}

uint32_t Timer1B_GetTickValue(void)
{
    return IO_BITS_GET(RTC->TM1B_CNT, MASK_ALL);
}

/* Enable timer1B interrupt. */
void Timer1B_EnableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);

    IO_BIT_SET(RTC->TM1B_CTRL, TM_INT_CLEAR);
    IO_BIT_SET(RTC->TM1B_CTRL, TM_IE);

    NVIC_EnableIRQ(RTC_IRQn);
}

/* Disable timer1B interrupt. */
void Timer1B_DisableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);

    IO_BIT_SET(RTC->TM1B_CTRL, TM_INT_CLEAR);
    IO_BIT_CLEAR(RTC->TM1B_CTRL, TM_IE);

    NVIC_DisableIRQ(RTC_IRQn);
}

void Timer1B_Enable(void)
{
    Timer1B_EnableInterrupt();
    IO_BIT_SET(RTC->TM1B_CTRL, TM_EN);
}

void Timer1B_Disable(void)
{
    Timer1B_DisableInterrupt();
    IO_BIT_CLEAR(RTC->TM1B_CTRL, TM_EN);
}

void Timer1B_SetPrescaler(uint32_t value)
{
    IO_BITMASK_SET(RTC->TM1B_CTRL, COUNTER_DIV_MASK, value << COUNTER_DIV_POS);
}

void Timer1C_SetCntValue(uint32_t value)
{
    IO_BITMASK_SET(RTC->TM1C_CNT, MASK_ALL, value);
}

void Timer1C_SetCmpValue(uint32_t value)
{
    IO_BITMASK_SET(RTC->TM1C_CMP, MASK_ALL, value);
}

void Timer1C_SetTickValue(uint32_t value)
{
    Timer1C_SetCntValue(0);
    Timer1C_SetCmpValue(value);
}

uint32_t Timer1C_GetTickValue(void)
{
    return IO_BITS_GET(RTC->TM1C_CNT, MASK_ALL);
}

/* Enable timer1C interrupt. */
void Timer1C_EnableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);

    IO_BIT_SET(RTC->TM1C_CTRL, TM_INT_CLEAR);
    IO_BIT_SET(RTC->TM1C_CTRL, TM_IE);

    NVIC_EnableIRQ(RTC_IRQn);
}

/* Disable timer1C interrupt. */
void Timer1C_DisableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);

    IO_BIT_SET(RTC->TM1C_CTRL, TM_INT_CLEAR);
    IO_BIT_CLEAR(RTC->TM1C_CTRL, TM_IE);

    NVIC_DisableIRQ(RTC_IRQn);
}

void Timer1C_Enable(void)
{
    Timer1C_EnableInterrupt();
    IO_BIT_SET(RTC->TM1C_CTRL, TM_EN);
}

void Timer1C_Disable(void)
{
    Timer1C_DisableInterrupt();
    IO_BIT_CLEAR(RTC->TM1C_CTRL, TM_EN);
}

void Timer1C_SetPrescaler(uint32_t value)
{
    IO_BITMASK_SET(RTC->TM1C_CTRL, COUNTER_DIV_MASK, value << COUNTER_DIV_POS);
}

#else //#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)
/* Set total second. */
static void Timer_SetSecond(uint32_t value)
{
    Timer0_Disable();
    Timer0_InitSecond(value);
    Timer0_InitMilliSecond(0);
    Timer0_Enable();
}
#endif

/**
* @brief The counter downwards from value `PRESCALER*RTC_CLK` to 0 at frequency RTC_CLK,
*   There is a INTERRUPT signal when the counter matched CMP_VALUE.
*       The default prescaler is 1.
*
* @param prescaler - the second factor
* @return
* @note Max number is 2^32/RTC_CLK = 2^32 / 32768 = 131072s
*/
//static void Timer1_SetPrescaler(int prescaler)
//{
//    sRtcDriverT.prescaler= prescaler;
//}

/* Timer1 starts to tick */
void Timer1_Enable(void)
{
    Timer1_EnableInterrupt();
    #if defined CHIP_F6601C
    CHECK_IO_BIT_SET(&RTC->TM1_CTRL, TM_EN);
    #else
    IO_BIT_SET(RTC->TM1_CTRL, TM_EN);
    #endif

}

/* Timer1 stops ticking */
void Timer1_Disable(void)
{
    Timer1_DisableInterrupt();
    #if defined CHIP_F6601C
    CHECK_IO_BIT_CLEAR(&RTC->TM1_CTRL, TM_EN);
    #else
    IO_BIT_CLEAR(RTC->TM1_CTRL, TM_EN);
    #endif
}

/* Enable timer1 interrupt. */
void Timer1_EnableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);
    #if defined CHIP_F6601C
    CHECK_IO_BIT_SET(&RTC->TM1_CTRL, TM_INT_CLEAR);
    CHECK_IO_BIT_SET(&RTC->TM1_CTRL, TM_IE);
    #else
    IO_BIT_SET(RTC->TM1_CTRL, TM_INT_CLEAR);
    IO_BIT_SET(RTC->TM1_CTRL, TM_IE);
    #endif
    NVIC_EnableIRQ(RTC_IRQn);
}

/* Disable timer1 interrupt. */
void Timer1_DisableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);
    #if defined CHIP_F6601C
    CHECK_IO_BIT_SET(&RTC->TM1_CTRL, TM_INT_CLEAR);
    CHECK_IO_BIT_CLEAR(&RTC->TM1_CTRL, TM_IE);
    #else
    IO_BIT_SET(RTC->TM1_CTRL, TM_INT_CLEAR);
    IO_BIT_CLEAR(RTC->TM1_CTRL, TM_IE);
    #endif

    NVIC_DisableIRQ(RTC_IRQn);
}

/**
* @brief Set current counter value.
* @param
* @return
* @note  Counts downwards at the frequency RTC_CLK
*/
void Timer2_SetCntValue(uint32_t value)
{   
    #if defined CHIP_F6601C
    CHECK_IO_BITMASK_SET(&RTC->TM2_CNT, MASK_ALL, value);
    #else
    IO_BITMASK_SET(RTC->TM2_CNT, MASK_ALL, value);
    #endif
}

/**
* @brief Set load value.
* @param
* @return
* @note Counts downwards from this loaded value
*/
void Timer2_SetLoadValue(uint32_t value)
{   
    #if defined CHIP_F6601C
    CHECK_IO_BITMASK_SET(&RTC->TM2_LOAD, MASK_ALL, value);
    #else
    IO_BITMASK_SET(RTC->TM2_LOAD, MASK_ALL, value);
    #endif
}

/* The counter decreases 1 on one tick. */
void Timer2_SetTickValue(uint32_t value)
{
    Timer2_SetCntValue(value);
    Timer2_SetLoadValue(value);
}

uint32_t Timer2_GetTickValue(void)
{   
    #if defined CHIP_F6601C
    return CHECK_IO_BITS_GET(&RTC->TM2_CNT, MASK_ALL);
    #else
    return IO_BITS_GET(RTC->TM2_CNT, MASK_ALL);
    #endif
}

void Timer2_SetPrescaler(uint32_t value)
{
    //do nothing.
}


/* Timer2 starts to tick. */
void Timer2_Enable(void)
{
    Timer2_EnableInterrupt();
    #if defined CHIP_F6601C
    CHECK_IO_BIT_SET(&RTC->TM2_CTRL, TM_EN);
    #else
    IO_BIT_SET(RTC->TM2_CTRL, TM_EN);
    #endif
}

/* Timer2 stops ticking. */
void Timer2_Disable(void)
{
    Timer2_DisableInterrupt();
    #if defined CHIP_F6601C
    CHECK_IO_BIT_CLEAR(&RTC->TM2_CTRL, TM_EN);
    #else
    IO_BIT_CLEAR(RTC->TM2_CTRL, TM_EN);
    #endif
}

/* Enable timer2 interrupt. */
void Timer2_EnableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);
    #if defined CHIP_F6601C
    CHECK_IO_BIT_SET(&RTC->TM2_CTRL, TM_INT_CLEAR);
    CHECK_IO_BIT_SET(&RTC->TM2_CTRL, TM_IE);
    #else
    IO_BIT_SET(RTC->TM2_CTRL, TM_INT_CLEAR);
    IO_BIT_SET(RTC->TM2_CTRL, TM_IE);
    #endif

    NVIC_EnableIRQ(RTC_IRQn);
}

/* Disable timer2 interrupt. */
void Timer2_DisableInterrupt(void)
{
    NVIC_ClearPendingIRQ(RTC_IRQn);
    #if defined CHIP_F6601C
    CHECK_IO_BIT_SET(&RTC->TM2_CTRL, TM_INT_CLEAR);
    CHECK_IO_BIT_CLEAR(&RTC->TM2_CTRL, TM_IE);
    #else
    IO_BIT_SET(RTC->TM2_CTRL, TM_INT_CLEAR);
    IO_BIT_CLEAR(RTC->TM2_CTRL, TM_IE);
    #endif

    NVIC_DisableIRQ(RTC_IRQn);
}

/**
* @brief The counter downwards from value `PRESCALER*RTC_CLK` to 0 at frequency RTC_CLK,
*   There is a INTERRUPT signal when the counter reaches 0.
*       The default prescaler is 1.
*
* @param prescaler - the second factor
* @return
* @note Max number is 2^32/RTC_CLK = 2^32 / 32768 = 131072s
*/
void RTC_SetPrescaler(int prescaler)
{
    sRtcDriverT.prescaler = prescaler;
    Timer2_SetTickValue(prescaler * sRtcDriverT.rtc_clk_freq);
}

/* Stop ticking. */
void Timer_DisableAll(void)
{   
    #if defined CHIP_F6601C
    CHECK_IO_BIT_CLEAR(&RTC->TM0_CTRL, TM_EN);
    CHECK_IO_BIT_CLEAR(&RTC->TM1_CTRL, TM_EN);
    CHECK_IO_BIT_CLEAR(&RTC->TM2_CTRL, TM_EN);
    #else
    IO_BIT_CLEAR(RTC->TM0_CTRL, TM_EN);
    IO_BIT_CLEAR(RTC->TM1_CTRL, TM_EN);
    IO_BIT_CLEAR(RTC->TM2_CTRL, TM_EN);
    #endif
}

/* Call the timer/alarm hanlder when TotalTickSecond is equal to AimSecond. */
static void RTC_AimCall(void)
{
    uint32_t currentMillisecond = 0;
    uint32_t currentSecond = 0;
    RTC_Timer_T *timer = &sRtcDriverT.rtc_timer;
    currentMillisecond = timer->get_current_msec();
    currentSecond      = timer->get_current_sec();
    timer->set_aim_sec((currentMillisecond + timer->cycle_timer.cycle.tv_msec) / 1000 + currentSecond + timer->cycle_timer.cycle.tv_sec);
    timer->set_aim_msec((currentMillisecond + timer->cycle_timer.cycle.tv_msec) % 1000);
    if (sRtcDriverT.rtc_timer.alarm_on)
    {
        sRtcDriverT.rtc_timer.alarm_on = False;
        if (sRtcDriverT.alarm_handler)
        {
            sRtcDriverT.alarm_handler(RTC_ALARM);
        }
    }
    else
    {
        if (sRtcDriverT.timer_handler)
        {
            sRtcDriverT.timer_handler(RTC_TIMER);
        }
    }
}

#endif /* End of defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611) || defined (CHIP_GM6611B) */

uint32_t RTC_GetFrequence(int index)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return 0;
    }
    return sRtcDriverT.rtc_counter[index].freq;
}

int RTC_SetCounter(int index, uint32_t count)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }
    sRtcDriverT.rtc_counter[index].set_tick_value(count);
    return 0;
}

int RTC_StartCounter(int index)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }
    sRtcDriverT.rtc_counter[index].enable();
    return 0;
}

int RTC_StopCounter(int index)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }
    sRtcDriverT.rtc_counter[index].disable();
    return 0;
}

uint32_t RTC_GetCurrentCount(int index)
{
    return sRtcDriverT.rtc_counter[index].get_tick_value();
}

int RTC_SetCounterPrescaler(int index, uint32_t value)
{
    if (index < 0 || index >= COUNTER_CNT)
    {
        return -1;
    }
    sRtcDriverT.rtc_counter[index].set_prescaler(value);
    return 0;
}

/**
* @brief Start iterative callback every `cycle` seconds.
* @param
* @return status: RTC_STATUS_OK - 0
* @note
* the valid two-types argument description on GM6621 as follows,
* the max msec = MAX_WAKEUP_CNT*1000*2*(PRESCALE+1)/RTCCLK
* (MAX_WAKEUP_CNT=65535, PRESCALE=121, RTCCLK=8000000)
* 1. pTimer->cycle.tv_sec = 1;
* 2. pTimer->cycle.tv_msec = xxx;
*/
int RTC_StartTimer(const Timer_T *pTimer)
{
    RTC_Timer_T *timer = &sRtcDriverT.rtc_timer;

#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)||defined(CHIP_F6601C)
    uint32_t sec_value = 0;
    uint32_t millisec_value = 9;
    uint32_t currentMillisecond = 0;
    uint32_t currentSecond = 0;

    timer->cycle_timer.cycle  = pTimer->cycle;
    timer->alarm_on = False;

    currentSecond      = timer->get_current_sec();
    currentMillisecond = timer->get_current_msec();

    sec_value = (currentMillisecond + timer->cycle_timer.cycle.tv_msec) / 1000 +
                currentSecond + timer->cycle_timer.cycle.tv_sec;
    millisec_value = (currentMillisecond + timer->cycle_timer.cycle.tv_msec) % 1000;

    timer->set_aim_sec(sec_value);
    timer->set_aim_msec(millisec_value);
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    uint32_t cnt = 0;
    timer->cycle_timer.cycle  = pTimer->cycle;

    sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_msec = sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_sec * 1000 + sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_msec;

    cnt = (TICKS_PER_1S_CALENADR) * sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_msec / 1000;
    if (cnt > 0 && cnt <= MAX_WAKEUP_CNT)
    {
        sRtcDriverT.current_wakeup_index = 4;
        RTC_StartWakeup(cnt);
        RTC_EnableWakeupIrq(sRtcDriverT.current_wakeup_index);
    }
#endif
    return RTC_STATUS_OK;
}

/* Clear timer's time to zero. */
void RTC_StopTimer(void)
{
    RTC_Timer_T *timer = &sRtcDriverT.rtc_timer;
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_StopWakeup();
    RTC_DisableWakeupIrq(sRtcDriverT.current_wakeup_index);
    sRtcDriverT.current_wakeup_index = -1;
#endif
    timer->cycle_timer.cycle.tv_sec = 0;
    timer->cycle_timer.cycle.tv_msec = 0;
}

/* Set the alarm to a future time. */
void RTC_SetAlarm(UTCTime_T *pTime)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)
    Time_T t = 0;
    sRtcDriverT.rtc_timer.alarm_on = True;
    sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_sec  = 0;
    sRtcDriverT.rtc_timer.cycle_timer.cycle.tv_msec = 0;

    pTime->wday  = UtcTime_CalcWeekday(pTime->year, pTime->mon, pTime->day);
    pTime->year -= 1900;
    pTime->mon  -= 1;

    ConvertToWday0(pTime->wday);

    t = RTC_ConvertToTickSecond(pTime);

    sRtcDriverT.rtc_timer.set_aim_sec(t);
    sRtcDriverT.rtc_timer.set_aim_msec(0);
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    sRtcDriverT.rtc_timer.alarm_on = True;
    RTC_EnableAlarm();
    RTC_Unlock();
    /* alarm on the day */
    if (pTime->wday == 0)
    {
        RTC_ConfigAlarm(0, pTime->day, pTime->hour, pTime->min, pTime->sec, 0, 0);
    }
    /* alarm on the weekday */
    else
    {
        RTC_ConfigAlarm(1, pTime->day, pTime->hour, pTime->min, pTime->sec, 0, 0);
    }
    RTC_Lock();

#endif
}

/* Cancel the alarm. */
void RTC_CancelAlarm(void)
{
    sRtcDriverT.rtc_timer.alarm_on = False;
}

#if 0
/* Register a timer/alarm callback. */
void RTC_SetCallback(uint32_t callType, Timer_Callback callback)
{
    if (callType < MAX_TIMERTYPE_CNT)
    {
        sRtcDriverT.cycle_timerHandler[callType] = callback;
    }
}
#endif

#if !defined (RTC_DISABLE_INNER_ISR)

STATIC_ISR(RTC_IRQn, RTC_ISR);

#if defined (CHIP_GM6611B)
    STATIC_ISR(Calendar_IRQn, RTC_ISR);
#endif
/**
* @brief to check the expected time of th timer/alarm.
*       if the seconds are matched, call the registered handler
*       to notify the observer.
* @param
* @return
* @note
*/
static void RTC_ISR(void)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B)||defined(CHIP_F6601C) || defined (CHIP_GM6611) || defined (CHIP_GM6611B)
    #if defined(CHIP_F6601C)
    if (CHECK_IO_BIT_GET(&RTC->TM0_CTRL, TM_INT_CLEAR))
    {
        CHECK_IO_BIT_SET(&RTC->TM0_CTRL, TM_INT_CLEAR);
    #else
    if (IO_BIT_GET(RTC->TM0_CTRL, TM_INT_CLEAR))
    {
        IO_BIT_SET(RTC->TM0_CTRL, TM_INT_CLEAR);
    #endif
        RTC_AimCall();
    }
    
    #if defined(CHIP_F6601C)
    if (CHECK_IO_BIT_GET(&RTC->TM1_CTRL, TM_INT_CLEAR))
    {
        CHECK_IO_BIT_SET(&RTC->TM1_CTRL, TM_INT_CLEAR);
    #else
    if (IO_BIT_GET(RTC->TM1_CTRL, TM_INT_CLEAR))
    {
        IO_BIT_SET(RTC->TM1_CTRL, TM_INT_CLEAR);
    #endif
        if (sRtcDriverT.counter_handler)
        {
            sRtcDriverT.counter_handler(0);
        }
    }
    #if defined(CHIP_F6601C)
    if (CHECK_IO_BIT_GET(&RTC->TM2_CTRL, TM_INT_CLEAR))
    {
        CHECK_IO_BIT_SET(&RTC->TM2_CTRL, TM_INT_CLEAR);
    #else
    if (IO_BIT_GET(RTC->TM2_CTRL, TM_INT_CLEAR))
    {
        IO_BIT_SET(RTC->TM2_CTRL, TM_INT_CLEAR);
    #endif        
        if (sRtcDriverT.counter_handler)
        {
            sRtcDriverT.counter_handler(1);
        }
    }
#if defined (CHIP_GM6611B)
    if (IO_BIT_GET(RTC->TM1B_CTRL, TM_INT_CLEAR))
    {
        IO_BIT_SET(RTC->TM1B_CTRL, TM_INT_CLEAR);
        if (sRtcDriverT.counter_handler)
        {
            sRtcDriverT.counter_handler(2);
        }
    }

    if (IO_BIT_GET(RTC->TM1C_CTRL, TM_INT_CLEAR))
    {
        IO_BIT_SET(RTC->TM1C_CTRL, TM_INT_CLEAR);
        if (sRtcDriverT.counter_handler)
        {
            sRtcDriverT.counter_handler(3);
        }
    }
#endif //defined (CHIP_GM6611B)
#endif  //defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611) || defined (CHIP_GM6611B)
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    if (!sRtcDriverT.rtc_timer.alarm_on)
    {
        RTC_Unlock();
        if ((sRtcDriverT.current_wakeup_index == 4) &&
                IO_BIT_GET(RTC->FLAG, IRQ_WAKEUP_WP4F_EN))
        {
            IO_BIT_SET(RTC->INTCLR, IRQ_WAKEUP_WP4F_EN);
            if (sRtcDriverT.timer_handler)
            {
                sRtcDriverT.timer_handler(RTC_TIMER);
            }
        }
    }
    else
    {
        if (IO_BIT_GET(RTC->FLAG, IRQ_ALARM_IE))
        {
            IO_BIT_SET(RTC->INTCLR, IRQ_ALARM_IE);
            RTC_DisableAlarm();
            if (sRtcDriverT.alarm_handler)
            {
                sRtcDriverT.alarm_handler(RTC_ALARM);
            }
        }
        RTC_Lock();
    }
#endif
}

#endif

#if 0
/* Get info if timer interrupt occurs. */
static uint32_t RTC_InterruptStatus_Get(void)
{
    return IO_BITS_GET(RTC->TM_INTST, INTST_MASK);
}
#endif

/**
* @brief Return the current time as a `Time_T` and also put it in *t
* if t is not NULL. Time is represented as seconds from Jan 1 00:00:00 1970.
* @param t - seconds from Jan 1 00:00:00 1970.
* @return Time_T - (32 bits) seconds from Jan 1 00:00:00 1970.
* @note
*/
Time_T RTC_GetTickSecond(Time_T *t)
{
    Time_T res = 0;

#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)||defined(CHIP_F6601C)
    res = sRtcDriverT.rtc_timer.get_current_sec();
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    UTCTime_T time;

    Time_Get(&time);
    time.year -= 1990;
    time.mon -= 1;
    ConvertToWday0(time.wday);
    res = RTC_ConvertToTickSecond(&time);
#endif

    if (t != NULL)
    {
        *t = res;
    }
    return res;
}

/**
* @brief Set Time_T to RTC counter.
* @param when - seconds from Jan 1 00:00:00 1970.
* @return
* @note
*/
void RTC_SetTickSecond(const Time_T *pTime)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) ||defined(CHIP_F6601C)|| defined (CHIP_GM6611)
    Timer_SetSecond(*pTime);
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    UTCTime_T *pUtcTime = 0;
    pUtcTime = RTC_ConvertToUtcTime(pTime);
    pUtcTime->year += 1990;
    pUtcTime->mon += 1;
    RTC_SetDateTime(pUtcTime->year, pUtcTime->mon, pUtcTime->day,
                    pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
#endif
}

/**
* @brief Convert the seconds from Jan 1 00:00:00 1970 to UTC time.
* @param t - seconds from Jan 1 00:00:00 1970.
* @return the `UTCTime_T' representation of *t in UTC.
* @note
*/
UTCTime_T *RTC_ConvertToUtcTime(const Time_T *t)
{
    return UtcTime_Convert(t, &sRtcDriverT.utc_time);
}

/**
* @brief Convert UTC time to the seconds from Jan 1 00:00:00 1970.
* @param tp - UTC time
* @return the seconds from Jan 1 00:00:00 1970.
* @note
*/
Time_T RTC_ConvertToTickSecond(UTCTime_T *tp)
{
    return UtcTime_InternalMake(tp);
}

/**
* @brief Set time
* @param hour:min:sec
* @return
* @note this operation will pause the RTC tick momently.
*/
void RTC_SetTime(uint32_t hour, uint32_t min, uint32_t sec)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6611)
    UTCTime_T *utcTime;

    Time_T t;
    Time_T newTime;

    RTC_GetTickSecond(&t);

    utcTime = RTC_ConvertToUtcTime(&t);

    utcTime->hour = hour;
    utcTime->min  = min;
    utcTime->sec  = sec;

    newTime = RTC_ConvertToTickSecond(utcTime);

    RTC_SetTickSecond(&newTime);
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_Unlock();
    Time_Set(hour, min, sec, 0, 0);
    RTC_Lock();
#endif

}

/**
* @brief Set date
* @param year/mon/day
* @return
* @note this operation will pause the RTC tick momently.
*/
void RTC_SetDate(uint32_t year, uint32_t mon, uint32_t day)
{
#if defined (CHIP_GM6601)|| defined (CHIP_GM6611)

    UTCTime_T utcTime;

    Time_T newTime;

    utcTime.wday = UtcTime_CalcWeekday(year, mon, day);
    utcTime.year = year - 1900;
    utcTime.mon  = mon  - 1;
    utcTime.day  = day;
    ConvertToWday0(utcTime.wday);

    newTime = RTC_ConvertToTickSecond(&utcTime);

    RTC_SetTickSecond(&newTime);
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_Unlock();
    Date_Set(year, mon, day, UtcTime_CalcWeekday(year, mon, day));
    RTC_Lock();
#endif
}

/**
* @brief Set date and time
* @param year/mon/day hour:min:sec
* @return
* @note this operation will pause the RTC tick momently.
*/
void RTC_SetDateTime(uint32_t year, uint32_t mon, uint32_t day,
                     uint32_t hour, uint32_t min, uint32_t sec)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6611)
    UTCTime_T utcTime;

    Time_T newTime;

    utcTime.wday = UtcTime_CalcWeekday(year, mon, day);
    utcTime.year = year - 1900;
    utcTime.mon  = mon  - 1;
    utcTime.day  = day;
    utcTime.hour = hour;
    utcTime.min  = min;
    utcTime.sec  = sec;

    ConvertToWday0(utcTime.wday);

    newTime = RTC_ConvertToTickSecond(&utcTime);
    RTC_SetTickSecond(&newTime);
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_Unlock();
    Date_Set(year, mon, day, UtcTime_CalcWeekday(year, mon, day));
    Time_Set(hour, min, sec, 0, 0);
    RTC_Lock();
#endif
}

/**
* @brief Get date and time
* @param none
* @return UtcTime pointer
* @note
*/
UTCTime_T *RTC_GetTime(void)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B)|| defined (CHIP_F6601C) || defined (CHIP_GM6611)
    Time_T currentSecond = 0;
    currentSecond = RTC_GetTickSecond(NULL);
    return RTC_ConvertToUtcTime(&currentSecond);
#elif defined (CHIP_GM6621)|| defined (CHIP_GM6611B)
    Time_Get(&sRtcDriverT.utc_time);
    return &sRtcDriverT.utc_time;
#endif
}


static void RTC_SetDriver_Clk(RTC_Clock_T clock)
{
    sRtcDriverT.rtc_clk_freq = rtc_clk_freq[clock];
}


static void RTC_SetDefaultPrescale(void)
{
#if defined (CHIP_GM6611B)

    sRtcDriverT.prescaler = DEFAULT_PRESCALE_32K;
#endif
#if defined (CHIP_GM6621)
    if (sRtcDriverT.rtc_clk_freq == RTC_32K_CLK)
    {
        sRtcDriverT.prescaler = DEFAULT_PRESCALE_32K_8M;
    }
    else if (sRtcDriverT.rtc_clk_freq == RTC_8M_CLK)
    {
        sRtcDriverT.prescaler = DEFAULT_PRESCALE_32K_8M;
    }
#endif
}

static void RTC_Check_Config(RTC_Clock_T clock)
{
#if defined (CHIP_GM6621)
    unsigned int correct_freq = 0;
    if (RTC_CLOCK_FCLK == clock)
    {
        correct_freq = 8000000;
    }
    else if (RTC_CLOCK_OSCLS == clock || RTC_CLOCK_IRCLS == clock)
    {
        correct_freq = 32768;
    }
    else if (RTC_CLOCK_OSCHS == clock || RTC_CLOCK_IRCHS == clock)
    {
        correct_freq = 8000000;
    }

    if (correct_freq != RTC_CLK)
    {
#if DEBUG_MODE
        PR_DEBUG("Current RTC_CLK is %dHz, why not %dHz(RTC_Clock_T==%d)?\n", RTC_CLK, correct_freq, clock);
#endif
    }
#endif
}

void RTC_HwReset(void)
{
#if defined (CHIP_GM6621)
    AOPDEV->RSTC    &= ~(0x1 << 16);
    AOPDEV->RSTC    |= (0x1 << 16);
#endif
}

/**
* @brief Set Rtc clock being derived from oscillator
            OSC_LS(32.768K) (RTC0_CLOCK_OSCLS)
            or
            OSC_HS(8M) (RTC0_CLOCK_PLLSRC + OSC_EXTERNAL_CLOCK)
*           and enable interrupt.
* @param
* @return
* @note
*/
void RTC_Init(RTC_Config_T *pConfig)
{
    PeriphClk_Config_T config;

    config.periphClkMask = CLOCK_PERIPH_RTC0_MASK;
    config.rtc0Clock     = pConfig->clock;


#if !defined (CHIP_GM6621)
    config.periphClkMask |= CLOCK_PERIPH_RTC1_MASK;
    config.rtc1Clock     = pConfig->clock;

    config.periphClkMask |= CLOCK_PERIPH_RTC2_MASK;
    config.rtc2Clock     = pConfig->clock;
#endif

    RTC_Check_Config(pConfig->clock);
    RTC_HwReset();
#if defined (CHIP_GM6611B)
    /*
        0: FCLK
        1: OSC_LS_CLK(32.768KHz)
        2: IRC_LS_CLK
        3: IRC_HS_CLK
        4: OSC_HS_CLK
     */
    config.periphClkMask |= CLOCK_PERIPH_CALENDAR_MASK;
    config.calendarClock = pConfig->clock;
#endif

    /*get rtc clk freq*/
    RTC_SetDriver_Clk(pConfig->clock);

    /*set prescale default value*/
    RTC_SetDefaultPrescale();

    /* Init sRtcDriverT */
    RTC_GlobalInit(pConfig);
    PeriphClk_SetConfig(&config);
#if !defined (CHIP_GM6621)
	#if defined(CHIP_F6601C)
    Timer0_SetTickValue(sRtcDriverT.rtc_clk_freq/1000-1);//ÉèÖÃtick
	#else
    Timer0_SetTickValue(TICKS_PER_1S);
	#endif
#endif
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_SetBinMode(True);
    RTC_Unlock();
    IO_BITMASK_SET(RTC->SUBSEC, BIN_SUBSECOND_MASK, 0);
    /* 1sec = 31249*subsecond (8000K / 2 / (PRESCALE+1) / (TICKS_PER_1S+1)) */
    TickValue_Set(sRtcDriverT.prescaler_calendar, sRtcDriverT.ticks_per_1s);
    RTC_Lock();

    RTC_WaitClockReady();

    /* no need to unlock RTCCLR */
    /* clear all interrupt pending flag */
    IO_BITMASK_SET(RTC->INTCLR, MASK_ALL, IRQ_CLEAR_ALL_PENDING_FLAG_MASK);
    RTC_InitWakeupFreq();
#endif
    sRtcDriverT.alarm_handler = pConfig->alarmCallback;
    sRtcDriverT.timer_handler = pConfig->timerCallback;
    sRtcDriverT.counter_handler = pConfig->counterCallback;

#if (DEBUG_MODE)
    RTC_PrintArgs();
#endif
}



/* Start Rtc counter. */
void RTC_Start(void)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)||defined(CHIP_F6601C)
    Timer0_Enable();
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_HoldTime(0);
#endif
}

/* Stop Rtc counter.*/
void RTC_Stop(void)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)||defined(CHIP_F6601C)
    Timer0_Disable();
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_HoldTime(1);
#endif

}

/**
* @brief Reset the time to Jan 1 00:00:00 1970.
* @param
* @return
* @note
*/
void RTC_Reset(void)
{
#if defined (CHIP_GM6601) || defined (CHIP_GM6601B) || defined (CHIP_GM6611)||defined(CHIP_F6601C)
    Timer_SetSecond(0);
#elif defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    RTC_Unlock();
    Date_Set(1970, 1, 1, 4);
    Time_Set(0, 0, 0, 0, 0);
    RTC_Lock();
#endif
}

#if 0
static void RTC_TicksCalc(void)
{
    uint32_t freq = sRtcDriverT.rtc_clk_freq;
#if defined (CHIP_GM6611B)
    /* tm0 tick max: 2^10 - 1 = 1023(0x3FF)
     * tm0 prescaler max: 2^32 - 1 = 0xFFFFFFFF
     */
    /* tm1/tm2 tick max: 2^32 - 1 = 0xFFFFFFFF
     */
    /* calendar tick max: 2^16 - 1 = 65535(0xFFFF)
     * calendar prescaler max: 2^7 - 1 = 127(0x3F)
     */
    if (freq == RTC_CLK_FREQ_32K)
    {
        sRtcDriverT.prescaler             = 63;
        sRtcDriverT.prescaler_calendar    = 63;
    }
    else if (freq == RTC_CLK_FREQ_48M)
    {
        sRtcDriverT.prescaler             = 46874;
        sRtcDriverT.prescaler_calendar    = 124;
    }
    else if (freq == RTC_CLK_FREQ_8M)
    {
        sRtcDriverT.prescaler             = 7811;
        sRtcDriverT.prescaler_calendar    = 124;
    }
    sRtcDriverT.ticks_per_1s          = freq / (sRtcDriverT.prescaler + 1) - 1;
    sRtcDriverT.ticks_per_1s_calendar = freq / 2 / (sRtcDriverT.prescaler + 1) - 1;
#else
    sRtcDriverT.prescaler             = 121;
    sRtcDriverT.prescaler_calendar    = 121;
    sRtcDriverT.ticks_per_1s          = freq / (sRtcDriverT.prescaler + 1) - 1;
    sRtcDriverT.ticks_per_1s_calendar = sRtcDriverT.ticks_per_1s;
#endif
}
#endif

static void RTC_GlobalInit(RTC_Config_T *pConfig)
{
    RTC_Counter_T *counter;

#if 0
    RTC_TicksCalc();
#endif

#if !defined (CHIP_GM6621)
    RTC_Timer_T *timer;
    sRtcDriverT.rtc_clk_freq = RTC_CLK;
    timer = &sRtcDriverT.rtc_timer;
    timer->enable           = Timer0_Enable;
    timer->disable          = Timer0_Disable;
    timer->alarm_on         = False;
    timer->set_aim_sec      = Timer0_SetAimSecond;
    timer->set_aim_msec     = Timer0_SetAimMilliSecond;
    timer->get_current_sec  = Timer0_GetSecond;
    timer->get_current_msec = Timer0_GetMilliSecond;

    memset(&timer->cycle_timer, 0, sizeof(Timer_T));
    counter = &sRtcDriverT.rtc_counter[0];
    counter->enable         = Timer1_Enable;
    counter->disable        = Timer1_Disable;
    counter->set_tick_value = Timer1_SetTickValue;
    counter->get_tick_value = Timer1_GetTickValue;
	#if defined CHIP_F6601C
	counter->freq           = sRtcDriverT.rtc_clk_freq / precaler_value[pConfig->counter_precaler] ; 
	#else
    counter->freq           = sRtcDriverT.rtc_clk_freq / 16; /* default */
	#endif
    counter->set_prescaler  = Timer1_SetPrescaler;

    counter = &sRtcDriverT.rtc_counter[1];
    counter->enable         = Timer2_Enable;
    counter->disable        = Timer2_Disable;
    counter->set_tick_value = Timer2_SetTickValue;
    counter->get_tick_value = Timer2_GetTickValue;
    counter->freq           = sRtcDriverT.rtc_clk_freq; /* default */
    counter->set_prescaler  = Timer2_SetPrescaler;
#if defined (CHIP_GM6611B)
    sRtcDriverT.ticks_per_1s = (sRtcDriverT.rtc_clk_freq / 2 / ((sRtcDriverT.prescaler + 1))) - 1;
    if (sRtcDriverT.rtc_clk_freq == 32768)
    {
        sRtcDriverT.prescaler_calendar = 63;
        sRtcDriverT.ticks_per_1s_calendar = (sRtcDriverT.rtc_clk_freq / 2 / (sRtcDriverT.prescaler_calendar + 1) - 1);
    }
    else if (sRtcDriverT.rtc_clk_freq == 8000000)
    {
        sRtcDriverT.prescaler_calendar = 124;
        sRtcDriverT.ticks_per_1s_calendar = ((sRtcDriverT.rtc_clk_freq / 3) / 2 / (sRtcDriverT.prescaler_calendar + 1) - 1);
    }
    else
    {
        sRtcDriverT.prescaler_calendar = 124;
        sRtcDriverT.ticks_per_1s_calendar = (sRtcDriverT.rtc_clk_freq / 2 / (sRtcDriverT.prescaler_calendar + 1) - 1);
    }


    counter = &sRtcDriverT.rtc_counter[2];
    counter->enable         = Timer1B_Enable;
    counter->disable        = Timer1B_Disable;
    counter->set_tick_value = Timer1B_SetTickValue;
    counter->get_tick_value = Timer1B_GetTickValue;
    counter->freq           = sRtcDriverT.rtc_clk_freq / 16; /* default */
    counter->set_prescaler  = Timer1B_SetPrescaler;

    counter = &sRtcDriverT.rtc_counter[3];
    counter->enable         = Timer1C_Enable;
    counter->disable        = Timer1C_Disable;
    counter->set_tick_value = Timer1C_SetTickValue;
    counter->get_tick_value = Timer1C_GetTickValue;
    counter->freq           = sRtcDriverT.rtc_clk_freq / 16; /* default */
    counter->set_prescaler  = Timer1C_SetPrescaler;
#endif //defined (CHIP_GM6611B)
#else //CHIP_GM6621
    sRtcDriverT.rtc_clk_freq = RTC_CLK;
    sRtcDriverT.ticks_per_1s = (sRtcDriverT.rtc_clk_freq / (2 * (sRtcDriverT.prescaler + 1))) - 1;
    sRtcDriverT.prescaler_calendar = sRtcDriverT.prescaler;
    sRtcDriverT.ticks_per_1s_calendar = sRtcDriverT.ticks_per_1s;

    counter = &sRtcDriverT.rtc_counter[0];
    counter->enable         = BaseTimer_Enable;
    counter->disable        = BaseTimer_Disable;
    counter->set_tick_value = BaseTimer_SetTickValue;
    counter->get_tick_value = BaseTimer_GetTickValue;
    counter->freq           = SYS_CLOCK;
    counter->set_prescaler  = BaseTimer_SetPrescaler;
#endif
    memset(&sRtcDriverT.utc_time, 0, sizeof(UTCTime_T));
#if defined (CHIP_GM6621) || defined (CHIP_GM6611B)
    sRtcDriverT.bin_mode             = True;
    sRtcDriverT.current_wakeup_index = -1;
    sRtcDriverT.tdivsel              = 0;
    sRtcDriverT.locked               = True;
    memset(&sRtcDriverT.wakeup_cycle, 0, sizeof(uint32_t) * 5);
#if defined(CHIP_GM6621)
    sRtcDriverT.calendar_irq = RTC_IRQn;
#else
    sRtcDriverT.calendar_irq = Calendar_IRQn;
#endif
    sRtcDriverT.alarm_handler        = NULL;
    sRtcDriverT.timer_handler        = NULL;
    sRtcDriverT.counter_handler      = NULL;
#endif
}

static int RTC_DayOfMon(int year, int mon)
{
    int maxday = 30;

    if (mon == 2)
    {
        if (((year % 4 == 0) && (year % 100) != 0) || (year % 400 == 0))
        {
            maxday = 29;
        }
        else
        {
            maxday = 28;
        }
    }
    else if (1 == mon || 3 == mon || 5 == mon || 7 == mon || 8 == mon || 10 == mon || 12 == mon)
    {
        maxday = 31;
    }
    else if (2 == mon || 4 == mon || 6 == mon || 9 == mon || 11 == mon)
    {
        maxday = 30;
    }
    return maxday;
}


int RTC_NextSec(UTCTime_T *pUtcTime, int sec)
{
    int maxday, added_hour, added_min, added_sec;
    if (sec > 24 * 3600)
    {
        return -1;
    }
    added_hour = sec / 3600;
    added_min  = (sec - added_hour * 3600) / 60;
    added_sec  = sec - added_hour * 3600 - added_min * 60;

    pUtcTime->sec += added_sec;
    pUtcTime->min += added_min;
    pUtcTime->hour += added_hour;

    if (pUtcTime->sec >= 60)
    {
        pUtcTime->min += (pUtcTime->sec / 60);
        pUtcTime->sec = pUtcTime->sec % 60;
    }
    if (pUtcTime->min >= 60)
    {
        pUtcTime->hour += (pUtcTime->min / 60);
        pUtcTime->min = pUtcTime->min % 60;
    }
    if (pUtcTime->hour >= 24)
    {
        pUtcTime->day += (pUtcTime->hour / 24);
        pUtcTime->hour = pUtcTime->hour % 24;
        if (pUtcTime->wday != 0)
        {
            pUtcTime->wday += (pUtcTime->hour / 24);
            pUtcTime->wday = pUtcTime->wday % 7;
        }
    }
    maxday = RTC_DayOfMon(pUtcTime->year, pUtcTime->mon);
    if (pUtcTime->day > maxday)
    {
        pUtcTime->day = 1;
        pUtcTime->mon += 1;
    }
    if (pUtcTime->mon > 12)
    {
        pUtcTime->mon = 1;
        pUtcTime->year += 1;
    }
    if (pUtcTime->year >= 4000)
    {
        return -1;
    }
    return 0;
}

