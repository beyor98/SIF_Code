
#include <stdio.h>
#include <common.h>

#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <drv/dev/uart.h>


#define LOW                 0       //低电平
#define HIGH                1       //高电平

#define DATA_REV_PIN        GPIOC8     //定义数据接收引脚

#define SYNC_TIME_NUM       992     //992Tosc中的992
#define SHORT_TIME_NUM      32      //一个逻辑周期中短的时间：32Tosc中的32
#define LONG_TIME_NUM       64      //一个逻辑周期中长的时间：64Tosc中的64
#define LOGIC_CYCLE_NUM     96      //一个逻辑周期  SHORT_TIME_NUM + LONG_TIME_NUM
#define HALF_LOGIC_CYCLE    48      //一个逻辑周期的1/2，即  LOGIC_CYCLE_NUM/2

#define REV_BIT_NUM         8       //接收的bit位个数，看是按字节接收还是按字接收，1字节=8bit，1字=2字节=16bit
#define REV_DATA_NUM        12      //接收的数据个数



typedef enum
{
    INITIAL_STATE=0,            //初始状态，等待接收同步信号
    SYNC_L_STATE=1,             //接收同步低电平信号状态
    SYNC_H_STATE=2,             //接收同步高电平信号状态
    DATA_REV_STATE=3,           //读取数据码电平状态
    RESTART_REV_STATE=4         //接收过程出错重新接收状态
}REV_STATE_e;                   //接收数据状态枚举


unsigned char receive_state=0;      //接收数据状态
unsigned char receive_bit_num=0;    //接收的bit位个数
unsigned char receive_data_num=0;   //接收的数据个数

unsigned char receive_data_buf[REV_DATA_NUM]={0};   //接收数据缓存数组-如果一帧数据有多个数据打开注释
unsigned int  time_1ms_cnt=0; //高低电平时间计数

int start_500us_count_flag=0; //开始高低电平计时标记
int start_3ms_count_flag=0;
int countdown_3ms_flag=0;		//开启3ms定时器倒计时
int has_read_bit = 0;               //1-已经读取一个bit位
int read_success=0;                 //一帧数据是否读取成功，0-不成功，1-成功
int is_read = FALSE;
int val =0;

int etm2_timerinit(void);
void gpio_input_init(void);
void Receive_Data_Baud_Adjust_Bit_Handle(void);   //接收数据处理—带校准位，即波特率自适应



int processData(unsigned char data) 
{
	//printf("enter the processDate !!");
	return 0;
}


unsigned char verifyFrame(unsigned char frame[]) 
{
    unsigned char i;
		unsigned char checksum = 0;
		
		if (frame[0] == 0x08 && frame[1] == 0x61) 	// 校验帧头
		{ 
        for (i = 0; i < (REV_DATA_NUM-1); i++) 
				{
					checksum ^= frame[i];			// 校验帧尾
        }
        return checksum == frame[REV_DATA_NUM-1]; // 返回校验结果
    }
		
    return 0; 	// 帧头不匹配，校验失败
}


void ETM1Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)		//3ms定时器
{
		if(start_3ms_count_flag == 1)
		{
				is_read = TRUE;
		}
}


void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)		//0.5ms定时器
{
		if(start_500us_count_flag==1)
		{
				time_1ms_cnt++;     
		}
		if(countdown_3ms_flag)
		{
				countdown_3ms_flag--;
				if(countdown_3ms_flag == 0)
				{
						ETM_StopTimeBase(ETM2);
						ETM_StartTimeBase(ETM1);
						is_read = TRUE;
				}
		}
}


