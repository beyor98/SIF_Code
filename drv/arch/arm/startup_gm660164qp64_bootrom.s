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

;GM6601_CHIPSET  	EQU     1
;GM6601B_FPGA    	EQU     1
GM6601B_CHIPSET 	EQU     1
;GM6611B_CHIPSET  	EQU     1

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
                DCD     INT04_Handler  ;
                DCD     INT05_Handler  ;
                DCD     INT06_Handler  ;
                DCD     INT07_Handler  ;
                DCD     INT08_Handler  ;
                DCD     INT09_Handler  ;
                DCD     INT10_Handler  ;
                DCD     INT11_Handler  ;
                DCD     INT12_Handler  ;
                DCD     INT13_Handler  ;
                DCD     INT14_Handler  ;
                DCD     INT15_Handler  ;

                ; External Interrupts
                DCD     INT16_Handler  ; 
                DCD     LVD_Handler  ; 
                ;DCD     INT18_Handler  ; 
                ;DCD     INT19_Handler  ; 
                ;DCD     INT20_Handler  ; 
                ;DCD     INT21_Handler  ; 
                ;DCD     INT22_Handler  ; 
                ;DCD     INT23_Handler  ; 
                ;DCD     INT24_Handler  ; 
                ;DCD     INT25_Handler  ; 
                ;DCD     INT26_Handler  ; 
                ;DCD     INT27_Handler  ; 
                ;DCD     INT28_Handler  ; 
                ;DCD     INT29_Handler  ; 
                ;DCD     INT30_Handler  ; 
                ;DCD     INT31_Handler  ; 
                ;DCD     INT32_Handler  ; 
                ;DCD     INT33_Handler  ; 
                ;DCD     INT34_Handler  ; 
                ;DCD     INT35_Handler  ; 
                ;DCD     INT36_Handler  ; 
                ;DCD     INT37_Handler  ; 
                ;DCD     INT38_Handler  ; 
                ;DCD     INT39_Handler  ; 
                ;DCD     INT40_Handler  ; 
                ;DCD     INT41_Handler  ; 
                ;DCD     INT42_Handler  ; 
                ;DCD     INT43_Handler  ; 
                ;DCD     INT44_Handler  ; 
                ;DCD     INT45_Handler  ; 
                ;DCD     INT46_Handler  ; 
                ;DCD     INT47_Handler  ; 
__Vectors_End

