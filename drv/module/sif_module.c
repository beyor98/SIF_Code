
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
    INITIAL_STATE=0,            //��ʼ״̬���ȴ�����ͬ���ź�
    SYNC_L_STATE=1,             //����ͬ���͵�ƽ�ź�״̬
    SYNC_H_STATE=2,             //����ͬ���ߵ�ƽ�ź�״̬
    DATA_REV_STATE=3,           //��ȡ�������ƽ״̬
    RESTART_REV_STATE=4         //���չ��̳������½���״̬
}REV_STATE_e;                   //��������״̬ö��


unsigned char receive_state=0;
unsigned char receive_bit_num=0;
unsigned char receive_data_num=0;

unsigned char receive_data_buf[REV_DATA_NUM]={0};
unsigned int  time_1ms_cnt=0;

int start_500us_count_flag=0; 
int start_3ms_count_flag=0;
int countdown_3ms_flag=0;		//����3ms��ʱ������ʱ
int has_read_bit = 0;           //1-�Ѿ���ȡһ��bitλ
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
		
		if (frame[0] == 0x08 && frame[1] == 0x61) 	// У��֡ͷ
		{ 
        for (i = 0; i < (REV_DATA_NUM-1); i++) 
		{
					checksum ^= frame[i];			// У��֡β
        }
        return checksum == frame[REV_DATA_NUM-1];
    }
		
    return 0; 	// ֡ͷ��ƥ�䣬У��ʧ��
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
        case INITIAL_STATE:                         //��ʼ״̬��δ���յ�ͬ����Ϣ������ͬ���ж�
            if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //�жϽ������ŵĵ�ƽ״̬���������͵�ƽʱ����ʼ��ʱ
            {
                receive_bit_num = REV_BIT_NUM;
                receive_data_num = 0;
                time_1ms_cnt = 0; 
                start_500us_count_flag = 1;
								ETM_StartTimeBase(ETM2);
                receive_state = SYNC_L_STATE;       //�����ȡͬ���͵�ƽ�ź�״̬
            }
            break;
        
        case SYNC_L_STATE:
            if (time_1ms_cnt > 60)    //����͵�ƽʱ��>30ms
            {                                       
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)
                {
                    time_1ms_cnt = 0;         
										start_500us_count_flag = 0;   
                    receive_state = SYNC_H_STATE;   //�����ȡͬ���źŸߵ�ƽ״̬
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

							    countdown_3ms_flag=3;				//ʹ��etm2����3*0.5=1.5ms��ʱ��
								start_3ms_count_flag = 1;  
								memset(&receive_data_buf, 0, sizeof(receive_data_buf));
								receive_state = DATA_REV_STATE;	
						} 
						
        case DATA_REV_STATE:
            //ȡ�����е㣬�����û�ж�ȡһ��bitλ����ʱ������Ѿ�>=3msһ������
            if ((has_read_bit==0) && (is_read == TRUE)) //��һ��is_readΪ*0.5=1.5ms��ʱ����ʱ���жϣ�����Ϊ3ms���ڶ�ʱ
            {
                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);
#if SIF_DEBUG								
				GPIO_PinToggle(GPIOA8);		//����У׼ʹ��
#endif								
                has_read_bit = 1;
								is_read = FALSE;
            }
            //����Ѿ���ȡһ��bitλ��һ���߼����ڹ�ȥ
            if (has_read_bit==1)
            {
                has_read_bit = 0;
                receive_bit_num--;
                if (receive_bit_num==0)         //���һ���ֽ�8��bitλ�������
                {		
                    receive_data_num++; 
                    receive_bit_num = REV_BIT_NUM;
                    if (receive_data_num == REV_DATA_NUM)   //������ݲɼ����
                    {
                        read_success = 1;                   //һ֡���ݶ�ȡ�ɹ�
                        start_500us_count_flag = 0;   
                        time_1ms_cnt = 0;
												start_3ms_count_flag = 0;
												ETM_StopTimeBase(ETM1);
												receive_state = INITIAL_STATE;
                    }  
                }
				else
				{
						//���������ݻ�������һλ�����ݴӸ�bitλ��ʼ����
						receive_data_buf[receive_data_num] = receive_data_buf[receive_data_num] << 1;
				}
            }        
            break;

        case RESTART_REV_STATE:                     //���½�������״̬
            start_500us_count_flag = 0; 
						start_3ms_count_flag = 0;
            time_1ms_cnt = 0;
						is_read = FALSE;
            receive_state = INITIAL_STATE;          //����״̬��0    	
            break;
    }
}


void sif_process(void)
{
		unsigned char i;

		Receive_Data_Baud_Adjust_Bit_Handle();
		
		if (read_success == 1)              //����ɹ���ȡһ֡����
		{
				for(i=0; i<REV_DATA_NUM; i++)
				{
						printf("0x%02X ",receive_data_buf[i]);
				}
				printf("\n");
						
				if (verifyFrame(receive_data_buf)) 
				{
						// У��ͨ������������
						printf("---verifyFrame success!!---\n");
				} 
				else 
				{
						printf("---verifyFrame false!!---\n");// У��ʧ��
				}
				read_success = 0;               //��ȡһ֡������0
		}
		aip1640_test();
}
