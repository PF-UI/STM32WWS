/*************************************************************************
 ??:??????Wifi??????????
 ????:?????????????????
**************************************************************************/
#include "oled.h"
#include "SysTick.h"
#include "system.h"
#include "usart.h"
#include "dht11.h"
#include "led.h"
#include "string.h"
#include "relay.h"
#include "key.h"
#include "beep.h"
#include "timer.h"
#include "esp8266.h"  // ??ESP8266?????

// ??????
extern unsigned char RxBuffer[100], RxCounter;  // ?????????
extern char Temp[2], Humi[2];
unsigned char Beep_State1 = 0; 
unsigned char Beep_State2 = 0; 
char Max_Temp[2] = "25";  // ???????
char Max_Humi[2] = "50";  // ???????

// 通过ESP8266发送数据到TCP服务器
void ESP8266_SendData(char *data, u16 len)
{
    char cmd[30];
    // 发送数据长度指令（单连接模式）
    sprintf(cmd, "AT+CIPSEND=%d\r\n", len);
    printf("%s", cmd);
    delay_ms(100);
    
    // 等待ESP8266返回">"提示符
    if(ESP8266_Wait_Resp(">", 1000))
    {
        printf("%s", data); // 发送实际数据
        delay_ms(100);
        // 等待发送成功响应
        if(ESP8266_Wait_Resp("SEND OK", 1000))
        {
            OLED_ShowString(0, 6, "Send OK    ");
        }
        else
        {
            OLED_ShowString(0, 6, "Send Fail  ");
        }
    }
    else
    {
        OLED_ShowString(0, 6, "No '>' Resp");
    }
    Clear_Buffer(); // 清空缓冲区
}

