/**
 * @file      esp8266client.c
 */

// Includes.
#include "ESP8266Client.h"
#include <string.h>
#include <assert.h>
#include "fifo.h"
#include "main.h"

#define ESP8266_EOK      0 /* 没有错误 */
#define ESP8266_ERROR    1 /* 通用错误 */
#define ESP8266_ETIMEOUT 2 /* 超时错误 */
#define ESP8266_EINVAL   3 /* 参数错误 */

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;
extern uint8_t rxBuffer[RX_BUFFER_SIZE];
extern uint8_t txBuffer[TX_BUFFER_SIZE];
extern int recv_end_flag;
extern int rx_len;
// extern struct fifo rxFifo;

static struct
{
    uint8_t buf[RX_BUFFER_SIZE]; /* 帧接收缓冲 */
    struct
    {
        uint16_t len : 15;  /* 帧接收长度，sta[14:0] */
        uint16_t finsh : 1; /* 帧接收完成标志，sta[15] */
    } sta;                  /* 帧状态信息 */
} g_uart_rx_frame = {0};

void ESP8266_printf(char *fmt, ...)
{
    va_list args;
    uint16_t len;
    va_start(args, fmt);
    vsprintf((char *)g_uart_rx_frame.buf, fmt, args);
    va_end(args);
    len = strlen((const char *)g_uart_rx_frame.buf);
    HAL_UART_Transmit(&huart2, g_uart_rx_frame.buf, len, HAL_MAX_DELAY);
}
void ESP82_RecvReset(void)
{
    g_uart_rx_frame.sta.len   = 0;
    g_uart_rx_frame.sta.finsh = 0;
}

uint8_t *ESP82_GetFrame(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = '\0';
        return g_uart_rx_frame.buf;
    }
    else
    {
        return NULL;
    }
}

uint16_t ESP82_GetFrameLen(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        return g_uart_rx_frame.sta.len;
    }
    else
    {
        return ESP8266_EOK;
    }
}

void ESP8266_IRQHandler(void)
{
    uint8_t tmp;

    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_ORE) != RESET) /* UART接收过载错误中断 */
    {
        __HAL_UART_CLEAR_OREFLAG(&huart2); /* 清除接收过载错误中断标志 */
        (void)huart2.Instance->SR;         /* 先读SR寄存器，再读DR寄存器 */
        (void)huart2.Instance->DR;
    }

    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) /* UART接收中断 */
    {
        HAL_UART_Receive(&huart2, &tmp, 1, HAL_MAX_DELAY); /* UART接收数据 */

        if (g_uart_rx_frame.sta.len < (RX_BUFFER_SIZE - 1)) /* 判断UART接收缓冲是否溢出
                                                             * 留出一位给结束符'\0'
                                                             */
        {
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp; /* 将接收到的数据写入缓冲 */
            g_uart_rx_frame.sta.len++;                          /* 更新接收到的数据长度 */
        }
        else                                                    /* UART接收缓冲溢出 */
        {
            g_uart_rx_frame.sta.len                      = 0;   /* 覆盖之前收到的数据 */
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp; /* 将接收到的数据写入缓冲 */
            g_uart_rx_frame.sta.len++;                          /* 更新接收到的数据长度 */
        }
    }

    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET) /* UART总线空闲中断 */
    {
        g_uart_rx_frame.sta.finsh = 1;      /* 标记帧接收完成 */
        __HAL_UART_CLEAR_IDLEFLAG(&huart2); /* 清除UART总线空闲中断 */
    }
}

/*
 * @brief INTERNAL Sends command to the module.
 * @param command The string command.
 * @param commandLength Length of the command.
 * @param clearBuffers UART and response buffers are cleared if this is true.
 */
