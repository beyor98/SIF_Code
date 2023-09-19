/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 *
 * @file          pes.h
 *
 * @author        Mike.Zheng
 *
 * @version       1.1.0
 *
 * @date          2017/05/19
 *
 * @brief         Header file of pes driver.
 *
 * @note
 *    2017/05/19, Mike.Zheng, V1.0.0
 *        Initial version.
 */

#ifndef _PES_H_
#define _PES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <common.h>

/**
 *  pes channel define
 */
typedef enum
{
    PES_EVENT_CHANNEL0          = 0x00,            /**< pes channel0 */
    PES_EVENT_CHANNEL1                ,            /**< pes channel1 */
    PES_EVENT_CHANNEL2                ,            /**< pes channel2 */
    PES_EVENT_CHANNEL3                ,            /**< pes channel3 */
    PES_EVENT_CHANNEL4                ,            /**< pes channel4 */
    PES_EVENT_CHANNEL5                ,            /**< pes channel5 */
    PES_EVENT_CHANNEL6                ,            /**< pes channel6 */
    PES_EVENT_CHANNEL7                ,            /**< pes channel7 */
    PES_EVENT_CHANNEL8                ,            /**< pes channel8 */
    PES_EVENT_CHANNEL9                ,            /**< pes channel9 */
    PES_EVENT_CHANNEL10               ,            /**< pes channel10 */
    PES_EVENT_CHANNEL11               ,            /**< pes channel11 */
    PES_INVAILD_CHANNEL
}PES_ChannelIndex_T;


/**
 *  pes output event wave define
 */
typedef enum
{

    PES_SYNC_BYPASS             = 0x00,             /**< pes channel output event wave keep the same as
                                                     *   master output event wave,
                                                     *   becare: this mode is invaild in deepsleep mode,
                                                     *   but vaild in normal mode
                                                     */

    PES_SYNC_RISING_EDGE             ,              /**< pes channel output is a pulse when receive rasing
                                                     *   edge from pes master source becare: this mode is invaild
                                                     *   in deepsleep mode, but vaild in normal mode
                                                     */

    PES_SYNC_FALLING_EDGE            ,              /**< pes channel output is a pulse when receive falling edge
                                                     *   from pes master source, becare:this mode is invaild in
                                                     *   deepsleep mode, but vaild in normal mode
                                                     */


    PES_SYNC_RISINGF_FALLING_EDGE    ,              /**< pes channel output is a pulse when receive both rasing
                                                     *   or falling edge from pes master source becare: this mode
                                                     *   is invaild in deepsleep mode, but vaild in normal mode
                                                     */

    PES_ASYNC                                       /**< pes channel output event wave keep the same as master
                                                     *   output event wave,and not sync clk this mode is vaild at
                                                     *   both normal and deepsleep mode
                                                     */
}PES_TrigerSlaveSyncMode_T;

/**
 *  pes channel define
 */
typedef enum
{
    PES_OUTPUT_EVENT_FROM_CHANNEL0          = 0x00, /**< pes channel0 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL1                , /**< pes channel1 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL2                , /**< pes channel2 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL3                , /**< pes channel3 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL4                , /**< pes channel4 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL5                , /**< pes channel5 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL6                , /**< pes channel6 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL7                , /**< pes channel7 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL8                , /**< pes channel8 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL9                , /**< pes channel9 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL10               , /**< pes channel10 generate event */
    PES_OUTPUT_EVENT_FROM_CHANNEL11               , /**< pes channel11 generate event */
}PES_MasterPesEventTrigerOutput_T;

/**
 *  pes io pin define
 */
