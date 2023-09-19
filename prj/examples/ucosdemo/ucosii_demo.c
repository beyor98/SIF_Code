/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file ucosii_dmeo.c
 *
 * @version 1.0.0
 *
 * @date 2016/07/05
 *
 * @contents Verification and example code of ucosii demo.
 *
 * @history
 *	   Date			    By			  Comments			    Ver
 *	=========		=========		 =========			 =========
 *	2017/03/20		Mike Zheng		  Create			  1.0.0
 **********************************************************************/

#include "stdio.h" 
#include <common.h>
#include <drv/dev/uart.h>
#include <includes.h>



extern  void  OSTimeSet (INT32U ticks);


#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF 
#endif
#ifdef ERR_PRINTF
    #undef ERR_PRINTF 
#endif
#ifdef WARNING_PRINTF
    #undef WARNING_PRINTF 
#endif

#define DEBUG_PRINTF
#define ERR_PRINTF
#define WARNING_PRINTF

#ifndef DEBUG_PRINTF
    #define pr_debug(...)       do{}while(0)
#else
    #define pr_debug            printf
#endif

#ifndef ERR_PRINTF
    #define pr_err(...)        do{}while(0)
#else
    #define pr_err             printf
#endif

#ifndef WARNING_PRINTF
    #define pr_warning(...)    do{}while(0)
#else
    #define pr_warning         printf
#endif

UART_Handle_T g_uart0Handle;


#define FirstTaskStkLength  512
OS_STK  FirstTaskStk[FirstTaskStkLength];
uint8_t FirstTaskPrio =5 ;


#define SecondTaskStkLength  512
OS_STK  SecondTaskStk[SecondTaskStkLength];
uint8_t SecondTaskPrio =4 ;

#define TICK_S          100




void SecondTask(void * arg)
{

	while(1)
	{

        	pr_debug("\r\n*****UCOS Second Task******\n");
            	OSTimeDly(3*TICK_S);
	}
}

void FirstTask(void * arg)
{

	/*initial systick for ucos time tick*/
	 SysTick_Init();
	/*initial interrupt prio or enable or disable*/
	OSTaskCreate (SecondTask,(void *)0, &(SecondTaskStk[SecondTaskStkLength - 1]), SecondTaskPrio);	
	

	while(1)
	{
             pr_debug("\r\n*****UCOS First Task*******\n");
		OSTimeDly(3*TICK_S);
	}
}


int main()
{

    OSInit();	 
     /* init for uart printf log */
    DebugConsoleInit(115200);
   // printf("\rOS Test\n");
    OSTaskCreate (FirstTask,(void *)0, &(FirstTaskStk[FirstTaskStkLength - 1]), FirstTaskPrio);
    OSStart();
    /* run first task*/

    while(1);
    }
