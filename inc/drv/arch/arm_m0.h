/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 *
 * @file arm_m0.h
 *
 * @version 1.0.0
 *
 * @date 2015/08/21
 *
 * @contents Hal of cpu specific function.
 *
 * @history
 *	Date				    By				Comments					Ver.
 *	=========		=========		 =========			 =========
 *	2015/08/21		Wen Liu			 Create					 1.0.0
 **********************************************************************/

#ifndef _ARM_M0_H
#define _ARM_M0_H

/* Macro to enable/disable all interrupts. */

#ifndef KEIL
#define enable_interrupt __ASM(" CPSIE i")
#else
#define enable_interrupt  __enable_irq()
#endif

#ifndef KEIL
#define disable_interrupt __ASM(" CPSID i")
#else
#define disable_interrupt __disable_irq()
#endif


/* Macro to enable/disable specific interrupt. */

#define disable_irq(irq)             NVIC_DisableIRQ(irq)
#define enable_irq(irq)              NVIC_EnableIRQ(irq)
#define set_irq_priority(irq, prio)  NVIC_SetPriority(irq, prio)

#endif	/* _ARM_M0_H */

