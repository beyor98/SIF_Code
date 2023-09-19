/***********************************************************************
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @filename systick.h
 *
 * @author wen
 *
 * @version 1.0.0
 *
 * @date 2015/11/19
 *
 * @description Systick utility routines.
 *
 * @history
 *  Date                   By            Comments                 Ver.
 *  =========       =========        =========           =========
 *  2015/11/19      Wen Liu          Create                  1.0.0
 **********************************************************************/
#include <common.h>
#include <drv/arch/systick.h>
STATIC_ISR(SysTick_IRQn, Systick_InterruptHandler);


#define SYSTICK_RELOAD_VALUE SYSTICK_MS_TICKS

/**
 * Warning!!!
 * Never change any code here becasuse all sdk drivers depend on s_sysTime. 
 * Please call SysTick_Init_Ex to change systick irq frequency, priority and starttime.
 */
static volatile uint64_t s_sysTime  = 0;
static uint32_t s_MSIntNum = 1;
static uint32_t s_CallbackIntNum = 1;
static uint32_t s_TimerIntCnt = 0;
static uint32_t s_CallbackIntCnt = 0;
static uint32_t reload_ticks = SYSTICK_RELOAD_VALUE;
/*****************************************************************************
*
* @brief Init standard cortex-m0 systick with max reload value with systick
*        interrupt disabled.
*
* @param     none
* @retval    none
*
******************************************************************************/

void SysTick_Init(void)
{
    /*Disable systick*/
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    s_sysTime  = 0;
    s_TimerIntCnt = 0;
    s_CallbackIntCnt = 0;
    s_MSIntNum = 1;
    s_CallbackIntNum = 1;
    reload_ticks = SYSTICK_RELOAD_VALUE;
    SysTick->VAL  = 0UL;       /* Clear current timer value */
    SysTick->LOAD = SYSTICK_RELOAD_VALUE - 1UL;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}

/*****************************************************************************
*
* @brief Init standard cortex-m0 systick with used defined reload value with systick
*        interrupt disabled.
*
* @param[in]  interval: interval between two periodic interrupts or two callbacks.
*                      
* @retval    none
*
******************************************************************************/

