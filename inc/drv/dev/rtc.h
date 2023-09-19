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
 * @brief         Header file of GM6601/GM6601B/GM6611/GM6621 RTC Driver
 *
 * @note
 *    2017/04/19, yangy, V1.0.0
 *        Initial version.
 *    2017/05/12, yangy, V1.1.0
 *        Compatible with GM6601/6611/6621
 */


#ifndef _RTC_H_
#define _RTC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>


typedef int Time_T;
////typedef unsigned int uint32_t;

#define COUNTER_DIV16           (0x00)
#define COUNTER_DIV32           (0x01)
#define COUNTER_DIV64           (0x2)
#define COUNTER_DIV256          (0x3)

/**
 * Coordinated Universal Time structure
 */
typedef struct
{
    int sec;        /**< Seconds.       [0-59]                  */
    int min;        /**< Minutes.       [0-59]                  */
    int hour;       /**< Hours.         [0-23]                  */
    int day;        /**< Day.           [1-31]                  */
    int mon;        /**< Month.         [1-12]                  */
    int year;       /**< Year.          [1902-2037]             */
    int wday;       /**< Day of week.   [1-7] , 1-Monday ...    */
    int yday;       /**< Days in year.  [0-365]                 */
} UTCTime_T;

/**
 * @defgroup Error_Macros
 * Specify macros of Error
 * @{
 */
static int sTimeErrno = 0;

#define EINVAL      22

#define EOVERFLOW   EINVAL

#define RTC_STATUS_OK      0
#define RTC_STATUS_FAILED -1
/**
 * @}
 */

/**
 * TimeValue structure for Timer_T.
 */
typedef struct
{
    Time_T tv_sec;      /**< Seconds.      */
    Time_T tv_msec;     /**< MilliSeconds. */
} TimeValue_T;

/**
 * Timer_T structure. used by the timer or alarm.
 */
typedef struct
{
    TimeValue_T cycle;  /**< Time to call the timer/alarm handler periodly */
} Timer_T;

/**
 * @defgroup Callback_Macros
 * Specify macros of Callback
 * @{
 */
typedef void (*Timer_Callback)(int);

#define RTC_ALARM 0
#define RTC_TIMER 1
/**
 * @}
 */

/**
 * RTC Clock Source
 */
#if defined(CHIP_GM6601)
typedef enum
{
    RTC_CLOCK_PLLSRC            = 0, /**< RTC clock is derived from PLL(8MHz) */
    RTC_CLOCK_OSCLS,                 /**< RTC clock is derived from oscillator LS(32768Hz) */
    RTC_CLOCK_IRCLS,                 /**< RTC clock is derived from IRC LS */
    RTC_CLOCK_PLL6X,                 /**< RTC clock is derived from PLL multipled by 6 */
} RTC_Clock_T;

#elif (defined (CHIP_GM6601B)||defined (CHIP_F6601C))
typedef enum
{
    RTC_CLOCK_PLLSRC            = 0, /**< RTC clock is derived from PLL(8MHz) */
    RTC_CLOCK_LOSC,                  /**< RTC clock is derived from oscillator LS(32768Hz) */
    RTC_CLOCK_LIRC,                  /**< RTC clock is derived from IRC LS */
    RTC_CLOCK_FCLK,                  /**< RTC clock is derived from FCLK */
} RTC_Clock_T;

#elif defined (CHIP_GM6611)
typedef enum
{
    RTC_CLOCK_OSCLS             = 0, /**< RTC clock is derived from PLL(8MHz) */
    RTC_CLOCK_FCLK,                  /**< RTC clock is derived from FCLK(48MHz) */
} RTC_Clock_T;
#elif defined (CHIP_GM6611B)
typedef enum
{
    RTC_CLOCK_FCLK              = 0, /**< RTC clock is derived from FCLK(48MHz)*/
    RTC_CLOCK_OSCLS,                 /**< RTC clock is derived from oscillator LS(32768Hz)*/
} RTC_Clock_T;
#elif defined (CHIP_GM6621)
typedef enum
{
    RTC_CLOCK_FCLK              = 0, /**< RTC clock is derived from FCLK(48MHz) */
    RTC_CLOCK_OSCLS,                 /**< RTC clock is derived from oscillator LS(32768Hz) */
    RTC_CLOCK_IRCLS,                 /**< RTC clock is derived from IRC LS */
    RTC_CLOCK_IRCHS,                 /**< RTC clock is derived from IRC HS  */
    RTC_CLOCK_OSCHS,                 /**< RTC clock is derived from oscillator HS(8Mhz) */
} RTC_Clock_T;
#endif


/**
 * RTC configuration structure
 *
 * This structure holds the configuration settings for RTC initialization.
 *
 */
typedef struct
{
    Timer_Callback  alarmCallback;
    Timer_Callback  timerCallback;
    Timer_Callback  counterCallback;
    RTC_Clock_T     clock;
    #if defined CHIP_F6601C
    uint8_t counter_precaler;
    #endif
} RTC_Config_T;

/**
 * @brief Initialize the RTC default clock OSCLS and enable timer0.
 *        Callbacks ought to be set to NULL even if not used.
 * @param  pConfig  The initial configuration of RTC.
 * @retval          None.
 */
void RTC_Init(RTC_Config_T *pConfig);



/**
 * @brief The RTC time elapses, with counter tick at the frequency that
 */
void RTC_Start(void);

/**
 * @brief Stop counter tick, that the time will be paused.
 */
void RTC_Stop(void);

/**
 * @brief Rest time to 1970/01/01 00:00:00.
 */
void RTC_Reset(void);

