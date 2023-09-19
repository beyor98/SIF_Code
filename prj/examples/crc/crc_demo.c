/*====================================================================
 *
 * Copyright (C) 2015 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file crc_demo.c
 *
 * @version 1.0.0
 * 
 * @date 2016/07/27 
 *    
 * @contents Verification and example code of Cyclic redundancy check (CRC). 
 * 
 * @history
 *	   Date			    By			  Comments			    Ver
 *	=========		=========		 =========			 =========
 *	2016/07/27		Mike Zheng		  Create			   1.0.0
 **********************************************************************/
 
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/crc.h>

/******************************************************************************
* Global variables
******************************************************************************/

/******************************************************************************
* Constants and macros
******************************************************************************/
#define DBG_PRINTF
#define ERR_PRINTF

#ifndef DBG_PRINTF
    #define PR_DEBUG(...)       do {} while (0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do {} while (0)
#else
    #define PR_ERR             printf
#endif


/******************************************************************************
* Local types
******************************************************************************/
static CRC_Handle_T s_Handle;

/******************************************************************************
* Local function prototypes
******************************************************************************/

/******************************************************************************
* Local variables
******************************************************************************/
/******************************************************************************
* Local functions
******************************************************************************/
int main (void);
/******************************************************************************
* Global functions
******************************************************************************/

/********************************************************************/

static RET_CODE  CalcCrc7()
{
    RET_CODE ret = RET_OK;
    CRC_Config_T config;
    uint32_t crcResult = 0;
    uint8_t MessageSource[] = {"1234567"};
    /**
     * the exceptCrc is the real val according to CRC Accumulate Parameter config,
     * used to check whether hw calc the result is corret
     */
    uint32_t exceptCrc = 0x2c;     
                                   
    ret = CRC_Init(CRC_DEV, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("\rerr:%d %s:%d\n",ret,__FUNCTION__,__LINE__);
        return ret;
    }

    /* Config CRC Accumulate Parameter */
    config.polynomial        = 0x09;                                  /* cfg crc7 polynomial */                                        
    config.initValue         = 0;                                     /* cfg crc7 init value */    
    config.polyLength        = CRC_POLYLENGTH_7B;                     /* crc7 mode */
    config.inputFormat       = INPUT_FORMAT_BYTES;                    /* inputdata calc crc by byte */
    config.inputInversion    = INPUT_INVERSION_NONE ;                 /* inputdata not inversion */
    config.outputInversionEn = FALSE;                                 /* outputdata not inversion */
    config.xorValue          = 0;                                     /* cfg crc7 xor value */  
    CRC_AccumulateConfig(&s_Handle, &config); 

    /*calc the crc val and compare the result with the execpt crc val*/
    crcResult = CRC_GetAccumulateResult(&s_Handle, &MessageSource[0] ,(sizeof(MessageSource)-1));
    if (crcResult == exceptCrc )
    {
        PR_DEBUG("\rCRC-7/MMC calc success\n"); 
    }
    else
    {
        PR_ERR("CRC7-MMC Fail exceptCrc:0x%x  crcResult:0x%x\n",exceptCrc,crcResult); 
        PR_ERR("polynomial:0x%x\n",config.polynomial);
        PR_ERR("initValue:0x%x\n",config.initValue); 
        PR_ERR("polyLength:0x%x\n",config.polyLength); 
        PR_ERR("inputFormat:0x%x\n",config.inputFormat); 
        PR_ERR("inputInversion:0x%x\n",config.inputInversion);  
        PR_ERR("outputInversionEn:0x%x\n",config.outputInversionEn);  
        PR_ERR("xorValue:0x%x\n",config.xorValue); 
        ret = RET_ERROR; 
    }

    return ret;
}

