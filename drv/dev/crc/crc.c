/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file          CRC.c
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 * 
 * @date          2016/04/01 
 *    
 * @brief         Header file of CRC driver.
 * 
 * @note
 *    2016/04/01, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/07/05, Mike.Zheng, V1.1.0
 *        Update coding style.
 */
 
#include <drv/dev/CRC.h>

static CRC_Handle_T *s_pHandle = NULL;

static RET_CODE CRC_WaitCalcFinish(CRC_Device_T *pCRC, int32_t timeoutUs);
static uint32_t CRC_Handle8(CRC_Device_T *pCRC, uint8_t *pBuffer, uint32_t length); 
static uint32_t CRC_Handle16(CRC_Device_T *pCRC, uint16_t *pBuffer, uint32_t length);
static uint32_t CRC_Handle32(CRC_Device_T *pCRC, uint32_t *pBuffer, uint32_t length);

static RET_CODE CRC_WaitCalcFinish(CRC_Device_T *pCRC, int32_t timeoutUs)
{
    RET_CODE ret =  RET_OK; 
    WaitTimer_T waitTimer;
    /* Initialize timeout structure */
    WaitTimerInit(&waitTimer, timeoutUs, 1);

    while (1) 
    {
        /* Exit if crc calc finish */
        if (pCRC->ST & CRC_ST_DNE)
        {
            break;
        }
		/* Exit when timeout */
        if (WaitTillTimeout(&waitTimer) == RET_TIMEOUT)
        {
            ret = RET_TIMEOUT;
            break;
        }
    }

    return ret;
}

RET_CODE CRC_Init(CRC_Device_T *pCRC, CRC_Handle_T *hCRC)
{
    RET_CODE ret = RET_OK;
   
    if (!(pCRC  && hCRC))
    {
        CRC_PR_ERR("\r%s: input par error\r\n",__FUNCTION__);
        ret = RET_INVPARAM;
        goto out; 
    }
    /*Crc syschip init*/
    CRC_RESET();
    CRC_CLOCK_ENABLE();
    /* Remove Compler Warning */
    s_pHandle = s_pHandle;
    /* Remember Usr Define Handle */
    s_pHandle = hCRC;
    hCRC->device = pCRC;
    XORVAL_ENABLE(pCRC);
    INTERRUPT_DISABLE(pCRC);
out:
    return ret;
}

/**             
  * @brief  Enter 8-bit inputdata to the CRC calculator.
  *         Specific data handling to optimize processing time.  
  * @param  pCRC: CRC device point
  * @param  pBuffer: pointer to the input data buffer
  * @param  length: input data buffer length(byte length)
  * @retval uint32_t CRC 
*/

static uint32_t CRC_Handle8(CRC_Device_T *pCRC, uint8_t *pBuffer, uint32_t length)

{
    uint32_t i = 0; /* input data buffer index */

    /* Processing time optimization: 4 bytes are entered in a row with a single word write,
     * last bytes must be carefully fed to the CRC calculator to ensure a correct type
     * handling by the IP
     */

    /* calc 32bit at one time */
    SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_32B);
    for (i = 0; i < (length / 4); i++)
    {
        pCRC->DA = (((uint32_t)pBuffer[4*i]<<24)      |\
                      ((uint32_t)pBuffer[4*i+1]<<16)    |\
                      ((uint32_t)pBuffer[4*i+2]<<8)     |\
                      ((uint32_t)pBuffer[4*i+3]));
    }

    /* last bytes specific handling according different byte rest */
    if (length % 4)
    {
        /* calc 8bit at one time */
        if (length % 4 == 1)
        {
            SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_8B); 
            pCRC->DA = pBuffer[4*i];
        }
        /* calc 16bit at one time */ 
        else if (length % 4 == 2)
        {
            SET_DATA_PROCESS_BITWIDTH(pCRC,DATA_PROCESS_WIDTH_16B); 
            pCRC->DA = (pBuffer[4*i] << 8) | (pBuffer[4*i+1]);
        }
        else if (length % 4 == 3)
        {
            /* calc 16bit at one time */ 
            SET_DATA_PROCESS_BITWIDTH(pCRC,DATA_PROCESS_WIDTH_16B); 
            pCRC->DA = (pBuffer[4*i] << 8) | (pBuffer[4*i+1]);
            /* calc 8bit at one time */ 
            SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_8B);
            pCRC->DA = pBuffer[4*i+2];       
        }
    }

    if(CRC_WaitCalcFinish(pCRC, CRC_ACCUMULATE_TIEMOUT_US) == RET_TIMEOUT)
    {
        CRC_PR_ERR("\r%s:CRC format words accumulate timeout\n",__FUNCTION__);

    }
    /* Return the CRC computed value */ 
    return pCRC->RST;
}

