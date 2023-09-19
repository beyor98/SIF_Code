/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart.h
 *
 * @author        wen.liu
 *
 * @version       1.1.0
 *
 * @date          2016/06/13
 *
 * @brief         Header file of uart driver.
 *
 * @note
 *    2015/08/21, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#include <drv/dev/dma.h>

/**
 * @defgroup UART_STOPBITS_COUNT
 * @brief Specify UART stop bit count
 * @{
 */
#define UART_STOPBITS_1                     ((uint32_t)0x0000)
#define UART_STOPBITS_2                     ((uint32_t)0x0001)
#define UART_STOPBITS_1_5                   ((uint32_t)0x0002)
#define UART_STOPBITS_0_5                   ((uint32_t)0x0003)
/**
 * @}
 */

/**
 * @defgroup UART_DATABITS_COUNT
 * @brief Specify UART data bit count
 * @{
 */
#define UART_DATABITS_5                     ((uint32_t)0x0000)
#define UART_DATABITS_6                     ((uint32_t)0x0001)
#define UART_DATABITS_7                     ((uint32_t)0x0002)
#define UART_DATABITS_8                     ((uint32_t)0x0003)
#define UART_DATABITS_9                     ((uint32_t)0x0004)
/**
 * @}
 */

/**
 * General uart events
 */
typedef enum
{
    UART_EVENT_NONE = 0,               /**< There is nothing happened. */
    UART_EVENT_TX_EMPTY = 1,          /**< Transmit one character over(Holding register empty)
                                            in single-byte mode. */
    UART_EVENT_RX_CHAR = 2,            /**< Receive one valid character in single-byte mode, param
                                            is the received character. */
    UART_EVENT_RX_ERROR = 4,           /**< Rx error indication, maybe parity error, frame error... */
    UART_EVENT_TX_OVER = 8,            /**< Transmission is over, that is to say all user data
                                            is pushed into TX FIFO, or pushed into buffer in
                                            ring buffer mode. */
    UART_EVENT_RX_OVER = 16,           /**< All expected data received and pushed into user buffer. */
    UART_EVENT_RXFIFO_TIMEOUT = 32,    /**< No rx fifo operation occurs during continuous 4
                                            character time, param is received data size. */
    UART_EVENT_TX_WATERMARK = 64,      /**< It is tx fifo threshhold notification, param is remained
                                            data size in tx fifo. */
    UART_EVENT_RX_WATERMARK = 128,     /**< It is rx fifo threshhold notification, param is received
                                            data size in rx fifo. */
    UART_EVENT_TX_IDLE = 0xC0,          /**< All data transmitted, and tx line in idle status. */
} UART_Event_T;

/**
 * General uart interrupts
 */
typedef enum
{
    UART_RX_INT = 1,                   /**< General UART rx interrupt, shall be enabled to
                                            get RX_ERROR/RX_OVER/RX_CHAR events */
    UART_TX_INT = 2,                   /**< General UART tx interrupt, shall be enabled to
                                            get TX_EMPTY/TX_OVER events */
    UART_RX_FIFO_INT = 4,              /**< UART rx fifo interrupt, shall be enabled to get
                                            RXFIFO_TIMEOUT or RX_WATERMARK events */
    UART_TX_FIFO_INT = 8,              /**< UART tx fifo interrupt, shall be enabled to get
                                            TX_WATERMARK event */
    UART_RX_DMA_INT = 16,              /**< UART rx dma interrupt    */
    UART_TX_DMA_INT = 32,              /**< UART tx dma interrupt    */
} UART_Interrupt_T;

/**
 * General uart work mode
 */
typedef enum
{
    UART_MODE_RX = 0,                  /**< Receive characters allowed */
    UART_MODE_TX,                      /**< Transmit characters allowed */
    UART_MODE_DISABLE,                 /**< Recieve and transmit characters not allowed */
    UART_MODE_DUPLEX,                  /**< Recieve and transmit characters both allowed */
} UART_Mode_T;

/**
 * General uart parity types
 */
typedef enum
{
    UART_PARITY_ODD = 0,               /**< Check data by odd parity */
    UART_PARITY_EVEN,                  /**< Check data by even parity */
    UART_PARITY_NONE                   /**< No parity bit */
} UART_Parity_T;

/**
 * General receiver line status
 */
