
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

unsigned int  H_L_Level_time_cnt=0; //�ߵ͵�ƽʱ�����

unsigned int  Tosc = 6;             //����ʱ����Ԫ��һ�������������Ӧ�ģ�����˵���ߵ͵�ƽ��ʱ�䣬����һ��Toscʱ������
                                    //������Ĳ���ͼ��Ҫ�� 32Tosc = 1ms = 1000us
                                    //���ԣ�һ��Tosc = 1000us/32 �� 31us������ʱ��0���ζ�ʱʱ��Ϊ5us������ʵ��һ��Tosc = 30us/5us = 6 ��
                                    //�������Ϊ 5us ����Ϊ���õ�һ����ʱ�����ζ�ʱʱ�䣬�������ʱʱ��Ļ������ֲ�����һ��ʵ��ʱ��Tosc�����ڲ����ϣ����λ������Toscʱ����Ԫ
                                    //���Զ�̬�ĵ����ߵ͵�ƽ��ʱ�䣬ֻҪTosc�ı䣬992Tosc��32Tosc�Լ�64Tosc��Ӧ��ʱ��Ҳ����֮�ı䣻Ȼ�����Ƿ�����˼�����ٲ�֪�������ʵ�����£�
                                    //ȥ��ȡͬ���źŸߵ�ƽ��ʱ�䣬��������ʱ�����H_L_Level_time_cnt * 5us�Ǹߵ�ƽ����ʵά��ʱ�� = 32 * Tosc * 5us = SHORT_TIME_NUM * Tosc * 5us
                                    //Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM
                                    //�ڱ������У�һ֡���� = 992Tosc+32Tosc+(64+32)*8*12Tosc = 10240*Tosc ��
                                    //1���� = 1000000us�����ζ�ʱ 5us����1s / 5us = 200000 ��
                                    //��1���ӿ��Խ��� 200000 / (10240 * Tosc) = 200000/10240/Tosc = 19/Tosc ֡����

int start_H_L_Level_timming_flag=0; //��ʼ�ߵ͵�ƽ��ʱ���
int has_read_bit = 0;               //1-�Ѿ���ȡһ��bitλ
int read_success=0;                 //һ֡�����Ƿ��ȡ�ɹ���0-���ɹ���1-�ɹ�

int count=0;

/* �������� -------------------------------------------------------------------*/
int etm_timerinit(void);
void gpio_input_init(void);
void Receive_Data_Baud_Adjust_Bit_Handle(void);   //�������ݴ�����У׼λ��������������Ӧ


