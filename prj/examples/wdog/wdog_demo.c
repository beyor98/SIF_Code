/*====================================================================
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file wdog_demo.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/05
 *
 * @contents Verification and example code of WDOG Output
 *
 * @history
 *     Date             By            Comments              Ver
 *  =========       =========        =========           =========
 *  2016/07/05      Mike Zheng        Create                     1.0.0
 *  2016/11/22    Mike.Zheng      Update For Release   1.1.0
 *  2017/03/15      kenny.cmt         Update For GM6611    1.0.0
 **********************************************************************/

#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/wdog.h>


/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#define DBG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do {} while (0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{} while (0)
#else
    #define PR_WARNINR         printf
#endif

#define WDOG_INIT_RESET_TIME     5000    /* uint ms */
#define WDOG_TEST_LOOP_TIME      3000    /* uint ms */
#define WDOG_FEED_CNT            3       /* wdog test feed watchdong cnt */

#if (defined(CHIP_GM6611))
    #define NORMAL                    0    /* normal mode*/
    #define NORMAL_EWI                  1    /* normal early warning interrupt mode*/

    #define WDT_CLK_30K           1

#endif /*(defined(CHIP_GM6611)*/
#if (defined(CHIP_GM6611B))
    #define NORMAL                      1    /* normal mode*/
    #define NORMAL_EWI                    0    /* normal early warning interrupt mode*/
    #define WDT_CLK_30K             1
#endif /*(defined(CHIP_GM6611B)*/

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    #define WDT_CLK_OSC_LS_32768        0
    #define WDT_CLK_IRC_LS_30K          0
    #define WDT_CLK_OSC_48M             0
    #define WDT_CLK_PLL_8M              1
#endif /*(defined(CHIP_GM6601B)))*/

#if (defined(CHIP_GM6621))
    #define WDT_CLK_OSC_LS_32768        1
    #define WDT_CLK_IRC_LS_30K          0
    #define WDT_CLK_OSC_48M             0
    #define WDT_CLK_PLL_8M              0
#endif /*(defined(CHIP_GM6621)))*/
/******************************************************************************
* Local types
******************************************************************************/

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
static WDOG_Handle_T s_Handle;

/******************************************************************************
* Local functions
******************************************************************************/
int main(void);

#if !defined(CHIP_GM6621)
static void WDOG_Callback(WDOG_Handle_T *pHandle, uint8_t event, unsigned long Param);

/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/
static void WDOG_Callback(WDOG_Handle_T *pHandle, uint8_t event, unsigned long Param)
{
    if (event == WDOG_EVENT_REOOT_WARNING)
    {
        PR_WARNINR("\rChip Will Resert,Please Do Some Actions\n");
    }
}
#endif

int main(void)
{
    RET_CODE ret = RET_OK;
    WDOG_Device_T *pDev = WDOG_DEV;
    WDOG_Config_T  wdogCfg;
    uint8_t  feedCnt = WDOG_FEED_CNT;

    SysTick_Init();
    /* init for uart printf log */
    DebugConsoleInit(19200);

    PR_DEBUG("\nWdog App Test Start Boot\n");

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
    memset(&wdogCfg, 0, sizeof(WDOG_Config_T));

    wdogCfg.wdogWorkEWIMode  = WDOG_NORMAL_MODE;
    wdogCfg.callBack  = NULL;
#if WDT_CLK_OSC_LS_32768
    wdogCfg.clkSrc    = CLKSRC_OSC_LS_CLK;
#endif
#if WDT_CLK_IRC_LS_30K
    wdogCfg.clkSrc    = CLKSRC_IRC_LS_CLK;
#endif
#if WDT_CLK_OSC_48M
    wdogCfg.clkSrc    = CLKSRC_FCLK;
#endif
#if WDT_CLK_PLL_8M
    wdogCfg.clkSrc    = CLKSRC_PLL_SRC_CLK;
#endif
    wdogCfg.resetTime = WDOG_INIT_RESET_TIME;
    wdogCfg.windowTime = 0;


#endif /*((defined(CHIP_GM6601B)))*/

#if (defined(CHIP_GM6611B))
#if NORMAL
    wdogCfg.wdogWorkEWIMode  = WDOG_NORMAL_MODE;
    wdogCfg.callBack  = NULL;
#if WDT_CLK_30K
    wdogCfg.clkSrc    = CLKSRC_IRC_LS_CLK;
#endif
    wdogCfg.resetTime = WDOG_INIT_RESET_TIME;
    wdogCfg.windowTime = 0;
#endif /* End NORMAL */
#if NORMAL_EWI
    wdogCfg.wdogWorkEWIMode  = WDOG_NORMAL_EWI;
    wdogCfg.callBack  = WDOG_Callback;
#if WDT_CLK_30K
    wdogCfg.clkSrc    = CLKSRC_IRC_LS_CLK;
#endif
    wdogCfg.resetTime = WDOG_INIT_RESET_TIME;
    wdogCfg.windowTime = 0;
#endif /* End NORMAL_EWI*/
#endif /*(defined(CHIP_GM6611B))*/
#if (defined(CHIP_GM6621))
    memset(&wdogCfg, 0, sizeof(WDOG_Config_T));

    wdogCfg.wdogWorkEWIMode  = WDOG_NORMAL_MODE;
    wdogCfg.callBack  = NULL;
#if WDT_CLK_OSC_LS_32768
    wdogCfg.clkSrc    = CLKSRC_OSC_LS_CLK;
#endif
#if WDT_CLK_IRC_LS_30K
    wdogCfg.clkSrc    = CLKSRC_IRC_LS_CLK;
#endif
#if WDT_CLK_OSC_48M
    wdogCfg.clkSrc    = CLKSRC_FCLK;
#endif
#if WDT_CLK_PLL_8M
    wdogCfg.clkSrc    = CLKSRC_PLL_SRC_CLK;
#endif
    wdogCfg.resetTime = WDOG_INIT_RESET_TIME;
    wdogCfg.windowTime = 0;

#endif /*((defined(CHIP_GM6621)))*/
    ret = WDOG_Init(pDev, &wdogCfg, &s_Handle);
    if (ret)
    {
        PR_ERR("err:%d WDOG_Init\n", ret);
        goto out;
    }

    WDOG_Enable(&s_Handle);

    while (feedCnt--)
    {
        /**
         * not reset chip ,because before reset time coming,
         * we feed it in time.
         */
        SysTick_DelayMS(WDOG_TEST_LOOP_TIME);
        WDOG_FeedWdog(&s_Handle);
        PR_DEBUG("FeedWdog :%d\n", feedCnt);
    }
    /**
     * after WDOG_INIT_RESET_TIME wdog callback will call,
     * chip will reset when callback return.
     */
    PR_DEBUG("Chip will reset ...\n");

    while (1)
    {
        PR_DEBUG("Waiting for ......\n");
        SysTick_DelayMS(1000);
    }
out:
    return ret;
}
