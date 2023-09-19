 /**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          WDOG.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2016/07/05
 *
 * @brief         Header file of WDOG driver.
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

#include <drv/dev/WDOG.h>

static WDOG_Handle_T *s_pHandle = NULL;


static void WDOG_IrqHandler(void);
static RET_CODE WDOG_SetLoadVal(WDOG_Device_T *pWDOG, WDOG_Config_T *pConfig);
static RET_CODE WDOG_SetWindowLoad(WDOG_Device_T *pWDOG,WDOG_Config_T *pConfig);
#if   (defined(CHIP_GM6601)) || (defined(CHIP_GM6601B)) || (defined(CHIP_GM6621)) || (defined(CHIP_F6601C))
static RET_CODE WDOG_SetClkSrc(WDOG_CLKSRC_T clkSrc);
#endif

STATIC_ISR(WDOG_IRQn, WDOG_IrqHandler);

WDOG_Callback_T pWdogCallback = NULL;

#if (defined(CHIP_GM6601))
static void WDOG_ResetChipEnable(WDOG_Device_T *pWDOG)
{
    WDOG_UNLOCK(pWDOG);
    IO_BIT_SET(pWDOG->CTRL, WDOG_CTRL_RE);
    WDOG_LOCK(pWDOG);
}

static void WDOG_SetLoadCnt(WDOG_Device_T *pWDOG, uint32_t loadCnt)
{
    WDOG_UNLOCK(pWDOG);
    pWDOG->LD = loadCnt;
    WDOG_LOCK(pWDOG);
}
#endif /*(defined(CHIP_GM6601))*/

static void WDOG_IrqHandler(void)
{
#if (defined(CHIP_GM6601))
    WDOG_IntClear(s_pHandle);
    /* Notify usr todo some action before chip reset */
    if (pWdogCallback != NULL)
    {
        pWdogCallback(s_pHandle, WDOG_EVENT_REOOT_WARNING, 0);
    }

#endif /*(defined(CHIP_GM6601))*/

#if ((defined(CHIP_GM6601B)) || (defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621) || defined(CHIP_F6601C)))

    WDOG_IntClear(s_pHandle);
    WDOG_PR_ERR("\rclear int\n");

    /*notice usr todo some action before chip reset*/
    if (pWdogCallback != NULL)
    {
        pWdogCallback(s_pHandle, WDOG_EVENT_REOOT_WARNING, 0);
    }
    WDOG_PR_ERR("\rthen return mainloop\n");

#endif /*(defined(CHIP_GM6611))*/
}

