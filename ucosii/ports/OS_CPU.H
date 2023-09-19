/*
*********************************************************************************************************
*                                               uC/OS-II
*                                        The Real-Time Kernel
*
*                         (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                          All Rights Reserved
*
* File         : OS_CPU.H
* By           : Jean J. Labrosse
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                               ARM7 Port
*
*                 Target           : ARM7 (Includes ARM7TDMI, ARM710T, ARM720T, ARM740T)
*                 Ported by        : Michael Anburaj
*                 URL              : http://geocities.com/michaelanburaj/    Email : michaelanburaj@hotmail.com
*
*********************************************************************************************************
*/

#ifndef __OS_CPU_H__
#define __OS_CPU_H__


#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/*
*********************************************************************************************************
*                                              DATA TYPES
*                                         (Compiler Specific)
*         compiler : armcc 
*         some basic data types are modified.
*         date: July,20 2004
*        modified by sherly zhu
*********************************************************************************************************
*/


typedef unsigned char  BOOLEAN;                 
typedef unsigned char  INT8U;                         	/*  Unsigned  8 bit integer               */
typedef signed   char  	INT8S;                         	/*  Signed  8 bit integer                  */
typedef unsigned short	INT16U;                       	/* Unsigned  16 bit integer              */
typedef signed   short  INT16S;                        	/* Signed  16 bit integer                 */
typedef unsigned int   	INT32U;                        	/* Unsigned  32 bit integer              */
typedef signed   int     	INT32S;                         	/* Signed  32 bit integer                 */
typedef float              	FP32;                             	/* Single precision floating point (32bit)   */
typedef double           	FP64;                             	/* Double precision floating point (64bit)  */

typedef INT32U         	OS_STK;                         	/* Each stack entry is 32-bit wide           */
typedef unsigned int   	OS_CPU_SR;                	/* Define size of CPU status register (PSR = 32 bits) */



/* The following data type definition is for backward compatibility    */
#define BYTE            INT8S
#define UBYTE          INT8U
#define WORD           INT16S
#define UWORD         INT16U
#define LONG            INT32S
#define ULONG          INT32U


/* 
*********************************************************************************************************
*                              ARM
*
* Method #1:  Disable/Enable interrupts using simple instructions.  After critical section, interrupts
*             will be enabled even if they were disabled before entering the critical section.
*
* Method #2:  Disable/Enable interrupts by preserving the state of interrupts.  In other words, if 
*             interrupts were disabled before entering the critical section, they will be disabled when
*             leaving the critical section.
*
* Method #3:  Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking you
*             would store the state of the interrupt disable flag in the local variable 'cpu_sr' and then
*             disable interrupts.  'cpu_sr' is allocated in all of uC/OS-II's functions that need to 
*             disable interrupts.  You would restore the interrupt disable state by copying back 'cpu_sr'
*             into the CPU's status register.
*
* Note     :  In this ARM7 Port: Method #1 not implemeted.
*             ------------------------------
*            | Method | SDT 2.51 | ADS 1.2  |
*            |========+==========+==========|
*            |   1    |   N/I    |   N/I    |
*            |   2    |   yes    |   no     |
*            |   3    |   yes    |   yes    |
*             ------------------------------
*
*********************************************************************************************************
*/
/* Don't modify these lines. ADS can only support OS_CRITICAL_METHOD 3. */
#define  OS_CRITICAL_METHOD    3


#define ADS          

#if defined (ADS)
#define  OS_CRITICAL_METHOD    3
#elif defined (SDT)
#define  OS_CRITICAL_METHOD    2
#else
        #error Please define a valid tool chain
#endif

#define  OS_CRITICAL_METHOD    3

#if      OS_CRITICAL_METHOD == 2
#define  OS_ENTER_CRITICAL() IRQFIQDE                     /* Disable interrupts                        */
/* Note: R0 register need not be saved, for it gets saved outside. Refer SDT userguide 6-5 */
#define IRQFIQDE __asm                       \
{                                            \
        mrs r0,CPSR;                         \
        stmfd sp!,{r0};                      \
        orr r0,r0,#NOINT;                    \
        msr CPSR_c,r0;                       \
}

#define  OS_EXIT_CRITICAL() IRQFIQRE                      /* Restore  interrupts                       */
#define IRQFIQRE __asm                       \
{                                            \
        ldmfd sp!,{r0};                      \
        msr CPSR_c,r0;                       \
}
#endif


#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
OS_CPU_SR  OS_CPU_SR_Save(void);
void       OS_CPU_SR_Restore(OS_CPU_SR cpu_sr);
#endif


#if  OS_CRITICAL_METHOD == 3
#define  OS_ENTER_CRITICAL()         do { cpu_sr = OS_CPU_SR_Save(); } while (0) /* Save    CPU status word & disable interrupts.*/
#define  OS_EXIT_CRITICAL()           do { OS_CPU_SR_Restore(cpu_sr); } while (0) /* Restore CPU status word.                     */
#endif





/*
*********************************************************************************************************
*                           ARM Miscellaneous
*********************************************************************************************************
*/

#define  OS_STK_GROWTH        1                       /* Stack grows from HIGH to LOW memory   */
#define  OS_TASK_SW()         OSCtxSw()

/*
*********************************************************************************************************
*                                            GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              PROTOTYPES
*********************************************************************************************************
*/




void OSCtxSw (void);
void OSIntCtxSw (void);
void OSStartHighRdy (void);

void OSPendSV (void);
void  OS_CPU_SysTickHandler (void);

OS_CPU_EXT INT32U OsEnterSum; 
#endif /*__OS_CPU_H__*/
 
