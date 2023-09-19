/*****************************************************************************
 * GM66xx startup code for use with Gree IDE
 * Version : 0.1
 * Author  : wizephen.wang
 *****************************************************************************/

#if defined (DEBUG)
#pragma GCC push_options
#pragma GCC optimize ("Og")
#endif

#if defined (__cplusplus)
#ifdef __REDLIB__
#error Redlib does not support C++
#else
	
/*****************************************************************************
 *
 * The entry point for the C++ library startup
 *
 *****************************************************************************/
extern "C" {
    extern void __libc_init_array(void);
}
#endif
#endif

#define WEAK __attribute__ ((weak))
#define WEAK_AV __attribute__ ((weak, section(".after_vectors")))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

/*****************************************************************************/
#if defined (__cplusplus)
extern "C" {
#endif

/*****************************************************************************
 * Flash Configuration block : 16-byte flash configuration field that stores
 * default protection settings (loaded on reset) and security information that
 * allows the MCU to restrict access to the Flash Memory module.
 * Placed at address 0x400 by the linker script.
 *****************************************************************************/

__attribute__ ((used,section(".FlashConfig"))) const struct {
    unsigned int word1;
    unsigned int word2;
    unsigned int word3;
    unsigned int word4;
#if defined (CHIP_GM6611)

#endif
} Flash_Config = {0xFFFFFFFF, 
                  0xFFFFFFFF, 
				  0xFFFFFFFF, 
				  0xFFFFFFFF
#if defined (CHIP_GM6611)

#endif				  
				  };

/*****************************************************************************
 * Declaration of external SystemInit function
 *****************************************************************************/
#if defined (__USE_CMSIS)
extern void SystemInit(void);
#endif

/*****************************************************************************
 * Forward declaration of the core exception handlers.
 * When the application defines a handler (with the same name), this will
 * automatically take precedence over these weak definitions
*****************************************************************************/
     void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void IntDefaultHandler(void);

/*****************************************************************************
 * Forward declaration of the application IRQ handlers. When the application
 * defines a handler (with the same name), this will automatically take
 * precedence over weak definitions below
*****************************************************************************/

WEAK void INT15_Handler(void);

WEAK void INT16_Handler(void);
WEAK void INT17_Handler(void);
WEAK void INT18_Handler(void);
WEAK void INT19_Handler(void);
WEAK void INT20_Handler(void);
WEAK void INT21_Handler(void);
WEAK void INT22_Handler(void);
WEAK void INT23_Handler(void);
WEAK void INT24_Handler(void);
WEAK void INT25_Handler(void);
WEAK void INT26_Handler(void);
WEAK void INT27_Handler(void);
WEAK void INT28_Handler(void);
WEAK void INT29_Handler(void);
WEAK void INT30_Handler(void);
WEAK void INT31_Handler(void);
WEAK void INT32_Handler(void);
WEAK void INT33_Handler(void);
WEAK void INT34_Handler(void);
WEAK void INT35_Handler(void);
WEAK void INT36_Handler(void);
WEAK void INT37_Handler(void);
WEAK void INT38_Handler(void);
WEAK void INT39_Handler(void);
WEAK void INT40_Handler(void);
WEAK void INT41_Handler(void);
WEAK void INT42_Handler(void);
WEAK void INT43_Handler(void);
WEAK void INT44_Handler(void);
WEAK void INT45_Handler(void);
WEAK void INT46_Handler(void);
WEAK void INT47_Handler(void);

/*****************************************************************************
 * The entry point for the application.
 * __main() is the entry point for Redlib based applications
 * main() is the entry point for Newlib based applications
 *****************************************************************************/
#if defined (__REDLIB__)
extern void __main(void);
#endif
extern int main(void);

/*****************************************************************************
 * External declaration for the pointer to the stack top from the Linker Script
 *****************************************************************************/
extern void _vStackTop(void);

/*****************************************************************************/
#if defined (__cplusplus)
}
#endif

/*****************************************************************************
 * The vector table.
 * This relies on the linker script to place at correct location in memory.
*****************************************************************************/
extern void (* const g_pfnVectors[])(void);
extern void * __Vectors __attribute__ ((alias ("g_pfnVectors")));

