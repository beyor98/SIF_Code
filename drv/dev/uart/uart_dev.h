/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          uart_dev.h
 *
 * @author        wen.liu
 *
 * @version       1.0.0
 *
 * @date          2016/06/13
 *
 * @brief         CMSIS Peripheral Access Layer for GM6601
 *
 * @note
 *    2016/02/25, wen.liu, V1.0.0
 *        Initial version.
 *    2016/06/13, wen.liu, V1.1.0
 *        Update coding style.
 */
#ifndef _UART_DEV_H_
#define _UART_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CHIP_GM6601
#define UART_IP_VERSION 1
#else
#define UART_IP_VERSION 2
#endif

#if (UART_IP_VERSION == 1)
/* UART Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t DR;                        /* UART Transmit/Recieve Data Register: 0x0 */
    __IO uint32_t TC;                        /* UART Timing Configuration Register: 0x4 */
    __IO uint32_t C1;                        /* UART Control Register 1: 0x8 */
    __IO uint32_t C2;                        /* UART Control Register 2: 0xc */
    __IO uint32_t C3;                        /* UART Control Register 3: 0x10 */
    __IO uint32_t LS;                        /* UART Line Status Register: 0x14 */
    __IO uint32_t TH;                        /* UART RX/TX Threshold Register: 0x18 */
    __IO uint32_t IE;                        /* UART Interrupt Enable Register: 0x1c */
    __IO uint32_t IS;                        /* UART Interrupt Status Register: 0x20 */
    __IO uint32_t DMA;                       /* UART DMA Control Register: 0x24 */
} UART_Device_T;

/******************  Bit definition for UART CONTROL register  *******************/
#define  UART_TC_SCLK                        ((uint32_t)0x0000FFFF)            /* UART Sample Clock Prescaler */
#define  UART_TC_SCLK_FIELD(x)               (((uint32_t)(x))&0x0000FFFF)
#define  UART_TC_DST                         ((uint32_t)0x000F0000)            /* UART Sample Times of Data/Start bits, default is 16x oversampling. */
#define  UART_TC_OST                         ((uint32_t)0x03F00000)            /* UART Other Sample Times for parity and stop bits, default is 42 times. */
#define  UART_TC_STEN                        ((uint32_t)0x10000000)            /* Enable Sample Times Configuration */

#define  UART_C1_AGEN                        ((uint32_t)0x00000100)            /* Enable Anti-Glitch function */
#define  UART_C1_AGTHS                       ((uint32_t)0x00000070)            /* Anti-Glitch Threshold of Start Bit */
#define  UART_C1_AGTHS_FIELD(x)              ((x)<<4)
#define  UART_C1_AGTHD                       ((uint32_t)0x0000000F)            /* Anti-Glitch Threshold of Data Bit */
#define  UART_C1_AGTHD_FIELD(x)              ((x)<<0)


#define  UART_C2_FFEN                        ((uint32_t)0x00000001)            /* UART FIFO Enable/Disable */
#define  UART_C2_RFRST                       ((uint32_t)0x00000002)            /* UART RX FIFO Reset */
#define  UART_C2_TFRST                       ((uint32_t)0x00000004)            /* UART TX FIFO Reset */
#define  UART_C2_RXDIS                       ((uint32_t)0x00000100)            /* Disable UART RX */
#define  UART_C2_TXDIS                       ((uint32_t)0x00000200)            /* Disable UART TX */
#define  UART_C2_RXINV                       ((uint32_t)0x00000400)            /* Invert RX Polarity */
#define  UART_C2_TXINV                       ((uint32_t)0x00000800)            /* Invert TX Polarity */
#define  UART_C2_RX9BIT                      ((uint32_t)0x00010000)            /* Enable RX 9-bit data */
#define  UART_C2_TX9BIT                      ((uint32_t)0x00020000)            /* Enable TX 9-bit data */
#define  UART_C2_TXMGAP                      ((uint32_t)0xFF000000)            /* TX Mininal Gap */

