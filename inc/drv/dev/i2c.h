/**
*
* Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
*
* @file          i2c.h
*
* @author        wizephen.wang
*
* @version       1.0.0
*
* @date          2016/07/05
*
* @brief         Header file of i2c driver.
*
* @note
*    2016/07/05, wizephen.wang, V1.0.0
*        Initial version.
*/

#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <drv/dev/dma.h>




/* Declaration of I2C handle structure */
struct I2C_Handle;

/** Prototype of the user callback function */
typedef void (*I2C_Callback_T)(struct I2C_Handle *hI2C, uint8_t event, unsigned long param);

/**
 * I2C event type
 */
typedef enum
{
    I2C_EVENT_TRANSFER_DONE     = 0x00,  /**< I2C transaction is completed */
    I2C_EVENT_TX_FIFO_OVERFLOW  = 0x01,  /**< I2C TxFIFO overflows */
    I2C_EVENT_RX_FIFO_OVERFLOW  = 0x02,  /**< I2C RxFIFO overflows */
    I2C_EVENT_RX_FIFO_UNDERFLOW = 0x03,  /**< I2C RxFIFO underlows */
    I2C_EVENT_ARB_LOST          = 0x04,  /**< I2C arbitration losts */
    I2C_EVENT_TERMINATED        = 0x08,  /**< I2C transaction terminated unexpectedly*/
    I2C_EVENT_DMAERROR          = 0x10,  /**< I2C DMA error*/
    I2C_EVNET_TRANSFER_ERROR    = 0xFF   /**< I2C transaction error */
} I2C_Event_T;

/**
 * I2C error code type
 */
typedef enum
{
    I2C_ERRCODE_NOERROR               = 0x00,  /**< No error */
    I2C_ERRCODE_ARBITRATION_LOST      = 0x01,  /**< I2C arbitration-lost error */
    I2C_ERRCODE_TERMINATED_UNEXPECTED = 0x02,  /**< I2C terminates unexpectedly */
    I2C_ERRCODE_TIMEOUT               = 0x04,  /**< Timeout */
    I2C_ERRCODE_DMAERROR              = 0x08,  /**< Error caused by DMA */
    I2C_ERRCODE_NOTREADY              = 0x10,  /**< I2C is not ready */
    I2C_ERRCODE_TRANSACERR            = 0x20,  /**< I2C transcation error */
} I2C_ErrorCode_T;


/**
 * I2C address mode
 */
typedef enum
{
    I2C_7BIT_MODE  = 0x00,  /**< 7-Bit address mode */
    I2C_10BIT_MODE = 0x01   /**< 10-Bit address mode */
} I2C_AddrMode_T;

/**
 * I2C speed mode
 */
typedef enum
{
    I2C_STANDARD_MODE = 0x00,  /**< Standard speed mode */
    I2C_FAST_MODE     = 0x01,  /**< Fast speed mode */
} I2C_SpeedMode_T;

/**
 * I2C Tx mode
 */
typedef enum
{
    I2C_TARGET_ADDRESS_MODE = 0x00,  /**< Target address mode */
    I2C_GENERAL_CALL_MODE   = 0x02,  /**< General call mode */
    I2C_START_BYTE_MODE     = 0x03   /**< Start byte mode */
} I2C_MasterTxMode_T;

/**
 * I2C working mode
 */
typedef enum
{
    I2C_MASTER_MODE = 0x00,  /**< Master mode */
    I2C_SLAVE_MODE  = 0x01   /**< Slave mode */
} I2C_WorkMode_T;

/**
 * I2C command type
 */
typedef enum
{
    I2C_DATA_CMD_NULL    = 0x00,  /**< no sense */
    I2C_DATA_CMD_RESTART = 0x01,  /**< Restart command */
    I2C_DATA_CMD_STOP    = 0x02   /**< Stop command */
} I2C_DataCmdType_T;


/**
 * I2C read/write type
 */
typedef enum
{
    I2C_DATACMD_READ     = 0x00,  /**< I2C read */
    I2C_DATACMD_WRITE    = 0x01,  /**< I2C write */
} I2C_RdWrType_T;


/**
 * I2C peripheral state
 */
