
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

unsigned int  H_L_Level_time_cnt=0; //高低电平时间计数

unsigned int  Tosc = 6;             //波形时基单元，一般带波特率自适应的，不会说明高低电平的时间，会用一个Tosc时基描述
                                    //如上面的波形图，要求 32Tosc = 1ms = 1000us
                                    //所以：一个Tosc = 1000us/32 ≈ 31us，而定时器0单次定时时间为5us，所以实际一个Tosc = 30us/5us = 6 次
                                    //可以理解为 5us 是人为设置的一个定时器单次定时时间，再这个定时时间的基础上又产生了一个实际时基Tosc，用在波形上，波形基于这个Tosc时基单元
                                    //可以动态的调整高低电平的时间，只要Tosc改变，992Tosc和32Tosc以及64Tosc对应的时间也会随之改变；然后我们反过来思考，再不知道波特率的情况下，
                                    //去读取同步信号高电平的时间，将读到的时间计数H_L_Level_time_cnt * 5us是高电平的真实维持时间 = 32 * Tosc * 5us = SHORT_TIME_NUM * Tosc * 5us
                                    //Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM
                                    //在本案例中，一帧数据 = 992Tosc+32Tosc+(64+32)*8*12Tosc = 10240*Tosc 次
                                    //1秒钟 = 1000000us，单次定时 5us，则1s / 5us = 200000 次
                                    //则1秒钟可以接收 200000 / (10240 * Tosc) = 200000/10240/Tosc = 19/Tosc 帧数据

int start_H_L_Level_timming_flag=0; //开始高低电平计时标记
int has_read_bit = 0;               //1-已经读取一个bit位
int read_success=0;                 //一帧数据是否读取成功，0-不成功，1-成功

int count=0;

/* 函数声明 -------------------------------------------------------------------*/
int etm_timerinit(void);
void gpio_input_init(void);
void Receive_Data_Baud_Adjust_Bit_Handle(void);   //接收数据处理—带校准位，即波特率自适应


/*******************************************************************************
 *函数名称 : processData
 *函数功能 : 数据处理
*******************************************************************************/
int processData(unsigned char data) 
{
	printf("enter the processDate !!");
	return 0;
}


unsigned char verifyFrame(unsigned char frame[]) 
{
    unsigned char i;
		unsigned char checksum = 0;
				
		if (frame[0] == 0x08 && frame[1] == 0x61) 
		{ 
				// 校验帧头
        for (i = 0; i < (REV_DATA_NUM-1); i++) 
				{
					checksum ^= frame[i];
        }
        return checksum == frame[REV_DATA_NUM-1]; // 返回校验结果
    }
		
    return 0; // 帧头不匹配，校验失败
}

void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
		if(1)		//if (start_H_L_Level_timming_flag==1)
		{
				H_L_Level_time_cnt++;     //高低电平维持时间计数变量
				
		}
		count++;
		if(count==100)
		{
			count=0;
			GPIO_PinToggle(GPIOA8);
		}
	
}