#define  UART_C3_DTB                         ((uint32_t)0x00000003)            /* UART Data Bits */
#define  UART_C3_DTB_FIELD(x)                ((x)<<0)
#define  UART_C3_STB                         ((uint32_t)0x00000004)            /* UART Stop Bits */
#define  UART_C3_PEN                         ((uint32_t)0x00000008)            /* UART Parity Enable */
#define  UART_C3_EOP                         ((uint32_t)0x00000010)            /* UART Even/Odd Parity */
#define  UART_C3_SP                          ((uint32_t)0x00000020)            /* UART Stick Parity */
#define  UART_C3_BRK                         ((uint32_t)0x00000040)            /* UART Break Control */
#define  UART_C3_LOOP                        ((uint32_t)0x00100000)            /* UART Loop Control */
#define  UART_C3_DTR                         ((uint32_t)0x00010000)            /* UART DTR modem signal */
#define  UART_C3_RTS                         ((uint32_t)0x00020000)            /* UART RTS modem signal */

#define  UART_IE_RX                          ((uint32_t)0x00000001)            /* Enable Received Data Interrupt */
#define  UART_IE_THRE                        ((uint32_t)0x00000002)            /* Enable Transmitter Holding Register Empty Interrupt */
#define  UART_IE_RLS                         ((uint32_t)0x00000004)            /* Enable Receiver Line Status Interrupt */
//#define  UART_IE_EMSI                        ((uint32_t)0x00000008)            /* Enable Modem Status Interrupt */
#define  UART_IE_TFTH                        ((uint32_t)0x00010000)            /* Enable TX FIFO Threshold Interrupt*/
#define  UART_IE_RFTH                        ((uint32_t)0x00020000)            /* Enable RX FIFO Threshold Interrupt*/

#define  UART_IS_PI                          ((uint32_t)0x00000001)            /* Pending Interrupt, 0: Pending interrupts; 1: No pending interrupts */
#define  UART_IS_IID                         ((uint32_t)0x0000000E)            /* Interrupt ID 6: Receiver line status, cleard by read LSR, priority 1 */
/*              4: Receiver data available, cleared by read TRBR, priority 2 */
/*(Only in FIFO mode)c: FIFO timeout indication, cleared by read TRBR, priority 2 */
/*              2: Transmitter Holding Register Empty, cleared by ISR read */
/*                 or new character write, priority 3 */
/*              0: Modem interrupt, cleared by read MSR */

#define  UART_IS_IID_THRE                    0x02   /* Transmitter Holding Register Empty Interrupt */
#define  UART_IS_IID_RLS                     0x06   /* Receive Line Status Interrupt (OE/PE/FE/BRK) */
#define  UART_IS_IID_DR                      0x04   /* Receive data */
#define  UART_IS_IID_RXTM                    0x0c   /* Rx FIFO timeout */

#define  UART_IS_TFTH                        ((uint32_t)0x00010000)            /* TX FIFO Threshold Interrupt Status */
#define  UART_IS_RFTH                        ((uint32_t)0x00020000)            /* RX FIFO Threshold Interrupt Status */


#define  UART_LS_DR                          ((uint32_t)0x00000001)            /* Data Ready */
#define  UART_LS_OE                          ((uint32_t)0x00000002)            /* Overrun Error */
#define  UART_LS_PE                          ((uint32_t)0x00000004)            /* Parity Error */
#define  UART_LS_FE                          ((uint32_t)0x00000008)            /* Framing Error */
#define  UART_LS_BRK                         ((uint32_t)0x00000010)            /* Break Control */
#define  UART_LS_THRE                        ((uint32_t)0x00000020)            /* UART Transmitter Holding Register is Empty */
#define  UART_LS_TIDL                        ((uint32_t)0x00000040)            /* UART Transmitter is idle */
#define  UART_LS_RFE                         ((uint32_t)0x00000080)            /* UART RX FIFO include error byte */
#define  UART_LS_RXE                         (UART_LS_OE | UART_LS_PE | UART_LS_FE | UART_LS_RFE) /* UART receives error character */

#define  UART_TH_RFTH                        ((uint32_t)0x001F0000)            /* UART RX FIFO Max trigger level */
#define  UART_TH_RFTH_FIELD(x)               ((x)<<16)

#define  UART_TH_RFLV                        ((uint32_t)0x1F000000)            /* UART RX FIFO Threshold */
#define  UART_TH_RFLV_GET(reg)               (((reg) & UART_TH_RFLV) >> 24)

#define  UART_TH_TFTH                        ((uint32_t)0x0000001F)            /* UART TX Max trigger level */
#define  UART_TH_TFTH_FIELD(x)               ((x)<<0)

#define  UART_TH_TFLV                        ((uint32_t)0x00001F00)            /* UART TX FIFO Threshold */
#define  UART_TH_TFLV_GET(reg)               (((reg) & UART_TH_TFLV) >> 8)

