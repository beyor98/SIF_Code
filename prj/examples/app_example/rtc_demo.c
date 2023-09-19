/*====================================================================
 *
 * Copyright (C) 2020 Gree Microelectronics.  All Rights Reserved.
 *
 * @file can_demo.c
 *
 * @version 1.0.0
 *
 * @date 2020/12/24
 *
 * @contents Verification and example code of Typical applications.
 *
 * @history
 *  Date                 By           Comments        Ver.
 *  =========       ============     =========    =========
 *  2020/12/24      software group    Create        1.0.0
 **********************************************************************/
#include "stdio.h"
#include <common.h>
#include <drv/dev/rtc.h>
#include <drv/dev/clock.h>

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#define DBG_PR_EN              1
#if DBG_PR_EN
    #define DBG_PR       printf
#else
    #define DBG_PR(...)  do{}while(0)
#endif

/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
volatile static int step = 0;

/******************************************************************************
* Local functions
******************************************************************************/
int main(void);
/******************************************************************************
* Global functions
******************************************************************************/
void Time_Print(void)
{
    UTCTime_T *pUtcTime;
    pUtcTime = RTC_GetTime();
    DBG_PR("%04d/%02d/%02d(%d) %02d:%02d:%02d\n",
           pUtcTime->year, pUtcTime->mon, pUtcTime->day,
           pUtcTime->wday, pUtcTime->hour, pUtcTime->min, pUtcTime->sec);
}

void Exit(void)
{
    RTC_StopTimer();
    RTC_Stop();
    DBG_PR("Bye\n");
}




/* will be called upon expiration of the timer or the alarm */
void Callback(int type)
{
    static int cnt = 0;
    if (type == RTC_ALARM)
    {
        Timer_T timer;
        uint32_t cycle = 1;

#if defined (CHIP_GM6621)
        cycle = 1;
#endif
        DBG_PR("now start the timer every %us\n", cycle);
        timer.cycle.tv_sec  = cycle;
        timer.cycle.tv_msec = 0;
        RTC_CancelAlarm();
        RTC_StartTimer(&timer);
    }
    else if (type == RTC_TIMER)
    {
        Time_Print();
        if (cnt++ == 40)
        {
            cnt = 0;
            step++;
            RTC_StopTimer();
            DBG_PR("now start next step\n");

        }
        else if (cnt == 10)
        {
            Timer_T timer;
            uint32_t cycle = 50;
            DBG_PR("now reset the timer every %ums\n", cycle);
            timer.cycle.tv_sec  = 0;
            timer.cycle.tv_msec = cycle;

            RTC_StartTimer(&timer);
        }
    }
}


void CounterCall(int id)
{
    static int counter_cnt = 0;
    uint32_t freq = RTC_GetFrequence(id);
    counter_cnt++;
    DBG_PR("id :%d\n", id);
#if defined (CHIP_GM6621)
    RTC_SetCounter(id, freq / 16);
    if (counter_cnt == 100)
    {
        DBG_PR("set prescaler to div32\n");
        RTC_SetCounter(id, freq / 32);
    }
    else if (counter_cnt == 200)
    {
        DBG_PR("stop counter\n");
        RTC_StopCounter(id);
        step++;
    }
#else
    RTC_SetCounter(id, freq);
    if (counter_cnt == 10)
    {
        DBG_PR("set prescaler to div32\n");
        RTC_SetCounterPrescaler(id, COUNTER_DIV32);
    }
    else if (counter_cnt == 15)
    {
        DBG_PR("stop counter\n");
        RTC_StopCounter(id);
        step++;
    }
#endif
    Time_Print();
}