/**
 * @brief Get RTC seconds from 1970/01/01 00:00:00.
 *
 * @param  pTime    Tick seconds from Jan 1, 1970. 00:00:00.
 * @retval          Tick seconds from Jan 1, 1970. 00:00:00.
 */
Time_T RTC_GetTickSecond(Time_T *pTime);

/**
 * @brief Store RTC seconds from 1970/01/01 00:00:00 to RTC module.
 *
 * @param  pTime    Tick seconds from Jan 1, 1970. 00:00:00.
 * @retval          None.
 */
void RTC_SetTickSecond(const Time_T *pTime);

/**
 * @brief Convert Time_T seconds to UTC time.
 *
 * @param   pTime   the seconds which are needed to be converted.
 * @retval  Return the corresponding utc time structure.
 */
UTCTime_T *RTC_ConvertToUtcTime(const Time_T *pTime);

/**
 * @brief Convert UTC time to Time_T seconds.
 *
 * @param   pUtcTime    The utc time which is needed to be converted.
 * @retval              Return the corresponding seconds.
 */
Time_T RTC_ConvertToTickSecond(UTCTime_T *pUtcTime);

/**
 * @brief Start the iterative timer.
 *
 * GM6621 NOTICE Limitation
 * Only 1s or max_msec xxxms are allowed,
 * the max_msec = MAX_WAKEUP_CNT * 1000 * 2 * (PRESCALE+1) / RTCCLK
 * eg. MAX_WAKEUP_CNT=65535, PRESCALE=121, RTCCLK=8000000, max_msec=1998ms
 *
 * @param   pTimer      Indication the cycle of the timer.
 * @retval              status
 */
int RTC_StartTimer(const Timer_T *pTimer);

/**
 * @brief Stop the timer.
 */
void RTC_StopTimer(void);

/**
 * @brief Set the future time alarm.
 * NOTICE Both of the alarm and the timer can not be effective at the same time.
 *
 * @param   pUtcTime    A future utc time of the alarm.
 * @retval              None.
 */
void RTC_SetAlarm(UTCTime_T *pUtcTime);

/**
 * @brief Cancel the alarm.
 */
void RTC_CancelAlarm(void);

/**
 * @brief Set UTC time directly.
 *
 * @param   hour    The hour 0~23.
 * @param   min     The minute 0~59.
 * @param   sec     The second 0~60.
 * @retval          None.
 */
void RTC_SetTime(uint32_t hour, uint32_t min, uint32_t sec);

/**
 * @brief Set UTC date directly.
 *
 * @param   year    The year 1902~2037.
 * @param   mon     The month 1~12.
 * @param   day     The day 1~31.
 * @retval          None.
 */
void RTC_SetDate(uint32_t year, uint32_t mon, uint32_t day);

/**
 * @brief Set UTC date time directly.
 *
 * @param   year    The year 1902~2037.
 * @param   mon     The month 1~12.
 * @param   day     The day 1~31.
 * @param   hour    The hour 0~23.
 * @param   min     The minute 0~59.
 * @param   sec     The second 0~60.
 * @retval          None.
 */
void RTC_SetDateTime(uint32_t year, uint32_t mon, uint32_t day,
                     uint32_t hour, uint32_t min, uint32_t sec);

/**
 * @brief Get UTC time.
 * @retval  UTCTime_T*  Current time with UTC format.
 */
UTCTime_T *RTC_GetTime(void);

/**
 * @brief Get RTC Frequence
 *
 * @param   id          Counter ID.
 * @retval  RTC clock, unit:Hz. 0: failed.
 */
uint32_t RTC_GetFrequence(int id);

/**
 * @brief Set RTC Counter
 *
 * @param   id          Counter ID.
 * @param   count       Max loading value.
 * @retval              -1: failed.
 */
int RTC_SetCounter(int id, uint32_t count);

/**
 * @brief Start to count 1 per Hz.
 * has Counter0/Counter1
 *
 * GM6611B NOTICE
 * has Counter0/Counter1/Counter2/Counter3
 *
 * GM6621 NOTICE
 * has only Counter0 (decreasing counter)
 *
 * Counter1 is a decreasing counter, others are increasing counters.
 *
 * @param   id          Counter ID.
 * @retval              -1: failed.
 */
int RTC_StartCounter(int id);

/**
 * @brief Stop counting.
 *
 * @param   id          Counter ID.
 * @retval              -1: failed.
 */
int RTC_StopCounter(int id);

/**
 * @brief Get Current Counter Value
 *
 * @param   id          Counter ID.
 * @retval              Current Counter Value.
 */
uint32_t RTC_GetCurrentCount(int id);

/**
 * @brief Set Counter Prescaler
 *
 * Set the counting frequency that prescaled from RTC_CLK
 *
 * GM6611B NOTICE Limitation
 * No effect to set counter1.
 *
 * GM6621 NOTICE Limitation
 * No effect to set counter0,
 * but can set the count to FREQ/DIV_XX to monitor XX prescaler.
 *
 * @param   id          counter id.
 *                      value = 0, DIV_16 by default
 *                      value = 1, DIV_32
 *                      value = 2, DIV_64
 *                      value = 3, DIV_256
 * @retval  value       Current Counter Value.
 */
int RTC_SetCounterPrescaler(int id, uint32_t value);

/**
 * @brief Get the next time after x sec
 *
 * @param   UTCTime_T - basic utc time
 *          sec       - the interval time.
 * @retval  0 - OK, -1 - failed
 */
int RTC_NextSec(UTCTime_T *pUtcTime, int sec);
#if defined (CHIP_GM6621)
void RTC_TriggerACMPEnable(void);
#endif

#ifdef __cplusplus
}
#endif

#endif //_RTC_H_