#define  UART_DMA_REN                        ((uint32_t)0x00000001)            /* Enable RX DMA */
#define  UART_DMA_RTH                        ((uint32_t)0x00001F00)            /* FIFO Threshhold for RX DMA */
#define  UART_DMA_RTH_FIELD(x)               ((x)<<8)
#define  UART_DMA_TEN                        ((uint32_t)0x00010000)            /* Enable TX DMA */
#define  UART_DMA_TTH                        ((uint32_t)0x1F000000)            /* FIFO Threshhold for TX DMA */
#define  UART_DMA_TTH_FIELD(x)               ((x)<<24)

/* Base addresses of UART Control Registers */
#define UART0_BASE                               (0x40004000u)
#define UART1_BASE                               (0x40005000u)
#define UART2_BASE                               (0x40006000u)

/* UART Device Pointer */
#define UART0                                    ((UART_Device_T *)UART0_BASE)
#define UART1                                    ((UART_Device_T *)UART1_BASE)
#define UART2                                    ((UART_Device_T *)UART2_BASE)

#define MAX_UART_TX_FIFO_SIZE                    16
#define MAX_UART_RX_FIFO_SIZE                    16

#define MAX_UART_DEVICE                          (3)

#endif

#if (UART_IP_VERSION == 2)

/* UART Control Register Layout Type Definition */
typedef struct
{
    __IO uint32_t DR;                        /* UART Transmit/Recieve Data Register: 0x0 */
    __IO uint32_t TC;                        /* UART Timing Configuration Register: 0x4 */
    __IO uint32_t C1;                        /* UART Control Register 1: 0x8 */
    __IO uint32_t C2;                        /* UART Control Register 2: 0xc */
    __IO uint32_t C3;                        /* UART Control Register 3: 0x10 */
    __IO uint32_t LS;                        /* UART Line Status Register: 0x14 */
    __IO uint32_t TH;                        /* UART RX/TX Threshold Register: 0x18 */
    __IO uint32_t IE;                        /* UART Interrupt Enable Register: 0x1C */
    __IO uint32_t IS;                        /* UART Interrupt Status Register: 0x20 */
    __IO uint32_t DMA;                       /* UART DMA Control Register: 0x24 */
    __IO uint32_t GTPR;                      /* UART Guard Time Prescaler Register: 0x28 */
    __IO uint32_t RTOR;                      /* UART Receiver Timeout Register: 0x2C */
} UART_Device_T;

/******************  Bit definition for UART CONTROL register  *******************/
#define  UART_TC_SCLK                        ((uint32_t)0x0000FFFF)            /* UART Sample Clock Prescaler */
#define  UART_TC_SCLK_FIELD(x)               (((uint32_t)(x))&0x0000FFFF)
#define  UART_TC_DST                         ((uint32_t)0x000F0000)            /* UART Sample Times of Data/Start bits. */
#define  UART_TC_OST                         ((uint32_t)0x03F00000)            /* UART Other Sample Times for parity and stop bits. */
#define  UART_TC_ABRMOD                      ((uint32_t)0x0C000000)            /* ABRMOD Configuration */
#define  UART_TC_ABRMOD_FIELD(x)             ((x)<<26)                         /* ABRMOD Configuration */
#define  UART_TC_STEN                        ((uint32_t)0x10000000)            /* Enable Sample Times Configuration */

#if (defined(CHIP_GM6601B) || defined(CHIP_F6601C))
#define  UART_C1_AGTHD                       ((uint32_t)0x0000000F)            /* Anti-Glitch Threshold of Data Bit */
#define  UART_C1_AGTHS                       ((uint32_t)0x00000070)            /* Anti-Glitch Threshold of Start Bit */
#define  UART_C1_AGEN                        ((uint32_t)0x00000100)            /* Enable Anti-Glitch function */