void SysTick_Init_Ex(SYSTICK_Interval_T interval, uint32_t priority, uint64_t start_systime)
{
    /*Disable systick*/
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    s_sysTime = start_systime;
    s_TimerIntCnt = 0;
    s_CallbackIntCnt = 0;
    if (interval >= SYSTICK_2MS)
    {
        s_MSIntNum = 1;
        s_CallbackIntNum = interval - SYSTICK_2MS + 2;
        reload_ticks = SYSTICK_RELOAD_VALUE;
        SysTick->LOAD = SYSTICK_RELOAD_VALUE - 1UL;
    }
    else
    {
        s_MSIntNum = interval;
        s_CallbackIntNum = 1;
        reload_ticks = SYSTICK_RELOAD_VALUE/interval;
        SysTick->LOAD = SYSTICK_RELOAD_VALUE/interval - 1UL;
    }
    SysTick->VAL  = 0UL;       /* Clear current timer value */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;

    NVIC_SetPriority(SysTick_IRQn, (priority&((uint32_t)(1<<__NVIC_PRIO_BITS) - 1)));
}
/*****************************************************************************
*
* @brief Stop standard cortex-m0 systick.
*
* @param     none
* @retval    none
*
******************************************************************************/
void SysTick_Disable(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*****************************************************************************
*
* @brief Get standard cortex-m0 systick.
*
* @param     none
* @retval    Current value of sys timer.
*
******************************************************************************/
uint32_t SysTick_Get(void)
{
    return SysTick->VAL;
}

uint32_t SysTick_Interval(uint32_t old_tick, uint32_t cur_tick)
{
    uint32_t distance = (cur_tick <= old_tick)?(old_tick - cur_tick):((reload_ticks - cur_tick) + old_tick);
    return distance;
}

/*****************************************************************************
*
* @brief Delay some ticks after an interval to the expected tick arrives, no matter
*        what is current tick. The expected tick after the interval is returned after
*        delay time is over, and returns immediately if current tick is overdue. The
*        function is used to assure us-accuracy and must be run with all interrupts
*        disabled.
*
* @param[in]  exp_tick: The tick value expected, the new tick value after the interval
*                       is returned before the funtion is returned.
* @param[in]  interval: The tick value after expected tick which should expires before
*                       the funtion is returned.
*
* @retval     none
*
******************************************************************************/
uint32_t SysTick_DelayAfterTick(uint32_t ExpTick, uint32_t Interval)
{
    uint32_t exp_tick = ExpTick;
    uint32_t cur_tick, distance;
    uint32_t min_distance;
    
    exp_tick = (exp_tick >= Interval) ? (exp_tick - Interval) : ((reload_ticks + exp_tick) - Interval);
    min_distance = reload_ticks;
    do
    {
        cur_tick = (uint32_t)(SysTick->VAL);
        distance = (exp_tick <= cur_tick) ? (cur_tick - exp_tick) : ((reload_ticks - exp_tick) + cur_tick);
        if (distance > min_distance) /* Distance increasement means that current time is overdue. */
        {
            break;
        }
        min_distance = distance;
    } while (1);

    return exp_tick;

}

/*****************************************************************************
*
* @brief Delay ticks with minimum delay interval.
*
* @param[in]  Delay ticks of sys clock
* @retval     none
*
******************************************************************************/
void SysTick_DelayTicks(uint32_t ticks)
{
    uint32_t distance, cur_tick;
    uint32_t tick = SysTick->VAL;
    uint32_t max_distance;
    
    max_distance = 0;
    do
    {
        cur_tick = SysTick->VAL;
        distance = (cur_tick <= tick)?(tick - cur_tick):((reload_ticks - cur_tick) + tick);
        /* Distance decreasement means loopback occurs */
        if (distance >= ticks || distance < max_distance)
        {
            break;
        }
        max_distance = distance;
    } while (1);//((tick - SysTick->VAL) & 0x00FFFFFF) < ticks);
}

/*****************************************************************************
*
* @brief Get systime in milliseconds after chip is powered on.
*
* @param      none
* @retval     Systime in milliseconds after chip is powered on.
*
******************************************************************************/
uint64_t SysTick_GetSysTime(void)
{
   return s_sysTime;
}

/*****************************************************************************
*
* @brief Delay some milliseconds. Accuracy is not assured within one milliseconds.
*
* @param[in]  Delay interval by ms
* @retval     none
*
******************************************************************************/
void SysTick_DelayMS(uint32_t ms)
{
    
    while (ms >= 60)
    {
        SysTick_DelayUS(60000);
        ms -= 60;
    }
    SysTick_DelayUS((uint16_t)(ms*1000));
#if 0    
	uint64_t start_time = s_sysTime;
    uint32_t distance;
    
    while (1)
    {
        distance = s_sysTime - start_time;
        /* Delay more 1ms than expected */
        if (distance > ms)
        {
            break;
        }
    }
#endif
}

/*****************************************************************************
*
* @brief Delay some microseconds.
*
* @param[in]  Delay interval by us
* @retval     none
******************************************************************************/
void SysTick_DelayUS(uint16_t us)
{
    uint32_t distance, cur_tick, interval;
    uint32_t start_tick = SysTick->VAL;
    uint32_t ticks = SYSTICK_US_TICKS*us;
    uint32_t max_distance;
    uint64_t start_time;
    
    
    /* Interval ticks = (ticks not waited at the beginning) + (wait ticks)  */
    interval   = ticks + (s_TimerIntCnt + 1)*reload_ticks - start_tick;
    start_time = s_sysTime;
    
    /* Wait systick turn round for several times */
    while (ticks >= reload_ticks)
    {
        max_distance = 0;
        /* Wait for one half of reload_ticks */
        do
        {
            cur_tick = SysTick->VAL;
            distance = (cur_tick <= start_tick)?(start_tick - cur_tick):((reload_ticks - cur_tick) + start_tick);
            /* Distance decreasement means loopback occurs */
            if (distance < max_distance)
            {
                break;
            }
            max_distance = distance; 
            /* Be sure that this loop exits in case that os is enabled(reload_ticks <= SYSTICK_MS_TICKS). */
            distance = s_sysTime - start_time; /* I have to do this to avoid spending much time for 64bit multiplication */
            if (distance*SYSTICK_MS_TICKS > interval)
            {
                return;
            }
        } while (1);
    
        ticks -= reload_ticks;
    }
    
    max_distance = 0;
    /* Wait for remain ticks */
    do
    {
        cur_tick = SysTick->VAL;
        distance = (cur_tick <= start_tick)?(start_tick - cur_tick):((reload_ticks - cur_tick) + start_tick);
        /* Distance decreasement means loopback occurs */
        if (distance >= ticks || distance < max_distance)
        {
            break;
        }
        max_distance = distance; 
        distance = s_sysTime - start_time; /* I have to do this to avoid spending much time for 64bit multiplication */
        if (distance*SYSTICK_MS_TICKS > interval)
        {
           return;
        }
    } while (1);    
}

void WaitTimerInit(WaitTimer_T *waitTimer, uint32_t timeout, uint32_t wakeupPeriod)
{
    waitTimer->startTime = s_sysTime;
    waitTimer->wakeupPeriod = wakeupPeriod;
    waitTimer->timeout = timeout;
}

RET_CODE WaitTillTimeout(WaitTimer_T *waitTimer)
{
    uint64_t distance;
    uint32_t cur_tick = SysTick->VAL;

    if (waitTimer->timeout == 0)
    {
        return RET_OK;
    }
/**
 * Warning!!!!!!!!!
 * Never change code here, it will make all drivers timeout unexpectedly and fail to work forever. 
 */
    distance = s_sysTime - waitTimer->startTime;
    /* Wait more 1ms than expected */
    if (distance > waitTimer->timeout)
    {
        return RET_TIMEOUT;
    }
    
    /* Delete it to make sure delay does not take much time if os is enabled. */
    //SysTick_DelayUS(waitTimer->wakeupPeriod);

    return RET_ONGOING;
}

__weak void Systick_Callback(void)
{
    
}

void Systick_InterruptHandler(void)
{
/**
 * Warning!!!!!!!!!
 * Never touch s_sysTime, it will make all drivers timeout unexpectedly and fail to work forever. 
 */
    
    if (++s_TimerIntCnt >= s_MSIntNum)
    {
        /* for sdk driver used for delay*/
        s_sysTime++;
        s_TimerIntCnt = 0;
    }
    
    if (++s_CallbackIntCnt >= s_CallbackIntNum)
    {
        Systick_Callback();
        s_CallbackIntCnt = 0;
    }
    
}
