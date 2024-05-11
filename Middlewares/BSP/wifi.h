#ifndef _WIFI_H
#define _WIFI_H

#include "main.h"

#define ESP8266_UART_IRQHandler           USART2_IRQHandler
#define ESP8266_UART_RX_BUF_SIZE 64
#define ESP8266_UART_TX_BUF_SIZE 64 

typedef struct
{
    uint8_t buf[ESP8266_UART_RX_BUF_SIZE];              /* 帧接收缓冲 */
    struct
    {
        uint16_t len    : 15;                               /* 帧接收长度，sta[14:0] */
        uint16_t finsh  : 1;                                /* 帧接收完成标志，sta[15] */
    } sta;                                                  /* 帧状态信息 */
} UART_RX_FRAME;  

/* ESP8266错误代码 */
#define ESP8266_EOK         0   /* 没有错误 */
#define ESP8266_ERROR       1   /* 通用错误 */
#define ESP8266_ETIMEOUT    2   /* 超时错误 */
#define ESP8266_EINVAL      3   /* 参数错误 */

void esp8266Rst(void);                                            /* ATK-MW8266D硬件复位 */
uint8_t esp8266SendAtCmd(char *cmd, char *ack, uint32_t timeout);    /* ATK-MW8266D发送AT指令 */
uint8_t esp8266_init(void);                                /* ATK-MW8266D初始化 */
uint8_t esp8266_restore(void);                                          /* ATK-MW8266D恢复出厂设置 */
uint8_t esp8266_at_test(void);                                          /* ATK-MW8266D AT指令测试 */
uint8_t esp8266_set_mode(uint8_t mode);                                 /* 设置ATK-MW8266D工作模式 */
uint8_t esp8266_sw_reset(void);                                         /* ATK-MW8266D软件复位 */
uint8_t esp8266_ate_config(uint8_t cfg);                                /* ATK-MW8266D设置回显模式 */
uint8_t esp8266_join_ap(char *ssid, char *pwd);                         /* ATK-MW8266D连接WIFI */
uint8_t esp8266_get_ip(char *buf);                                      /* ATK-MW8266D获取IP地址 */
uint8_t esp8266_connect_tcp_server(char *server_ip, char *server_port); /* ATK-MW8266D连接TCP服务器 */
uint8_t esp8266_enter_unvarnished(void);                                /* ATK-MW8266D进入透传 */
void esp8266_exit_unvarnished(void);                                    /* ATK-MW8266D退出透传 */
uint8_t esp8266_connect_atkcld(char *id, char *pwd);                    /* ATK-MW8266D连接原子云服务器 */
uint8_t esp8266_disconnect_atkcld(void);                                /* ATK-MW8266D断开原子云服务器连接 */


/*esp8266uart*/
void espUart_init(void);
void espUart_rx_restart(void);
void espUart_printf(char *fmt, ...);
uint8_t *espUart_rx_get_frame(void);
uint16_t espUart_rx_get_frame_len(void);

#endif 