RET_CODE WDOG_Init(WDOG_Device_T *pWDOG, WDOG_Config_T *pConfig, WDOG_Handle_T *pHandle)
{
    RET_CODE ret = RET_OK;

    if ((pWDOG && pConfig && pHandle) == NULL )
    {
        WDOG_PR_ERR("%s:input paramer error\n",__FUNCTION__);
        ret = RET_INVPARAM;
        goto out;
    }

    s_pHandle = pHandle;
    pHandle->device = pWDOG;


    /* Disable WDOG irq and WDOG cnt will hold */
#if  (defined(CHIP_GM6601)) || (defined(CHIP_GM6601B)) || (defined(CHIP_GM6621)) || (defined(CHIP_F6601C))
    /* Set WDOG srcclk */
    if (WDOG_SetClkSrc(pConfig->clkSrc) != RET_OK)
    {
        WDOG_PR_ERR("\rerr:%s:WDOG_SetClkSrc\n",__FUNCTION__);
        ret = RET_ERROR;
        goto out;
    }

    WDOG_Disable(pHandle);
#endif /*(defined(CHIP_GM6601))*/
#if  (defined(CHIP_GM6601))
    /* Enable WDOG to reset chip */
    WDOG_ResetChipEnable(pWDOG);
#endif /*(defined(CHIP_GM6601))*/

#if ((defined(CHIP_GM6601B)) ||  (defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621)) || (defined(CHIP_F6601C)))
/* enable wdog to reset chip */
    if (pConfig->wdogWorkEWIMode == WDOG_WINDOW_MODE)
    {
        if (WDOG_SetWindowLoad(pWDOG,pConfig) != RET_OK)
        {
            WDOG_PR_ERR("\rerr:%s:WDOG_Set_window_LoadVal\n",__FUNCTION__);
            ret = RET_ERROR;
            goto out;
        }
        /* Set WDOG window mode */
        WDOG_SetWindowMode(pHandle);
    }
    else if (pConfig->wdogWorkEWIMode == WDOG_NORMAL_MODE)
    {
        /* Set WDOG normal mode */
        WDOG_SetNormalMode(pHandle);
    }
    else if (pConfig->wdogWorkEWIMode == WDOG_WINDOW_EWI)
    {
        if (WDOG_SetWindowLoad(pWDOG,pConfig) != RET_OK)
        {
            WDOG_PR_ERR("\rerr:%s:WDOG_Set_window_LoadVal\n",__FUNCTION__);
            ret = RET_ERROR;
            goto out;
        }
        /* Set WDOG window and EWI mode */
        WDOG_SetWindowMode_EWI(pHandle);
    }
    else if (pConfig->wdogWorkEWIMode == WDOG_NORMAL_EWI)
    {
        /* Set WDOG normal and EWI mode */
        WDOG_SetNormalMode_EWI(pHandle);
    }

#endif   /*(defined(CHIP_GM6611))*/
    /* Set wdogclk and loadcnt according to usr set reboot time : second is the time uint */
    if (WDOG_SetLoadVal(pWDOG,pConfig) != RET_OK)
    {
        WDOG_PR_ERR("\rerr:%s:WDOG_Set_SrcClk_LoadVal\n",__FUNCTION__);
        ret = RET_ERROR;
        goto out;
    }
    /* Enabel nvic WDOG irq and register irq callback */
    pWdogCallback = pConfig->callBack;
    NVIC_EnableIRQ(WDOG_IRQn);

out:
    return ret;
}

#if ((defined(CHIP_GM6601B)) || (defined(CHIP_GM6601)) || (defined(CHIP_GM6621)) || defined(CHIP_F6601C))

static RET_CODE WDOG_SetLoadVal(WDOG_Device_T *pWDOG,WDOG_Config_T *pConfig)
{
    RET_CODE ret = RET_OK ;

    WDOG_UNLOCK(pWDOG);

    WDOG_WAIT_WRITEDONE();
    /*Accoding different WDOG srcclk to set WDOG cnt to match the usr set reboot time */
    switch (pConfig->clkSrc)
    {
    case CLKSRC_FCLK:
        pWDOG->LD = ((SYS_CLOCK / MS_DIV) * pConfig->resetTime);
        break;

    case CLKSRC_OSC_LS_CLK:
        pWDOG->LD = ((OSC_LS_CLK * pConfig->resetTime) / MS_DIV);
        break;

    case CLKSRC_IRC_LS_CLK:
        pWDOG->LD = ((IRC_LS_CLK / MS_DIV) * pConfig->resetTime);
        break;

    case CLKSRC_PLL_SRC_CLK:
        pWDOG->LD = ((PLL_SRC_CLK / MS_DIV) * pConfig->resetTime);
        break;

    default:
        WDOG_PR_ERR("Err WDOG_SYS_CLK:%d\n",(int32_t)pConfig->clkSrc);
        ret = RET_INVPARAM;
        break;
    }

    WDOG_LOCK(pWDOG);

    return ret;
}
#endif  /*end ((defined(CHIP_GM6601)) || (defined(CHIP_GM6621))) */

#if	(defined(CHIP_GM6601)) || (defined(CHIP_GM6601B)) || (defined(CHIP_GM6621)) || defined(CHIP_F6601C)
static RET_CODE WDOG_SetClkSrc(WDOG_CLKSRC_T clkSrc)
{
    RET_CODE ret = RET_OK;
    PeriphClk_Config_T config;

    config.periphClkMask = CLOCK_PERIPH_WDT_MASK;
    config.wdtClock = clkSrc;
    ret = PeriphClk_SetConfig(&config);

    return ret;

}
#endif