void Receive_Data_Baud_Adjust_Bit_Handle(void)
{
    switch (receive_state)                      //检测当前接收数据状态
    {
        case INITIAL_STATE:                         //初始状态，未接收到同步信息，进行同步判断
            if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //判断接收引脚的电平状态，当读到低电平时，开始计时
            {
                receive_bit_num = REV_BIT_NUM;      //重置bit位计数器
                receive_data_num = 0;               //重置接收数据个数
                time_1ms_cnt = 0; 
                start_500us_count_flag = 1;
								ETM_StartTimeBase(ETM2);
                receive_state = SYNC_L_STATE;       //进入读取同步低电平信号状态
            }
            break;
        
        case SYNC_L_STATE:                          //在读取同步低电平信号期间
            if (time_1ms_cnt > 60)    //如果低电平时间>30ms
            {                                       
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)           //判断接收引脚的电平状态，当读到高电平时
                {
                    time_1ms_cnt = 0;         
										start_500us_count_flag = 0;   
                    receive_state = SYNC_H_STATE;   //进入读取同步信号高电平状态
										ETM_StopTimeBase(ETM2);
                }     
            }
            else
            {
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)           //同步信号低电平检测期间读到高电平重新计时
                {
                    receive_state = RESTART_REV_STATE;      //进入重新接收状态
                }  
            }
            break;

        case SYNC_H_STATE:                          //在读取同步高电平信号期间
						if (GPIO_PinRead(DATA_REV_PIN) == LOW)            //>=同步信号高电平检测时间后读到低电平
						{
								ETM_StartTimeBase(ETM2);
								countdown_3ms_flag=3;		//使用etm2开启1.5ms定时器
								start_3ms_count_flag = 1;
								memset(&receive_data_buf, 0, sizeof(receive_data_buf));
								receive_state = DATA_REV_STATE;	
						} 
						
        case DATA_REV_STATE:          //在读取数据码电平期间
            //取中间时间点进行判断
            //如果还没有读取一个bit位，且时间计数已经>=3ms一个周期
            if ((has_read_bit==0) && (is_read == TRUE)) //is_read为0.75ms定时器中断的标志位
            {
                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);		//单个字节放入数组，放够8bit
								GPIO_PinToggle(GPIOA8);
                has_read_bit = 1;
								is_read = FALSE;
            }
            //如果已经读取一个bit位，且时间计数已经>=96Tosc，说明一个逻辑周期过去了
            if (has_read_bit==1)
            {
                has_read_bit = 0;               //清0，读取下一个bit位
                receive_bit_num--;              //接收的bit数--
                if (receive_bit_num==0)         //如果一个字节8个bit位接收完成
                {		
                    receive_data_num++;         //接收的数据个数++
                    receive_bit_num = REV_BIT_NUM;  //重置接收bit位个数
                    if (receive_data_num == REV_DATA_NUM)   //如果数据采集完毕
                    {
                        read_success = 1;                   //一帧数据读取成功
                        start_500us_count_flag = 0;   
                        time_1ms_cnt = 0;             //定时器计数值清0
												start_3ms_count_flag = 0;
												ETM_StopTimeBase(ETM1);
												receive_state = INITIAL_STATE;      //接收状态清0   
                    }  
                }
								else
								{
										//将接收数据缓存左移一位，数据从高bit位开始接收
										receive_data_buf[receive_data_num] = receive_data_buf[receive_data_num] << 1;
								}
            }        
            break;

        case RESTART_REV_STATE:                     //重新接收数据状态
            start_500us_count_flag = 0; 
						start_3ms_count_flag = 0;
            time_1ms_cnt = 0;                 //定时器计数值清0
						is_read = FALSE;
            receive_state = INITIAL_STATE;          //接收状态清0    	
            break;
    }
}


void sif_process(void)
{
		unsigned char i;

		Receive_Data_Baud_Adjust_Bit_Handle();
		
		if (read_success == 1)              //如果成功读取一帧数据
		{
				for(i=0; i<REV_DATA_NUM; i++)
				{
						printf("0x%02X ",receive_data_buf[i]);
				}
				printf("\n");
						
				if (verifyFrame(receive_data_buf)) 
				{
						// 校验通过，处理数据
						printf("---verifyFrame success!!---\n");
				} 
				else 
				{
						// 校验失败
				}
				read_success = 0;               //读取一帧数据清0
		}
    
}
