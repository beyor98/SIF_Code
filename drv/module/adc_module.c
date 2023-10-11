#include "stdio.h"
#include <common.h>
#include <drv/dev/uart.h>
#include <drv/dev/adc.h>
#include <drv/dev/gpio.h>


#ifdef DEBUG_PRINTF
    #undef DEBUG_PRINTF
#endif
#ifdef ERR_PRINTF
    #undef ERR_PRINTF
#endif
#ifdef WARNING_PRINTF
    #undef WARNING_PRINTF
#endif

#define DEBUG_PRINTF
#define WARNING_PRINTF
#define ERR_PRINTF

#ifndef DEBUG_PRINTF
    #define PR_DEBUG(...)       do{}while(0)
#else
    #define PR_DEBUG            printf
#endif

#ifndef ERR_PRINTF
    #define PR_ERR(...)        do{}while(0)
#else
    #define PR_ERR             printf
#endif

#ifndef WARNING_PRINTF
    #define PR_WARNINR(...)    do{}while(0)
#else
    #define PR_WARNINR         printf
#endif

#define ADC_CHANNEL_LENGTH      16
#define STANDARD_VOL						30
#define MAX_CONVERVOL						1090
#define ADC_CONVERT_TIMEOUT     1000000    /* ADC Timeout 1 s */


typedef struct
{
	uint32_t pin;				   /**< Specifies the GPIO pins to be configured */
	GPIO_Device_T *device;		   /**< GPIO device */
	uint32_t mode;				   /**< Specifies the operating mode for the selected pins */
	uint32_t pull;				   /**< Specifies the Pull-up or Pull-up X2 activation for the selected pins */
} Device_Pinmux_T;

static ADC_Handle_T s_Handle;
static ADC_ChannelConfig_T channleConfig;
static uint32_t s_convertValue;

static Device_Pinmux_T s_pinmux[ADC_CHANNEL_LENGTH] = {
		{GPIO_PIN_9,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_0 pinmux config paramter */
		{GPIO_PIN_10, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_1 pinmux config paramter */
		{GPIO_PIN_11, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_2 pinmux config paramter ----vol_value----*/
		{GPIO_PIN_12, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_3 pinmux config paramter */
		{GPIO_PIN_13, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_4 pinmux config paramter */
		{GPIO_PIN_14, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_5 pinmux config paramter */
		{GPIO_PIN_15, GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_6 pinmux config paramter */
		{GPIO_PIN_0,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_7 pinmux config paramter */
		{GPIO_PIN_1,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_8 pinmux config paramter */
		{GPIO_PIN_2,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_9 pinmux config paramter */
		{GPIO_PIN_3,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_10 pinmux config paramter */
		{GPIO_PIN_4,  GPIOC, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_11 pinmux config paramter */
		{GPIO_PIN_5,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_12 pinmux config paramter */
		{GPIO_PIN_6,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_13 pinmux config paramter */
		{GPIO_PIN_7,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_14 pinmux config paramter */
		{GPIO_PIN_8,  GPIOB, GPIO_MODE_ANALOG, GPIO_NOPULL}, /* ADC_CHANNEL_15 pinmux config paramter */
};

static RET_CODE ADC_ConfigPinmux(void);
static void ADC_Callback(ADC_Handle_T *pHander, ADC_Event_T event, unsigned long param);


static void ADC_Callback(ADC_Handle_T *pHander, ADC_Event_T event, unsigned long param)
{
	if (event == ADC_EVENT_COMPERE_MATCH)
	{
		PR_DEBUG("ADC_EVENT_COMPERE_MATCH\n");
	}
	else if(event == ADC_EVENT_OVERFLOW)
	{
		PR_WARNINR("ADC_EVENT_OVERFLOW\n");
	}

}

static RET_CODE ADC_ConfigPinmux(void)
{
		RET_CODE ret = RET_OK;
    GPIO_PinConfig_T GPIO_InitStruct;
    Device_Pinmux_T *pADCPinmux = &s_pinmux[ADC_CHANNEL_2];

    memset(&GPIO_InitStruct,0,sizeof(GPIO_PinConfig_T));
		GPIO_InitStruct.pin =  pADCPinmux->pin;
    GPIO_InitStruct.mode = pADCPinmux->mode;
    GPIO_InitStruct.pull = pADCPinmux->pull;
    GPIO_Init(pADCPinmux->device, &GPIO_InitStruct);
	if (ret != RET_OK)
	{
		PR_ERR("Err:ACMP_ConfigPinmux\n");
	}

	return ret;
}

int adc_init(void)
{
		RET_CODE ret = RET_OK;
    ADC_CommonConfig_T commonConfig;

    ret = ADC_ConfigPinmux();
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_ConfigPinmux\n",__FUNCTION__);
        goto out;
    }

		/* config ADC common parameter */
		commonConfig.continuousMode = ADC_SINGLE_MODE;
		commonConfig.enableDma = DISABLE;
		commonConfig.callback = ADC_Callback;
		commonConfig.trigerSource = ADC_SW_TRIGER;
    ret = ADC_Init(ADC_DEV, &commonConfig, &s_Handle);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_Init\n",__FUNCTION__);
        goto out;
    }

    /* config ADC channel sample paramter */
		channleConfig.resolution = ADC_RESOLUTION_12BIT;
    channleConfig.sampleTime = ADC_SAMPLETIME_32CLK;
		channleConfig.sampleEdge = ADC_SAMPLE_POSITIVE;
		
		//channleConfig.channelIndex = DISABLE;
		channleConfig.channelIndex = ADC_CHANNEL_2;
    ret = ADC_ChannelConfig(&s_Handle, &channleConfig);
    if (ret != RET_OK)
    {
        PR_ERR("%s:Err ADC_ChannelSampleConfig\n",__FUNCTION__);
        goto out;
    }
out:
	return ret;
}

int adc_get_voltage(void)
{
		int vol = 0;
		int ret = 0;
		ret = ADC_ReadConvertValue(&s_Handle,&s_convertValue, ADC_CONVERT_TIMEOUT);
		//ret = ADC_ReadConvertValue(&s_Handle,&s_convertValue, TIMEOUT_NO_BLOCKING);
		if (ret == RET_OK)
		{
				vol = s_convertValue*STANDARD_VOL/MAX_CONVERVOL;
				PR_DEBUG("Channel:%d Convert Success Value:%d, vol = %d\n",channleConfig.channelIndex,s_convertValue,vol);
				return vol;
		}
		else
		{
				PR_ERR("%s:Err:%d ADC_ReadConvertValue\n",__FUNCTION__,ret);
				return ret;
		}
}




