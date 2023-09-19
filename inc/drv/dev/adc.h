/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file             adc.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2016/06/29
 *
 * @brief         Header file of adc driver.
 *
 * @note
 *    2016/04/08, Mike.Zheng, V1.0.0
 *        Initial version.
 *    2016/04/11, Mike.Zheng, V1.1.0
 *        Update coding style.
 *    2016/11/22, Mike.Zheng, V1.2.0
 *        Update For Release.
 */

#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>
#if defined(CHIP_GM6621)
#include <drv/dev/pes.h>
#endif
#define HW32_REG(ADDRESS)  (*((volatile unsigned long  *)(ADDRESS)))
/*Declaration of ADC handle structure*/
struct ADC_Handle;

#define MAX_SEQUENCE_NUM                   8        /**< ADC max sequence sample channel num for one time */

/**
 * adc compare match mode
 */
typedef enum
{
    ADC_COMPARE_MORE_THAN                  = 0x00,  /**< When convert value more than compareHighValue will send a event to usr */
    ADC_COMPARE_EQUAL_TO,                           /**< When convert equal compareHighValue will send a event to usr */
    ADC_COMPARE_LESS_THAN,                          /**< When convert value less than compareHighValue will send a event to usr */
    ADC_COMPARE_WITHIN_RANGE                        /**< When convert value between compareLowValue and compareHighValue will send a event to usr */
} ADC_CompareMode_T;

/**
 * ADC triger mode
 */
typedef enum
{
    ADC_SINGLE_MODE                        = 0x01,    /**< Need sw triger to start convet everytime */
} ADC_ContinousMode_T;

/**
 * ADC resolution
 */
typedef enum
{
    ADC_RESOLUTION_12BIT                   = 0x00,   /**< Resolution of analog to digital conversion is 12bit */
    ADC_RESOLUTION_10BIT,                            /**< Resolution of analog to digital conversion is 10bit */
    ADC_RESOLUTION_8BIT                              /**< Resolution of analog to digital conversion is 8bit */
} ADC_Resolution_T;

/**
 * ADC sample time
 */
typedef enum
{
#if defined(CHIP_GM6601)
    ADC_SAMPLETIME_2CLK                    = 0x00,  /**< Convert sample time is 2.5 adcclk */
    ADC_SAMPLETIME_6CLK,                            /**< Convert ample timeis 6.5 adcclk */
    ADC_SAMPLETIME_16CLK,                           /**< Convert sample timeis 16.5 adcclk */
    ADC_SAMPLETIME_26CLK                            /**< Convert sample time is 26.5 adcclk */
#elif (defined(CHIP_GM6611) || defined(CHIP_GM6611B)||defined(CHIP_GM6601B)||defined(CHIP_F6601C)  || defined(CHIP_GM6621))
    ADC_SAMPLETIME_3CLK                    = 0x00,  /**< Convert sample time is 3 adcclk */
    ADC_SAMPLETIME_4CLK,                            /**< Convert ample timeis 4 adcclk */
    ADC_SAMPLETIME_6CLK,                            /**< Convert sample timeis 6 adcclk */
    ADC_SAMPLETIME_8CLK,                            /**< Convert sample time is 8 adcclk */
    ADC_SAMPLETIME_12CLK,                           /**< Convert sample time is 12 adcclk */
    ADC_SAMPLETIME_16CLK,                           /**< Convert ample timeis 16 adcclk */
    ADC_SAMPLETIME_24CLK,                           /**< Convert sample timeis 24 adcclk */
    ADC_SAMPLETIME_32CLK                            /**< Convert sample time is 32 adcclk */
#endif
} ADC_SampleTime_T;

#if (defined(CHIP_GM6611)||defined(CHIP_GM6611B)|| defined(CHIP_GM6601B) ||defined(CHIP_F6601C) || (defined(CHIP_GM6621)))
/**
 * ADC sample time
 */
typedef enum
{
    ADC_SAMPLE_POSITIVE                    = 0x00,  /**< Sample at positive edge of ADC clock */
    ADC_SAMPLE_NEGATIVE                             /**< Sample at negative edge of ADC clock */
} ADC_SampleEdge_T;

/**
 * ADC triger source
 */
