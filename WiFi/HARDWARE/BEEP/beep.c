#include "beep.h"


/*******************************************************************************
* �� �� ��         : BEEP_Init
* ��������		   : ��������ʼ��
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/

void BEEP_Init()	  //�˿ڳ�ʼ��
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//�ر�jtag��ʹ��SWD��������SWDģʽ����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //PA7�˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);					     //�����趨������ʼ��GPIOA	
			 
  GPIO_SetBits(GPIOA,GPIO_Pin_7);				//PA7 �˿�����,���Ϊ��	

 
}


