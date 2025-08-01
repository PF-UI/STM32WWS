#include "relay.h"

void RELAY_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);  
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable , ENABLE); //Full SWJ Disabled (JTAG-DP + SW-DP)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  // PA1 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOA	
 
  GPIO_SetBits(GPIOA,GPIO_Pin_1); 		//PA1 �˿����ã����Ϊ��
}
 
