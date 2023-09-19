
;** File name: 			    os_cpu_a.asm
;** Last modified Date:     2007.01.18
;** Last Version: 		    1.0
;** Descriptions: 	        The assembly functions that a uC/OS-II port requires
;**                         uC/OS-II移植所需要的汇编函数  

;*********************************************************************************************************
;  Declarations for the interrupt handlers that are used by the application.              
;  本应用所用到的声明                                       
;*********************************************************************************************************
                EXTERN  OSRunning      
				          
                EXTERN  OSPrioCur
                EXTERN  OSPrioHighRdy
                EXTERN  OSTCBCur
                EXTERN  OSTCBHighRdy
                EXTERN  OSIntNesting
                EXTERN  OSIntExit
                EXTERN  OSTaskSwHook
             
;*********************************************************************************************************
;  Declarations for the exported functions             
;  输出外部声明                          
;*********************************************************************************************************

                EXPORT  OSStartHighRdy
                EXPORT  OSCtxSw
                EXPORT  OSIntCtxSw
				EXPORT  OS_CPU_SR_Save                                      
				EXPORT  OS_CPU_SR_Restore
				
                EXPORT  INT14_Handler
                EXPORT  intDisAll
				EXPORT	intEnAll


;*********************************************************************************************************
;  Registers or macros used by this file                                
;  本文件用到的寄存器和宏                             
;*********************************************************************************************************							    
NVIC_INT_CTRL           EQU     0xE000ED04                              ;  Interrupt control state  
                                                                        ;  register.
                                                                        ;  中断控制寄存器

NVIC_SYSPRI2            EQU     0xE000ED20                              ;  System priority register (2)
                                                                        ;  系统优先级寄存器（2）

NVIC_PENDSV_PRI         EQU     0xFFFF0000                              ;  PendSV and system tick priority
                                                                        ;  (Both are lowest,0xff)  
                                                                        ;  软件中断和系统节拍中断
                                                                        ;  （都为最低，0xff）.
NVIC_PENDSVSET          EQU     0x10000000                                   ;  Value to trigger PendSV  
                                                                        ;  exception.触发软件中断的值.

OS_CRITICAL_INT_PRIO    EQU     (0 << 5)		                        ;  Critical setions manage prio. 
                                                                        ; (equal or bigger number managed) 
                                                                        ;  临界代码管理的优先级（数值等于
																		;  或大于被管理）
	        



;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8


;*********************************************************************************************************
;** Function name:			OSStartHighRdy
;** Descriptions:			Uses the context switcher  to cause the first task to start.
;**                         使用调度器运行第一个任务   
;** Input parameters:	    None 无
;** Output parameters:	    None 无
;** Returned value:			None 无
;*********************************************************************************************************
OSStartHighRdy

        LDR     R4, =NVIC_SYSPRI2                                       ;  set the PendSV exception 
                                                                        ;  priority设置PendSV优先级
        LDR     R5, =NVIC_PENDSV_PRI	
        STR     R5, [R4]
	    
        MOVS    R4, #0                                                  ;  set the PSP to 0 for initial  
                                                                        ;  context switch call 使PSP等于0
        MSR     PSP, R4

        LDR     R4, =OSRunning                                          ;  OSRunning = TRUE
        MOVS    R5, #1
        STRB    R5, [R4]

        LDR     R4, =NVIC_INT_CTRL                                      ;  trigger the PendSV exception
                                                                        ;  触发软件中断
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]

        CPSIE   I                                                       ;  enable interrupts at processor 
                                                                        ;  level使能所有优先级的中断
OSStartHang
        B       OSStartHang             



;*******************************************************************************************************
;** Function name:			OSCtxSw
;** Descriptions:		    Perform a contex switch from task level  任务级上下文切换                          
;** Input parameters:	    None 无
;** Output parameters:	    None 无
;** Returned value:			None 无        
;*********************************************************************************************************
OSCtxSw
        PUSH    {R4, R5}
		LDR     R4, =NVIC_INT_CTRL                                      ;  trigger the PendSV exception
                                                                        ;  触发软件中断
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]
		POP     {R4, R5}
        BX      LR


