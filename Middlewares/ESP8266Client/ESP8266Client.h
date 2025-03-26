/**
 * @file      esp8266client.c
 * @author    Atakan S.
 * @date      01/01/2019
 * @version   1.0
 * @brief     ESP8266 Client Mode Driver.
 *
 * @copyright Copyright (c) 2018 Atakan SARIOGLU ~ www.atakansarioglu.com
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files (the "Software"),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *  DEALINGS IN THE SOFTWARE.
 */

#ifndef _ESP8266CLIENT_H_
#define _ESP8266CLIENT_H_

// Includes.
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "main.h"

extern  UART_HandleTypeDef huart2;

#define ESP8266_EOK         0   /* 没有错误 */
#define ESP8266_ERROR       1   /* 通用错误 */
#define ESP8266_ETIMEOUT    2   /* 超时错误 */
#define ESP8266_EINVAL      3   /* 参数错误 */


void ESP8266_IRQHandler(void);
// Prototypes.
void ESP82_Init(void);
void ESP82_RecvReset(void);
uint8_t ESP82AtTest(void);
uint8_t ESP82_swRst(void);
uint8_t ESP82_get_ip(void);
uint8_t *ESP82_GetFrame(void);
uint16_t ESP82_GetFrameLen(void);
void ESP82_exitTransMode(void);
uint8_t ESP82_enterTransMode(void);
uint8_t ESP82_set_mode(uint8_t mode);
uint8_t ESP82_ateConfig(uint8_t cfg);
uint8_t ESP82_JoinAP(char *ssid,char *pwd);
uint8_t ESP82_connect_tcp_server(char *ip, uint16_t port);
uint8_t ESP82_sendCmd(char *cmd, char *ack, uint32_t timeout);

void HAL_UART_IdleCpltCallback(UART_HandleTypeDef *huart);
#endif
