#ifndef _WIFI_H
#define _WIFI_H

#include "main.h"
#include "core_json.h"

#define WIFI_SSID        "RavSense"
#define WIFI_PASSWD      "rwwdz123456"

#define MQTT_CLIENT_ID   "ESP_OTA\\,signmethod=hmacsha1\\,timestamp=1687594902069|"   
#define MQTT_USER_NAME   "ESP_OTA"
#define MQTT_PASSWD      "40BB1CB64363E1CCF491CFA8D73A08207B8F9664"
#define BROKER_ASDDRESS  "a1TGt6tIcAE.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SUB_TOPIC        "/sys/a1TGt6tIcAE/mqtt_stm32/thing/service/property/set"
#define PUB_TOPIC        "/sys/a1TGt6tIcAE/mqtt_stm32/thing/event/property/post"
#define JSON_FORMAT      "{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}"
/*
{
  "ProductKey": "k1644sbngGw",
  "DeviceName": "ESP_OTA",
  "DeviceSecret": "7de1b392a12d3ac5fbb52876c4311671"
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


