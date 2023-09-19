;******************** (C) COPYRIGHT 2015 Gree Microelectronics ********************
;* File Name          : startup_gma0164qp64.s
;* Author             : Wen Liu
;* Version            : V1.0.0
;* Date               : 2015/08/20
;* Description        : GMA0164QP64 startup file for arm-cc.
;*                      This module do following:
;*                      - Set the initial SP & PC
;*                      - Build vector table with external interrupt handlers
;*                      - Branches to main routine
;*******************************************************************************


; Amount of memory (in bytes) allocated for Stack
; Tailor this value to your application needs
; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
                EXPORT  __Vectors_End
                EXPORT  __Vectors_Size

__Vectors       DCD     __initial_sp   ; Top of Stack
                DCD     Reset_Handler  ; Reset Handler
                DCD     INT02_Handler  ; 
                DCD     INT03_Handler  ;                                
__Vectors_End

__Vectors_Size 	EQU     __Vectors_End - __Vectors

                IF      :LNOT::DEF:RAM_TARGET
                AREA    |.ARM.__at_0x400|, CODE, READONLY                
                DCB     0xFF,       0xFF,       0xFF,       0xFF
                ENDIF
                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]                
                
                IMPORT  __main                
                LDR     R0, =__main                
				BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

Default_Handler PROC
                ;IMPORT  Default_INTHanlder
                EXPORT  INT02_Handler     [WEAK]
                EXPORT  INT03_Handler     [WEAK]
                ;EXPORT  INT04_Handler     [WEAK]
                ;EXPORT  INT05_Handler     [WEAK]
                ;EXPORT  INT06_Handler     [WEAK]
                ;EXPORT  INT07_Handler     [WEAK]
                ;EXPORT  INT08_Handler     [WEAK]
                ;EXPORT  INT09_Handler     [WEAK]
                ;EXPORT  INT10_Handler     [WEAK]
                ;EXPORT  INT11_Handler     [WEAK]
                ;EXPORT  INT12_Handler     [WEAK]
                ;EXPORT  INT13_Handler     [WEAK]
                ;EXPORT  INT14_Handler     [WEAK]
                ;EXPORT  INT15_Handler     [WEAK]
                ;EXPORT  INT16_Handler     [WEAK]
                ;EXPORT  INT17_Handler     [WEAK]
                ;EXPORT  INT18_Handler     [WEAK]
                ;EXPORT  INT19_Handler     [WEAK]
                ;EXPORT  INT20_Handler     [WEAK]
                ;EXPORT  INT21_Handler     [WEAK]
                ;EXPORT  INT22_Handler     [WEAK]
                ;EXPORT  INT23_Handler     [WEAK]
                ;EXPORT  INT24_Handler     [WEAK]
                ;EXPORT  INT25_Handler     [WEAK]
                ;EXPORT  INT26_Handler     [WEAK]
                ;EXPORT  INT27_Handler     [WEAK]
                ;EXPORT  INT28_Handler     [WEAK]
                ;EXPORT  INT29_Handler     [WEAK]
                ;EXPORT  INT30_Handler     [WEAK]
                ;EXPORT  INT31_Handler     [WEAK]
                ;EXPORT  INT32_Handler     [WEAK]
                ;EXPORT  INT33_Handler     [WEAK]
                ;EXPORT  INT34_Handler     [WEAK]
                ;EXPORT  INT35_Handler     [WEAK]
                ;EXPORT  INT36_Handler     [WEAK]
                ;EXPORT  INT37_Handler     [WEAK]
                ;EXPORT  INT38_Handler     [WEAK]
                ;EXPORT  INT39_Handler     [WEAK]
                ;EXPORT  INT40_Handler     [WEAK]
                ;EXPORT  INT41_Handler     [WEAK]
                ;EXPORT  INT42_Handler     [WEAK]
                ;EXPORT  INT43_Handler     [WEAK]
                ;EXPORT  INT44_Handler     [WEAK]
                ;EXPORT  INT45_Handler     [WEAK]
                ;EXPORT  INT46_Handler     [WEAK]
                ;EXPORT  INT47_Handler     [WEAK]
INT02_Handler
INT03_Handler
;INT04_Handler
;INT05_Handler
;INT06_Handler
;INT07_Handler
;INT08_Handler
;INT09_Handler
;INT10_Handler
;INT11_Handler
;INT12_Handler
;INT13_Handler
;INT14_Handler
;INT15_Handler
;INT16_Handler
;INT17_Handler
;INT18_Handler
;INT19_Handler
;INT20_Handler
;INT21_Handler
;INT22_Handler
;INT23_Handler
;INT24_Handler
;INT25_Handler
;INT26_Handler
;INT27_Handler
;INT28_Handler
;INT29_Handler
;INT30_Handler
;INT31_Handler
;INT32_Handler
;INT33_Handler
;INT34_Handler
;INT35_Handler
;INT36_Handler
;INT37_Handler
;INT38_Handler
;INT39_Handler
;INT40_Handler
;INT41_Handler
;INT42_Handler
;INT43_Handler
;INT44_Handler
;INT45_Handler
;INT46_Handler
;INT47_Handler
                B        .
                ;LDR     R0, =Default_INTHanlder
                ;BX      R0

                ENDP


                ALIGN

;*******************************************************************************
; User Stack and Heap initialization
;*******************************************************************************
                 IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, = (Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
;*****END OF FILE*****