int main(void)
{ 	
	unsigned char i, j;
	unsigned char count = 0;
	unsigned char TAlarm_flag = 0;  // ??????
	unsigned char HAlarm_flag = 0;  // ??????
	unsigned char key = 0;          // ???
	int key1_State = 0;             // ??1??
	int key2_State = 0;             // ??2??
	int key3_State = 0;             // ??3??
	int relay_State = 0;            // ?????
	unsigned char TAlarm_State = 0; // ??????
	unsigned char HAlarm_State = 0; // ??????	
	char send_buf[50]; // 发送数据缓冲区
	// ?????
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  // ???????
	SysTick_Init(72);               // ??SysTick?????(72MHz)
	USART1_Init(115200);            // ????1?????
	USART2_Init(115200);            // ????2?????
	TIM3_Int_Init(999, 7199);       // 100ms????(72MHz??)
	LED_Init();		 
	KEY_Init();
	BEEP_Init();
	OLED_Init();
	OLED_Clear();
  
	ESP8266_Init();                 
	delay_ms(300);
	delay_ms(300);
	delay_ms(300);
	OLED_Clear();
	
	
	
	
	while(DHT11_Init())	
	{ 	
		OLED_ShowString(18, 3, "DHT11 Error"); 	
	}	
	OLED_ShowString(18, 3, "DHT11 OK    ");
	delay_ms(300);		
	OLED_Clear(); 			
	

	OLED_ShowString(0, 0, "T:");               
	OLED_ShowCHinese(5*8, 0, 4);               
	OLED_ShowString(56, 0, "<  >");
	OLED_ShowChar(56+8, 0, Max_Temp[0]);
	OLED_ShowChar(56+16, 0, Max_Temp[1]);
	
	OLED_ShowString(0, 3, "H:  %RH");          
	OLED_ShowString(56, 3, "<  >");
	OLED_ShowChar(56+8, 3, Max_Humi[0]);
	OLED_ShowChar(56+16, 3, Max_Humi[1]);
	
	OLED_ShowString(0, 6, "D:");               
	
	while(1)
	{
		
		DHT11_Read_Data();
		delay_ms(10);

		OLED_ShowChar(16, 0, Temp[0]);
		OLED_ShowChar(16+8, 0, Temp[1]);
		OLED_ShowChar(16, 3, Humi[0]);
		OLED_ShowChar(16+8, 3, Humi[1]);
		delay_ms(10);	
		
		
      // 采集温湿度（1秒一次）
        if(count % 10 == 0)
        {
            DHT11_Read_Data(); // 读取DHT11数据
            // 显示当前温湿度
            OLED_ShowChar(16, 0, Temp[0]);
            OLED_ShowChar(24, 0, Temp[1]);
            OLED_ShowChar(16, 3, Humi[0]);
            OLED_ShowChar(24, 3, Humi[1]);
        }
        
        // 每2秒发送一次温湿度数据（符合上传协议）
        if(count >= 20) // 100ms*50=5000ms=5秒
        {
            // 合并温湿度数据并一次性发送
            sprintf(send_buf, "VFD1A+%c%c.%cVF\r\nVFD2A000%c%cVF\r\n", Temp[0], Temp[1], Temp[2], Humi[0], Humi[1]);
            ESP8266_SendData(send_buf, strlen(send_buf));
            count = 0;
        }
        count++;
		
		
		if(strstr((const char*)RxBuffer, "CRD0AONCR"))    
		{
			relay_State = 1;  
			key3_State = 1; 
			Clear_Buffer();	
		}	
		if(strstr((const char*)RxBuffer, "CRD0AOFCR"))	
		{ 
			relay_State = 0;  
			key3_State = 0; 
			Clear_Buffer();	
		}		
		if(strstr((const char*)RxBuffer, "CRALM1ONCR"))	
		{  
			TAlarm_State = 1;  
			key1_State = 1;
			Clear_Buffer();	
		}
		if(strstr((const char*)RxBuffer, "CRALM1OFCR"))	
		{ 
			TAlarm_State = 0;   
			key1_State = 0;			
			Clear_Buffer();	
		}		
		if(strstr((const char*)RxBuffer, "CRALM2ONCR"))	
		{	 
			HAlarm_State = 1;  
			key2_State = 1;		
			Clear_Buffer();	
		}
		if(strstr((const char*)RxBuffer, "CRALM2OFCR"))	
		{   
			HAlarm_State = 0;   
			key2_State = 0;		
			Clear_Buffer();	
		}
		

		for(j = 0; j < 100; j++)
		{

			if(RxBuffer[j] == 'S' && RxBuffer[j+1] == '1')
			{  
				if(RxBuffer[4+j] >= '0' && RxBuffer[4+j] <= '9' 
				&& RxBuffer[5+j] >= '0' && RxBuffer[5+j] <= '9')
				{
					Max_Temp[0] = RxBuffer[4+j];
					Max_Temp[1] = RxBuffer[5+j];
				}
				Clear_Buffer();
			}

			if(RxBuffer[j] == 'S' && RxBuffer[j+1] == '2')
			{  
				if(RxBuffer[6+j] >= '0' && RxBuffer[6+j] <= '9' 
				&& RxBuffer[7+j] >= '0' && RxBuffer[7+j] <= '9')
				{
					Max_Humi[0] = RxBuffer[6+j];
					Max_Humi[1] = RxBuffer[7+j];
				}
				Clear_Buffer();
			}		
		}

		OLED_ShowChar(56+8, 0, Max_Temp[0]);
		OLED_ShowChar(56+16, 0, Max_Temp[1]);	
		OLED_ShowChar(56+8, 3, Max_Humi[0]);
		OLED_ShowChar(56+16, 3, Max_Humi[1]);	

		key = KEY_Scan(0);			 
		switch(key)
		{				 
			case KEY1_PRES:
				key1_State++;	break;
			case KEY2_PRES:
				key2_State++;	break;	
			case KEY3_PRES:
				key3_State++;	break;			
			default:
				delay_ms(1);	break;
		} 		

		if(relay_State == 1)
		{ 			 	
			if(key3_State % 2 == 0) 
			{
				RELAY = 0;
				OLED_ShowString(16, 6, "OFF ");
			}			 
			else
			{				 
				RELAY = 1;	
				OLED_ShowString(16, 6, "ON ");				 
			}	 
		}		 
		if(relay_State == 0)
		{ 
			if(key3_State % 2 == 1) 
			{				
				RELAY = 1;
				OLED_ShowString(16, 6, "ON ");
			}	
			else			 
			{
				RELAY = 0;
				OLED_ShowString(16, 6, "OFF ");
			}				 
		}   

		if(TAlarm_State == 1)
		{ 			 	
			if(key1_State % 2 == 0) 
			{
				TAlarm_flag = 0;
				OLED_ShowString(96, 0, "OFF ");  				 
			}			 
			else
			{
				TAlarm_flag = 1;
				OLED_ShowString(96, 0, "ON ");  		 
			}	 
		}		 
		if(TAlarm_State == 0)
		{ 
			if(key1_State % 2 == 1) 
			{
				TAlarm_flag = 1;
				OLED_ShowString(96, 0, "ON "); 				 
			}	
			else			 
			{
				TAlarm_flag = 0;
				OLED_ShowString(96, 0, "OFF "); 				 
			}				 
		}  

		if(HAlarm_State == 1)
		{ 			 	
			if(key2_State % 2 == 0) 
			{
				HAlarm_flag = 0;
				OLED_ShowString(96, 3, "OFF");    				 
			}			 
			else
			{
				HAlarm_flag = 1;
				OLED_ShowString(96, 3, "ON "); 		 
			}			 
		}		 
		if(HAlarm_State == 0)
		{ 
			if(key2_State % 2 == 1) 
			{
				HAlarm_flag = 1;
				OLED_ShowString(96, 3, "ON "); 
			}	
			else			 
			{
				HAlarm_flag = 0;
				OLED_ShowString(96, 3, "OFF");
			}	 
		}   		 

		
		if(TAlarm_flag)
		{				
			if(Temp[0] > Max_Temp[0] || 
			   (Temp[0] == Max_Temp[0] && Temp[1] >= Max_Temp[1]))
			{
				D3 = ~D3;  
				delay_ms(10);	
				Beep_State1 = 1;
			}
			else
			{
				D3 = 1;	  
				Beep_State1 = 0;
			}  
		}		
		else
		{
			D3 = 1;
			Beep_State1 = 0;	 
		} 
		
		if(HAlarm_flag)
		{	     
			if(Humi[0] > Max_Humi[0] || 
			   (Humi[0] == Max_Humi[0] && Humi[1] >= Max_Humi[1]))
			{
				D4 = ~D4;  
				delay_ms(10);	
				Beep_State2 = 1;
			}
			else
			{
				D4 = 1;	  
				Beep_State2 = 0;
			}  
		}
		else
		{
			D4 = 1;
			Beep_State2 = 0;	 
		} 	 
	}
}