typedef enum
{
#if (defined(CHIP_GM6611)||defined(CHIP_GM6611B)|| defined(CHIP_GM6601B)||defined(CHIP_F6601C) )
    ADC_RTC2_TRIGER,                                /**<  ADC tiger by rtc2 */
    ADC_ETM0_TRIGER,                                /**<  ADC tiger by etm0 */
    ADC_ETM1_TRIGER,                                /**<  ADC tiger by etm1 */
    ADC_ETM2_TRIGER,                                /**<  ADC tiger by etm2 */
    ADC_ETM3_TRIGER,                                /**<  ADC tiger by etm3 */
    ADC_ACMP_TRIGER,                                /**<  ADC tiger by acmp */
    ADC_GPIO_TRIGER,                                /**<  ADC tiger by gpio */
    ADC_SW_TRIGER                                   /**<  ADC tiger by software */
#elif (defined(CHIP_GM6621))
    ADC_HW_TRIGER_PESCH0                    = 0x0,    /**< Set PES channel 0 as hw triger source */
    ADC_HW_TRIGER_PESCH1,                             /**< Set PES channel 1 as hw triger source */
    ADC_HW_TRIGER_PESCH2,                             /**< Set PES channel 2 as hw triger source */
    ADC_HW_TRIGER_PESCH3,                             /**< Set PES channel 3 as hw triger source */
    ADC_HW_TRIGER_PESCH4,                             /**< Set PES channel 4 as hw triger source */
    ADC_HW_TRIGER_PESCH5,                             /**< Set PES channel 5 as hw triger source */
    ADC_HW_TRIGER_PESCH6,                             /**< Set PES channel 6 as hw triger source */
    ADC_HW_TRIGER_PESCH7,                             /**< Set PES channel 7 as hw triger source */
    ADC_HW_TRIGER_PESCH8,                             /**< Set PES channel 8 as hw triger source */
    ADC_HW_TRIGER_PESCH9,                             /**< Set PES channel 9 as hw triger source */
    ADC_HW_TRIGER_PESCH10,                            /**< Set PES channel 10 as hw triger source */
    ADC_HW_TRIGER_PESCH11,                            /**< Set PES channel 11 as hw triger source*/
#endif
} ADC_TrigerSource_T;
#endif

/**
 * ADC channel index
 */
typedef enum
{
    ADC_CHANNEL_0                          = 0x00,  /**< ADC sample channel num index is 0 */
    ADC_CHANNEL_1,                                  /**< ADC sample channel num index is 1 */
    ADC_CHANNEL_2,                                  /**< ADC sample channel num index is 2 */
    ADC_CHANNEL_3,                                  /**< ADC sample channel num index is 3 */
    ADC_CHANNEL_4,                                  /**< ADC sample channel num index is 4 */
    ADC_CHANNEL_5,                                  /**< ADC sample channel num index is 5 */
    ADC_CHANNEL_6,                                  /**< ADC sample channel num index is 6 */
    ADC_CHANNEL_7,                                  /**< ADC sample channel num index is 7 */
    ADC_CHANNEL_8,                                  /**< ADC sample channel num index is 8 */
    ADC_CHANNEL_9,                                  /**< ADC sample channel num index is 9 */
    ADC_CHANNEL_10,                                 /**< ADC sample channel num index is 10 */
    ADC_CHANNEL_11,                                 /**< ADC sample channel num index is 11 */
    ADC_CHANNEL_12,                                 /**< ADC sample channel num index is 12 */
    ADC_CHANNEL_13,                                 /**< ADC sample channel num index is 13 */
    ADC_CHANNEL_14,                                 /**< ADC sample channel num index is 14 */
    ADC_CHANNEL_15,                                 /**< ADC sample channel num index is 15 */
#if (defined(CHIP_GM6621))
    ADC_CHANNEL_16,
    ADC_CHANNEL_17,
    ADC_CHANNEL_18,
    ADC_CHANNEL_19,
    ADC_CHANNEL_20,
    ADC_CHANNEL_21,
    ADC_CHANNEL_22,
    ADC_CHANNEL_23,
    ADC_CHANNEL_24,
    ADC_CHANNEL_25,
    ADC_CHANNEL_26,                                 /**< ADC sample channel num index is 26, special,only detect battery vol */
    ADC_CHANNEL_27,
    ADC_CHANNEL_28,
    ADC_CHANNEL_29,
    ADC_CHANNEL_30,
    ADC_CHANNEL_31,
#endif
} ADC_ChannelIndex_T;

/**
 * ADC  triger mode, for GM6621
 */
#if (defined(CHIP_GM6621))
typedef enum
{
    ADC_SW_TRIGER                          = 0x0,    /**< ADC software triger mode */
    ADC_HW_TRIGER                                    /**< ADC hardware triger mode */
} ADC_TrigerMode_T;

typedef enum
{
    XAVREF                          = 0x0,    /**< used for ADC VREFH */
    VCCANA                                    /**< used for ADC Battery Detect */
} ADC_PWRSEL_T;

