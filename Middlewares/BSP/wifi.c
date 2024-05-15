#include "wifi.h"

uint8_t receiveBuff[ESP8266_UART_RX_BUF_SIZE]; /* 帧接收缓冲 */
uint8_t sendBuff[ESP8266_UART_TX_BUF_SIZE];

uint8_t receiveStart=0;
uint16_t receiveCount=0;
uint16_t receiveEnd=0;

void esp8266_init(void)
{
    printf("1.SETTING STATION MODE\r\n");

    while (esp8266_send_cmd((uint8_t *)"AT+CWMODE=1\r\n", strlen("AT+CWMODE=1\r\n"), "OK")!=0)
    {
        HAL_Delay(1000);
    }

    printf("2.CLOSE ESP8266 ECHO\r\n");

    while (esp8266_send_cmd((uint8_t *)"ATE0\r\n", strlen("ATE0\r\n"), "OK")!=0)
    {
        HAL_Delay(1000);
    }

    printf("3.NO AUTO CONNECT WIFI\r\n");

    while (esp8266_send_cmd((uint8_t *)"AT+CWAUTOCONN=0\r\n", strlen("AT+CWAUTOCONN=0\r\n"), "OK")!=0)
    {
        HAL_Delay(1000);
    }

    printf("4.RESET ESP8266\r\n");

    while (esp8266_reset() != 0)
    {
        HAL_Delay(5000);
    }

    printf("5.CONFIG WIFI NETWORK\r\n");

    while (esp8266_config_network() != 0)
    {
        HAL_Delay(8000);
    }
	printf("6.MQTT USER CONFIG\r\n");
	while(esp8266_send_cmd((uint8_t *)"AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n",
                          strlen("AT+MQTTUSERCFG=0,1,\""MQTT_CLIENT_ID"\",\""MQTT_USER_NAME"\",\""MQTT_PASSWD"\",0,0,\"\"\r\n"),"OK")!=0)
	{
		HAL_Delay(2000);
	}
	printf("7.CONNECT MQTT BROKER\r\n");

	while(esp8266_connect_server() != 0)
	{
		HAL_Delay(8000);
	}
	printf("8.SUBSCRIBE TOPIC\r\n");

	while(esp8266_send_cmd((uint8_t *)"AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n",strlen("AT+MQTTSUB=0,\""SUB_TOPIC"\",0\r\n"),"OK")!=0)
	{
		HAL_Delay(2000);
	}
	printf("9.ESP8266 INIT OK!!!\r\n");
 
}

void espUart_rx_data(void)
{
    unsigned char receiveData=0;

    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET)
    {
        HAL_UART_Receive(&huart2, &receiveData, 1, 1000);
        receiveBuff[receiveCount++] = receiveData;
        receiveStart= 1;
        receiveEnd = 0;
    }
}

void espUart_receiver_clear(uint16_t len)
{
    memset(receiveBuff, 0x00, len);
    receiveCount = 0;
    receiveStart = 0;
    receiveEnd = 0;
}

uint8_t esp8266_send_cmd(unsigned char *cmd, unsigned char len, char *rec_data)
{
    unsigned char retval =0;
    unsigned int count = 0;

    HAL_UART_Transmit(&huart2, cmd, len, 1000);

    while ((receiveStart == 0)&&(count<3000))
    {
        count++;
        HAL_Delay(1);
    }

    if (count >= 3000)
    {
        retval = 1;
    }
    else
    {
        do
        {
            receiveEnd++;
            HAL_Delay(1);
        } while (receiveEnd < 1000);

        retval = 2;

        if (strstr((const char*)receiveBuff, rec_data))
        {
            retval = 0;
        }
    }

    espUart_receiver_clear(receiveCount);
    return retval;
}

uint8_t esp8266_config_network(void)
{
    uint8_t retval =0;
    uint16_t count = 0;

    HAL_UART_Transmit(&huart2, (unsigned char *)"AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n", strlen("AT+CWJAP=\""WIFI_SSID"\",\""WIFI_PASSWD"\"\r\n"), 1000);

    while ((receiveStart == 0)&&(count<3000))
    {
        count++;
        HAL_Delay(1);
    }

    if (count >= 3000)
    {
        retval = 1;
    }
    else
    {
        HAL_Delay(8000);

        if (strstr((const char*)receiveBuff, "OK"))
        {
            retval = 0;
        }
        else
        {
            retval = 1;
        }
    }

    espUart_receiver_clear(receiveCount);
    return retval;
}