static RET_CODE  CalcCrc8()
{
    RET_CODE ret = RET_OK;
    CRC_Config_T config;
    uint32_t crcResult = 0;
    uint8_t MessageSource[] = {"1234567"};
    /**
     * the exceptCrc is the real val according to CRC Accumulate Parameter config,
     * used to check whether hw calc the result is corret
     */
    uint32_t exceptCrc = 0x48;  

    ret = CRC_Init(CRC_DEV, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("\rerr:%d %s:%d\n",ret,__FUNCTION__,__LINE__);
        return ret;
    }
    
    /* config to calc crc8-rohc mode */ 
    config.polynomial        = 0x07;                                  /* set crc8 polynomial */                                                          
    config.initValue         = 0xff;                                  /* cfg crc8 init value */
    config.polyLength        = CRC_POLYLENGTH_8B;                     /* crc8 mode */
    config.inputFormat       = INPUT_FORMAT_BYTES;                    /* inputdata calc crc by byte */
    config.inputInversion    = INPUT_INVERSION_BY_BYTE ;              /* inputdata inversion */
    config.outputInversionEn = TRUE;                                  /* outputdata inversion */
    config.xorValue          = 0;                                     /* cfg crc7 xor value */  
    CRC_AccumulateConfig(&s_Handle, &config);
    
    /* calc the crc val and compare the result with the execpt crc val */
    crcResult = CRC_GetAccumulateResult(&s_Handle, &MessageSource[0] ,(sizeof(MessageSource)-1));
    if (crcResult == exceptCrc )
    {
        PR_DEBUG("\rCRC8-ROHC calc success\n"); 
    }
    else
    {
        PR_ERR("CRC8-ROHC Fail exceptCrc:0x%x  crcResult:0x%x\n",exceptCrc,crcResult); 
        PR_ERR("polynomial:0x%x\n",config.polynomial);
        PR_ERR("initValue:0x%x\n",config.initValue); 
        PR_ERR("polyLength:0x%x\n",config.polyLength); 
        PR_ERR("inputFormat:0x%x\n",config.inputFormat); 
        PR_ERR("inputInversion:0x%x\n",config.inputInversion);  
        PR_ERR("outputInversionEn:0x%x\n",config.outputInversionEn);  
        PR_ERR("xorValue:0x%x\n",config.xorValue);                                        
        ret = RET_ERROR; 
    }

    return ret;
}

static RET_CODE  CalcCrc16()
{
    RET_CODE ret = RET_OK;
    CRC_Config_T config;
    uint32_t crcResult = 0;
    uint8_t MessageSource[] = {"1234567"};
    /**
     * the exceptCrc is the real val according to CRC Accumulate Parameter config,
     * used to check whether hw calc the result is corret
     */
    uint32_t exceptCrc = 0x6297;   

    ret = CRC_Init(CRC_DEV, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("\rerr:%d %s:%d\n",ret,__FUNCTION__,__LINE__);
        return ret;
    }
    
    /* config to calc crc16-maxim mode */ 
    config.polynomial        = 0x8005;                                /* set crc16 polynomial */                                                          
    config.initValue         = 0;                                     /* cfg crc16 init value */
    config.polyLength        = CRC_POLYLENGTH_16B;                    /* crc16 mode */
    config.inputFormat       = INPUT_FORMAT_BYTES;                    /* inputdata calc crc by byte */
    config.inputInversion    = INPUT_INVERSION_BY_BYTE ;              /* inputdata not inversion */
    config.outputInversionEn = TRUE;                                  /* outputdata inversion */
    config.xorValue          = 0xffff;                                /* cfg crc16 xor value */  
    CRC_AccumulateConfig(&s_Handle, &config);

    /* calc the crc val and compare the result with the execpt crc val */
    crcResult = CRC_GetAccumulateResult(&s_Handle, &MessageSource[0] ,(sizeof(MessageSource)-1));
    if (crcResult == exceptCrc )
    {
        PR_DEBUG("\rCRC16-MAXIM calc success\n"); 
    }
    else
    {
        PR_ERR("\rCRC16-MAXIM Fail exceptCrc:0x%x  crcResult:0x%x\n",exceptCrc,crcResult); 
        PR_ERR("\rpolynomial:0x%x\n",config.polynomial);
        PR_ERR("\rinitValue:0x%x\n",config.initValue); 
        PR_ERR("\rpolyLength:0x%x\n",config.polyLength); 
        PR_ERR("\rinputFormat:0x%x\n",config.inputFormat); 
        PR_ERR("\rinputInversion:0x%x\n",config.inputInversion);  
        PR_ERR("\routputInversionEn:0x%x\n",config.outputInversionEn);  
        PR_ERR("\rxorValue:0x%x\n",config.xorValue);                                        
        ret = RET_ERROR; 
    }

    return ret;
}


