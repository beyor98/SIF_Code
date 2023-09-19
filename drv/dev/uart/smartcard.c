#include <drv/dev/uart.h>

#ifdef CHIP_F6621B

static void pull_poweren_down(void);
static void pull_poweren_up(void);
static void init_reset_gpio(void);
static void init_detect_gpio(void);
static void sc_config(UART_Device_T *pUART);
static void sc_init_atr(void);
static int sc_detect(void);
static int sc_get_atr(uint8_t *card);
static uint8_t sc_decode_atr(uint8_t *card);
static int USART_ByteReceive(UART_Device_T *pUART, uint8_t *Data, uint32_t TimeOut);
static void sc_ptsconfig(void);
static void pull_reset_up(void);
static void pull_reset_down(void);

static uint32_t F_Table[16] = {0, 372, 558, 744, 1116, 1488, 1860, 0,
                               0, 512, 768, 1024, 1536, 2048, 0, 0
                              };
static uint32_t D_Table[8] = {0, 1, 2, 4, 8, 16, 0, 0};

SC_ATR g_atr;
UART_Device_T *gUART = NULL;

int SC_Init(UART_Device_T *pUART)
{
    ASSERT((pUART == UART0) || (pUART == UART1) || (pUART == UART2));
    gUART = pUART;

    init_reset_gpio();
    init_detect_gpio();
    sc_config(pUART);
    sc_init_atr();

    if (sc_detect())
    {
        pull_poweren_down();

        uint8_t atr_str[40] = {0};
        int len = sc_get_atr(atr_str);
        if (len > 0)
        {
            if (sc_decode_atr(atr_str) == T0_PROTOCOL)
            {
                sc_ptsconfig();
                SysTick_DelayMS(400);
                return RET_OK;
            }
            else
            {
                SC_DeInit();
            }
        }
    }
    else
    {
        pull_poweren_up();
    }

    return RET_ERROR;
}

void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus)
{
    uint32_t i = 0;
    uint8_t locData = 0;
    uint8_t locData2 = 0;

    /* Reset responce buffer ---------------------------------------------------*/
    for (i = 0; i < LCmax; i++)
    {
        SC_ResponceStatus->Data[i] = 0;
    }

    SC_ResponceStatus->SW1 = 0;
    SC_ResponceStatus->SW2 = 0;

    /* Send header -------------------------------------------------------------*/
    UART_PutChar(gUART, SC_ADPU->Header.CLA);
    UART_PutChar(gUART, SC_ADPU->Header.INS);
    UART_PutChar(gUART, SC_ADPU->Header.P1);
    UART_PutChar(gUART, SC_ADPU->Header.P2);

    /* Send body length to/from SC ---------------------------------------------*/
    if (SC_ADPU->Body.LC)
    {
        UART_PutChar(gUART, SC_ADPU->Body.LC);
    }
    else if (SC_ADPU->Body.LE)
    {
        UART_PutChar(gUART, SC_ADPU->Body.LE);
    }

    /* --------------------------------------------------------
       Wait Procedure byte from card:
       1 - ACK
       2 - NULL
       3 - SW1; SW2
       -------------------------------------------------------- */

    if ((USART_ByteReceive(gUART, &locData, SC_Receive_Timeout)) == RET_OK)
    {
        if (((locData & (uint8_t)0xF0) == 0x60) || ((locData & (uint8_t)0xF0) == 0x90))
        {
            SC_ResponceStatus->SW1 = locData;
            if ((USART_ByteReceive(gUART, &locData2, SC_Receive_Timeout)) == RET_OK)
            {
                SC_ResponceStatus->SW2 = locData2;
            }
        }
        else if (((locData & (uint8_t)0xFE) == (((uint8_t)~(SC_ADPU->Header.INS)) & (uint8_t)0xFE)) ||
                 ((locData & (uint8_t)0xFE) == (SC_ADPU->Header.INS & (uint8_t)0xFE)))
        {
            SC_ResponceStatus->Data[0] = locData; /* ACK received */
        }
    }

    /* If no status bytes received ---------------------------------------------*/
    if (SC_ResponceStatus->SW1 == 0x00)
    {
        /* Send body data to SC--------------------------------------------------*/
        if (SC_ADPU->Body.LC)
        {
            for (i = 0; i < SC_ADPU->Body.LC; i++)
            {
                UART_PutChar(gUART, SC_ADPU->Body.Data[i]);
            }
        }

        /* Or receive body data from SC ------------------------------------------*/
        else if (SC_ADPU->Body.LE)
        {
            for (i = 0; i < SC_ADPU->Body.LE; i++)
            {
                if (USART_ByteReceive(gUART, &locData, SC_Receive_Timeout) == RET_OK)
                {
                    SC_ResponceStatus->Data[i] = locData;
                }
            }
        }

        /* Wait SW1 --------------------------------------------------------------*/
        i = 0;
        while (i < 10)
        {
            if (USART_ByteReceive(gUART, &locData, SC_Receive_Timeout) == RET_OK)
            {
                SC_ResponceStatus->SW1 = locData;
                i = 11;
            }
            else
            {
                i++;
            }
        }

        /* Wait SW2 ------------------------------------------------------------*/
        i = 0;
        while (i < 10)
        {
            if (USART_ByteReceive(gUART, &locData, SC_Receive_Timeout) == RET_OK)
            {
                SC_ResponceStatus->SW2 = locData;
                i = 11;
            }
            else
            {
                i++;
            }
        }
    }
}