uint8_t esp8266_connect_server(void)
{
    uint8_t retval=0;
    uint16_t count = 0;

    HAL_UART_Transmit(&huart2, (unsigned char *)"AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n", strlen("AT+MQTTCONN=0,\""BROKER_ASDDRESS"\",1883,0\r\n"), 1000);

    while ((receiveStart == 0)&&(count<4000))
    {
        count++;
        HAL_Delay(1);
    }

    if (count >= 4000)
    {
        retval = 1;
    }
    else
    {
        HAL_Delay(5000);

        if (strstr((const char*)receiveBuff, "OK"))
        {
            retval = 0;
        }
        else
        {
            retval = 1;
        }
    }

    espUart_receiver_clear(receiveCount);
    return retval;
}

uint8_t esp8266_reset(void)
{
    uint8_t retval =0;
    uint16_t count = 0;

    HAL_UART_Transmit(&huart2, (unsigned char *)"AT+RST\r\n", 8, 1000);

    while ((receiveStart == 0)&&(count<2000))
    {
        count++;
        HAL_Delay(1);
    }

    if (count >= 2000)
    {
        retval = 1;
    }
    else
    {
        HAL_Delay(5000);

        if (strstr((const char*)receiveBuff, "OK"))
        {
            retval = 0;
        }
        else
        {
            retval = 1;
        }
    }

    espUart_receiver_clear(receiveCount);
    return retval;
}

uint8_t esp8266_send_msg(void)
{
    uint8_t retval =0;
    uint16_t count = 0;
    static uint8_t error_count=0;
    unsigned char msg_buf[256];

    // sprintf((char *)msg_buf,"AT+MQTTPUB=0,\""PUB_TOPIC"\",\""JSON_FORMAT"\",0,0\r\n",temp_value,humi_value);
    HAL_UART_Transmit(&huart2, (unsigned char *)msg_buf, strlen((const char *)msg_buf), 1000);
    HAL_UART_Transmit(&huart1, (unsigned char *)msg_buf, strlen((const char *)msg_buf), 1000);

    while ((receiveStart == 0)&&(count<500))
    {
        count++;
        HAL_Delay(1);
    }

    if (count >= 500)
    {
        retval = 1;
    }
    else
    {
        HAL_Delay(50);

        if (strstr((const char*)receiveBuff, "OK"))
        {
            retval = 0;
            error_count=0;
        }
        else
        {
            error_count++;

            if (error_count==5)
            {
                error_count=0;
                printf("RECONNECT MQTT BROKER!!!\r\n");
                esp8266_init();
            }
        }
    }

    espUart_receiver_clear(receiveCount);
    return retval;
}

uint8_t esp8266_receive_msg(void)
{
    uint8_t retval =0;
    int msg_len=0;
    uint8_t msg_body[128] = {0};

    if (receiveStart == 1)
    {
        do
        {
            receiveEnd++;
            HAL_Delay(1);
        } while (receiveEnd < 5);

        if (strstr((const char*)receiveBuff, "+MQTTSUBRECV:"))
        {
            sscanf((const char *)receiveBuff, "+MQTTSUBRECV:0,\""SUB_TOPIC"\",%d,%s", &msg_len, msg_body);
            printf("len:%d,msg:%s\r\n", msg_len, msg_body);

            if (strlen((const char*)msg_body)== msg_len)
            {
                //retval = parse_json_msg(msg_body,msg_len);
            }
            else
            {
                retval = 1;
            }
        }
        else
        {
            retval = 1;
        }
    }
    else
    {
        retval = 1;
    }

    espUart_receiver_clear(receiveCount);
    return retval;
}

uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len)
{
 uint8_t retval =0;

 JSONStatus_t result;
 char query[] = "params.light";
 size_t queryLength = sizeof( query ) - 1;
 char * value;
 size_t valueLength;
 result = JSON_Validate((const char *)json_msg, json_len);
 if( result == JSONSuccess)
 {
   result = JSON_Search((char *)json_msg, json_len, query, queryLength,&value, &valueLength );
   if( result == JSONSuccess)
   {
     char save = value[valueLength];
     value[valueLength] = '\0';
     printf("Found: %s %d-> %s\n", query, valueLength,value);
     value[valueLength] = save;
     retval = 0;
   }
   else
   {
     retval = 1;
   }
 }
 else
 {
   retval = 1;
 }
 return retval;
}

void espUart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t i, len;

    va_start(ap, fmt);
    vsprintf((char *)sendBuff, fmt, ap);
    va_end(ap);

    len = strlen((const char *)sendBuff);

    for (i=0; i<len; i++)
    {
        while (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) == RESET);

        HAL_UART_Transmit(&huart2, sendBuff, len, HAL_MAX_DELAY);
    }
}


