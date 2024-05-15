#ifndef _WIFI_H
#define _WIFI_H

#include "main.h"
#include "core_json.h"

#define WIFI_SSID        "RavSense"
#define WIFI_PASSWD      "rwwdz123456"

#define MQTT_CLIENT_ID   "stm32_espota|securemode=2,signmethod=hmacsha1,timestamp=1715763984940|"   
#define MQTT_USER_NAME   "ESP_OTA&k1644sbngGw"
#define MQTT_PASSWD      "BA8D7F524D426F46ABB1C956B8D3060CF2B47A63"
#define BROKER_ASDDRESS  "k1644sbngGw.iot-as-mgtt.cn-shanghai.aliyuncs.com"
#define SUB_TOPIC        "/sys/k1644sbngGw/ESP_OTA/thing/service/property/set"
#define PUB_TOPIC        "/sys/k1644sbngGw/ESP_OTA/thing/event/property/post"
#define JSON_FORMAT      "{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}"
/* 阿里云物联网平台设备信息
{
  "ProductKey": "k1644sbngGw",
  "DeviceName": "ESP_OTA",
  "DeviceSecret": "7de1b392a12d3ac5fbb52876c4311671"
  mqttclintID:  stm32_espota|securemode=2,signmethod=hmacsha1,timestamp=1715763984940|
  mqttusername:  ESP_OTA&k1644sbngGw
  mqttpassword: BA8D7F524D426F46ABB1C956B8D3060CF2B47A63
  BrokerAdder: k1644sbngGw.iot-as-mgtt.cn-shanghai.aliyuncs.com
  port:1883
  消息发布主题格式：/sys/ProductKey/DeviceName/thing/.event/property/post
  消息订阅主题格式：/sys/ProductKey/DeviceName/thing/service/.property/set
}
*/

#define ESP8266_UART_RX_BUF_SIZE 512
#define ESP8266_UART_TX_BUF_SIZE 512

void espUart_rx_data(void);
uint8_t esp8266_receive_msg(void);
uint8_t esp8266_send_msg(void);
void esp8266_init(void);
uint8_t parse_json_msg(uint8_t *json_msg,uint8_t json_len);
uint8_t esp8266_reset(void);
uint8_t esp8266_connect_server(void);
uint8_t esp8266_config_network(void);
uint8_t esp8266_send_cmd(unsigned char *cmd,unsigned char len,char *rec_data);
void espUart_receiver_clear(uint16_t len);
void espUart_rx_data(void);
void espUart_printf(char *fmt, ...);




#endif