uint8_t ESP82_sendCmd(char *cmd, char *ack, uint32_t timeout)
{
    uint8_t *ret = 0;
    ESP82_RecvReset();
    ESP8266_printf("%s\r\n", cmd);

    if ((ack == NULL) || (timeout == 0))
    {
        return ESP8266_EOK;
    }
    else
    {
        while (timeout > 0)
        {
            ret = ESP82_GetFrame();

            if (ret != NULL)
            {
                if (strstr(ret, ack) != NULL)
                {
                    return 1;
                }
                else
                {
                    ESP82_RecvReset();
                }
            }

            timeout--;
            HAL_Delay(1);
        }

        return ESP8266_ETIMEOUT;
    }
}
/*
 * @brief Initialize the UART and the module.
 * @param baud UART baud-rate.
 * @param parity UART parity setting (0:no-parity, 1:odd, 2:even).
 * @param getTime_ms_functionHandler Function handler for getting time in ms.
 */
void ESP82_Init(void)
{
    HAL_GPIO_WritePin(WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_SET);
}

uint8_t ESP82AtTest(void)
{
    uint8_t ret;
    ret = ESP82_sendCmd("AT", "OK", 500);

    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }

    return ESP8266_ERROR;
}

uint8_t ESP82_set_mode(uint8_t mode)
{
    uint8_t ret;

    switch (mode)
    {
        case 1:
        {
            ret = ESP82_sendCmd("AT+CWMODE=1", "OK", 500); /* Station模式 */
            break;
        }

        case 2:
        {
            ret = ESP82_sendCmd("AT+CWMODE=2", "OK", 500); /* AP模式 */
            break;
        }

        case 3:
        {
            ret = ESP82_sendCmd("AT+CWMODE=3", "OK", 500); /* AP+Station模式 */
            break;
        }

        default:
        {
            return ESP8266_EINVAL;
        }
    }

    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}

uint8_t ESP82_swRst(void)
{
    uint8_t ret;

    ret = ESP82_sendCmd("AT+RST", "OK", 5000);

    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}

uint8_t ESP82_ateConfig(uint8_t cfg)
{
    uint8_t ret;

    switch (cfg)
    {
        case 0:
        {
            ret = ESP82_sendCmd("ATE0", "OK", 500); /* 关闭回显 */
            break;
        }

        case 1:
        {
            ret = ESP82_sendCmd("ATE1", "OK", 500); /* 打开回显 */
            break;
        }

        default:
        {
            return ESP8266_EINVAL;
        }
    }

    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}

uint8_t ESP82_JoinAP(char *ssid, char *pwd)
{
    uint8_t ret;
    char cmd[64];

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    ret = ESP82_sendCmd(cmd, "OK", 5000);

    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}

uint8_t ESP82_get_ip(void)
{
    uint8_t ret;

    ret = ESP82_sendCmd("AT+CIFSR", "OK", 500);

    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}

uint8_t ESP82_connect_tcp_server(char *ip, uint16_t port)
{
    uint8_t ret;
    char cmd[64];
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d", ip, port);
    ret = ESP82_sendCmd(cmd, "CONNECT", 5000);

    if (ret == ESP8266_EOK)
    {
		printf("connect server success\r\n");
        return ESP8266_EOK;
        
    }
    else
    {
        printf("connect server fail\r\n");
        return ESP8266_ERROR;

    }
}

uint8_t ESP82_enterTransMode(void)
{
    uint8_t ret;

    ret = ESP82_sendCmd("AT+CIPMODE=1", "OK", 500);
    ret += ESP82_sendCmd("AT+CIPSEND", ">", 500);

    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}

void ESP82_exitTransMode(void)
{
    ESP8266_printf("+++");
}

// void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart)
//{
//     if (huart == &huart2 && recv_end_flag == 1) {
//        // fifo_in(&rxFifo, rxBuffer, rx_len);
//         memset(rxBuffer + rx_len, 0, RX_BUFFER_SIZE - rx_len);
//         HAL_UART_Receive_DMA(&huart2, rxBuffer, RX_BUFFER_SIZE);
//     }
// }
