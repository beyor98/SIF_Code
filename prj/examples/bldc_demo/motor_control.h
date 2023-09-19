/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          motor_control.h
 *
 * @author        mark.lee
 *
 * @version       1.0.0
 * 
 * @date          2016/11/23 
 *    
 * @brief         This header file provides the set of functions for 
 *                Motor Control library 
 * 
 * @note
 *    2016/11/23, mark.lee, V1.0.0
 *        Initial version. 
 */
#ifndef _MOTOR_CONTROL_H_
#define _MOTOR_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <drv/dev/clock.h>
#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <drv/dev/adc.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAX_DUTY_CYCLE 			1499    /* Maximum value of duty cycle */  	
#define MIN_DUTY_CYCLE 			100      /* Minimum value of duty cycle */	

/**
 * List of motor status.
 */
typedef enum 
{
    IDLE,
    ALIGNMENT,    
    STARTUP, 
    RUN,
    STOP
} Motor_Status_T;

/** 
  * @brief  Six Step parameters  
  */
typedef struct
{
    uint8_t stepNumber;          /**< Step number variable for SixStep algorithm */
    uint8_t demagnCnt;           /**< Demagnetization counter */
    uint8_t counter;             /**< The number of pulse width change */
    uint8_t speedLoop;           /**< Speed loop control flag */
    uint16_t dutyCycle;          /**< Pulse value for SixStep algorithm */   
    uint32_t PWMTicks;           /**< Number of tick */
    uint32_t bemf;               /**< The voltage for BEMF detection */
    Motor_Status_T status;       /**< Status variable for SixStep algorithm */
} MC_Sixstep_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/
/**
 * @brief  Set the peripherals (TIMx, GPIO etc.) for each step
 *
 * @param stepNumber step number selected
 * @param dutyCycle   duty cycle value
 * @retval None
 */
void MC_SixStep_Table(uint8_t stepNumber, uint16_t dutyCycle);

/**
 * @brief Initialitation function for SixStep library
 *
 * @param pCfg Pointer to MC_Sixstep_T structure
 * 
 * @retval None
 */
void MC_SixStep_Init(MC_Sixstep_T *pCfg);

/**
 * @brief Start the Motor
 *
 * @param pCfg Pointer to MC_Sixstep_T structure
 * 
 * @retval None
 */
void MC_StartMotor(MC_Sixstep_T *pCfg);

/**
 * @brief Stop the Motor
 *
 * @param pCfg Pointer to MC_Sixstep_T structure
 * 
 * @retval None
 */
void MC_StopMotor(MC_Sixstep_T *pCfg);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MOTOR_CONTROL_H_*/

