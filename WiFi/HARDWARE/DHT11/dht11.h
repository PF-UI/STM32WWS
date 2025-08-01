#ifndef __DHT11_H
#define __DHT11_H 
#include "system.h"   

#define DHT11_IO_IN()  {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=8<<16;}
#define DHT11_IO_OUT() {GPIOA->CRL&=0XFFF0FFFF;GPIOA->CRL|=3<<16;}
//IO��������											   
#define	DHT11_DQ_OUT PAout(4) //���ݶ˿�	PA4
#define	DHT11_DQ_IN  PAin(4)  //���ݶ˿�	PA4


u8 DHT11_Init(void);//��ʼ��DHT11
u8 DHT11_Read_Data(void);//��ȡ��ʪ��
u8 DHT11_Read_Byte(void);//����һ���ֽ�
u8 DHT11_Read_Bit(void);//����һ��λ
u8 DHT11_Check(void);//����Ƿ����DHT11
void DHT11_Rst(void);//��λDHT11    

#endif



  

