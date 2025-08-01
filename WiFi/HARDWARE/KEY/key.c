#include "key.h"
#include "SysTick.h"
 	    
//������ʼ������ 
//PB0��PB1���ó�����
void KEY_Init(void)
{	
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��PORTBʱ��
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;//PB10��PB11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	
} 
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//����ֵ:0��û���κΰ�������

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(KEY1==0||KEY2==0||KEY3==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEY1==0)return KEY1_PRES;
		else if(KEY2==0)return KEY2_PRES;
		else if(KEY3==0)return KEY3_PRES;
					
	}
	else if(KEY1==1&&KEY2==1&&KEY3==1)
	key_up=1; 	     
	return 0;// �ް�������
}