void SC_DeInit(void)
{
    pull_reset_up();
}


static void sc_init_atr(void)
{
    uint32_t i = 0;

    g_atr.TS = 0;
    g_atr.T0 = 0;
    for (i = 0; i < SETUP_LENGTH; i++)
    {
        g_atr.T[i] = 0;
    }
    for (i = 0; i < HIST_LENGTH; i++)
    {
        g_atr.H[i] = 0;
    }
    g_atr.Tlength = 0;
    g_atr.Hlength = 0;
}

/*
    DATA   -- IO115 -- USARTA_DATA
    POWEN  -- IO116 -- XPA0
    CLOCK  -- IO117 -- USARTA_CLK
    RESET  -- IO118 -- XPC11
    DETECT -- IO119 -- XPA13
*/
static int sc_get_atr(uint8_t *card)
{
    uint8_t data = 0;
    uint32_t count = 0;

    //pull down RST
    pull_reset_down();

    //pull up DATA
    //DATA default is UP

    //sc clock is 3MHZ, 400 clock <= fff <= 40000 clock
    //400 * (1 / 3000000) = 133.33us
    SysTick_DelayUS(500);

    //pull up RST
    pull_reset_up();

    while (1)
    {
        if ((USART_ByteReceive(gUART, &data, SC_Receive_Timeout)) == RET_OK)
        {
            card[count++] = data;
        }
        else
        {
            break;
        }
    }

    int i = 0;
    for (; i < count; i++)
    {
        printf("atr data:0x%02x\n", card[i]);
    }
    printf("atr byte count = 0x%02x\n", count);

    return count;
}

