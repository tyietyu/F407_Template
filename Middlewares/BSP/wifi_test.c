#include "wifi_test.h"
#include "mqtt.h"

#define ProductKey   "a1GLM2BFQK0"
#define DeviceName   "iot_mq2"
#define DeviceSecret "caa5b2684101072f3dfe0f04688e0a7f"

// 订阅与发布的主题
#define SET_TOPIC  "/sys/a1GLM2BFQK0/iot_mq2/thing/service/property/set"
#define POST_TOPIC "/sys/a1GLM2BFQK0/iot_mq2/thing/event/property/post"

char mqtt_message[200]; // 上报数据缓存区

void wifi_test(void)
{
    // 阿里云物联网服务器的设备证书
    uint32_t time_cnt = 0;
    uint32_t i;

    if (esp8266_init())
    {
        printf("ESP8266硬件检测错误.\n");
    }

    // 1. 初始化阿里云登录参数
    Aliyun_LoginInit(ProductKey, DeviceName, DeviceSecret);
    // 2. MQTT协议初始化
    MQTT_Init();

    // 3. 连接阿里云服务器
    while (MQTT_Connect(MQTT_ClientID, MQTT_UserName, MQTT_PassWord))
    {
        printf("阿里云服务器连接失败,正在重试...\n");
        HAL_Delay(500);
    }

    printf("阿里云服务器连接成功.\n");

    // 3. 订阅主题
    if (MQTT_SubscribeTopic(SET_TOPIC, 0, 1))
    {
        printf("主题订阅失败.\n");
    }
    else
    {
        printf("主题订阅成功.\n");
    }

    while (1)
    {
        if (keyVal == 0)
        {
            time_cnt = 0;
            sprintf(mqtt_message, "{\"method\":\"thing.event.property.post\",\"id\":\"0000000001\",\"params\":{\"mq2\":55},\"version\":\"1.0.0\"}");
            MQTT_PublishData(POST_TOPIC, mqtt_message, 0);
            printf("发送状态1\r\n");
        }
        else if (keyVal == 1)
        {
            time_cnt = 0;
            sprintf(mqtt_message, "{\"method\":\"thing.event.property.post\",\"id\":\"0000000001\",\"params\":{\"mq2\":66},\"version\":\"1.0.0\"}");
            MQTT_PublishData(POST_TOPIC, mqtt_message, 0);
            printf("发送状态0\r\n");
        }

        if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_RXNE) != RESET)
        {
            g_uart_rx_frame.buf[ESP8266_UART_RX_BUF_SIZE-1] = '\0';

            for (i = 0; i < ESP8266_UART_RX_BUF_SIZE; i++)
            {
                printf("%c", g_uart_rx_frame.buf[i]);
            }

            // 清除接收缓冲区
            memset((void *)g_uart_rx_frame.buf, 0, ESP8266_UART_RX_BUF_SIZE);
            __HAL_UART_CLEAR_FEFLAG(&g_uart_handle);
        }

        // 定时发送心跳包,保持连接
        HAL_Delay(10);
        time_cnt++;

        if (time_cnt == 500)
        {
            MQTT_SentHeart(); // 发送心跳包
            time_cnt = 0;
        }
    }
}