/**             
 * @brief  Enter 16-bit input data to the CRC calculator.
 *         Specific data handling to optimize processing time.  
 * @param  pCRC: CRC device point
 * @param  pBuffer: pointer to the input data buffer
 * @param  length: input data buffer length(half word length)
 * @retval uint32_t CRC 
*/  
static uint32_t CRC_Handle16(CRC_Device_T *pCRC, uint16_t *pBuffer, uint32_t length)
{
    uint32_t i = 0;  /* input data buffer index */
    
    if (length % 2)
    {
        CRC_PR_ERR("\rerr Buffer_Length %d INPUT_FORMAT_HALFWORDS Buffer_Length\n",length);
        return RET_INVPARAM;
    }
    
    /**
     * Processing time optimization:
     * 2 HalfWords are entered in a row with a single word write,
     * in case of odd length, last HalfWord must be carefully fed to
     * the CRC calculator to ensure correct type handling by the IP 
     */

    /*byte length convert to HalfWords length*/  
    length = length >> 1;
    /* calc 32bit at one time */
    for (i = 0; i < (length/2); i++)
    {
        SET_DATA_PROCESS_BITWIDTH(pCRC,DATA_PROCESS_WIDTH_32B); 
        pCRC->DA = (pBuffer[2*i]<<16) | pBuffer[2*i+1];     
    }
    /* last bytes specific handling according different byte rest*/  
    if (length % 2)
    {
        /* calc 16bit at one time */ 
        SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_16B); 
        pCRC->DA = pBuffer[2*i]; 
    }
    
    if (CRC_WaitCalcFinish(pCRC, CRC_ACCUMULATE_TIEMOUT_US) == RET_TIMEOUT)
    {
        CRC_PR_ERR("\r%s:CRC format words accumulate timeout\n",__FUNCTION__);
        return RET_TIMEOUT;
    }

    /* Return the CRC computed value */ 
    return pCRC->RST;
}

/**             
  * @brief  Enter 32-bit inputdata to the CRC calculator. 
  * @param  pCRC: CRC device point
  * @param  pBuffer: pointer to the input data buffer
  * @param  length: input data buffer length(byte length)
  * @retval uint32_t CRC 
*/

static uint32_t CRC_Handle32(CRC_Device_T *pCRC, uint32_t *pBuffer, uint32_t length)
{
    uint32_t i = 0; /* input data buffer index */

    if (length % 4)
    {
        CRC_PR_ERR("\rerr Buffer_Length %d INPUT_FORMAT_WORDS Buffer_Length\n",length);
        return RET_INVPARAM;
    }

    SET_DATA_PROCESS_BITWIDTH(pCRC, DATA_PROCESS_WIDTH_32B);

    /*byte length convert to word length*/
    length = length >> 2;
    /**
     * Processing time optimization: 4 bytes are entered in a row with a single word write, 
     * calc 32bit at one time 
     */
    for (i = 0; i < length; i++)
    {
        pCRC->DA = pBuffer[i];
    }
    if (RET_TIMEOUT == CRC_WaitCalcFinish(pCRC, CRC_ACCUMULATE_TIEMOUT_US))
    {
        CRC_PR_ERR("\r%s:CRC format words accumulate timeout\n",__FUNCTION__);
        return RET_TIMEOUT;
    }
    
    /* Return the CRC computed value */ 
    return pCRC->RST;
}


void CRC_AccumulateConfig(CRC_Handle_T *hCRC, CRC_Config_T *pConfig)
{
    CRC_Device_T *pCRC = hCRC->device;

    hCRC->inputFormat = pConfig->inputFormat;
    SET_INITVAL(pCRC, pConfig->initValue);
    SET_POL_BITWIDTH(pCRC, pConfig->polyLength);
    SET_POL(pCRC, pConfig->polynomial);
    SET_XORVAl(pCRC, pConfig->xorValue);
    SET_INPUTDATA_INVERSION(pCRC, pConfig->inputInversion);
    SET_OUTPUTDATA_INVERSION(pCRC, pConfig->outputInversionEn);
}

uint32_t CRC_GetAccumulateResult(CRC_Handle_T *hCRC, uint8_t *pBuffer, uint32_t length)
{
    uint32_t temp = 0;  /* CRC calc result  */
	
    CRC_Device_T *pCRC = hCRC->device;
    /* get usr set inputFormat */
    CRC_InputFormat_T inputFormat = hCRC->inputFormat;
    /*load init data,ready for start calc*/
    CALC_RESTART(pCRC);
    
    switch (inputFormat)
    {
    case INPUT_FORMAT_BYTES:         
        temp = CRC_Handle8(pCRC, (uint8_t*)pBuffer, length);
        break; 
     case INPUT_FORMAT_HALFWORDS: 
        temp = CRC_Handle16(pCRC, (uint16_t*)pBuffer, length);
        break;   
    case INPUT_FORMAT_WORDS:
        temp = CRC_Handle32(pCRC, (uint32_t*)pBuffer, length);
        break;

    default:
        
        CRC_PR_ERR("\r%s:%d inputdata format error\n",__FUNCTION__,__LINE__);
        temp = RET_INVPARAM;
        break;
	}

    return temp;
}