#define  UART_C2_FFEN                        ((uint32_t)0x00000001)            /* UART FIFO Enable/Disable */
#define  UART_C2_RFRST                       ((uint32_t)0x00000002)            /* UART RX FIFO Reset */
#define  UART_C2_TFRST                       ((uint32_t)0x00000004)            /* UART TX FIFO Reset */
#define  UART_C2_RXDIS                       ((uint32_t)0x00000100)            /* Disable UART RX */
#define  UART_C2_TXDIS                       ((uint32_t)0x00000200)            /* Disable UART TX */
#define  UART_C2_RXINV                       ((uint32_t)0x00000400)            /* Invert RX input function */
#define  UART_C2_TXINV                       ((uint32_t)0x00000800)            /* Invert TX output function */
#define  UART_C2_RX9BIT                      ((uint32_t)0x00010000)            /* Enable RX 9-bit data */
#define  UART_C2_TX9BIT                      ((uint32_t)0x00020000)            /* Enable TX 9-bit data */
#define  UART_C2_TXSLEN                      ((uint32_t)0x30000000)            /* Stop bits length */
#define  UART_C2_TXSLEN_FIELD(x)             ((x)<<28)                         /* Stop bits length */
#define  UART_C2_TXSEN                       ((uint32_t)0x40000000)            /* TX Mininal Gap */
#define  UART_C2_ABREN                       ((uint32_t)0x80000000)            /* Auto baud rate enable */

#define  UART_C3_DTB                         ((uint32_t)0x00000003)            /* UART Data Bits */
#define  UART_C3_DTB_FIELD(x)                ((x)<<0)
#define  UART_C3_STB                         ((uint32_t)0x00000004)            /* UART Stop Bits */
#define  UART_C3_PEN                         ((uint32_t)0x00000008)            /* UART Parity Enable */
#define  UART_C3_EOP                         ((uint32_t)0x00000010)            /* UART Even/Odd Parity */
#define  UART_C3_SP                          ((uint32_t)0x00000020)            /* UART Stick Parity */
#define  UART_C3_BRK                         ((uint32_t)0x00000040)            /* UART Break Control */
#define  UART_C3_RTS                         ((uint32_t)0x00010000)            /* UART RTS modem signal */
#define  UART_C3_DTR                         ((uint32_t)0x00020000)            /* UART DTR modem signal */
#define  UART_C3_RIO                         ((uint32_t)0x00040000)            /* UART RI OUT value */
#define  UART_C3_DCDO                        ((uint32_t)0x00080000)            /* UART DCD OUT value */
#define  UART_C3_LOOP                        ((uint32_t)0x00100000)            /* UART Loop Control */
#else

#define  UART_C1_AGTHD                       ((uint32_t)0x0000000F)            /* Anti-Glitch Threshold of Data Bit */
#define  UART_C1_AGTHS                       ((uint32_t)0x00000070)            /* Anti-Glitch Threshold of Start Bit */
#define  UART_C1_AGEN                        ((uint32_t)0x00000100)            /* Enable Anti-Glitch function */
#define  UART_C1_CTSEN                       ((uint32_t)0x00004000)            /* CTS_HARD_CTRL_EN */
#define  UART_C1_RTSEN                       ((uint32_t)0x00008000)            /* RTS_HARD_CTRL_EN */
#define  UART_C1_HDEN                        ((uint32_t)0x00010000)            /* Half Mode Select */

#define  UART_C2_FFEN                        ((uint32_t)0x00000001)            /* UART FIFO Enable/Disable */
#define  UART_C2_RFRST                       ((uint32_t)0x00000002)            /* UART RX FIFO Reset */
#define  UART_C2_TFRST                       ((uint32_t)0x00000004)            /* UART TX FIFO Reset */
#define  UART_C2_LBDL                        ((uint32_t)0x00000020)            /* Lin break detection length */
#define  UART_C2_SBKRQ                       ((uint32_t)0x00000040)            /* send break frame request */
#define  UART_C2_LINEN                       ((uint32_t)0x00000080)            /* Lin mode enable */
#define  UART_C2_RXDIS                       ((uint32_t)0x00000100)            /* Disable UART RX */
#define  UART_C2_TXDIS                       ((uint32_t)0x00000200)            /* Disable UART TX */
#define  UART_C2_RXINV                       ((uint32_t)0x00000400)            /* Invert RX Polarity */
#define  UART_C2_TXINV                       ((uint32_t)0x00000800)            /* Invert TX Polarity */
#define  UART_C2_MSBSEL                      ((uint32_t)0x00001000)            /* set data bit order */
#define  UART_C2_RX9BIT                      ((uint32_t)0x00010000)            /* Enable RX 9-bit data */
#define  UART_C2_TX9BIT                      ((uint32_t)0x00020000)            /* Enable TX 9-bit data */
#define  UART_C2_CPHA                        ((uint32_t)0x00040000)            /* Clock phase */
#define  UART_C2_CPOL                        ((uint32_t)0x00080000)            /* Clock polarity */
#define  UART_C2_CLKEN                       ((uint32_t)0x00100000)            /* Clock enable */
#define  UART_C2_RTOEN                       ((uint32_t)0x00200000)            /* Receiver timeout enable */
#define  UART_C2_NACK                        ((uint32_t)0x00400000)            /* Not acknowledge received Interrupt enable  */
#define  UART_C2_SCEN                        ((uint32_t)0x00800000)            /* Smartcard mode enable */
#define  UART_C2_TXSLEN                      ((uint32_t)0x30000000)            /* Stop bits length  */
#define  UART_C2_TXSLEN_FIELD(x)             ((x)<<28)                         /* Stop bits length  */
#define  UART_C2_TXSEN                       ((uint32_t)0x40000000)            /* TX Mininal Gap */
#define  UART_C2_ABREN                       ((uint32_t)0x80000000)            /* Auto baud rate enable */