typedef enum
{
    UART_FRAME_ERROR = 1,               /**< Stop bit check fails */
    UART_PARITY_ERROR = 2,              /**< Parity check fails */
    UART_OVERRUN_ERROR = 4,             /**< Rx fifo/buffer overrun error */
    UART_BREAK_CHARACTER = 8,           /**< Break character detected */
    UART_TRANSMITTER_IDLE = 16,         /**< All data transmitted, and tx line in idle status */
    UART_FIFO_ERROR = 32,               /**< At least there is one error in rx fifo */
    UART_RX_ERROR = 0x100,              /**< There is error for RX character */
} UART_LineStatus_T;


typedef struct __UART_Handle_T UART_Handle_T;

/**
 * @brief Callback of uart interrupt handler to notify user an event along with parameters.
 *
 * @param hUART   Pointer to transaction handle, it maybe NULL if no handle is defined.
 * @param event   Uart event, it can be one of enum @ref  UART_Event_T.
 * @param param   Parameter of uart event, it is the received char if the event is UART_EVENT_RX_CHAR.
 *
 * @retval        None.
 */
typedef void (*UART_Callback_T)(UART_Handle_T *hUART, uint8_t event, ulong_t param);

/**
 * General uart configuration structure.
 */
typedef struct
{
    uint32_t        baudRate;          /**< UART baudrate */
    UART_Parity_T   parity;            /**< UART parity */
    uint8_t         wordLength;        /**< Data bits transmitted in one frame */
    uint8_t         stopBits;          /**< Stop bits between frames */
    uint8_t         reserved[2];
    UART_Callback_T callback;
} UART_Config_T;

/**
 * Configuration of transfer parameters
 */
typedef struct
{
    uint8_t        enableFifo;         /**< Enable rx/tx fifo or not */
    uint8_t        txWatermark;        /**< Tx fifo threshold */
    uint8_t        rxWatermark;        /**< Rx fifo threshold in */
    uint8_t        enableRxDMA;        /**< Move data to ram by dma */
    uint8_t        enableTxDMA;        /**< Read data from ram by dma */
} UART_TransferConfig_T;

/**
 * Configuration of transfer parameters
 */
typedef struct
{
    uint16_t       receivedSize;       /**< Received data size  */
    uint16_t       toBeTransmitedSize; /**< Size of to-be-transmiited data */
    uint32_t       lineStatus;         /**< Receive line status */
} UART_TransferStatus_T;

/**
 * Uart user handle structure.
 */
/**
 * Uart user handle structure.
 */
typedef struct __UART_Handle_T
{
    UART_Device_T *device;           /**< Pointer to uart device handle */
    uint16_t enableFifo;             /**< Enable rx/rx fifo */
    uint16_t index;                  /**< Index of all uart constant table for irq, dma channel, ... */
    uint32_t lineSavedStatus;        /**< Saved status of rx line since last read or last get_status calling */
    DMA_Handle_T RxDMAHandle;        /**< Rx dma handle */
    DMA_Handle_T TxDMAHandle;        /**< Tx dma handle */
    uint8_t  enableRxDMA;            /**< Move data to ram by dma */
    uint8_t  enableTxDMA;            /**< Read data from ram by dma */
    uint8_t  txWatermark;            /**< Watermark of tx fifo  */
    uint8_t  rxWatermark;            /**< Watermark of rx fifo  */
    volatile uint8_t  rxDMAStatus;   /**< Rx DMA Status */
    volatile uint8_t  txDMAStatus;   /**< Tx DMA Status */
    uint8_t *pTxData;                /**< Address of remaining data to send. */
    volatile uint16_t tcount;        /**< One-shot transmiited count */
    volatile uint16_t rcount;        /**< One-shot received count */
    volatile uint16_t txExpectedSize; /**< Size of the remaining data to send. */
    volatile uint16_t txSize;        /**< Size of the data to send out. */
    uint8_t *pRxData;                /**< Address of remaining data to receive. */
    volatile uint16_t rxExpectedSize; /**< Size of the remaining data to receive. */
    volatile uint16_t rxSize;        /**< Size of the data to receive. */
    uint32_t intmask;                /**< Enabled interrupts by user */
    volatile uint32_t status;        /**< User transaction status */
    UART_Callback_T callback;        /**< Callback definition to notify user in interrupt handler */
} UART_Handle_T;

/*******************************************************************************
 * Global function
 ******************************************************************************/

