#ifndef __MQTT_H_
#define __MQTT_H_
 
#include "main.h"
 
#define BYTE0(dwTemp)       (*( char *)(&dwTemp))
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))


extern char MQTT_ClientID[100]; //MQTT_客户端ID
extern char MQTT_UserName[100]; //MQTT_用户名
extern char MQTT_PassWord[100]; //MQTT_密码
 
//阿里云用户名初始化
void Aliyun_LoginInit(char *ProductKey,char *DeviceName,char *DeviceSecret);
//MQTT协议相关函数声明
uint8_t MQTT_PublishData(char *topic, char *message, uint8_t qos);
uint8_t MQTT_SubscribeTopic(char *topic,uint8_t qos,uint8_t whether);
void MQTT_Init(void);
uint8_t MQTT_Connect(char *ClientID,char *Username,char *Password);
void MQTT_SentHeart(void);
void MQTT_Disconnect(void);
void MQTT_SendBuf(uint8_t *buf,uint16_t len);
#endif

