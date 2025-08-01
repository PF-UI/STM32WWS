#include "usart.h"
#include "string.h"
#include "wdg.h"
#include "SysTick.h"
#include "oled.h"
#include "beep.h" 

extern char RxBuffer[100], RxCounter;
char *strx;

// ��ս��ջ�����
void Clear_Buffer(void)
{
    u8 i;
    for(i = 0; i < 100; i++)
        RxBuffer[i] = 0;
    RxCounter = 0;
    IWDG_Feed(); // ι��
}

// �ȴ�ESP8266����ָ����Ӧ����ʱʱ�䣺ms��
u8 ESP8266_Wait_Resp(const char *resp, u16 timeout)
{
    u16 t = 0;
    while(t < timeout)
    {
        if(strstr((const char*)RxBuffer, resp) != NULL)
        {
            return 1; // �ҵ���Ӧ
        }
        delay_ms(10);
        t += 10;
    }
    return 0; // ��ʱδ�ҵ�
}

// ESP8266��ʼ��������WiFi��TCP��������
void ESP8266_Init(void)
{   
    // ����ATָ��
    printf("AT\r\n"); 
    delay_ms(300);
    while(ESP8266_Wait_Resp("OK", 1000) == 0)
    {  
        printf("AT\r\n");        	
        delay_ms(500);			
        OLED_ShowString(0,0,"Checking AT...");
    }        
    Clear_Buffer();
            
    // ����STAģʽ
    printf("AT+CWMODE=1\r\n");  
    delay_ms(300);
    while(ESP8266_Wait_Resp("OK", 1000) == 0)    
    {  
        printf("AT+CWMODE=1\r\n");        	
        delay_ms(100);			
        OLED_ShowString(0,0,"Setting CWMODE...");
    }
    Clear_Buffer();
    
    // ����WiFi
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
    
    // ��ʾ���ӳɹ�
    OLED_ShowString(0,0,"WiFi Connected");
    delay_ms(1000);
    
    // ����TCP���������滻Ϊʵ�ʵ���IP�Ͷ˿ڣ�
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