void WDOG_Enable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->device;
    WDOG_UNLOCK(pWDOG);
    WDOG_WAIT_WRITEDONE();
    IO_BIT_SET(pWDOG->CTRL, WDOG_CTRL_IE);
    WDOG_LOCK(pWDOG);
}

void WDOG_Disable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->device;

    WDOG_UNLOCK(pWDOG);
    WDOG_WAIT_WRITEDONE();
    IO_BIT_CLEAR(pWDOG->CTRL, WDOG_CTRL_IE);
    WDOG_LOCK(pWDOG);
}

void WDOG_FeedWdog(WDOG_Handle_T* pHandle)
{
    WDOG_Device_T *pWDOG = pHandle->device;

#if (defined(CHIP_GM6601))
    WDOG_UNLOCK(pWDOG);
    /* Clear WDOG irq which is equal to feed wdog */
    pWDOG->IC = WDOG_CLEAR_INT_MAGICNUM;
    WDOG_LOCK(pWDOG);
#endif /*(defined(CHIP_GM6601)) */

#if ((defined(CHIP_GM6601B)) || (defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621)) || defined(CHIP_F6601C))

    WDOG_WAIT_WRITEDONE();
    pWDOG->KR = 0xAAAA;

#endif /*(defined(CHIP_GM6601B)) || (defined(CHIP_GM6611)) || (defined(CHIP_GM6621)) */
}

void WDOG_IntClear(WDOG_Handle_T* pHandle)
{
    WDOG_Device_T *pDev = pHandle->device;

    WDOG_UNLOCK(pDev);
    WDOG_WAIT_WRITEDONE();
    /* clear wdog irq which is equal to feedwdog */
    pDev->IC = WDOG_CLEAR_INT_MAGICNUM;
    WDOG_LOCK(pDev);

}
#if ((defined(CHIP_GM6601B)) || (defined(CHIP_GM6611)) || defined(CHIP_GM6611B) || (defined(CHIP_GM6621)) || defined(CHIP_F6601C))

static void WDOG_EWI_Enable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pDev = pHandle->device;

    WDOG_UNLOCK(pDev);
    WDOG_WAIT_WRITEDONE();
    IO_BIT_SET(pDev->CTRL, WDOG_INT_EN);
    WDOG_LOCK(pDev);
}
static void WDOG_EWI_Disable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pDev = pHandle->device;

    WDOG_UNLOCK(pDev);
    WDOG_WAIT_WRITEDONE();
    IO_BIT_CLEAR(pDev->CTRL, WDOG_INT_EN);
    WDOG_LOCK(pDev);
}
static void WDOG_WinMode_Enable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pDev = pHandle->device;

    WDOG_UNLOCK(pDev);
    WDOG_WAIT_WRITEDONE();
    IO_BIT_SET(pDev->CTRL, WDOG_WINDOW_EN);
    WDOG_LOCK(pDev);
}
static void WDOG_WinMode_Disable(WDOG_Handle_T *pHandle)
{
    WDOG_Device_T *pDev = pHandle->device;

    WDOG_UNLOCK(pDev);
    WDOG_WAIT_WRITEDONE();
    IO_BIT_CLEAR(pDev->CTRL, WDOG_WINDOW_EN);
    WDOG_LOCK(pDev);
}

void WDOG_SetWindowMode(WDOG_Handle_T *pHandle)
{
    WDOG_WinMode_Enable(pHandle);
    WDOG_EWI_Disable(pHandle);
}

void WDOG_SetWindowMode_EWI(WDOG_Handle_T *pHandle)
{
    WDOG_WinMode_Enable(pHandle);
    WDOG_EWI_Enable(pHandle);
}
void WDOG_SetNormalMode(WDOG_Handle_T *pHandle)
{
    WDOG_WinMode_Disable(pHandle);
    WDOG_EWI_Disable(pHandle);
}
void WDOG_SetNormalMode_EWI(WDOG_Handle_T *pHandle)
{
    WDOG_WinMode_Disable(pHandle);
    WDOG_EWI_Enable(pHandle);
}