typedef enum
{
    IO_PIN_XPC7                             = 0x00, /**< pes IO pin xpc7 */
    IO_PIN_XPB6                                   , /**< pes IO pin xpb6 */
    IO_PIN_XPB8                                   , /**< pes IO pin xpb8 */
    IO_PIN_XPC12                                  , /**< pes IO pin xpc12 */
    IO_PIN_XPB14                                  , /**< pes IO pin xpb14 */
    IO_PIN_XPC13                                  , /**< pes IO pin xpc13 */
    IO_PIN_XPC1                                   , /**< pes IO pin xpc1 */
    IO_PIN_XPD3                                   , /**< pes IO pin xpd3 */
    IO_PIN_XPC2                                   , /**< pes IO pin xpc2 */
    IO_PIN_XPA0                                   , /**< pes IO pin xpa0 */
    IO_PIN_XPA1                                   , /**< pes IO pin xpa1 */
    IO_PIN_XPA5                                   , /**< pes IO pin xpa5 */
    IO_PIN_XPA10                                  , /**< pes IO pin xpa10 */
    IO_PIN_XPA11                                  , /**< pes IO pin xpa11 */
    IO_PIN_XPA12                                  , /**< pes IO pin xpa12 */
    IO_PIN_XPA14                                  , /**< pes IO pin xpa14 */
    IO_PIN_XPB1                                   , /**< pes IO pin xpb1 */
}PES_IOPin_T;

/**
 *  pes output event wave define
 */
typedef enum
{
    PES_MASTER_PESOUT                       = 0x00, /**< pes channel output event as master */
    PES_MASTER_CPU                                , /**< cpu as pes channel master */
    PES_MASTER_DAM                                , /**< dma as pes channel master */
    PES_MASTER_IO                                 , /**< io as pes channel master */
    PES_MASTER_ETM20                              , /**< etm20 as pes channel master */
    PES_MASTER_ACMP                               , /**< acmp as pes channel master */
    PES_MASTER_ADC                                , /**< adc as pes channel master */
    PES_MASTER_RTCC                               , /**< rtcc as pes channel master */
    PES_MASTER_BASETIMER                          , /**< basetimer as pes channel master */
    PES_MASTER_ETM21                              , /**< erm21 as pes channel master */
    PES_MASTER_ETM22                                /**< erm22 as pes channel master */
}PES_MasterSource_T;

/**
 *  acmp as master generate pes event mode define
 */
typedef enum
{
    PES_ACMP0_OUTPUT                        = 0x00, /**< acmp as master generate event from acmp0 output */
    PES_ACMP1_OUTPUT                                /**< acmp as master generate event from acmp1 output */
}PES_MasterAcmpTrigerOutput_T;

/**
 *  adc as master generate pes event mode define
 */
typedef enum
{
    PES_ADC_EOC                             = 0x00, /**< adc as master generate event when adc eoc intrupter generate */
    PES_ADC_EOS                                   , /**< adc as master generate event when adc eos intrupter generate */
    PES_ADC_COMP_MATCH                              /**< adc as master generate event when adc compare match intrupter generate */
}PES_MasterAdcTrigerOutput_T;

/**
 *  etm20 as master generate pes event mode define
 */
typedef enum
{
    PES_ETM20_CH0_POSEDGE                   = 0x00, /**< etm20 as master generate event when etm20 ch0 generate posedge */
    PES_ETM20_CH0_NEGEDGE                         , /**< etm20 as master generate event when etm20 ch0 generate negedge */
    PES_ETM20_CH1_POSEDGE                         , /**< etm20 as master generate event when etm20 ch1 generate posedge */
    PES_ETM20_CH1_NEGEDGE                         , /**< etm20 as master generate event when etm20 ch1 generate negedge */
    PES_ETM20_CH2_POSEDGE                         , /**< etm20 as master generate event when etm20 ch2 generate posedge */
    PES_ETM20_CH2_NEGEDGE                         , /**< etm20 as master generate event when etm20 ch2 generate negedge */
    PES_ETM20_CH3_POSEDGE                         , /**< etm20 as master generate event when etm20 ch3 generate posedge */
    PES_ETM20_CH3_NEGEDGE                         , /**< etm20 as master generate event when etm20 ch3 generate negedge */
    PES_ETM20_DUTY0_MATCH                         , /**< etm20 as master generate event when etm20 generate duty0 match */
    PES_ETM20_DUTY1_MATCH                         , /**< etm20 as master generate event when etm20 generate duty1 match */
    PES_ETM20_UNDERFLOW                           , /**< etm20 as master generate event when etm20 generate underflow */
    PES_ETM20_OVERFLOW                              /**< etm20 as master generate event when etm20 generate overflow */
}PES_MasterEtm20TrigerOutput_T;