/**
 * @brief Initializes UART device and uart handle with user configuration.
 *
 * This function configures one UART device with the user-defined settings. The transaction
 * handle is also initialized if it is not NULL.
 * Example code:
 * @code
 *  UART_Config_T uartConfig;
 *
 *  uartConfig.baudRate = 115200U;
 *  uartConfig.parity =   UART_PARITY_EVEN;
 *  uartConfig.stopBits = UART_STOPBITS_1;
 *  UART_Init(UART0, &uartConfig, &g_uartHandle);
 * @endcode
 *
 * @param  pUART          UART device handle
 * @param  pConfig        Pointer to user-defined configuration
 * @param  hUART          Uart handle
 *
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE UART_Init(UART_Device_T *pUART, UART_Config_T *pConfig, UART_Handle_T *hUART);

/**
 * @brief Update uart configurations such as enable/disable fifo, enable/disable dma, and so on.
 *
 * This function updates uart configurations about data transferring parameters, including
 * dma engine usage, fifo threshold.
 *
 * @param  hUART          Uart handle
 * @param  pConfig        Pointer to transferring configuration
 *
 * @retval RET_ERROR      There is invalid configuration not supported
 * @retval RET_OK         Configure successfully
 */
RET_CODE UART_TransferConfig(UART_Handle_T *hUART, UART_TransferConfig_T *pConfig);

/**
 * @brief Deinitializes UART device.
 *
 * This function waits till uart is in idle, then deinitializes UART device, such as disables all
 * interrupts, disables rx/tx, gates off uart source clock,etc.
 *
 * @param  hUART          UART handle
 *
 * @retval                None
 */
void UART_DeInit(UART_Handle_T *hUART);

/**
 * @brief Transmit one character by uart tx bus.
 *
 * This function pushes one character into uart data register, it will wait if tx holding register
 * is not empty.
 *
 * @param  pUART          UART device handle.
 * @param  character      Character to be transmitted.
 *
 * @retval                None
 */
void UART_PutChar(UART_Device_T *pUART, uint8_t character);

/**
 * @brief  Receive one character from uart data register.
 *
 * This function gets one character from data register, it will not wait even if no data received.
 *
 * @param  pUART          UART device handler
 *
 * @retval                Received character from uart data register.
 */
uint16_t UART_GetChar(UART_Device_T *pUART);

/**
 * @brief  Disables or enables uart tx/rx function.
 *
 * This function enables/disables uart tx/rx function.
 *
 * @param  hUART          UART handle
 * @param  mode           UART work mode
 *
 * @retval                None
 */
void UART_SetMode(UART_Handle_T *hUART, UART_Mode_T mode);

/**
 * @brief  Get received/transmitted data size and rx line status.
 *
 * @param  hUART          UART handle
 * @param  status         Pointer to structure containing rx line status and received/transmitted
 *                        data size
 *
 * @retval                None
 */
void UART_GetTransferStatus(UART_Handle_T *hUART, UART_TransferStatus_T *status);

/**
 * @brief  Receive number of bytes by uart rx line.

 * This function receives data and pushes data into user buffer. This is blocking function if
 * timeout is TIMEOUT_WAIT_FOREVER, and non-blocking if timeout is TIMEOUT_NO_BLOCKING. The
 * actual received bytes is returned when the function exits.
 *
 * @param  hUART          UART handle.
 * @param  pData          Pointer to data buffer.
 * @param  expectedSize   Number of bytes expected to be received.
 * @param  pReceivedSize  Pointer to user variable which will be set with received data size
 *                        when function returns.
 * @param  timeout        Timeout paramter in millisecond unit. This function timeouts if uart
 *                        does not get expected size of data within the given timeout duration.
 *                        The special timeout parameter TIMEOUT_WAIT_FOREVER and TIMEOUT_NO_BLOCKING
 *                        means blocking and non-blocking transaction seperately.
 *
 * @retval RET_BUSY       Previous transmission is still not finished.
 * @retval RET_TIMEOUT    Timeout occurs.
 * @retval RET_ONGOING    User request is accepted and transaction is on progress.
 * @retval RET_OK         Expected size of data received and no error occurs.
 * @retval RET_ERROR      Some error occurs during data receiving.
 */
RET_CODE UART_Receive(UART_Handle_T *hUART, uint8_t *pData, uint16_t expectedSize, uint16_t *pReceivedSize, uint32_t timeout);