RET_CODE WDOG_GetCountVal(WDOG_Handle_T *pHandle)
{
    uint32_t result_val = 0;
    WDOG_Device_T *pWDOG = pHandle->device;

    WDOG_UNLOCK(pWDOG);
    result_val = pWDOG->VAL;
    WDOG_LOCK(pWDOG);

    return result_val;
}
RET_CODE WDOG_GetCntOutWinFlag(WDOG_Handle_T *pHandle)
{
    uint32_t outFlag = 0;
    WDOG_Device_T *pWDOG = pHandle->device;

    WDOG_UNLOCK(pWDOG);
    outFlag = (pWDOG->CTRL & 0X04);
    WDOG_LOCK(pWDOG);

    return outFlag;
}

#endif /*(defined(CHIP_GM6611)) */
#if  (defined(CHIP_GM6611) || defined(CHIP_GM6611B))
static RET_CODE WDOG_SetLoadVal(WDOG_Device_T *pWDOG,WDOG_Config_T *pConfig)
{
    RET_CODE ret = RET_OK ;
    WDOG_UNLOCK(pWDOG);
    WDOG_WAIT_WRITEDONE();
    switch (pConfig->clkSrc)
    {
        case CLKSRC_IRC_LS_CLK:
            pWDOG->LD = ((IRC_LS_CLK * pConfig->resetTime) / MS_DIV);
            break;
        default:
            WDOG_PR_ERR("Err WDOG_SYS_CLK:%d\n",(int32_t)pConfig->clkSrc);
            ret = RET_INVPARAM;
            break;
    }
    WDOG_LOCK(pWDOG);
    return ret;
}
static RET_CODE WDOG_SetWindowLoad(WDOG_Device_T *pWDOG,WDOG_Config_T *pConfig)
{
    RET_CODE ret = RET_OK ;
    WDOG_UNLOCK(pWDOG);
    WDOG_WAIT_WRITEDONE();
    switch (pConfig->clkSrc)
    {
        case CLKSRC_IRC_LS_CLK:
            pWDOG->WL = ((IRC_LS_CLK/MS_DIV)*pConfig->windowTime);
            break;
        default:
            WDOG_PR_ERR("\rErr WDOG_SYS_CLK:%d\n",(int32_t)pConfig->clkSrc);
            ret = RET_INVPARAM;
            break;
    }
    WDOG_LOCK(pWDOG);
    return ret;
}
#endif /*end (defined(CHIP_GM6611))*/

#if  ((defined(CHIP_GM6601B)) || (defined(CHIP_GM6621))  || defined(CHIP_F6601C))
static RET_CODE WDOG_SetWindowLoad(WDOG_Device_T *pWDOG,WDOG_Config_T *pConfig)
{
    RET_CODE ret = RET_OK ;

    WDOG_UNLOCK(pWDOG);
    WDOG_WAIT_WRITEDONE();
    /*accoding different wdog srcclk to set wdog cnt to match the usr set reboot time */
    switch (pConfig->clkSrc)
    {
    case CLKSRC_FCLK:
        pWDOG->WL = ((SYS_CLOCK/MS_DIV)*pConfig->windowTime);
     break;

    case CLKSRC_OSC_LS_CLK:
        pWDOG->WL = ((OSC_LS_CLK/MS_DIV)*pConfig->windowTime);
        break;

    case CLKSRC_IRC_LS_CLK:
        pWDOG->WL = ((IRC_LS_CLK*pConfig->windowTime)/MS_DIV);
        break;

    case CLKSRC_PLL_SRC_CLK:
        pWDOG->WL = ((PLL_SRC_CLK/MS_DIV)*pConfig->windowTime);
        break;

    default:
        WDOG_PR_ERR("\rErr WDOG_SYS_CLK:%d\n",(int32_t)pConfig->clkSrc);
        ret = RET_INVPARAM;
        break;
    }

    WDOG_LOCK(pWDOG);

    return ret;
}
#endif
