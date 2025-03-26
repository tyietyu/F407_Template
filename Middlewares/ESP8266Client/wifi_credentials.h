#ifndef _WIFI_CREDENTIALS_H
#define _WIFI_CREDENTIALS_H

#define WIFI_AP_SSID   "RavSense"
#define WIFI_AP_PASS   "rwwdz123456"
#define SERVER_ADDR    ("49.84.153.84")

#define MQTT_CLIENT_ID "ESP_OTA\\,signmethod=hmacsha1\\,timestamp=1687594902069|"
#define MQTT_USER_NAME "ESP_OTA"
#define MQTT_PASSWD    "40BB1CB64363E1CCF491CFA8D73A08207B8F9664"
// #define BROKER_ASDDRESS  "a1TGt6tIcAE.iot-as-mqtt.cn-shanghai.aliyuncs.com"
#define SUB_TOPIC   "/sys/a1TGt6tIcAE/mqtt_stm32/thing/service/property/set"
#define PUB_TOPIC   "/sys/a1TGt6tIcAE/mqtt_stm32/thing/event/property/post"
#define JSON_FORMAT "{\\\"params\\\":{\\\"temp\\\":%d\\,\\\"humi\\\":%d\\}\\,\\\"version\\\":\\\"1.0.0\\\"}"
/*
{
  "ProductKey": "k1644sbngGw",
  "DeviceName": "ESP_OTA",
  "DeviceSecret": "7de1b392a12d3ac5fbb52876c4311671"
}
*/

#endif