#endif

/**
 * Urs revive event type from driver
 */
typedef enum
{
    ADC_EVENT_COMPERE_MATCH                = 0x0,  /**< When convert value match the compare value will send this event to usr */
    ADC_EVENT_OVERFLOW,                            /**< When convet data fifo overflow will send ths event to usr */
    ADC_EVENT_FINISHED,                            /**< When convet finish overflow will send ths event to usr */
    ADC_EVENT_CONVERT_ERROR
} ADC_Event_T;

/**
* ADC compare parameter  structure.
*/
typedef struct
{
    uint16_t                    compareLowValue ;       /**< ADC compare match low vaule */
    uint16_t                    compareHighValue;       /**< ADC compare match high vaule */
    ADC_CompareMode_T           compareMode;            /**< ADC compare mode
                                                          * 1: Usr set compareMode to ADC_COMPARE_EQUAL_TO ,when adc convet
                                                          *    value =  compareHighValue , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          * 2: Usr set compareMode to ADC_COMPARE_MORE_THAN ,when adc convet
                                                          *    value >  compareHighValue , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          * 3: Usr set compareMode to ADC_COMPARE_LESS_THAN ,when adc convet
                                                          *    value <  compareHighValue , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          * 4: Usr set compareMode to ADC_COMPARE_WITHIN_RANGE ,when adc convet
                                                          *    (value > compareLowValue && value < compareHighValue)  , it will generate sent event to urs , if usr register
                                                          *    Callback and enable compare function
                                                          */
} ADC_CompareConfig_T;

typedef void (*ADC_Callback_T)(struct ADC_Handle *hADC, ADC_Event_T event, unsigned long param);
#if (defined(CHIP_GM6621))
typedef void (*ADC_PesTrigerSource_T)(struct ADC_Handle *hADC);
#endif
/**
* ADC common config structure.
*/
typedef struct
{
    uint32_t                    enableDma;              /**< Whether use dma to transfer data */
    ADC_Callback_T              callback;               /**< Driver send usr event to call this function */
    ADC_ContinousMode_T         continuousMode;         /**< ADC convet work mode : sigal or repeat */
#if (defined(CHIP_GM6611)||defined(CHIP_GM6611B)|| defined(CHIP_GM6601B)||defined(CHIP_F6601C)  || defined(CHIP_GM6621))
    ADC_TrigerSource_T          trigerSource;            /**< ADC triger source : refer to struct ADC_TrigerSource_T */
#endif
#if (defined(CHIP_GM6621))
    ADC_PesTrigerSource_T       pestrigersrcfunc;        /**< ADC pes triger source function */
    ADC_TrigerMode_T            trigerMode;              /**< Select sw, or hw triger */
    uint8_t                     enableDisconMode;        /**< Whether use discontinuous mode */
    uint8_t                     lowPowerEn;              /**< ADC low power mode enable */
    uint8_t                     power_supply_sel;
#endif
} ADC_CommonConfig_T;

/**
* ADC handle structure.
*/
typedef struct ADC_Handle
{
    uint8_t                     lock;                               /**< ADC device lock */
    uint8_t                     dmaEn;                              /**<  Enable:dma mode tx data  Disable:CPU mode tx data */
    volatile ADC_BLOCKSTATUS_T  blockStatus;                        /**< Blockenable eable: blockpoll read value blockEnable DISABLE: noblock read value */
    volatile ADC_ConvertStatus_T status;                             /**< ADC machine status */
    uint8_t                     expectFinishStatus;                 /**< Check use dam or cpu mode to wait adc convert finish */
    uint8_t                     length;                             /**< Adc SequenceChannle length */
    uint32_t                   *pConvertData;                       /**< Point to adc convert buf */
    uint8_t                     pSequence[MAX_SEQUENCE_NUM];        /**< Point to adc chanel sequence */
    ADC_Resolution_T            resolution;                         /**< ADC convet value precise */
    ADC_ContinousMode_T         continuousMode;                     /**< ADC convet work mode : sigal or repeat */
#if (defined(CHIP_GM6611)|| defined(CHIP_GM6601B) ||defined(CHIP_F6601C) || defined(CHIP_GM6611B)|| defined(CHIP_GM6621))
    ADC_TrigerSource_T          trigerSource;                       /**< ADC triger source : refer to struct ADC_TrigerSource_T */
#endif
    ADC_Device_T               *device;                             /**< Pointer to adc device handle */
    DMA_Handle_T               *pDMAHandle;                         /**< Pointer to DMA process Hander */
    ADC_Callback_T              callback;                           /**< Driver send usr event to call this function */
#if (defined(CHIP_GM6621))
    ADC_PesTrigerSource_T       pestrigersrcfunc;                   /**< ADC pes triger source function */
    ADC_TrigerMode_T            trigerMode;                         /**< select sw, or hw triger */
    uint8_t                     enableDisconMode;                   /**< Whether use discontinuous mode */
    uint8_t                     lowPowerEn;                        /**< ADC low power mode enable */
    uint8_t                     power_supply_sel;
#endif

} ADC_Handle_T;