;*********************************************************************************************************
;** Function name:			OSIntCtxSw
;** Descriptions:		    called by OSIntExit() when it determines a context switch is needed as the
;**                         result of an interrupt.
;**                         中断级任务切换                 
;** Input parameters:	    None 无
;** Output parameters:	    None 无
;** Returned value:			None 无
;*********************************************************************************************************
OSIntCtxSw
        PUSH    {R4, R5}
		LDR     R4, =NVIC_INT_CTRL                                      ;  trigger the PendSV exception
                                                                        ;  触发软件中断
        LDR     R5, =NVIC_PENDSVSET
        STR     R5, [R4]
		POP     {R4, R5}
        BX      LR
        NOP
;********************************************************************************************************
;** Function name:			INT14_Handler
;** Descriptions:		   	Redfine INT14_Handler to OS_CPU_PendSVHandler function instead of weak Link                                       
;** Input parameters:	    None 无
;** Output parameters:	    None 无
;** Returned value:			None 无
;********************************************************************************************************

INT14_Handler
    LDR     R0, =OS_CPU_PendSVHandler
    BX      R0
;********************************************************************************************************
;                                      CRITICAL SECTION FUNCTIONS
;
; Description : Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking, the
;               state of the interrupt disable flag is stored in the local variable 'cpu_sr' & interrupts
;               are then disabled ('cpu_sr' is allocated in all functions that need to disable interrupts).
;               The previous interrupt state is restored by copying 'cpu_sr' into the CPU's status register.
;
; Prototypes  : CPU_SR  CPU_SR_Save   (void);
;               void    CPU_SR_Restore(CPU_SR  cpu_sr);
;
; Note(s)     : (1) These functions are used in general like this :
;
;                       void  Task (void  *p_arg)
;                       {
;                           CPU_SR_ALLOC();                     /* Allocate storage for CPU status register */
;                               :
;                               :
;                           CPU_CRITICAL_ENTER();               /* cpu_sr = CPU_SR_Save();                  */
;                               :
;                               :
;                           CPU_CRITICAL_EXIT();                /* CPU_SR_Restore(cpu_sr);                  */
;                               :
;                       }
;********************************************************************************************************


OS_CPU_SR_Save
    MRS     R0, PRIMASK                                         ; Set prio int mask to mask all (except faults)
    CPSID   I
    BX      LR

OS_CPU_SR_Restore
    MSR     PRIMASK, R0
    BX      LR

;********************************************************************************************************
;                                         HANDLE PendSV EXCEPTION
;                                     void OS_CPU_PendSVHandler(void)
;
; Note(s) : 1) PendSV is used to cause a context switch.  This is a recommended method for performing
;              context switches with Cortex-M3.  This is because the Cortex-M3 auto-saves half of the
;              processor context on any exception, and restores same on return from exception.  So only
;              saving of R4-R11 is required and fixing up the stack pointers.  Using the PendSV exception
;              this way means that context saving and restoring is identical whether it is initiated from
;              a thread or occurs due to an interrupt or exception.
;
;           2) Pseudo-code is:
;              a) Get the process SP, if 0 then skip (goto d) the saving part (first context switch);
;              b) Save remaining regs r4-r11 on process stack;
;              c) Save the process SP in its TCB, OSTCBCur->OSTCBStkPtr = SP;
;              d) Call OSTaskSwHook();
;              e) Get current high priority, OSPrioCur = OSPrioHighRdy;
;              f) Get current ready thread TCB, OSTCBCur = OSTCBHighRdy;
;              g) Get new process SP from TCB, SP = OSTCBHighRdy->OSTCBStkPtr;
;              h) Restore R4-R11 from new process stack;
;              i) Perform exception return which will restore remaining context.
;
;           3) On entry into PendSV handler:
;              a) The following have been saved on the process stack (by processor):
;                 xPSR, PC, LR, R12, R0-R3
;              b) Processor mode is switched to Handler mode (from Thread mode)
;              c) Stack is Main stack (switched from Process stack)
;              d) OSTCBCur      points to the OS_TCB of the task to suspend
;                 OSTCBHighRdy  points to the OS_TCB of the task to resume
;
;           4) Since PendSV is set to lowest priority in the system (by OSStartHighRdy() above), we
;              know that it will only be run when no other exception or interrupt is active, and
;              therefore safe to assume that context being switched out was using the process stack (PSP).
;********************************************************************************************************

