#include "wifi.h"

static UART_HandleTypeDef g_uart_handle;
static uint8_t g_uart_tx_buf[ESP8266_UART_TX_BUF_SIZE]; /* ESP8266 UART发送缓冲 */
static struct
{
    uint8_t buf[ESP8266_UART_RX_BUF_SIZE];              /* 帧接收缓冲 */
    struct
    {
        uint16_t len    : 15;                               /* 帧接收长度，sta[14:0] */
        uint16_t finsh  : 1;                                /* 帧接收完成标志，sta[15] */
    } sta;                                                  /* 帧状态信息 */
} g_uart_rx_frame = {0};                                    /* ESP8266 UART接收帧缓冲信息结构体 */

void esp8266Rst(void)
{
    HAL_GPIO_WritePin(WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(200);
    HAL_GPIO_WritePin(WIFI_RST_GPIO_Port, WIFI_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(500);
}

uint8_t esp8266_sw_reset(void)
{
    uint8_t ret;
    ret = esp8266SendAtCmd("AT+RST", "OK", 500);
    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    } else {
        return ESP8266_ERROR;
    }
}

uint8_t esp8266SendAtCmd(char *cmd, char *ack, uint32_t waittime)
{
    uint8_t *res = NULL;
    espUart_rx_restart();
    espUart_printf("%s\r\n", cmd);
    if (ack == NULL || (timeout == 0)) {
        return ESP8266_EOK;
    } else {
        while (waittime > 0) {
            res = espUart_rx_get_frame();
            if (res != NULL) {
                if (strstr((const char *)res, ack) != NULL) {
                    return ESP8266_EOK;
                } else {
                    espUart_rx_restart();
                }
            }
            waittime--;
            HAL_Delay(1);
        }
        return ESP8266_ETIMEOUT;
    }
}

uint8_t esp8266_init(void)
{
    esp8266Rst();                         /* ATK-MW8266D硬件复位 */
    espUart_init();                       /* ATK-MW8266D UART初始化 */
    if (ESP8266_at_test() != ESP8266_EOK) /* ATK-MW8266D AT指令测试 */
    {
        return ESP8266_ERROR;
    }

    return ESP8266_EOK;
}

uint8_t esp8266_at_test(void)
{
    uint8_t ret;
    uint8_t i;

    for (i = 0; i < 10; i++) {
        ret = esp8266SendAtCmd("AT", "OK", 500);
        if (ret == ESP8266_EOK) {
            return ESP8266_EOK;
        }
    }
    return ESP8266_ERROR;
}

uint8_t esp8266_restore(void)
{
    uint8_t ret;
    ret = esp8266SendAtCmd("AT+RESTORE", "ready", 3000);
    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    } else {
        return ESP8266_ERROR;
    }
}

uint8_t esp8266_set_mode(uint8_t mode)
{
    uint8_t ret;
    switch (mode) {
        case 1: {
            ret = esp8266SendAtCmd("AT+CWMODE=1", "OK", 500); /* Station模式 */
            break;
        }
        case 2: {
            ret = esp8266SendAtCmd("AT+CWMODE=2", "OK", 500); /* AP模式 */
            break;
        }
        case 3: {
            ret = esp8266SendAtCmd("AT+CWMODE=3", "OK", 500); /* AP+Station模式 */
            break;
        }
        default: {
            return ESP8266_EINVAL;
        }
    }

    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    } else {
        return ESP8266_ERROR;
    }
}

uint8_t esp8266_ate_config(uint8_t cfg)
{
    uint8_t ret;

    switch (cfg) {
        case 0: {
            ret = esp8266SendAtCmd("ATE0", "OK", 500); /* 关闭回显 */
            break;
        }
        case 1: {
            ret = esp8266SendAtCmd("ATE1", "OK", 500); /* 打开回显 */
            break;
        }
        default: {
            return ESP8266_EINVAL;
        }
    }

    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    } else {
        return ESP8266_ERROR;
    }
}

uint8_t esp8266_join_ap(char *ssid, char *pwd)
{
    uint8_t ret;
    char cmd[64];

    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);
    ret = esp8266SendAtCmd(cmd, "WIFI GOT IP", 10000);
    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    } else {
        return ESP8266_ERROR;
    }
}

uint8_t esp8266_get_ip(char *buf)
{
    uint8_t ret;
    char *p_start;
    char *p_end;

    ret = esp8266SendAtCmd("AT+CIFSR", "OK", 500);
    if (ret != ESP8266_EOK) {
        return ESP8266_ERROR;
    }

    p_start = strstr((const char *)espUart_rx_get_frame(), "\"");
    p_end   = strstr(p_start + 1, "\"");
    *p_end  = '\0';
    sprintf(buf, "%s", p_start + 1);
    return ESP8266_EOK;
}

