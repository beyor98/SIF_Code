
#include <stdio.h>
#include <common.h>

#include <drv/dev/gpio.h>
#include <drv/dev/etm.h>
#include <drv/dev/uart.h>


#define LOW                 0       //�͵�ƽ
#define HIGH                1       //�ߵ�ƽ

#define DATA_REV_PIN        GPIOC8     //�������ݽ�������

#define SYNC_TIME_NUM       992     //992Tosc�е�992
#define SHORT_TIME_NUM      32      //һ���߼������ж̵�ʱ�䣺32Tosc�е�32
#define LONG_TIME_NUM       64      //һ���߼������г���ʱ�䣺64Tosc�е�64
#define LOGIC_CYCLE_NUM     96      //һ���߼�����  SHORT_TIME_NUM + LONG_TIME_NUM
#define HALF_LOGIC_CYCLE    48      //һ���߼����ڵ�1/2����  LOGIC_CYCLE_NUM/2

#define REV_BIT_NUM         8       //���յ�bitλ���������ǰ��ֽڽ��ջ��ǰ��ֽ��գ�1�ֽ�=8bit��1��=2�ֽ�=16bit
#define REV_DATA_NUM        12      //���յ����ݸ���



typedef enum
{
    INITIAL_STATE=0,            //��ʼ״̬���ȴ�����ͬ���ź�
    SYNC_L_STATE=1,             //����ͬ���͵�ƽ�ź�״̬
    SYNC_H_STATE=2,             //����ͬ���ߵ�ƽ�ź�״̬
    DATA_REV_STATE=3,           //��ȡ�������ƽ״̬
    RESTART_REV_STATE=4         //���չ��̳������½���״̬
}REV_STATE_e;                   //��������״̬ö��


unsigned char receive_state=0;      //��������״̬
unsigned char receive_bit_num=0;    //���յ�bitλ����
unsigned char receive_data_num=0;   //���յ����ݸ���

unsigned char receive_data_buf[REV_DATA_NUM]={0};   //�������ݻ�������-���һ֡�����ж�����ݴ�ע��
unsigned int  time_1ms_cnt=0; //�ߵ͵�ƽʱ�����

int start_500us_count_flag=0; //��ʼ�ߵ͵�ƽ��ʱ���
int start_3ms_count_flag=0;
int countdown_3ms_flag=0;		//����3ms��ʱ������ʱ
int has_read_bit = 0;               //1-�Ѿ���ȡһ��bitλ
int read_success=0;                 //һ֡�����Ƿ��ȡ�ɹ���0-���ɹ���1-�ɹ�
int is_read = FALSE;
int val =0;

int etm2_timerinit(void);
void gpio_input_init(void);
void Receive_Data_Baud_Adjust_Bit_Handle(void);   //�������ݴ�����У׼λ��������������Ӧ



int processData(unsigned char data) 
{
	//printf("enter the processDate !!");
	return 0;
}


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
        return checksum == frame[REV_DATA_NUM-1]; // ����У����
    }
		
    return 0; 	// ֡ͷ��ƥ�䣬У��ʧ��
}


void ETM1Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)		//3ms��ʱ��
{
		if(start_3ms_count_flag == 1)
		{
				is_read = TRUE;
		}
}


void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)		//0.5ms��ʱ��
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
    switch (receive_state)                      //��⵱ǰ��������״̬
    {
        case INITIAL_STATE:                         //��ʼ״̬��δ���յ�ͬ����Ϣ������ͬ���ж�
            if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //�жϽ������ŵĵ�ƽ״̬���������͵�ƽʱ����ʼ��ʱ
            {
                receive_bit_num = REV_BIT_NUM;      //����bitλ������
                receive_data_num = 0;               //���ý������ݸ���
                time_1ms_cnt = 0; 
                start_500us_count_flag = 1;
								ETM_StartTimeBase(ETM2);
                receive_state = SYNC_L_STATE;       //�����ȡͬ���͵�ƽ�ź�״̬
            }
            break;
        
        case SYNC_L_STATE:                          //�ڶ�ȡͬ���͵�ƽ�ź��ڼ�
            if (time_1ms_cnt > 60)    //����͵�ƽʱ��>30ms
            {                                       
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)           //�жϽ������ŵĵ�ƽ״̬���������ߵ�ƽʱ
                {
                    time_1ms_cnt = 0;         
										start_500us_count_flag = 0;   
                    receive_state = SYNC_H_STATE;   //�����ȡͬ���źŸߵ�ƽ״̬
										ETM_StopTimeBase(ETM2);
                }     
            }
            else
            {
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)           //ͬ���źŵ͵�ƽ����ڼ�����ߵ�ƽ���¼�ʱ
                {
                    receive_state = RESTART_REV_STATE;      //�������½���״̬
                }  
            }
            break;

        case SYNC_H_STATE:                          //�ڶ�ȡͬ���ߵ�ƽ�ź��ڼ�
						if (GPIO_PinRead(DATA_REV_PIN) == LOW)            //>=ͬ���źŸߵ�ƽ���ʱ�������͵�ƽ
						{
								ETM_StartTimeBase(ETM2);
								countdown_3ms_flag=3;		//ʹ��etm2����1.5ms��ʱ��
								start_3ms_count_flag = 1;
								memset(&receive_data_buf, 0, sizeof(receive_data_buf));
								receive_state = DATA_REV_STATE;	
						} 
						
        case DATA_REV_STATE:          //�ڶ�ȡ�������ƽ�ڼ�
            //ȡ�м�ʱ�������ж�
            //�����û�ж�ȡһ��bitλ����ʱ������Ѿ�>=3msһ������
            if ((has_read_bit==0) && (is_read == TRUE)) //is_readΪ0.75ms��ʱ���жϵı�־λ
            {
                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);		//�����ֽڷ������飬�Ź�8bit
								GPIO_PinToggle(GPIOA8);
                has_read_bit = 1;
								is_read = FALSE;
            }
            //����Ѿ���ȡһ��bitλ����ʱ������Ѿ�>=96Tosc��˵��һ���߼����ڹ�ȥ��
            if (has_read_bit==1)
            {
                has_read_bit = 0;               //��0����ȡ��һ��bitλ
                receive_bit_num--;              //���յ�bit��--
                if (receive_bit_num==0)         //���һ���ֽ�8��bitλ�������
                {		
                    receive_data_num++;         //���յ����ݸ���++
                    receive_bit_num = REV_BIT_NUM;  //���ý���bitλ����
                    if (receive_data_num == REV_DATA_NUM)   //������ݲɼ����
                    {
                        read_success = 1;                   //һ֡���ݶ�ȡ�ɹ�
                        start_500us_count_flag = 0;   
                        time_1ms_cnt = 0;             //��ʱ������ֵ��0
												start_3ms_count_flag = 0;
												ETM_StopTimeBase(ETM1);
												receive_state = INITIAL_STATE;      //����״̬��0   
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
            time_1ms_cnt = 0;                 //��ʱ������ֵ��0
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
						// У��ʧ��
				}
				read_success = 0;               //��ȡһ֡������0
		}
    
}