/**
* ADC channel config structure.
*/
typedef struct
{
    ADC_Resolution_T            resolution;             /**< ADC convet value precise */
    ADC_SampleTime_T            sampleTime;             /**< ADC sample time */
    ADC_ChannelIndex_T          channelIndex;           /**< ADC sample channel num */
#if (defined(CHIP_GM6611) || defined(CHIP_GM6601B) ||defined(CHIP_F6601C) ||defined(CHIP_GM6611B) || defined(CHIP_GM6621))
    ADC_SampleEdge_T            sampleEdge;             /**< ADC end of convert plus signal sample */
    uint8_t                     DataFifoFullWrEn;       /**< ADC data fifo write enable when it's full */
#endif
} ADC_ChannelConfig_T;

/**
* ADC sequence config structure.
*/
typedef struct
{
    ADC_Resolution_T            resolution;             /**< ADC convet value precise */
    ADC_SampleTime_T            sampleTime;             /**< ADC sample time */
    uint8_t                    *pSequence;              /**< Point to adc sequence buf */
    uint8_t                     length;                 /**< Sequence length, it should no more than MAX_SEQUENCE_NUM */
#if (defined(CHIP_GM6611) ||defined(CHIP_GM6611B)|| defined(CHIP_GM6601B) ||defined(CHIP_F6601C) || defined(CHIP_GM6621))
    ADC_SampleEdge_T            sampleEdge;             /**< ADC end of convert plus signal sample */
    uint8_t                     DataFifoFullWrEn;       /**< ADC data fifo write enabled/diabled when it's full */
#endif
} ADC_SequenceConfig_T;

/**
 * @brief Initializes ADC device with user configuration and user handle.
 *
 * This function configures ADC device with the user-defined settings.
 *
 * @param pADC           ADC device point
 * @param pConfig        Pointer to user-defined configuration.
 * @param hADC           Pointer to ADC handle.
 *
 * @retval RET_OK        ADC_Init success
 * @retval RET_ERROR     ADC_Init error
 * @retval RET_BUSY      ADC is using by others
 * @retval RET_INVPARAM  Usr input paramter is error
 */

RET_CODE ADC_Init(ADC_Device_T *pADC, ADC_CommonConfig_T *pConfig, ADC_Handle_T *hADC);

/**
 * @brief use to read adc convet value.
 *
 * This function use to read adc sequence convet  value.
 *
 * @param hADC Pointer to ADC handler.
 * @param pConvertData
 *        point to adc convert result buf, when usr config to tx use
 *        dma mode or config noblock mode,please make sure
 *        this pconvertdata must point golbal variable.
 * @param timeout
 *        timeout != TIMEOUT_NO_BLOCKING, this function is block until function return.if return RET_OK,usr can
 *        get the convert result from pConfig->pConvertData,timeout = TIMEOUT_NO_BLOCKING, this function will
 *        return immediately.usr should read the pConfig->convertData at callback recive ADC_EVENT_FINISHED event.
 *
 * @retval RET_OK        ADC convert success
 * @retval RET_ERROR     ADC convert error
 * @retval RET_BUSY      ADC is using by others
 * @retval RET_TIMEOUT   ADC is not convert finish during the seting timeout value
 * @retval RET_INVPARAM  Usr input paramter is error
 * @retval RET_ONGOING   ADC sequence convert is ongong,commonly it will happen when
 *                       usr set timeoutUs = TIMEOUT_NO_BLOCKING
 */
RET_CODE ADC_ReadConvertValue(ADC_Handle_T *hADC, uint32_t *pConvertData, uint32_t timeout);

/**
 * @brief Deinitializes ADC device.
 *
 * This function make ADC to stop mode, and release some resource.
 *
 * @param pADC          ADC device point
 * @param hADC          Pointer to ADC handle.
 *
 * @retval RET_OK       ADC_DeInit success
 * @retval RET_ERROR    ADC_DeInit error
 * @retval RET_BUSY     ADC is used by others
 */