typedef enum
{
    I2C_STATE_IDLE       = 0,       /**< I2C is in idle state */
    I2C_STATE_READY,                /**< I2C is ready to use */
    I2C_STATE_MASTER_TX,            /**< Master transfers data to slave */
    I2C_STATE_MASTER_RX,            /**< Master receives data to slave */
    I2C_STATE_MASTER_RX_RESTART,    /**< Master receives data to slave, restart mode */
    I2C_STATE_SLAVE_TX,             /**< Slave transfers data to master */
    I2C_STATE_SLAVE_RX,             /**< Slave receives data to master */
    I2C_STATE_SLAVE_TX_AFTER_RDREQ  /**< Slave transfers data to master after receiving a read-request */
} I2C_State_T;

/**
 * I2C configuraion structure
 */
typedef struct
{
    I2C_AddrMode_T     addrMode;      /**< I2C address mode */
    I2C_WorkMode_T     workMode;      /**< I2C working mode */
    I2C_SpeedMode_T    speedMode;     /**< I2C speed mode */
    I2C_MasterTxMode_T masterTxMode;  /**< I2C Tx mode */
    I2C_DataCmdType_T  restartMode;      /**< I2C restart mode */
    uint8_t            enableDMA;     /**< I2C DMA usage option */
    uint32_t localAddress;            /**< I2C local address, valid only in slave mode*/
    uint32_t baudrate;                /**< I2C baudrate */
    uint16_t *pdmaBuffer;             /**< DMA buffer, it must be specified when DMA is enabled and DMA_MEMORY_TO_PERIPH
                                         is the direction, must be 16Bit aligned by user app */
    uint16_t  dmaBufferLength;        /**< DMA buffer length*/
    I2C_Callback_T callback;          /**< callback function */
} I2C_Config_T;

typedef struct I2C_Handle
{
    volatile   I2C_State_T      state;        /**< I2C data-transmission state */
    I2C_Device_T    *pDevice;      /**< I2C device handler */
    I2C_ErrorCode_T  errorCode;    /**< I2C error code */
    DMA_Handle_T    *hDMA      ;   /**< DMA device handler */
    I2C_DataCmdType_T  restartMode;      /**< I2C restart mode */
    uint8_t            restartMode_f;
    uint8_t            memAddr;
    uint8_t          enableDMA;    /**< I2C DMA usage option */
    uint8_t  txFifoDepth;          /**< I2C TX FIFO depth */
    uint8_t  rxFifoDepth;          /**< I2C RX FIFO depth */
    uint8_t  txThreshld;           /**< I2C TX threshold */
    uint8_t  txFifoEmptyDepth;     /**< I2C TX FIFO depth */
    uint8_t  rxThreshld;           /**< I2C RX threshold */
    uint8_t *pTxBuffer;            /**< Pointer to Tx buffer */
    uint8_t *pRxBuffer;            /**< Pointer to Rx buffer */
    volatile uint16_t  txLength;            /**< Data length for transfering */
    volatile uint16_t  txRemained;          /**< Data remained for transfering */
    volatile uint16_t  rxLength;            /**< Data length for receiving */
    volatile uint16_t  rxRemained;          /**< Data remained for receiving */
    uint16_t *pdmaBuffer;          /**< DMA buffer, it must be specified when DMA is enabled and DMA_MEMORY_TO_PERIPH
                                        is the direction, must be 16Bit aligned by user app */
    uint16_t  dmaBufferLength;     /**< DMA buffer length */
    I2C_Callback_T callback;       /**< callback function */
} I2C_Handle_T;

/**
 * @brief  Initialize I2C peripheral with specified parameters.
 *
 * @param  pI2C Pointer to I2C device handler.
 * @param  pConfig Pointer to I2C configuration structure.
 * @param  hI2C Pointer to I2C transaction handler.
 *
 * @retval RET_OK Api was successfully executed.
 * @retval RET_BUSY DMA peripheral is being used.
 */
RET_CODE I2C_Init(I2C_Device_T *pI2C, I2C_Config_T *pConfig, I2C_Handle_T *hI2C);

/**
 * @brief  DeInitializes the I2C peripheral.
 *
 * @param  hI2C Pointer to I2C transaction handler.
 *
 * @retval Always return RET_OK.
 */
RET_CODE I2C_DeInit(I2C_Handle_T *hI2C);