/*******************************************************************************
 *函数名称 : Receive_Data_Baud_Adjust_Bit_Handle
 *函数功能 : 接收数据处理，波特率自适应
*******************************************************************************/
void Receive_Data_Baud_Adjust_Bit_Handle(void)
{
    switch (receive_state)                      //检测当前接收数据状态
    {
        case INITIAL_STATE:                         //初始状态，未接收到同步信息，进行同步判断
            if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //判断接收引脚的电平状态，当读到低电平时，开始计时
            {
                receive_bit_num = REV_BIT_NUM;      //重置bit位计数器
                receive_data_num = 0;               //重置接收数据个数
                H_L_Level_time_cnt = 0;             //高低电平计时变量清0
                start_H_L_Level_timming_flag = 1;   //开始高低电平计时
                receive_state = SYNC_L_STATE;       //进入读取同步低电平信号状态
            }
            break;
        
        case SYNC_L_STATE:                          //在读取同步低电平信号期间
            if (H_L_Level_time_cnt > SYNC_TIME_NUM*Tosc)    //如果低电平时间>SYNC_TIME_NUM*Tosc=992*3*5us
            {                                       //同步状态空闲时间大于15ms
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)           //判断接收引脚的电平状态，当读到高电平时
                {
                    H_L_Level_time_cnt = 0;         //高低电平计时变量清0
                    receive_state = SYNC_H_STATE;   //进入读取同步信号高电平状态
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
            
            //代码写法一：
            if (H_L_Level_time_cnt >= SHORT_TIME_NUM*Tosc)    //如果高电平时间 >= SHORT_TIME_NUM*Tosc=32 * 3 * 5us
            {
                if (GPIO_PinRead(DATA_REV_PIN) == LOW)            //>=同步信号高电平检测时间后读到低电平
                {
                    //进入这段逻辑有两种状态：1、高电平时间正好=32Tosc，2、高电平时间长，超过32Tosc
                    Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM;    //调整 Tosc 的值
									printf("Tosc_1 = %d\n", Tosc);
                    H_L_Level_time_cnt = 0;         //高低电平计时变量清0
                    //receive_state = DATA_REV_L_STATE;   //进入读取数据码低电平状态
										receive_state = DATA_REV_STATE;
                } 
            }
            else
            {
                if (GPIO_PinRead(DATA_REV_PIN) == LOW)            //同步信号高电平检测期间读到低电平
                {                                   //高电平时间短，没有满32Tosc，自动调整检测周期
                    //H_L_Level_time_cnt * 5us 要求是 SHORT_TIME_NUM * Tosc * 5us，即 H_L_Level_time_cnt = SHORT_TIME_NUM * Tosc
                    Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM;    //调整 Tosc 的值
									printf("Tosc_2 = %d\n", Tosc);
                    H_L_Level_time_cnt = 0;         //高低电平计时变量清0
                    //receive_state = DATA_REV_L_STATE;   //进入读取数据码低电平状态
									receive_state = DATA_REV_STATE;
                }  
            }
						
            
            //代码写法二：
            /*if (H_L_Level_time_cnt >= LOGIC_CYCLE_NUM*Tosc)   //如果高电平时间超过了（32+64=96）个Tosc，则认为超时
            {
                receive_state = RESTART_REV_STATE;      //进入重新接收状态
            }
            else
            {
                if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //同步信号高电平检测期间读到低电平
                {
                    //在同步信号高电平检测期间读到低电平可能有如下状态：
                    //1、高电平时间短，不满32Tosc
                    //2、高电平时间正好=32Tosc
                    //3、高电平时间长，超过32Tosc
                    //不管何种状态，都要 调整 Tosc 的值达到波特率自适应
                    //H_L_Level_time_cnt * 5us 要求是 SHORT_TIME_NUM * Tosc * 5us，
                    //即 H_L_Level_time_cnt = SHORT_TIME_NUM * Tosc
                    Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM;    //调整 Tosc 的值
										printf("Tosc = %d\n", Tosc);
                    H_L_Level_time_cnt = 0;         //高低电平计时变量清0
                    receive_state = DATA_REV_STATE;   //进入读取数据码低电平状态
                }
            }
            break;
						*/

        case DATA_REV_STATE:          //在读取数据码电平期间
            //逻辑“0”为 64Tosc低电平 + 32Tosc高电平
            //逻辑“1”为 32Tosc低电平 + 64Tosc高电平
            //如何判断当前为逻辑“0”还是逻辑“1”，关键在于寻找共同点
            //方法一：
            //不管是逻辑“0”还是逻辑“1”，周期一样，都是32Tosc + 64Tosc = 96Tosc
            //可以取中间时间点进行判断，96Tosc / 2 = 48Tosc，当计数>=48Tosc时读取引脚电平
            //如果还没有读取一个bit位，且时间计数已经>=48Tosc
            if ((has_read_bit==0) && (H_L_Level_time_cnt >= (HALF_LOGIC_CYCLE * Tosc))) 
            {
                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);		//单个字节放入数组，放够8bit
                has_read_bit = 1;
            }
						
						//方法二：
            //不管是逻辑“0”还是逻辑“1”，高低电平维持时间都是以 32Tosc 为基数，
            //64Tosc = 2 * 32Tosc，所以一个逻辑周期 96Tosc = 64Tosc + 32Tosc = 3 * 32Tosc
            //所以可以取一个逻辑周期的中间时间端进行判断，即>32Tosc 且 <64Tosc 这段时间内判断
//            if ((has_read_bit==0) && (H_L_Level_time_cnt > (SHORT_TIME_NUM * Tosc)) && (H_L_Level_time_cnt < (LONG_TIME_NUM * Tosc))) 
//            {
//                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);
//                has_read_bit = 1;
//            }

            //如果已经读取一个bit位，且时间计数已经>=96Tosc，说明一个逻辑周期过去了
            if ((has_read_bit==1) && (H_L_Level_time_cnt >= (LOGIC_CYCLE_NUM * Tosc)))
            {
                H_L_Level_time_cnt = 0;         //高低电平计时变量清0
                has_read_bit = 0;               //清0，读取下一个bit位
                receive_bit_num--;              //接收的bit数--
                
                if (receive_bit_num==0)         //如果一个字节8个bit位接收完成
                {		
                    receive_data_num++;         //接收的数据个数++
                    receive_bit_num = REV_BIT_NUM;  //重置接收bit位个数
                    if (receive_data_num == REV_DATA_NUM)   //如果数据采集完毕
                    {
                        read_success = 1;                   //一帧数据读取成功
                        start_H_L_Level_timming_flag = 0;   //停止高低电平计时
                        H_L_Level_time_cnt = 0;             //定时器计数值清0
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
            start_H_L_Level_timming_flag = 0;       //停止高低电平计时
            H_L_Level_time_cnt = 0;                 //定时器计数值清0
            receive_state = INITIAL_STATE;          //接收状态清0    	
            break;
    }
}

/*******************************************************************************
 *函数名称 : sif_process
 *函数功能 : 主函数入口
*******************************************************************************/
void sif_process(void)
{
		unsigned char i;
		unsigned char data;
    //gpio_input_init();	//gpio初始化，一线通协议输入
    //etm_timerinit();    //定时器0初始化，定时周期为：5微秒，回调函数为ETM2Callback

    while(1)
    {
        Receive_Data_Baud_Adjust_Bit_Handle();    //如果主循环中处理的任务比较多也可以在定时中断服务函数中调用
        
        if (read_success == 1)              //如果成功读取一帧数据
        {
						//一帧数据接收成功后先根据协议要求进行校验和，验证数据的正确性
            //如果数据正确，根据接收的数据进行分析获取需要的内容
            for(i=0; i<REV_DATA_NUM; i++)
						{
								printf("0x%02X ",receive_data_buf[i]);
						}
						printf("\n");
								
						if (verifyFrame(receive_data_buf)) 
						{
                // 校验通过，处理数据
                for (i = 2; i < REV_DATA_NUM; i++) 
								{
                    data = receive_data_buf[i];
                    processData(data); // 处理单个数据位
                }
            } 
						else 
						{
                // 校验失败，丢弃数据或采取其他处理措施
							printf("---verifyFrame false!!---\n");
            }
            read_success = 0;               //读取一帧数据清0
        }
    }
}