/*******************************************************************************
 *�������� : processData
 *�������� : ���ݴ���
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
				// У��֡ͷ
        for (i = 0; i < (REV_DATA_NUM-1); i++) 
				{
					checksum ^= frame[i];
        }
        return checksum == frame[REV_DATA_NUM-1]; // ����У����
    }
		
    return 0; // ֡ͷ��ƥ�䣬У��ʧ��
}

void ETM2Callback(ETM_Device_T *pETM, uint32_t Event, unsigned long Param)
{
		if(1)		//if (start_H_L_Level_timming_flag==1)
		{
				H_L_Level_time_cnt++;     //�ߵ͵�ƽά��ʱ���������
				
		}
		count++;
		if(count==100)
		{
			count=0;
			GPIO_PinToggle(GPIOA8);
		}
	
}

/*******************************************************************************
 *�������� : Receive_Data_Baud_Adjust_Bit_Handle
 *�������� : �������ݴ�������������Ӧ
*******************************************************************************/
void Receive_Data_Baud_Adjust_Bit_Handle(void)
{
    switch (receive_state)                      //��⵱ǰ��������״̬
    {
        case INITIAL_STATE:                         //��ʼ״̬��δ���յ�ͬ����Ϣ������ͬ���ж�
            if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //�жϽ������ŵĵ�ƽ״̬���������͵�ƽʱ����ʼ��ʱ
            {
                receive_bit_num = REV_BIT_NUM;      //����bitλ������
                receive_data_num = 0;               //���ý������ݸ���
                H_L_Level_time_cnt = 0;             //�ߵ͵�ƽ��ʱ������0
                start_H_L_Level_timming_flag = 1;   //��ʼ�ߵ͵�ƽ��ʱ
                receive_state = SYNC_L_STATE;       //�����ȡͬ���͵�ƽ�ź�״̬
            }
            break;
        
        case SYNC_L_STATE:                          //�ڶ�ȡͬ���͵�ƽ�ź��ڼ�
            if (H_L_Level_time_cnt > SYNC_TIME_NUM*Tosc)    //����͵�ƽʱ��>SYNC_TIME_NUM*Tosc=992*3*5us
            {                                       //ͬ��״̬����ʱ�����15ms
                if (GPIO_PinRead(DATA_REV_PIN) == HIGH)           //�жϽ������ŵĵ�ƽ״̬���������ߵ�ƽʱ
                {
                    H_L_Level_time_cnt = 0;         //�ߵ͵�ƽ��ʱ������0
                    receive_state = SYNC_H_STATE;   //�����ȡͬ���źŸߵ�ƽ״̬
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
            
            //����д��һ��
            if (H_L_Level_time_cnt >= SHORT_TIME_NUM*Tosc)    //����ߵ�ƽʱ�� >= SHORT_TIME_NUM*Tosc=32 * 3 * 5us
            {
                if (GPIO_PinRead(DATA_REV_PIN) == LOW)            //>=ͬ���źŸߵ�ƽ���ʱ�������͵�ƽ
                {
                    //��������߼�������״̬��1���ߵ�ƽʱ������=32Tosc��2���ߵ�ƽʱ�䳤������32Tosc
                    Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM;    //���� Tosc ��ֵ
									printf("Tosc_1 = %d\n", Tosc);
                    H_L_Level_time_cnt = 0;         //�ߵ͵�ƽ��ʱ������0
                    //receive_state = DATA_REV_L_STATE;   //�����ȡ������͵�ƽ״̬
										receive_state = DATA_REV_STATE;
                } 
            }
            else
            {
                if (GPIO_PinRead(DATA_REV_PIN) == LOW)            //ͬ���źŸߵ�ƽ����ڼ�����͵�ƽ
                {                                   //�ߵ�ƽʱ��̣�û����32Tosc���Զ������������
                    //H_L_Level_time_cnt * 5us Ҫ���� SHORT_TIME_NUM * Tosc * 5us���� H_L_Level_time_cnt = SHORT_TIME_NUM * Tosc
                    Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM;    //���� Tosc ��ֵ
									printf("Tosc_2 = %d\n", Tosc);
                    H_L_Level_time_cnt = 0;         //�ߵ͵�ƽ��ʱ������0
                    //receive_state = DATA_REV_L_STATE;   //�����ȡ������͵�ƽ״̬
									receive_state = DATA_REV_STATE;
                }  
            }
						
            
            //����д������
            /*if (H_L_Level_time_cnt >= LOGIC_CYCLE_NUM*Tosc)   //����ߵ�ƽʱ�䳬���ˣ�32+64=96����Tosc������Ϊ��ʱ
            {
                receive_state = RESTART_REV_STATE;      //�������½���״̬
            }
            else
            {
                if (GPIO_PinRead(DATA_REV_PIN) == LOW)                //ͬ���źŸߵ�ƽ����ڼ�����͵�ƽ
                {
                    //��ͬ���źŸߵ�ƽ����ڼ�����͵�ƽ����������״̬��
                    //1���ߵ�ƽʱ��̣�����32Tosc
                    //2���ߵ�ƽʱ������=32Tosc
                    //3���ߵ�ƽʱ�䳤������32Tosc
                    //���ܺ���״̬����Ҫ ���� Tosc ��ֵ�ﵽ����������Ӧ
                    //H_L_Level_time_cnt * 5us Ҫ���� SHORT_TIME_NUM * Tosc * 5us��
                    //�� H_L_Level_time_cnt = SHORT_TIME_NUM * Tosc
                    Tosc = H_L_Level_time_cnt / SHORT_TIME_NUM;    //���� Tosc ��ֵ
										printf("Tosc = %d\n", Tosc);
                    H_L_Level_time_cnt = 0;         //�ߵ͵�ƽ��ʱ������0
                    receive_state = DATA_REV_STATE;   //�����ȡ������͵�ƽ״̬
                }
            }
            break;
						*/

        case DATA_REV_STATE:          //�ڶ�ȡ�������ƽ�ڼ�
            //�߼���0��Ϊ 64Tosc�͵�ƽ + 32Tosc�ߵ�ƽ
            //�߼���1��Ϊ 32Tosc�͵�ƽ + 64Tosc�ߵ�ƽ
            //����жϵ�ǰΪ�߼���0�������߼���1�����ؼ�����Ѱ�ҹ�ͬ��
            //����һ��
            //�������߼���0�������߼���1��������һ��������32Tosc + 64Tosc = 96Tosc
            //����ȡ�м�ʱ�������жϣ�96Tosc / 2 = 48Tosc��������>=48Toscʱ��ȡ���ŵ�ƽ
            //�����û�ж�ȡһ��bitλ����ʱ������Ѿ�>=48Tosc
            if ((has_read_bit==0) && (H_L_Level_time_cnt >= (HALF_LOGIC_CYCLE * Tosc))) 
            {
                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);		//�����ֽڷ������飬�Ź�8bit
                has_read_bit = 1;
            }
						
						//��������
            //�������߼���0�������߼���1�����ߵ͵�ƽά��ʱ�䶼���� 32Tosc Ϊ������
            //64Tosc = 2 * 32Tosc������һ���߼����� 96Tosc = 64Tosc + 32Tosc = 3 * 32Tosc
            //���Կ���ȡһ���߼����ڵ��м�ʱ��˽����жϣ���>32Tosc �� <64Tosc ���ʱ�����ж�
//            if ((has_read_bit==0) && (H_L_Level_time_cnt > (SHORT_TIME_NUM * Tosc)) && (H_L_Level_time_cnt < (LONG_TIME_NUM * Tosc))) 
//            {
//                receive_data_buf[receive_data_num] |= GPIO_PinRead(DATA_REV_PIN);
//                has_read_bit = 1;
//            }

            //����Ѿ���ȡһ��bitλ����ʱ������Ѿ�>=96Tosc��˵��һ���߼����ڹ�ȥ��
            if ((has_read_bit==1) && (H_L_Level_time_cnt >= (LOGIC_CYCLE_NUM * Tosc)))
            {
                H_L_Level_time_cnt = 0;         //�ߵ͵�ƽ��ʱ������0
                has_read_bit = 0;               //��0����ȡ��һ��bitλ
                receive_bit_num--;              //���յ�bit��--
                
                if (receive_bit_num==0)         //���һ���ֽ�8��bitλ�������
                {		
                    receive_data_num++;         //���յ����ݸ���++
                    receive_bit_num = REV_BIT_NUM;  //���ý���bitλ����
                    if (receive_data_num == REV_DATA_NUM)   //������ݲɼ����
                    {
                        read_success = 1;                   //һ֡���ݶ�ȡ�ɹ�
                        start_H_L_Level_timming_flag = 0;   //ֹͣ�ߵ͵�ƽ��ʱ
                        H_L_Level_time_cnt = 0;             //��ʱ������ֵ��0
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
            start_H_L_Level_timming_flag = 0;       //ֹͣ�ߵ͵�ƽ��ʱ
            H_L_Level_time_cnt = 0;                 //��ʱ������ֵ��0
            receive_state = INITIAL_STATE;          //����״̬��0    	
            break;
    }
}

/*******************************************************************************
 *�������� : sif_process
 *�������� : ���������
*******************************************************************************/
void sif_process(void)
{
		unsigned char i;
		unsigned char data;
    //gpio_input_init();	//gpio��ʼ����һ��ͨЭ������
    //etm_timerinit();    //��ʱ��0��ʼ������ʱ����Ϊ��5΢�룬�ص�����ΪETM2Callback

    while(1)
    {
        Receive_Data_Baud_Adjust_Bit_Handle();    //�����ѭ���д��������Ƚ϶�Ҳ�����ڶ�ʱ�жϷ������е���
        
        if (read_success == 1)              //����ɹ���ȡһ֡����
        {
						//һ֡���ݽ��ճɹ����ȸ���Э��Ҫ�����У��ͣ���֤���ݵ���ȷ��
            //���������ȷ�����ݽ��յ����ݽ��з�����ȡ��Ҫ������
            for(i=0; i<REV_DATA_NUM; i++)
						{
								printf("0x%02X ",receive_data_buf[i]);
						}
						printf("\n");
								
						if (verifyFrame(receive_data_buf)) 
						{
                // У��ͨ������������
                for (i = 2; i < REV_DATA_NUM; i++) 
								{
                    data = receive_data_buf[i];
                    processData(data); // ����������λ
                }
            } 
						else 
						{
                // У��ʧ�ܣ��������ݻ��ȡ���������ʩ
							printf("---verifyFrame false!!---\n");
            }
            read_success = 0;               //��ȡһ֡������0
        }
    }
}
