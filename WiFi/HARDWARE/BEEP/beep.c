#include "beep.h"


/*******************************************************************************
* 函 数 名         : BEEP_Init
* 函数功能		   : 蜂鸣器初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void BEEP_Init()	  //端口初始化
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;
 	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//关闭jtag，使能SWD，可以用SWD模式调试
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //PA7端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	
  GPIO_Init(GPIOA, &GPIO_InitStructure);					     //根据设定参数初始化GPIOA	
			 
  GPIO_SetBits(GPIOA,GPIO_Pin_7);				//PA7 端口配置,输出为高	

 
}