/**
 *  etm21 as master generate pes event mode define
 */
typedef enum
{
    PES_ETM21_CH0_POSEDGE                   = 0x00, /**< etm21 as master generate event when etm21 ch0 generate posedge */
    PES_ETM21_CH0_NEGEDGE                         , /**< etm21 as master generate event when etm21 ch0 generate negedge */
    PES_ETM21_CH1_POSEDGE                         , /**< etm21 as master generate event when etm21 ch1 generate posedge */
    PES_ETM21_CH1_NEGEDGE                         , /**< etm21 as master generate event when etm21 ch1 generate negedge */
    PES_ETM21_DUTY0_MATCH                         , /**< etm21 as master generate event when etm21 generate duty0 match */
    PES_ETM21_DUTY1_MATCH                         , /**< etm21 as master generate event when etm21 generate duty1 match */
    PES_ETM21_UNDERFLOW                           , /**< etm21 as master generate event when etm21 generate underflow */
    PES_ETM21_OVERFLOW                              /**< etm21 as master generate event when etm21 generate overflow */
}PES_MasterEtm21TrigerOutput_T;

/**
 *  etm22 as master generate pes event mode define
 */
typedef enum
{
    PES_ETM22_CH0_POSEDGE                   = 0x00, /**< etm22 as master generate event when etm22 ch0 generate posedge */
    PES_ETM22_CH0_NEGEDGE                         , /**< etm22 as master generate event when etm22 ch0 generate negedge */
    PES_ETM22_CH1_POSEDGE                         , /**< etm22 as master generate event when etm22 ch1 generate posedge */
    PES_ETM22_CH1_NEGEDGE                         , /**< etm22 as master generate event when etm22 ch1 generate negedge */
    PES_ETM22_DUTY0_MATCH                         , /**< etm22 as master generate event when etm22 generate duty0 match */
    PES_ETM22_DUTY1_MATCH                         , /**< etm22 as master generate event when etm22 generate duty1 match */
    PES_ETM22_UNDERFLOW                           , /**< etm22 as master generate event when etm22 generate underflow */
    PES_ETM22_OVERFLOW                              /**< etm22 as master generate event when etm22 generate overflow */
}PES_MasterEtm22TrigerOutput_T;

/**
 *  cpu as master send pes wave define
 */
typedef enum
{
    PES_LOW_LEVEL                           = 0x00, /**< cpu as master send low level to pes channel */
    PES_HIGH_LEVEL                                , /**< cpu as master send high level to pes channel */
    PES_PULSE                                       /**< cpu as master send pulse to pes channel */
}PES_MasterCpuTriger_T;

/**
 *  cpu as master send pes wave define
 */
typedef enum
{
    IO_SEND_EVENT                           = 0x00, /**< io as master send event to pes channel */
    IO_RECEIVE_EVENT                              , /**< io as slave to receive event from pes channel */
}PES_IoDir_T;

/**
 * master triger output mode union
 */
typedef union
{
    PES_IOPin_T                           ioPinOutput;      /**< io as master generate pes mode enum */
    PES_MasterAcmpTrigerOutput_T          acmpOutput;       /**< acmp as master generate pes mode enum */
    PES_MasterAdcTrigerOutput_T           adcOutput;        /**< adc as master generate pes mode enum */
    PES_MasterEtm20TrigerOutput_T         etm20Output;      /**< etm20 as master generate pes mode enum */
    PES_MasterEtm21TrigerOutput_T         etm21Output;      /**< etm21 as master generate pes mode enum */
    PES_MasterEtm22TrigerOutput_T         etm22Output;      /**< etm22 as master generate pes mode enum */
    PES_MasterPesEventTrigerOutput_T      pesChannelOutput; /**< pes channel output as master generate pes mode enum */
} PES_MasterTrigerOutputMode_T;