OS_CPU_PendSVHandler
    CPSID   I                                                   ; Prevent interruption during context switch
    MRS     R0, PSP                                             ; PSP is process stack pointer

    CMP     R0, #0
    BEQ     OS_CPU_PendSVHandler_nosave                         ; Skip register save the first time

    SUBS    R0, R0, #0x20                                       ; Save remaining regs r4-11 on process stack
    STM     R0! ,   {R4-R7}
    MOV     R4  ,   R8
    MOV     R5  ,   R9
    MOV     R6  ,   R10
    MOV     R7  ,   R11
    STM     R0! ,   {R4-R7}
    SUBS    R0, R0, #0x20
    

    LDR     R1, =OSTCBCur                                       ; OSTCBCur->OSTCBStkPtr = SP;
    LDR     R1, [R1]
    STR     R0, [R1]                                            ; R0 is SP of process being switched out

                                                                ; At this point, entire context of process has been saved
OS_CPU_PendSVHandler_nosave
    PUSH    {R14}                                               ; Save LR exc_return value
    LDR     R0, =OSTaskSwHook                                   ; OSTaskSwHook();
    BLX     R0
    POP     {R0}  
    MOV     R14 , R0


    LDR     R0, =OSPrioCur                                      ; OSPrioCur = OSPrioHighRdy;
    LDR     R1, =OSPrioHighRdy
    LDRB    R2, [R1]
    STRB    R2, [R0]

    LDR     R0, =OSTCBCur                                       ; OSTCBCur  = OSTCBHighRdy;
    LDR     R1, =OSTCBHighRdy
    LDR     R2, [R1]
    STR     R2, [R0]											
    LDR     R0, [R2]                                            ; R0 is new process SP; SP = OSTCBHighRdy->OSTCBStkPtr;
    

	
	ADDS    R0  ,   R0, #0x10									; Move sp to frame to r8 ,restore r8~r11
	LDM     R0! ,   {R4-R7}
	MOV     R8  ,   R4
    MOV     R9  ,   R5
    MOV     R10 ,   R6
    MOV     R11 ,   R7
	
    SUBS    R0,    R0, #0x20									; Move sp to frame to r4 ,restore r4~r7
	LDM     R0! , {R4-R7}
	ADDS    R0  ,  R0, #0x10									; r0 = sp top

    MSR     PSP, R0                                             ; Load PSP with new process SP
    
   
  
    
	MOV      R0 , LR 
    MOVS     R1 , #0x04 
	ORRS     R0,  R0, R1
	MOV      LR , R0 
	
    CPSIE   I
    BX      LR                                                  ; Exception return will restore remaining context


;*********************************************************************************************************
;** Function name:      IntDisAll
;** Descriptions:	Disable all interrupts from the interrupt controller  关闭中断控制器的所有中断
;** Input parameters:	None 无
;** Output parameters:	None 无
;** Returned value:	None 无
;*********************************************************************************************************
intDisAll
    CPSID   I
    BX      LR
	
;*********************************************************************************************************
;** Function name:      intEnAll
;** Descriptions:	Enable all interrupts from the interrupt controller  使能中断控制器的所有中断
;** Input parameters:	None 无
;** Output parameters:	None 无
;** Returned value:	None 无
;*********************************************************************************************************
intEnAll
    CPSIE   I
    BX      LR
	NOP
	
    END
;*********************************************************************************************************
;  END FILE                                               
;*********************************************************************************************************