#define  UART_C3_DTB                         ((uint32_t)0x00000003)            /* UART Data Bits */
#define  UART_C3_DTB_FIELD(x)                ((x)<<0)
#define  UART_C3_STB                         ((uint32_t)0x00000004)            /* UART Stop Bits */
#define  UART_C3_PEN                         ((uint32_t)0x00000008)            /* UART Parity Enable */
#define  UART_C3_EOP                         ((uint32_t)0x00000010)            /* UART Even/Odd Parity */
#define  UART_C3_SP                          ((uint32_t)0x00000020)            /* UART Stick Parity */
#define  UART_C3_BRK                         ((uint32_t)0x00000040)            /* UART Break Control */
#define  UART_C3_MUTE_MODE                   ((uint32_t)0x00000100)            /* MUTE_MODE_EN, Mute mode enable */
#define  UART_C3_WAKE_SEL                    ((uint32_t)0x00000200)            /* WAKE_SEL, Receiver wakeup method */
#define  UART_C3_MUTE_RQ                     ((uint32_t)0x00000400)            /* MUTE_RQ: Mute mode request */
#define  UART_C3_ADDM7                       ((uint32_t)0x00000800)            /* ADDM7:7-bit Address Detection/4-bit Address Detection */
#define  UART_C3_RTS                         ((uint32_t)0x00010000)            /* UART RTS modem signal */
#define  UART_C3_DTR                         ((uint32_t)0x00020000)            /* UART DTR modem signal */
#define  UART_C3_RIO                         ((uint32_t)0x00040000)            /* UART RI OUT value */
#define  UART_C3_DCDO                        ((uint32_t)0x00080000)            /* UART DCD OUT value */
#define  UART_C3_LOOP                        ((uint32_t)0x00100000)            /* UART Loop Control */
#define  UART_C3_UART_ADD_3_0                ((uint32_t)0x0F000000)            /* UART_ADD[3:0]: Address of the UART node */
#define  UART_C3_UART_ADD_7_4                ((uint32_t)0xF0000000)            /* UART_ADD[7:4]: Address of the UART node */
#endif

#define  UART_LS_OE                          ((uint32_t)0x00000001)            /* Overrun Error */
#define  UART_LS_PE                          ((uint32_t)0x00000002)            /* Parity Error */
#define  UART_LS_FE                          ((uint32_t)0x00000004)            /* Framing Error */
#define  UART_LS_BRK                         ((uint32_t)0x00000008)            /* Break Control */
#define  UART_LS_RFE                         ((uint32_t)0x00000010)            /* UART RX FIFO include error byte */
#define  UART_LS_DR                          ((uint32_t)0x00000020)            /* Data Ready */
#define  UART_LS_THRE                        ((uint32_t)0x00000040)            /* UART Transmitter Holding Register is Empty */
#define  UART_LS_TIDL                        ((uint32_t)0x00000080)            /* UART Transmitter is idle */
#define  UART_LS_SBKF                        ((uint32_t)0x00000100)            /* UART Send break flag */
#define  UART_LS_CTS                         ((uint32_t)0x00010000)            /* Clear to Send input change status */
#define  UART_LS_DSRCS                       ((uint32_t)0x00020000)            /* Data Set Ready input change status */
#define  UART_LS_RIS                         ((uint32_t)0x00040000)            /* Ring Indicator input pos-edge status */
#define  UART_LS_DCDS                        ((uint32_t)0x00080000)            /* Data Carrier Detect input change status */
#define  UART_LS_CTSV                        ((uint32_t)0x00100000)            /* The value input signal CTS */
#define  UART_LS_DSRI                        ((uint32_t)0x00200000)            /* The value input signal DSR */
#define  UART_LS_RIIN                        ((uint32_t)0x00400000)            /* The value input signal RI (Ring Indicator) */
#define  UART_LS_DCDIN                       ((uint32_t)0x00800000)            /* The value input signal DCD (Data Carrier Detect) */
#define  UART_LS_RWU                         ((uint32_t)0x01000000)            /* RWU_ST, Receiver wakeup from Mute mode */
#define  UART_LS_ABRE                        ((uint32_t)0x02000000)            /* auto baud rate error */
#define  UART_LS_ABRF                        ((uint32_t)0x04000000)            /* auto baud rate flag */
#define  UART_LS_RXE                         (UART_LS_OE | UART_LS_PE | UART_LS_FE | UART_LS_RFE) /* UART receives error character */