__attribute__ ((used, section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    // Core Level - CM0
    &_vStackTop,                       /* The initial stack pointer */
    ResetISR,                          /* The reset handler */
    NMI_Handler,                       /* The NMI handler */
    HardFault_Handler,                 /* The hard fault handler */
    0,                                 /* Reserved */
    0,                                 /* Reserved */
    0,                                 /* Reserved */
    0,                                 /* Reserved */
    0,                                 /* Reserved */
    0,                                 /* Reserved */
    0,                                 /* Reserved */
    SVC_Handler,                       /* SVCall handler */
    0,                     
    0,                     
    PendSV_Handler,                    /* The PendSV handler */
    INT15_Handler,                     /* The SysTick handler */

    /*interrupts for CPU peripherals*/
    INT16_Handler,   
    INT17_Handler,   
    INT18_Handler,   
    INT19_Handler,   
    INT20_Handler,   
    INT21_Handler,   
	INT22_Handler,   
	INT23_Handler,   
	INT24_Handler,   
	INT25_Handler,   
    INT26_Handler,   
    INT27_Handler,   
	INT28_Handler,   
	INT29_Handler,   
	INT30_Handler,   
    INT31_Handler,   
	INT32_Handler,   
	INT33_Handler,   
    INT34_Handler,   
	INT35_Handler,   
	INT36_Handler,   
    INT37_Handler,   
    INT38_Handler,   
    INT39_Handler,   
    INT40_Handler,   
	INT41_Handler,   
	INT42_Handler,   
	INT43_Handler,   
	INT44_Handler,   
	INT45_Handler,   
	INT46_Handler,
	INT47_Handler,
}; /* End of g_pfnVectors */

/******************************************************************************
 * Functions to carry out the initialization of RW and BSS data sections. These
 * are written as separate functions rather than being inlined within the
 * ResetISR() function in order to cope with MCUs with multiple banks of
 * memory.
 ******************************************************************************/
__attribute__ ((section(".after_vectors.init_data")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int *pulSrc = (unsigned int*) romstart;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors.init_bss")))
void bss_init(unsigned int start, unsigned int len) {
	unsigned int *pulDest = (unsigned int*) start;
	unsigned int loop;
	for (loop = 0; loop < len; loop = loop + 4)
		*pulDest++ = 0;
}

/*****************************************************************************
 * The following symbols are constructs generated by the linker, indicating
 * the location of various points in the "Global Section Table". This table is
 * created by the linker via the Code Red managed linker script mechanism. It
 * contains the load address, execution address and length of each RW data
 * section and the execution and length of each BSS (zero initialized) section.
 ******************************************************************************/
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;

/*****************************************************************************
 * Reset entry point for your code.
 * Sets up a simple runtime environment and initializes the C/C++
 * library.
*****************************************************************************/
__attribute__ ((section(".after_vectors.reset")))
void ResetISR(void) {

    /*Disable interrupts*/
    __asm volatile ("cpsid i");
	
	/*System memory remap*/
#if defined (CHIP_GM6611)
    /*remap 0x00000000 to 0x10000000 main flash memory*/ 
	*((volatile unsigned int *)0x40000014) = 0x1ACC0003u;
    __asm volatile ("isb 0xF":::"memory");
	__asm volatile ("dsb 0xF":::"memory");
#endif

#if defined (__USE_CMSIS)
    SystemInit();
#else  
    /*maybe disable watchdog here*/	
#endif 
    
    /*Copy the data sections from flash to SRAM*/
	unsigned int LoadAddr, ExeAddr, SectionLen;
	unsigned int *SectionTableAddr;

	/*Load base address of Global Section Table*/
	SectionTableAddr = &__data_section_table;

    /*Copy the data sections from flash to SRAM.*/
	while (SectionTableAddr < &__data_section_table_end) {
		LoadAddr = *SectionTableAddr++;
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		data_init(LoadAddr, ExeAddr, SectionLen);
	}

	/*At this point, SectionTableAddr = &__bss_section_table;
	 *Zero fill the bss segment
	 */
	while (SectionTableAddr < &__bss_section_table_end) {
		ExeAddr = *SectionTableAddr++;
		SectionLen = *SectionTableAddr++;
		bss_init(ExeAddr, SectionLen);
	}

#if !defined (__USE_CMSIS)

     /*usually, VTOR register can be setup here for CPUs other than Coretex-M0*/
	 
#endif 

#if defined (__cplusplus)

    /* Call C++ library initialisation*/   
    __libc_init_array();
#endif
    /*Reenable interrupts*/
    __asm volatile ("cpsie i");

#if defined (__REDLIB__)
	/* Call the Redlib library, which in turn calls main() */
	__main();
#else
	main();
#endif

	/* main() shouldn't return, but if it does, we'll just enter an infinite loop */
	while (1) {
		;
	}
}

/*****************************************************************************
 *Default core exception handlers. Override the ones here by defining your own
 *handler routines in your application code.
 *****************************************************************************/
WEAK_AV void NMI_Handler(void)
{ while(1) {}
}

WEAK_AV void HardFault_Handler(void)
{ while(1) {}
}

WEAK_AV void SVC_Handler(void)
{ while(1) {}
}

WEAK_AV void PendSV_Handler(void)
{ while(1) {}
}

#if 0
WEAK_AV void SysTick_Handler(void)
{ while(1) {}
}
#endif

/*****************************************************************************
 * Processor ends up here if an unexpected interrupt occurs or a specific
 * handler is not present in the application code.
 *****************************************************************************/
WEAK_AV void IntDefaultHandler(void)
{ while(1) {}
}

WEAK_AV void INT15_Handler(void)
{ while(1) {}
}

WEAK_AV void INT16_Handler(void)
{ while(1) {}
}

WEAK_AV void INT17_Handler(void)
{ while(1) {}
}

WEAK_AV void INT18_Handler(void)
{ while(1) {}
}

WEAK_AV void INT19_Handler(void)
{ while(1) {}
}

WEAK_AV void INT20_Handler(void)
{ while(1) {}
}

WEAK_AV void INT21_Handler(void)
{ while(1) {}
}

WEAK_AV void INT22_Handler(void)
{ while(1) {}
}

WEAK_AV void INT23_Handler(void)
{ while(1) {}
}

WEAK_AV void INT24_Handler(void)
{ while(1) {}
}

WEAK_AV void INT25_Handler(void)
{ while(1) {}
}

WEAK_AV void INT26_Handler(void)
{ while(1) {}
}

WEAK_AV void INT27_Handler(void)
{ while(1) {}
}

WEAK_AV void INT28_Handler(void)
{ while(1) {}
}

WEAK_AV void INT29_Handler(void)
{ while(1) {}
}

WEAK_AV void INT30_Handler(void)
{ while(1) {}
}

WEAK_AV void INT31_Handler(void)
{ while(1) {}
}

WEAK_AV void INT32_Handler(void)
{ while(1) {}
}

WEAK_AV void INT33_Handler(void)
{ while(1) {}
}

WEAK_AV void INT34_Handler(void)
{ while(1) {}
}

WEAK_AV void INT35_Handler(void)
{ while(1) {}
}

WEAK_AV void INT36_Handler(void)
{ while(1) {}
}

WEAK_AV void INT37_Handler(void)
{ while(1) {}
}

WEAK_AV void INT38_Handler(void)
{ while(1) {}
}

WEAK_AV void INT39_Handler(void)
{ while(1) {}
}

WEAK_AV void INT40_Handler(void)
{ while(1) {}
}

WEAK_AV void INT41_Handler(void)
{ while(1) {}
}

WEAK_AV void INT42_Handler(void)
{ while(1) {}
}

WEAK_AV void INT43_Handler(void)
{ while(1) {}
}

WEAK_AV void INT44_Handler(void)
{ while(1) {}
}

WEAK_AV void INT45_Handler(void)
{ while(1) {}
}

WEAK_AV void INT46_Handler(void)
{ while(1) {}
}

WEAK_AV void INT47_Handler(void)
{ while(1) {}
}



/*****************************************************************************
 * Default application exception handlers. Override the ones here by defining
 * your own handler routines in your application code. These routines call
 * driver exception handlers or IntDefaultHandler() if no driver exception
 * handler is included.
 *****************************************************************************/

/*****************************************************************************/
#if defined (DEBUG)
#pragma GCC pop_options
#endif