int main(void)
{
    UTCTime_T utcTime;
    Timer_Callback callback = Callback;
    RTC_Config_T rtcConfig;
    int counter_id = 0;
    uint32_t freq = 0;
    uint32_t cnt_before_decrease = 0;
    uint32_t current_cnt = 0;
#if !defined (CHIP_GM6621)
    uint32_t cnt_before_increase = 0;
#endif
    float delta_time = 0;
#if defined (CHIP_GM6601B)
    OSC_Config_T s_OscConfig;
#endif
    int ret = -1;
#define YEAR    2017
#define MON     5
#define DAY     11
#define HOUR    23
#define MINUTE  59
#define SEC     55

    SysTick_Init();
    SysTick_DelayMS(500);

#if (SYS_CLOCK == 48000000)
    DebugConsoleInit(115200);
#else /* 8M */
    DebugConsoleInit(19200);
#endif

    DBG_PR("RTC Test Start\n");
    rtcConfig.alarmCallback = callback;
    rtcConfig.timerCallback = callback;
    rtcConfig.counterCallback = CounterCall;

#if defined (CHIP_GM6601B)
    rtcConfig.clock         = RTC_CLOCK_LOSC;
    s_OscConfig.oscMask  = OSC_MASK_EXTERNAL_LOW;
    s_OscConfig.oscState = OSC_EXTERNAL_LOW_ON;
    s_OscConfig.pllConfig.pllBypassEnable = 0;
    OSC_SetConfig(&s_OscConfig);
    SysTick_DelayMS(500);
#elif defined (CHIP_GM6621)
    rtcConfig.clock         = RTC_CLOCK_IRCLS;
#elif defined (CHIP_GM6611B)
    rtcConfig.clock         = RTC_CLOCK_OSCLS;
#endif

    RTC_Init(&rtcConfig);

    /* RTC works now */
    RTC_Start();

    RTC_SetDateTime(YEAR, MON, DAY, HOUR, MINUTE, SEC);

    DBG_PR("Alarm after 5 seconds\n");
    utcTime.year = YEAR;
    utcTime.mon  = MON;
    utcTime.day  = DAY;
    utcTime.hour = HOUR;
    utcTime.min  = MINUTE;
    utcTime.sec  = SEC; /* ensure sec < 60 */
    utcTime.wday = 0;

    RTC_NextSec(&utcTime, 5);
#if defined(CHIP_GM6601B)
    /*GM6601B not support alarm*/
    step++;
#else
    RTC_SetAlarm(&utcTime);
#endif
    while (step < 1);

    counter_id = 0;

    freq = RTC_GetFrequence(counter_id);
    DBG_PR("freq %dHz\n", freq);
    //RTC_SetCounterPrescaler is unuse for GM6621
    RTC_SetCounterPrescaler(counter_id, COUNTER_DIV16);
    RTC_SetCounter(counter_id, freq);
    DBG_PR("set counter %d\n", freq);

    RTC_StartCounter(counter_id);
    while (step < 2);

    DBG_PR("now use counter to measure time\n");
    /* counter1 is decreased */
    counter_id = 1;
#if defined (CHIP_GM6621)
    counter_id = 0;
#endif
    RTC_SetCounter(counter_id, 0xFFFFFFFF);
    cnt_before_decrease = RTC_GetCurrentCount(counter_id);
    RTC_StartCounter(counter_id);
    SysTick_DelayMS(275);
    current_cnt = RTC_GetCurrentCount(counter_id);
    delta_time = 1.0 * (cnt_before_decrease - current_cnt) / RTC_GetFrequence(counter_id);
    DBG_PR("interval time %.2fs\n", delta_time);
    RTC_StopCounter(counter_id);
    step++;
    while (step < 3);
#if !defined (CHIP_GM6621)
    /* counter0/counter2/counter3 is increased */
    counter_id = 2;
    ret = RTC_SetCounter(counter_id, 0xFFFFFFFF);
    if (ret < 0)
    {
        Exit();
        return ret;
    }
    cnt_before_increase = RTC_GetCurrentCount(counter_id);
    RTC_SetCounterPrescaler(counter_id, COUNTER_DIV16);
    RTC_StartCounter(counter_id);
    SysTick_DelayMS(512);
    current_cnt = RTC_GetCurrentCount(counter_id);
    delta_time = 1.0 * (current_cnt - cnt_before_increase) / RTC_GetFrequence(counter_id);
    DBG_PR("interval time %.2fs\n", delta_time);
    RTC_StopCounter(counter_id);
    step++;
    while (step < 4);
#endif
    Exit();

    return ret;
}