/**
 * pes master source config struct
 */
typedef struct
{
    PES_MasterSource_T                    masterSource;       /**< pes master source */
    PES_MasterTrigerOutputMode_T          masterTrigerOutput; /**< pes master generate pes event mode */
} PES_MasterSourceConfig_T;

/**
 * pes channel cascade struct
 */
typedef struct
{
    uint8_t         andNextChannelEnable; /**< enable/disable:pes channel output wave & with next pes channel output wave function */
    uint8_t         orpreChannelEnable;   /**< enable/disable:pes channel output wave | with pre pes channel output wave function */
} PES_ChannelCascadeEnConfig_T;

/**
 * pes channel config struct
 */
typedef struct
{
    PES_TrigerSlaveSyncMode_T             syncMode;              /**< slave reveive event sync mode */
    PES_MasterSourceConfig_T              masterSrcConfig;       /**< master src config */
    PES_MasterTrigerOutputMode_T          mode;                  /**< master generate event mode */
    PES_ChannelCascadeEnConfig_T          cascadeConfig;         /**< cascade Config */
    uint8_t                               channelOutputInverted; /**< enable/disable:pes channel output wave inverted */
} PES_ChannelConfig_T;

/**
 * @brief request pes channel.
 *
 * This function use to request a free pes channel.

 * @retval PES_ChannelIndex_T free pes channel Index
 */
PES_ChannelIndex_T PES_RequestEventChannel(void);

/**
 * @brief release event channel
 *
 * This function use to release event channel
 *
 * @param chIndex    The return value get from PES_RequestEventChannel
 *
 * @retval RET_BUSY  PES lock is not get, the resource is locked by others
 * @retval RET_OK    PES release event channel OK
 */
RET_CODE PES_ReleaseEventChannel(PES_ChannelIndex_T chIndex);

/**
 * @brief config pes event channel
 *
 * This function use to config pes event channel
 *
 * @param  pchCfg         PES channel config parameter,please refer to PES_ChannelConfig_T for detail
 * @param  chIndex        PES channel index
 *
 * @retval RET_INVPARAM   Input parameter is error
 * @retval RET_OK         PES config event channel OK
 */
RET_CODE PES_ConfigEventChannel(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex);

/**
 * @brief config pes function for cpu as slave
 *
 * This function use to config pes function for cpu as slave
 *
 * @param pchCfg          PES channel config par,please refer PES_ChannelConfig_T fore detail
 * @param chIndex         PES channel Index
 *
 * @retval void
 */
RET_CODE PES_CPUAsSlaveConfig(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex);

/**
 * @brief cpu as master to send pes event
 *
 * This function is use when cpu as master to send sw pes event
 *
 * @param trigerMode     Send triger mode
 * @param chIndex        the channel number
 * @retval               None
 */
void PES_CupAsMasterGenerateSWTrigerEvent(PES_MasterCpuTriger_T trigerMode, PES_ChannelIndex_T chIndex);

/**
 * @brief io as slave config
 *
 * This function is use to when io as slave , usr can use it to config io
 *
 * @param pchCfg         PES channel config par,please refer PES_ChannelConfig_T fore detail
 * @param chIndex        PES channel Index
 * @param slaveOutputPin pes IO pin
 *
 * @retval RET_INVPARAM  Input parameter is error
 * @retval RET_OK        IO as slave config ok
 *
 */
RET_CODE PES_IOAsSlaveConfig(PES_ChannelConfig_T *pchCfg, PES_ChannelIndex_T chIndex, PES_IOPin_T slaveOutputPin);


#ifdef __cplusplus
}
#endif

#endif /*_PES_H_*/



