/**
 *@brief Master I2C peripheral transmits bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C          A pointer to I2C_Handle_T structure containing Handle
 *                     Information of driver for a specified I2C peripheral.
 *@param targetAddress Target slave address
 *@param pData         A pointer to data buffer to be transferred.
 *@param size          The count of data to be transferred.
 *@param timeout       In Microsecond unit. It timeouts if I2C master does not transfer the
 *                     expected size of data out within the given timeout duration.
 *                     Specially, TIMEOUT_NO_BLOCKING indicates that Master
 *                     transfers data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *                     means that data transmission will proceed in Polling mode.
 *
 * @retval RET_OK      Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR   Failure occurred while api was being executed.
 */
RET_CODE I2C_MasterTransmit(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *pData, uint16_t size, int32_t timeout);

/**
 *@brief Master I2C peripheral receives bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C          A pointer to I2C_Handle_T structure containing Handle
 *                     Information of driver for a specified I2C peripheral.
 *@param targetAddress Target slave address
 *@param pData         A pointer to data buffer to be received.
 *@param size          The count of data to be received.
 *@param timeout       In Microsecond unit. It timeouts if I2C master does not receive the
 *                     expected size of data within the given timeout duration.
 *                     Specially, TIMEOUT_NO_BLOCKING indicates that Master
 *                     receives data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *                     means that data-receiving will proceed in Polling mode.
 *
 * @retval RET_OK      Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR   Failure occurred while api was being executed.
 */
RET_CODE I2C_MasterReceive(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *pData, uint16_t size, int32_t timeout);

/**
 *@brief Master I2C peripheral receives bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C          A pointer to I2C_Handle_T structure containing Handle
 *                     Information of driver for a specified I2C peripheral.
 *@param targetAddress Target slave address
 *@param memAddr       A pointer to memory address, where we want to read.
 *@param pData         A pointer to data buffer to be received.
 *@param size          The count of data to be received.
 *@param timeout       In Microsecond unit. It timeouts if I2C master does not receive the
 *                     expected size of data within the given timeout duration.
 *                     Specially, TIMEOUT_NO_BLOCKING indicates that Master
 *                     receives data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *                     means that data-receiving will proceed in Polling mode.
 *
 * @retval RET_OK      Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR   Failure occurred while api was being executed.
 */

RET_CODE I2C_MasterReceive_restart(I2C_Handle_T *hI2C, uint16_t targetAddress, uint8_t *memAddr, uint8_t *pData, uint16_t size, int32_t timeout);

/**
 *@brief Slave I2C peripheral transmits bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C          A pointer to I2C_Handle_T structure containing Handle
 *                     Information of driver for a specified I2C peripheral.
 *@param pData         A pointer to data buffer to be transferred.
 *@param size          The count of data to be transferred.
 *@param timeout       In Microsecond unit. It timeouts if I2C slave does not transfer the
 *                     expected size of data out within the given timeout duration.
 *                     Specially, TIMEOUT_NO_BLOCKING indicates that Slave
 *                     transfers data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
 *                     means that data transmission will proceed in Polling mode.
 *
 * @retval RET_OK      Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR   Failure occurred while api was being executed.
 */
RET_CODE I2C_SlaveTransmit(I2C_Handle_T *hI2C, uint8_t *pData, uint16_t size, int32_t timeout);

/**
 *@brief Slave I2C peripheral receives bytes of data in Polling
 *       or Interrupt Mode depending on timeout parameter.
 *
 *@param hI2C          A pointer to I2C_Handle_T structure containing Handle
 *                     Information of driver for a specified I2C peripheral.
 *@param pData         A pointer to data buffer to be received.
 *@param size          The count of data to be received.
 *@param Timeout:      In Millisecond unit. It timeouts when Timeout elapses in ms.
                       Specially, TIMEOUT_NO_BLOCKING indicates that Slave
                       receives data in Interrupt Mode. And any other than TIMEOUT_NO_BLOCKING
                       means that data-receiving will proceed in Polling mode.
 *
 * @retval RET_OK      Api was successfully executed.
 * @retval RET_TIMEOUT Timeout happens.
 * @retval RET_ERROR   Failure occurred while api was being executed.
 */
RET_CODE I2C_SlaveReceive(I2C_Handle_T *hI2C, uint8_t *pData, uint16_t size, int32_t timeout);

#ifdef __cplusplus
}
#endif

#endif /*_I2C_H_*/
