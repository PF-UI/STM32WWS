#ifndef __KEY_H
#define __KEY_H	 
#include "system.h"	 


#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡ����1���¶ȸ澯����  
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//��ȡ����2��ʪ�ȸ澯����
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//��ȡ����3 ��ʪ�豸����

#define KEY1_PRES	1		//KEY1  
#define KEY2_PRES	2	  //KEY1   
#define KEY3_PRES	3  //KEY1 

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);  	//����ɨ�躯��					    
#endif