static RET_CODE  CalcCrc32()
{
    RET_CODE ret = RET_OK;
    CRC_Config_T config;
    uint32_t crcResult = 0;
    uint8_t MessageSource[] = {"1234567"};
    /**
     * the exceptCrc is the real val according to CRC Accumulate Parameter config,
     * used to check whether hw calc the result is corret
     */
    uint32_t exceptCrc = 0x5003699f;    
    
    ret = CRC_Init(CRC_DEV, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("\rerr:%d %s:%d\n",ret,__FUNCTION__,__LINE__);
        return ret;
    }
                                        

    /*config to calc crc32 mode*/ 
    config.polynomial  = 0x4c11db7;                                   /* set crc32 polynomial */                                                          
    config.initValue = 0xffffffff;                                    /* cfg crc32 init value */
    config.polyLength = CRC_POLYLENGTH_32B;                           /* crc32 mode */
    config.inputFormat = INPUT_FORMAT_BYTES;                          /* inputdata calc crc by byte */
    config.inputInversion = INPUT_INVERSION_BY_BYTE ;                 /* inputdata inversion */
    config.outputInversionEn = TRUE;                                  /* outputdata inversion */
    config.xorValue = 0xffffffff;                                     /* cfg crc32 xor value */
    CRC_AccumulateConfig(&s_Handle, &config);

    /* calc the crc val and compare the result with the execpt crc val */
    crcResult = CRC_GetAccumulateResult(&s_Handle, &MessageSource[0] ,(sizeof(MessageSource)-1));
    if (crcResult == exceptCrc )
    {
        PR_DEBUG("\rCRC32 calc success\n"); 
    }
    else
    {
        PR_ERR("\rCRC32 Fail exceptCrc:0x%x  crcResult:0x%x\n",exceptCrc,crcResult); 
        PR_ERR("\rpolynomial:0x%x\n",config.polynomial);
        PR_ERR("\rinitValue:0x%x\n",config.initValue); 
        PR_ERR("\rpolyLength:0x%x\n",config.polyLength); 
        PR_ERR("\rinputFormat:0x%x\n",config.inputFormat); 
        PR_ERR("\rinputInversion:0x%x\n",config.inputInversion);  
        PR_ERR("\routputInversionEn:0x%x\n",config.outputInversionEn);  
        PR_ERR("\rxorValue:0x%x\n",config.xorValue);                                        
        ret = RET_ERROR; 
    }

    return ret;
}

int main()
{
    int ret = RET_OK ;

    SysTick_Init(); 
    /* init for uart printf log */
    DebugConsoleInit(115200);

    /*Test crc7*/
    ret = CalcCrc7();
    if (ret != RET_OK)
    {
        PR_ERR("\rCalcCrc7 Error\n");
        goto out;
    }

    /*Test crc8*/
    ret = CalcCrc8();
    if (ret != RET_OK)
    {
        PR_ERR("\rCalcCrc8 Error\n");
        goto out;
    }

    /*Test crc16*/
    ret = CalcCrc16();
    if (ret != RET_OK)
    {
        PR_ERR("\rCalcCrc16 Error\n");
        goto out;
    }

    /*Test crc32*/
    ret = CalcCrc32();
    if (ret != RET_OK)
    {
        PR_ERR("\rCalcCrc32 Error\n");
        goto out;
    }   

    PR_DEBUG("\rCrc Demo Pass\n");
out:
    return ret;
}


