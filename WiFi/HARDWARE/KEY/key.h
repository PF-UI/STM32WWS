#ifndef __KEY_H
#define __KEY_H	 
#include "system.h"	 


#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//读取按键1，温度告警开关  
#define KEY2  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10)//读取按键2，湿度告警开关
#define KEY3  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//读取按键3 ，湿设备开关

#define KEY1_PRES	1		//KEY1  
#define KEY2_PRES	2	  //KEY1   
#define KEY3_PRES	3  //KEY1 

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);  	//按键扫描函数					    
#endif
