/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file cpu_clock.c
 *
 * @version 1.0.0
 * 
 * @date 2015/08/21 
 *    
 * @contents Verification and example code of DMA. 
 * 
 * @history
 *	Date			   By			Comments		Ver.
 *	=========		============	=========	 =========
 *	2016/07/25		Wizephen.Wang	 Create		   1.0.0
 **********************************************************************/
#include "stdio.h" 
#include <common.h>
#include <drv/dev/uart.h>

/******************************************************************************
* Global variables
******************************************************************************/
DMA_Handle_T g_dmaHandle;

static volatile  uint32_t s_dmaComplete = 0;
static volatile  uint32_t s_dmaError = 0;

uint32_t g_srcData[16];
uint32_t g_destData[16];

/******************************************************************************
* Constants and macros
******************************************************************************/
   
/******************************************************************************
* Local types
******************************************************************************/
 
/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
/******************************************************************************
* Local functions
******************************************************************************/
int main (void);
/******************************************************************************
* Global functions
******************************************************************************/
void dmaCallback(struct DMA_Handle *hDMA, uint8_t event, unsigned long param)
{
    if (event == DMA_EVENT_TRANSFER_DONE)
    {
        /* DMA transaction completes */
        s_dmaComplete = 1;        
    }

    if (event == DMA_EVENT_TRANSFER_ERROR)
    {
        /* Error occurs during DMA transaction */
        s_dmaError = 1;                
    }
}

int main (void)
{
    uint8_t i = 0;
    DMA_Config_T dmaConfig;		    
	
    dmaConfig.callback      = dmaCallback;
    /* Software handshake for destination */    
    dmaConfig.destHandshake = DMA_SOFTWARE_HANDSHAKE;    
    /* Address increasement for destination */    
    dmaConfig.destIncrement = DMA_ADDR_INCREMENT;
    /* DMA data width for destination */    
    dmaConfig.destWidth     = DMA_WIDTH_32BIT;
    /* DMA direction */
    dmaConfig.direction     = DMA_MEMORY_TO_MEMORY;    
    dmaConfig.srcHandshake  = DMA_SOFTWARE_HANDSHAKE;
    dmaConfig.srcIncrement  = DMA_ADDR_INCREMENT;
    dmaConfig.srcWidth      = DMA_WIDTH_32BIT;
#ifdef CHIP_GM6621
	dmaConfig.requestType = DMA_REQUEST_M2M;
#endif

    DMA_Init(DMA0, &dmaConfig, &g_dmaHandle);
    
    /* Assign initial value for source */
    for (i =0;i < 16;i ++)
    {
        g_srcData[i] = 0x5AA50000 + i;
    }
    
    /* Start DMA */
    DMA_Start(&g_dmaHandle, (uint32_t) g_srcData, (uint32_t) g_destData, 16);    
    
    /* Wait till either DMA completes or error occures */
    while ((s_dmaComplete == 0) && (s_dmaError == 0)) ;
    
    if (s_dmaComplete == 1)
    {                        
        for (i =0;i < 16;i ++)
        {
            if (g_srcData[i] != g_destData[i])
            {            
                return RET_ERROR;
            }        
        }        
    }
    
    if (s_dmaError == 1)
    {                
        return RET_ERROR; 
    }
    
    DMA_DeInit(&g_dmaHandle);
    
    while (1) 
    {
        __asm ("nop");
    }
}