__Vectors_Size 	EQU     __Vectors_End - __Vectors
                								
                IF      :LNOT::DEF:RAM_TARGET
					IF      :DEF:GM6601_CHIPSET
						;for GM6601 shuttle version only
						AREA    |.ARM.__at_0x600|, CODE, READONLY                
						DCB     0xFF,       0x03,       0xFF,       0xFF                
						DCB     0xFF,       0xFF,       0xFF,       0xFF
                        ;reserved for SWD password if code bigger then 0x700						
						;AREA    |.ARM.__at_0x700|, CODE, READONLY                
						;DCB     0xFF,       0xFF,       0x01,       0x01                
						;DCB     0x00,       0x01,       0x01,       0x00 
					ENDIF
					IF      :DEF:GM6601B_CHIPSET
						;for GM6601B CHIPSET
						AREA   |.ARM.__at_0x600|, CODE, READONLY
						;DCB     0xFF,       0xFE,       0x00,       0xFF        ;0x1FFFF600, boot config, boot from info page
                        ;DCB     0x88,       0x20,       0x88,       0x45        ;0x1FFFF604, analog trimming, need match each chipset
                        ;DCB     0x00,       0xE8,       0x03,       0x00        ;0x1FFFF608, watchdog
         				;DCB     0x75,       0x43,       0x65,       0x53		;0x1FFFF60C, security identify 
						
						DCB     0xFF,       0xFF,       0xFF,       0xFF        ;0x1FFFF600, boot config, boot from info page
                        DCB     0xFF,       0xFF,       0xFF,       0xFF        ;0x1FFFF604, analog trimming, need match each chipset
                        DCB     0xFF,       0xFF,       0xFF,       0xFF        ;0x1FFFF608, watchdog
         				DCB     0xFF,       0xFF,       0xFF,       0xFF		;0x1FFFF60C, security identify 
						DCB     0xFF,       0xFF,       0xFF,       0xFF        ;0x1FFFF610, chip ID[31:0] 
						DCB     0xFF,       0xFF,       0xFF,       0xFF        ;0x1FFFF614, chip ID[63:32] 
						DCB     0xFF,       0xFF,       0xFF,       0xFF        ;0x1FFFF618, chip ID[95:64] 
						DCB     0xFF,       0xFF,       0xFF,       0xFF        ;0x1FFFF61c, chip ID[127:96] 
						
						;AREA   |.ARM.__at_0x700|, CODE, READONLY
						;DCB     0xFF,       0xFF,       0xFF,       0xFF 
						;DCB     0xFF,       0xFF,       0xFF,       0xFF 
					ENDIF
					IF      :DEF:GM6611B_CHIPSET
						;AREA   |.ARM.__at_0x3FFFC|, CODE, READONLY
						;DCB    0xFF,        0xFF,       0xFF,       0xFF        ;bootrom protect	
                    ENDIF
                ENDIF
								
                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
				;for main flash only, switch main flash(0x10000000) to 0x00000000
                ;MOVS  R2, #0x1
                ;MOVS  R0, R2, LSL#30 
                ;ADDS  R0, #0x14
                ;MOVS  R2, #0x01
                ;STR   R2, [R0, #0]
                ;DSB
                ;ISB                
                ;BL      label1
;label1
                ;IMPORT  SystemInit
			    ;test
				;LDR R0, =0x20000800
				;LDR R1, =0x12345670
				;STR R1, [R0, #0x00]
				
				;close SWD if info protect
				LDR R0, =0x40001000
				LDR R0, [R0,#0x18]
				MOVS R1, #8
				ANDS R0, R0, R1
				LSRS R0, R0, #3
				CMP R0, #1
				BEQ CLOSE_SWD
				B LABEL
CLOSE_SWD				
                IF      :DEF:GM6601B_CHIPSET
	            MOVS R1, #0x5a
				LDR  R2, =0x40000120
				STRB R1, [R2,#8]
                ENDIF

					
LABEL				
                IMPORT  __main
                ;LDR     R0, =SystemInit
                ;BLX     R0
				
				IF      :DEF:GM6601B_CHIPSET
				;open Hardware watchdog
				LDR R1, =0x1FFFF608
				LDR R3, =0xFFFFFFFF
				LDR R2, [R1,#0]
				CMP R2, R3
				BNE RUN_MAIN 
				LDR R4, =0x40001000
				LDR R6, [R4,#8]
FLASH_ST				
				LSLS R6, R6, #31
				BNE FLASH_ST
				STR R1, [R4,#0xc]
				LDR R5, =0x0003E800   ;500ms
				STR R5, [R4,#0x10]
				MOVS R1, #1
				STR R1, [R4,#4]
				ENDIF
RUN_MAIN					
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

Default_Handler PROC
                ;IMPORT  Default_INTHanlder
                EXPORT  INT02_Handler     [WEAK]
                EXPORT  INT03_Handler     [WEAK]
                EXPORT  INT04_Handler     [WEAK]
                EXPORT  INT05_Handler     [WEAK]
                EXPORT  INT06_Handler     [WEAK]
                EXPORT  INT07_Handler     [WEAK]
                EXPORT  INT08_Handler     [WEAK]
                EXPORT  INT09_Handler     [WEAK]
                EXPORT  INT10_Handler     [WEAK]
                EXPORT  INT11_Handler     [WEAK]
                EXPORT  INT12_Handler     [WEAK]
                EXPORT  INT13_Handler     [WEAK]
                EXPORT  INT14_Handler     [WEAK]
                EXPORT  INT15_Handler     [WEAK]
                EXPORT  INT16_Handler     [WEAK]
                EXPORT  LVD_Handler       [WEAK]
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
INT04_Handler
INT05_Handler
INT06_Handler
INT07_Handler
INT08_Handler
INT09_Handler
INT10_Handler
INT11_Handler
INT12_Handler
INT13_Handler
INT14_Handler
INT15_Handler
INT16_Handler
LVD_Handler
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