/**
 * @brief Transmits number of bytes by uart tx line.
 *
 * This function sends some data from buffer to uart tx line. It is blocking function if timeout
 * is TIMEOUT_WAIT_FOREVER, and non-blocking if timeout is TIMEOUT_NO_BLOCKING.
 *
 * @param  hUART           UART handle.
 * @param  pData           UART handle pointer.
 * @param  size            Number of bytes expected to be transmitted.
 * @param  timeout         Timeout paramter in millisecond unit. This function timeouts if uart
 *                         does not transmit expected size of data within the given timeout duration.
 *                         The special timeout parameter TIMEOUT_WAIT_FOREVER and TIMEOUT_NO_BLOCKING
 *                         means blocking and non-blocking transaction seperately.
 *
 * @retval RET_BUSY        Previous transmission still not finished.
 * @retval RET_TIMEOUT     Timeout occurs.
 * @retval RET_ONGOING     User request is accepted and transaction is ongoing.
 * @retval RET_OK          All data pushed into tx fifo. It does not indicate all data transmitted
 *                         by tx bus.
 */
RET_CODE UART_Transmit(UART_Handle_T *hUART, uint8_t *pData, uint16_t size, uint32_t timeout);

/**
 * @brief Enables UART interrupts.
 *
 * This function enables multiple uart interrupts.
 *
 * @param  hUART           UART handle.
 * @param  mask            Interrupt mask, please refer to enum UART_Interrupt_T..
 *
 * @retval                 None
 */
void UART_EnableInterrupts(UART_Handle_T *hUART, uint32_t mask);

/**
 * @brief Disables UART interrupts.
 *
 * This function disables multiple uart interrupts.
 *
 * @param  hUART           UART handle
 * @param  mask            Interrupt mask, please refer to enum UART_IntStatus_T
 *
 * @retval                 None
 */
void UART_DisableInterrupts(UART_Handle_T *hUART, uint32_t mask);

/**
 * @brief Get Fifo Data.
 *
 * This function is used to get Fifo Data.
 *
 *
 * @param  pUART          UART device handle.
 * @param  size           number of last data in Fifo.
 * @param  data           Fifo data pointer
 * @retval                None
 */
void UART_GetFifoData(UART_Device_T *pUART, uint8_t *size, uint8_t *data);

//#ifndef CHIP_F6621B
//#define CHIP_F6621B
//#endif

#if defined(CHIP_F6621B)

/* Exported constants --------------------------------------------------------*/
#define T0_PROTOCOL        0x00  /* T0 protocol */
#define DIRECT             0x3B  /* Direct bit convention */
#define INDIRECT           0x3F  /* Indirect bit convention */
#define SETUP_LENGTH       20
#define HIST_LENGTH        20
#define LCmax              40
#define SC_Receive_Timeout 0x5000  /* Direction to reader */

/* ATR structure - Answer To Reset -------------------------------------------*/
typedef struct
{
    uint8_t TS;               /* Bit Convention */
    uint8_t T0;               /* High nibble = Number of setup byte; low nibble = Number of historical byte */
    uint8_t T[SETUP_LENGTH];  /* Setup array */
    uint8_t H[HIST_LENGTH];   /* Historical array */
    uint8_t Tlength;          /* Setup array dimension */
    uint8_t Hlength;          /* Historical array dimension */
} SC_ATR;

/* ADPU-Header command structure ---------------------------------------------*/
typedef struct
{
    uint8_t CLA;  /* Command class */
    uint8_t INS;  /* Operation code */
    uint8_t P1;   /* Selection Mode */
    uint8_t P2;   /* Selection Option */
} SC_Header;

/* ADPU-Body command structure -----------------------------------------------*/
typedef struct
{
    uint8_t LC;           /* Data field length */
    uint8_t Data[LCmax];  /* Command parameters */
    uint8_t LE;           /* Expected length of data to be returned */
} SC_Body;

/* ADPU Command structure ----------------------------------------------------*/
typedef struct
{
    SC_Header Header;
    SC_Body Body;
} SC_ADPU_Commands;

/* SC response structure -----------------------------------------------------*/
typedef struct
{
    uint8_t Data[LCmax];  /* Data returned from the card */
    uint8_t SW1;          /* Command Processing status */
    uint8_t SW2;          /* Command Processing qualification */
} SC_ADPU_Responce;


int SC_Init(UART_Device_T *pUART);
void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus);
void SC_DeInit(void);

#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _UART_H_ */
