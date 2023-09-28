
#include <stdio.h>
#include <common.h>

#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <drv/dev/uart.h>


#define LOW                 0
#define HIGH                1

#define DATA_REV_PIN        GPIOC8
#define REV_BIT_NUM         8
#define REV_DATA_NUM        12

#define SIF_DEBUG						0

typedef enum
{
    INITIAL_STATE=0,            //初始状态，等待接收同步信号
    SYNC_L_STATE=1,             //接收同步低电平信号状态
    SYNC_H_STATE=2,             //接收同步高电平信号状态
    DATA_REV_STATE=3,           //读取数据码电平状态
    RESTART_REV_STATE=4         //接收过程出错重新接收状态
}REV_STATE_e;                   //接收数据状态枚举


unsigned char receive_state=0;
unsigned char receive_bit_num=0;
unsigned char receive_data_num=0;

unsigned char receive_data_buf[REV_DATA_NUM]={0};
unsigned int  time_1ms_cnt=0;

int start_500us_count_flag=0; 
int start_3ms_count_flag=0;
int countdown_3ms_flag=0;		//开启3ms定时器倒计时
int has_read_bit = 0;           //1-已经读取一个bit位
int read_success=0;
int is_read = FALSE;
int val =0;

int etm2_timerinit(void);
void gpio_input_init(void);
void Receive_Data_Baud_Adjust_Bit_Handle(void); 
void aip1640_test(void);



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
        return checksum == frame[REV_DATA_NUM-1];
    }
		
    return 0; 	// 帧头不匹配，校验失败
}


void ETM1Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)		//3ms ETM
{
		if(start_3ms_count_flag == 1)
		{
				is_read = TRUE;
		}
}


void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)		//0.5ms ETM
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
    switch (receive_state)
    {
        case INITIAL_STATE:                         //初始状态，未接收到同步信息，进行同步判断
            if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //判断接收引脚的电平状态，当读到低电平时，开始计时
            {
                receive_bit_num = REV_BIT_NUM;
                receive_data_num = 0;
                time_1ms_cnt = 0; 
                start_500us_count_flag = 1;
								ETM_StartTimeBase(ETM2);
                receive_state = SYNC_L_STATE;       //进入读取同步低电平信号状态
            }
            break;
        
        case SYNC_L_STATE:
            if (time_1ms_cnt > 60)    //如果低电平时间>30ms
            {                                       
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)
                {
                    time_1ms_cnt = 0;         
										start_500us_count_flag = 0;   
                    receive_state = SYNC_H_STATE;   //进入读取同步信号高电平状态
										ETM_StopTimeBase(ETM2);
                }     
            }
            else
            {
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)
                {
                    receive_state = RESTART_REV_STATE;
                }  
            }
            break;

        case SYNC_H_STATE:
						if (GPIO_PinRead(DATA_REV_PIN) == LOW) 
						{
								ETM_StartTimeBase(ETM2);

							    countdown_3ms_flag=3;				//使用etm2开启3*0.5=1.5ms定时器
								start_3ms_count_flag = 1;  
								memset(&receive_data_buf, 0, sizeof(receive_data_buf));
								receive_state = DATA_REV_STATE;	
						} 
						
        case DATA_REV_STATE:
            //取周期中点，如果还没有读取一个bit位，且时间计数已经>=3ms一个周期
            if ((has_read_bit==0) && (is_read == TRUE)) //第一个is_read为*0.5=1.5ms定时器定时器中断，后续为3ms周期定时
            {
                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);
#if SIF_DEBUG								
				GPIO_PinToggle(GPIOA8);		//调试校准使用
#endif								
                has_read_bit = 1;
								is_read = FALSE;
            }
            //如果已经读取一个bit位，一个逻辑周期过去
            if (has_read_bit==1)
            {
                has_read_bit = 0;
                receive_bit_num--;
                if (receive_bit_num==0)         //如果一个字节8个bit位接收完成
                {		
                    receive_data_num++; 
                    receive_bit_num = REV_BIT_NUM;
                    if (receive_data_num == REV_DATA_NUM)   //如果数据采集完毕
                    {
                        read_success = 1;                   //一帧数据读取成功
                        start_500us_count_flag = 0;   
                        time_1ms_cnt = 0;
												start_3ms_count_flag = 0;
												ETM_StopTimeBase(ETM1);
												receive_state = INITIAL_STATE;
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
            time_1ms_cnt = 0;
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
						printf("---verifyFrame false!!---\n");// 校验失败
				}
				read_success = 0;               //读取一帧数据清0
		}
		aip1640_test();
}
