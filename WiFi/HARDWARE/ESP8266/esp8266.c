#include "usart.h"
#include "string.h"
#include "wdg.h"
#include "SysTick.h"
#include "oled.h"
#include "beep.h" 

extern char RxBuffer[100], RxCounter;
char *strx;

// 清空接收缓冲区
void Clear_Buffer(void)
{
    u8 i;
    for(i = 0; i < 100; i++)
        RxBuffer[i] = 0;
    RxCounter = 0;
    IWDG_Feed(); // 喂狗
}

// 等待ESP8266返回指定响应（超时时间：ms）
u8 ESP8266_Wait_Resp(const char *resp, u16 timeout)
{
    u16 t = 0;
    while(t < timeout)
    {
        if(strstr((const char*)RxBuffer, resp) != NULL)
        {
            return 1; // 找到响应
        }
        delay_ms(10);
        t += 10;
    }
    return 0; // 超时未找到
}

// ESP8266初始化（连接WiFi和TCP服务器）
void ESP8266_Init(void)
{   
    // 测试AT指令
    printf("AT\r\n"); 
    delay_ms(300);
    while(ESP8266_Wait_Resp("OK", 1000) == 0)
    {  
        printf("AT\r\n");        	
        delay_ms(500);			
        OLED_ShowString(0,0,"Checking AT...");
    }        
    Clear_Buffer();
            
    // 设置STA模式
    printf("AT+CWMODE=1\r\n");  
    delay_ms(300);
    while(ESP8266_Wait_Resp("OK", 1000) == 0)    
    {  
        printf("AT+CWMODE=1\r\n");        	
        delay_ms(100);			
        OLED_ShowString(0,0,"Setting CWMODE...");
    }
    Clear_Buffer();
    
    // 连接WiFi
    printf("AT+CWJAP=\"PF2025\",\"12345678\"\r\n");
    OLED_ShowString(0,0,"Connecting WiFi...");
    while(ESP8266_Wait_Resp("OK", 10000) == 0)
    {  
        if(ESP8266_Wait_Resp("FAIL", 100) == 1)
        {
            OLED_ShowString(0,0,"WiFi FAIL");
            delay_ms(2000);
            Clear_Buffer();
        }
        printf("AT+CWJAP=\"PF2025\",\"12345678\"\r\n");        	
        delay_ms(1000);			
    }
    Clear_Buffer();
    
    // 显示连接成功
    OLED_ShowString(0,0,"WiFi Connected");
    delay_ms(1000);
    
    // 连接TCP服务器（替换为实际电脑IP和端口）
    printf("AT+CIPSTART=\"TCP\",\"192.168.236.169\",8888\r\n");
    OLED_ShowString(0,0,"Connecting Server...");
    while(ESP8266_Wait_Resp("OK", 5000) == 0)    
    {  
        if(ESP8266_Wait_Resp("ERROR", 100) == 1)
        {
            OLED_ShowString(0,0,"Server ERROR");
            delay_ms(2000);
            Clear_Buffer();
        }
        printf("AT+CIPSTART=\"TCP\",\"192.168.236.169\",8888\r\n");        	
        delay_ms(1000);			
    }
    Clear_Buffer();
    
    OLED_ShowString(0,0,"TCP Connected!");
    delay_ms(1000);
}