#define  UART_TH_TFTH                        ((uint32_t)0x0000001F)            /* UART TX FIFO Threshold */
#define  UART_TH_TFTH_FIELD(x)               ((x)<<0)
#define  UART_TH_TFLV                        ((uint32_t)0x00001F00)            /* UART TX FIFO data number */
#define  UART_TH_TFLV_GET(reg)               (((reg) & UART_TH_TFLV) >> 8)
#define  UART_TH_RFTH                        ((uint32_t)0x001F0000)            /* UART RX FIFO Threshold */
#define  UART_TH_RFTH_FIELD(x)               ((x)<<16)
#define  UART_TH_RFLV                        ((uint32_t)0x1F000000)            /* UART RX FIFO data number */
#define  UART_TH_RFLV_GET(reg)               (((reg) & UART_TH_RFLV) >> 24)

#define  UART_IE_OE                          ((uint32_t)0x00000001)            /* RX Overrun Error Interrupt Mask */
#define  UART_IE_PE                          ((uint32_t)0x00000002)            /* RX Parity Error Interrupt Mask */
#define  UART_IE_FE                          ((uint32_t)0x00000004)            /* RX Frame Error Interrupt Mask */
#define  UART_IE_BRK                         ((uint32_t)0x00000008)            /* RX Break Character Interrupt Mask */
#define  UART_IE_RXTM                        ((uint32_t)0x00000010)            /* RX Fifo Timeout Interrupt Mask */
#define  UART_IE_RX                          ((uint32_t)0x00000020)            /* RX Fifo Threshold or Rx Data Ready Interrupt Mask */
#define  UART_IE_THRE                        ((uint32_t)0x00000040)            /* Transmitter Holding Register Empty Interrupt Mask */
#define  UART_IE_TIDL                        ((uint32_t)0x00000080)            /* Transmitter Empty interrupt enable */
#define  UART_IE_CTSE                        ((uint32_t)0x00000100)            /* Enable CTS Status Interrupt */
#define  UART_IE_DSRE                        ((uint32_t)0x00000200)            /* Enable DSR Status Interrupt */
#define  UART_IE_RIIN                        ((uint32_t)0x00000400)            /* Enable RI Status Interrupt */
#define  UART_IE_CDIN                        ((uint32_t)0x00000800)            /* Enable CD Status Interrupt */
#define  UART_IE_LBDIE                       ((uint32_t)0x00001000)            /* UART Transmitter Idle Interrupt Mask */
#define  UART_IE_TFTH                        ((uint32_t)0x00010000)            /* TX FIFO Threshold Interrupt Mask */
#define  UART_IE_RFTH                        ((uint32_t)0x00020000)            /* RX FIFO Threshold Interrupt Mask */
#define  UART_IE_IDLEIE                      ((uint32_t)0x00040000)            /* IDLEIE: IDLE interrupt enable */
#define  UART_IE_CHARME                      ((uint32_t)0x00080000)            /* Character match interrupt enable */
#define  UART_IE_RTOIE                       ((uint32_t)0x00100000)            /* Receive time out interrupt enable */
#define  UART_IE_EOBIE                       ((uint32_t)0x00200000)            /* End of block interrupt enable */
#define  UART_IE_RLS                         ((uint32_t)0x0000000F)            /* Receive Line Error Interrupt Mask */