static uint8_t sc_decode_atr(uint8_t *card)
{
    uint32_t i = 0, flag = 0, buf = 0, protocol = 0;

    g_atr.TS = card[0];  /* Initial character */
    g_atr.T0 = card[1];  /* Format character */

    g_atr.Hlength = g_atr.T0 & (uint8_t)0x0F;

    if ((g_atr.T0 & (uint8_t)0x80) == 0x80)
    {
        flag = 1;
    }

    for (i = 0; i < 4; i++)
    {
        g_atr.Tlength = g_atr.Tlength + (((g_atr.T0 & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
    }

    for (i = 0; i < g_atr.Tlength; i++)
    {
        g_atr.T[i] = card[i + 2];
    }

    //T = 0 when TD1 not exist;
    //T = low-4-bits in TD1 when TD1 exists
    protocol = g_atr.T[g_atr.Tlength - 1] & (uint8_t)0x0F;

    while (flag)
    {
        if ((g_atr.T[g_atr.Tlength - 1] & (uint8_t)0x80) == 0x80)
        {
            flag = 1;
        }
        else
        {
            flag = 0;
        }

        buf = g_atr.Tlength;
        g_atr.Tlength = 0;

        for (i = 0; i < 4; i++)
        {
            g_atr.Tlength = g_atr.Tlength + (((g_atr.T[buf - 1] & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
        }

        for (i = 0; i < g_atr.Tlength; i++)
        {
            g_atr.T[buf + i] = card[i + 2 + buf];
        }
        g_atr.Tlength += (uint8_t)buf;
    }

    for (i = 0; i < g_atr.Hlength; i++)
    {
        g_atr.H[i] = card[i + 2 + g_atr.Tlength];
    }

    return (uint8_t)protocol;
}


static void sc_ptsconfig(void)
{
    uint8_t locData = 0;
    int PTSConfirmStatus = 0x01;
    int div = 0;

    if ((g_atr.T0 & (uint8_t)0x10) == 0x10)
    {
        if (g_atr.T[0] != 0x11)  //g_atr.T[0] == 0x11 ==> fi=372, di=1, default value not need to set
        {
            /* Send PTSS */
            UART_PutChar(gUART, 0xFF);

            /* Send PTS0 */
            UART_PutChar(gUART, 0x10);   //0x10 ==> PPS1 and T=0

            /* Send PTS1 */
            UART_PutChar(gUART, g_atr.T[0]);  //TA1 contains new baud-div, eg 0x94 ==> fi=512, di=8

            /* Send PCK */
            UART_PutChar(gUART, (uint8_t)0xFF ^ (uint8_t)0x10 ^ (uint8_t)g_atr.T[0]);

            if ((USART_ByteReceive(gUART, &locData, SC_Receive_Timeout)) == RET_OK)
            {
                if (locData != 0xFF)
                {
                    PTSConfirmStatus = 0x00;
                }
            }

            if ((USART_ByteReceive(gUART, &locData, SC_Receive_Timeout)) == RET_OK)
            {
                if (locData != 0x10)
                {
                    PTSConfirmStatus = 0x00;
                }
            }

            if ((USART_ByteReceive(gUART, &locData, SC_Receive_Timeout)) == RET_OK)
            {
                if (locData != g_atr.T[0])
                {
                    PTSConfirmStatus = 0x00;
                }
            }
            if ((USART_ByteReceive(gUART, &locData, SC_Receive_Timeout)) == RET_OK)
            {
                if (locData != ((uint8_t)0xFF ^ (uint8_t)0x10 ^ (uint8_t)g_atr.T[0]))
                {
                    PTSConfirmStatus = 0x00;
                }
            }
            else
            {
                PTSConfirmStatus = 0x00;
            }

            /* PTS Confirm */
            if (PTSConfirmStatus == 0x01)
            {
                div = F_Table[((g_atr.T[0] >> 4) & (uint8_t)0x0F)] / D_Table[(g_atr.T[0] & (uint8_t)0x0F)];
                IO_BITMASK_SET(gUART->TC, UART_TC_SCLK, UART_TC_SCLK_FIELD(div));
            }
        }
    }
}

static int USART_ByteReceive(UART_Device_T *pUART, uint8_t *Data, uint32_t TimeOut)
{
    uint32_t Counter = 0;

    while (!(pUART->LS & UART_LS_DR) && Counter <= TimeOut)
    {
        Counter++;
    }
    printf("Counter=0x%x\n", Counter);

    if (Counter <= TimeOut)
    {
        *Data = (uint8_t)UART_GetChar(pUART);
        return RET_OK;
    }
    else
    {
        return RET_ERROR;
    }
}

static void sc_config(UART_Device_T *pUART)
{
    //set uart clock
    //b00 - 1  48M / 1 = 48M
    //b01 - 2
    //b10 - 3  48M / 3 = 16M
    //b11 - 4
    IO_BIT_SET(SYSDEV->CLK1, 0x0);

    //enable sc
    IO_BIT_SET(pUART->C2, UART_C2_SCEN);
    IO_BIT_CLEAR(pUART->C1, UART_C1_HDEN);
    IO_BIT_CLEAR(pUART->C1, UART_C1_IREN);
    IO_BIT_CLEAR(pUART->C2, UART_C2_LINEN);

    //8 data bits
    IO_BITMASK_SET(pUART->C3, UART_C3_DTB, UART_DATABITS_8);

    //1.5 bits stop
    //   00 - 1 stopbit
    //   01 - 0.5 stopbit
    //   10 - 2 stopbit
    //   11 - 1.5 stopbit
    IO_BITMASK_SET(pUART->C2, UART_C2_TXSLEN, UART_C2_TXSLEN_FIELD(0x3));

    //config EVEN parity
    IO_BIT_SET(pUART->C3, UART_C3_PEN);
    IO_BIT_SET(pUART->C3, UART_C3_EOP);

    //set default uart baudrate: 48M/16/372 = 8064
    IO_BITMASK_SET(pUART->TC, UART_TC_SCLK, UART_TC_SCLK_FIELD(372));

    //set sc psc
    //00000: Reserved - do not program this value
    //00001: divides the source clock by 2
    //00010: divides the source clock by 4
    //00011: divides the source clock by 6
    //00100: divides the source clock by 8
    //00101: divides the source clock by 10
    //00110: divides the source clock by 12
    //00111: divides the source clock by 14
    //01000: divides the source clock by 16
    //uart 48M/(8*2) = 3M
    IO_BITMASK_SET(pUART->GTPR, UART_GTPR_PSC, UART_GTPR_PSC_FIELD(0x8));

    //set sc guard time to 2 bit
    IO_BITMASK_SET(pUART->GTPR, UART_GTPR_GT, UART_GTPR_GT_FIELD(2));

    //set sc NACK
    IO_BIT_SET(pUART->C2, UART_C2_NACK);

    //disable fifo
    IO_BIT_CLEAR(pUART->C2, UART_C2_FFEN);
}

static void pull_poweren_up(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinWrite(GPIOA0, GPIO_PIN_SET);
}

static void pull_poweren_down(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InitStruct.pin = GPIO_PIN_0;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_PinWrite(GPIOA0, GPIO_PIN_RESET);
}

static void init_detect_gpio(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InitStruct.pin = GPIO_PIN_13;
    GPIO_InitStruct.mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void init_reset_gpio(void)
{
    GPIO_PinConfig_T GPIO_InitStruct;
    GPIO_InitStruct.pin = GPIO_PIN_11;
    GPIO_InitStruct.mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.pull = GPIO_NOPULL;
    GPIO_InitStruct.alternate = 0;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
}

static int sc_detect(void)
{
    return GPIO_PinRead(GPIOA13);
}

static void pull_reset_up(void)
{
    GPIO_PinWrite(GPIOC11, GPIO_PIN_SET);
}

static void pull_reset_down(void)
{
    GPIO_PinWrite(GPIOC11, GPIO_PIN_RESET);
}

#endif