RET_CODE ADC_DeInit(ADC_Device_T *pADC, ADC_Handle_T *hADC);

/**
 * @brief config singal ADC channel parameter
 *
 * This function Config ADC channel config paramete before start ADC, when finish some
 * convert. if usr want to config another resolution ,sampleTime or channelindex,
 * usr call this function to change it befor read adc convet value,
 * then usr can continue to start to convert again , usr should not
 * need to call ADC_DeInit and ADC_Init to Config again.
 *
 * @param hADC         ADC device point
 * @param pConfig      Pointer to ADC channel config
 *
 * @retval RET_OK      ADC_ChannelConfig success
 * @retval RET_ERROR   ADC_ChannelConfig error
 * @retval RET_BUSY    ADC is used by others
 */
RET_CODE ADC_ChannelConfig(ADC_Handle_T *hADC, ADC_ChannelConfig_T *pConfig);

/**
 * @brief config singal ADC channel parameter
 *
 * This function Config ADC channel config paramete before start ADC, when finish some
 * convert. if usr want to config another resolution ,sampleTime or channelindex,
 * usr call this function to change it befor read adc convet value,
 * then usr can continue to start to convert again , usr should not
 * need to call ADC_DeInit and ADC_Init to Config again.
 *
 * @param hADC         ADC device point
 * @param pExternSequenceChannel      Pointer to ADC extern channel config
 * @param length       ADC extern channel config length
 *
 * @retval RET_OK      ADC_ChannelConfig success
 * @retval RET_ERROR   ADC_ChannelConfig error
 * @retval RET_BUSY    ADC is used by others
 */
RET_CODE ADC_SelectExternChannel(ADC_Handle_T *hADC, uint8_t *pExternSequenceChannel, uint8_t length);

/**
 * @brief config ADC sequence parameter
 *
 * This function config ADC sequence paramete before start ADC, when finish some
 * convert. if usr want to config another resolution ,sampleTime or sequenceConfig,
 * usr call this function to change it befor read adc convet value,
 * then usr can continue to start to convert again , usr should not
 * need to call ADC_DeInit and ADC_Init to Config again.
 *
 * @param hADC         Pointer to ADC handle
 * @param pConfig      Pointer to ADC sequence config
 *
 * @retval RET_OK     ADC_SequenceConfig success
 * @retval RET_ERROR  ADC_SequenceConfig error
 * @retval RET_BUSY   ADC is used by others
 */
RET_CODE ADC_SequenceConfig(ADC_Handle_T *hADC, ADC_SequenceConfig_T *pConfig);

/**
 * @brief config ADC compareparameter
 *
 * This function config ADC compare paramete before start ADC if usr want to use
 * ADC compare function should first call  ADC_CompareModeEnable function , when finish some
 * compare convert.if usr want to config another compare parameter , usr call this
 * function to change it, then continue to compare convet, usr should not need to call
 * ADC_DeInit and ADC_Init to config again.
 *
 * @param hADC Pointer to ADC handle.
 * @param pCompareConfig ADC compare parameter  point
 *
 * @retval RET_OK     ADC_CompareConfig success
 * @retval RET_ERROR  ADC_CompareConfig error
 * @retval RET_BUSY   ADC is used by others
 */
RET_CODE ADC_CompareConfig(ADC_Handle_T *hADC, ADC_CompareConfig_T *pCompareConfig);

/**
 * @brief Enable ADC compare Function
 *
 * This function  use to enable ADC compare function ,when usr want to use ADC compare function,
 * it should first call ADC_ChannelSampleConfig and then call ADC_CompareModeEnable
 *
 * @param hADC Pointer to ADC handle.
 *
 * @retval RET_OK      ADC_CompareModeEnable success
 * @retval RET_ERROR   ADC_CompareModeEnable error
 * @retval RET_BUSY    ADC is used by others
 */
RET_CODE ADC_CompareModeEnable(ADC_Handle_T *hADC);

/**
 * @brief Disable ADC compare Function
 *
 * This function use to disable ADC comparefunction ,when usr not want to use ADC comparefunction,
 * it shoud  call ADC_CompareModeDisable,default is disable ADC compare function.
 *
 * @param hADC Pointer to ADC handle.
 *
 * @retval RET_OK      ADC_CompareModeDisable success
 * @retval RET_ERROR   ADC_CompareModeDisable error
 * @retval RET_BUSY    ADC is used by others
 */

RET_CODE ADC_CompareModeDisable(ADC_Handle_T *hADC);

#ifdef __cplusplus
}
#endif

#endif /*_ADC_H_*/