#define  UART_IS_OE                          ((uint32_t)0x00000001)            /* RX Overrun Error Interrupt Status */
#define  UART_IS_PE                          ((uint32_t)0x00000002)            /* RX Parity Error Interrupt Status */
#define  UART_IS_FE                          ((uint32_t)0x00000004)            /* RX Frame Error Interrupt Status */
#define  UART_IS_BRK                         ((uint32_t)0x00000008)            /* RX Break Character Interrupt Status */
#define  UART_IS_RXTM                        ((uint32_t)0x00000010)            /* RX Fifo Timeout Interrupt Status */
#define  UART_IS_RX                          ((uint32_t)0x00000020)            /* RX Fifo Threshold or Rx Data Ready Interrupt Status */
#define  UART_IS_THRE                        ((uint32_t)0x00000040)            /* Transmitter Holding Register Empty Interrupt Status */
#define  UART_IS_TIDL                        ((uint32_t)0x00000080)            /* UART Transmitter Idle Interrupt Status */
#define  UART_IS_CTSE                        ((uint32_t)0x00000100)            /* CTS status change interrupt Status */
#define  UART_IS_DSRE                        ((uint32_t)0x00000200)            /* DSR status change interrupt */
#define  UART_IS_RIIN                        ((uint32_t)0x00000400)            /* RI status change interrupt */
#define  UART_IS_CDIN                        ((uint32_t)0x00000800)            /* CD status change interrupt */
#define  UART_IS_LBDF                        ((uint32_t)0x00001000)            /* UART Modem Interrupt Status */
#define  UART_IS_TFTH                        ((uint32_t)0x00010000)            /* TX FIFO Threshold Interrupt Status */
#define  UART_IS_RFTH                        ((uint32_t)0x00020000)            /* RX FIFO Threshold Interrupt Status */
#define  UART_IS_IDEL                        ((uint32_t)0x00040000)            /* IDLE: Idle line detected */
#define  UART_IS_CMF                         ((uint32_t)0x00080000)            /* CMF: Character match flag */
#define  UART_IS_RLS                         ((uint32_t)0x0000000F)            /* Receive Line Error Interrupt Status */

#define  UART_DMA_REN                        ((uint32_t)0x00000001)            /* Enable RX DMA */
#define  UART_DMA_RTH                        ((uint32_t)0x00001F00)            /* FIFO Threshhold for RX DMA */
#define  UART_DMA_RTH_FIELD(x)               ((x)<<8)
#define  UART_DMA_TEN                        ((uint32_t)0x00010000)            /* Enable TX DMA */
#define  UART_DMA_TTH                        ((uint32_t)0x1F000000)            /* FIFO Threshhold for TX DMA */
#define  UART_DMA_TTH_FIELD(x)               ((x)<<24)

#define  UART_GTPR_PSC                       ((uint32_t)0x000000FF)
#define  UART_GTPR_PSC_FIELD(x)              ((x)<<0)
#define  UART_GTPR_GT                        ((uint32_t)0x0000FF00)
#define  UART_GTPR_GT_FIELD(x)               ((x)<<8)
#define  UART_GTPR_SCARCNT                   ((uint32_t)0x00070000)
#define  UART_GTPR_SCARCNT_FIELD(x)          ((x)<<16)

#define  UART_GTPR_SYNC_DATA_NUM             ((uint32_t)0x07F80000)
#define  UART_GTPR_SYNC_DATA_NUM_FIELD(x)    ((x)<<19)

#define  UART_RTOR_RTO                       ((uint32_t)0x00FFFFFF)
#define  UART_RTOR_RTO_FIELD(x)              ((x)<<0)
#define  UART_RTOR_BLEN                      ((uint32_t)0xFF000000)
#define  UART_RTOR_BLEN_FIELD(x)             ((x)<<24)

/* Base addresses of UART Control Registers */
#define UART0_BASE                           (0x40004000u)
#define UART1_BASE                           (0x40005000u)
#define UART2_BASE                           (0x40006000u)

/* UART Device Pointer */
#define UART0                                ((UART_Device_T *)UART0_BASE)
#define UART1                                ((UART_Device_T *)UART1_BASE)
#define UART2                                ((UART_Device_T *)UART2_BASE)

#define MAX_UART_TX_FIFO_SIZE                16
#define MAX_UART_RX_FIFO_SIZE                16

#define MAX_UART_DEVICE                      3

#endif

#ifdef __cplusplus
}
#endif
#endif /* #ifndef _UART_DEV_H_ */