uint8_t esp8266_connect_tcp_server(char *server_ip, char *server_port)
{
    uint8_t ret;
    char cmd[64];

    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", server_ip, server_port);
    ret = esp8266SendAtCmd(cmd, "CONNECT", 5000);
    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    } else {
        return ESP8266_ERROR;
    }
}

uint8_t esp8266_enter_unvarnished(void)
{
    uint8_t ret;

    ret = esp8266SendAtCmd("AT+CIPMODE=1", "OK", 500);
    ret += esp8266SendAtCmd("AT+CIPSEND", ">", 500);
    if (ret == ESP8266_EOK) {
        return ESP8266_EOK;
    } else {
        return ESP8266_ERROR;
    }
}

void esp8266_exit_unvarnished(void)
{
    espUart_printf("+++");
}

uint8_t esp8266_connect_atkcld(char *id, char *pwd)
{
    uint8_t ret;
    char cmd[64];
    
    sprintf(cmd, "AT+ATKCLDSTA=\"%s\",\"%s\"", id, pwd);
    ret =esp8266SendAtCmd(cmd, "CLOUD CONNECTED", 10000);
    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}

uint8_t esp8266_disconnect_atkcld(void)
{
    uint8_t ret;
    
    ret = esp8266SendAtCmd("AT+ATKCLDCLS", "CLOUD DISCONNECT", 500);
    if (ret == ESP8266_EOK)
    {
        return ESP8266_EOK;
    }
    else
    {
        return ESP8266_ERROR;
    }
}


/*esp8266 Uart*/
void espUart_init(void)
{
    g_uart_handle.Instance          = USART2;               /* ATK-MW8266D UART */
    g_uart_handle.Init.BaudRate     = 115200;               /* 波特率 */
    g_uart_handle.Init.WordLength   = UART_WORDLENGTH_8B;   /* 数据位 */
    g_uart_handle.Init.StopBits     = UART_STOPBITS_1;      /* 停止位 */
    g_uart_handle.Init.Parity       = UART_PARITY_NONE;     /* 校验位 */
    g_uart_handle.Init.Mode         = UART_MODE_TX_RX;      /* 收发模式 */
    g_uart_handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;  /* 无硬件流控 */
    g_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16; /* 过采样 */
    HAL_UART_Init(&g_uart_handle);                          /* 使能ATK-MW8266D UART                                                                  */
}

void espUart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;

    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);

    len = strlen((const char *)g_uart_tx_buf);
    HAL_UART_Transmit(&huart2, g_uart_tx_buf, len, HAL_MAX_DELAY);
}

void espUart_rx_restart(void)
{
    g_uart_rx_frame.sta.len   = 0;
    g_uart_rx_frame.sta.finsh = 0;
}

uint8_t *espUart_rx_get_frame(void)
{
    if (g_uart_rx_frame.sta.finsh == 1) {
        g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = '\0';
        return g_uart_rx_frame.buf;
    } else {
        return NULL;
    }
}

uint16_t espUart_rx_get_frame_len(void)
{
    if (g_uart_rx_frame.sta.finsh == 1) {
        return g_uart_rx_frame.sta.len;
    } else {
        return 0;
    }
}

void ESP8266_UART_IRQHandler(void)
{
    uint8_t tmp;

    if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_ORE) != RESET) /* UART接收过载错误中断 */
    {
        __HAL_UART_CLEAR_OREFLAG(&g_uart_handle); /* 清除接收过载错误中断标志 */
        (void)g_uart_handle.Instance->SR;         /* 先读SR寄存器，再读DR寄存器 */
        (void)g_uart_handle.Instance->DR;
    }

    if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_RXNE) != RESET) /* UART接收中断 */
    {
        HAL_UART_Receive(&g_uart_handle, &tmp, 1, HAL_MAX_DELAY); /* UART接收数据 */

        if (g_uart_rx_frame.sta.len < (ESP8266_UART_RX_BUF_SIZE - 1)) /* 判断UART接收缓冲是否溢出
                                                                       * 留出一位给结束符'\0'
                                                                       */
        {
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp; /* 将接收到的数据写入缓冲 */
            g_uart_rx_frame.sta.len++;                          /* 更新接收到的数据长度 */
        } else                                                  /* UART接收缓冲溢出 */
        {
            g_uart_rx_frame.sta.len                      = 0;   /* 覆盖之前收到的数据 */
            g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp; /* 将接收到的数据写入缓冲 */
            g_uart_rx_frame.sta.len++;                          /* 更新接收到的数据长度 */
        }
    }

    if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_IDLE) != RESET) /* UART总线空闲中断 */
    {
        g_uart_rx_frame.sta.finsh = 1;             /* 标记帧接收完成 */
        __HAL_UART_CLEAR_IDLEFLAG(&g_uart_handle); /* 清除UART总线空闲中断 */
    }
}
