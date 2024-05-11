#ifndef WIFI_TEST_H
#define WIFI_TEST_H

#include "wifi.h"
#include "main.h"
#include "mqtt.h"

extern   volatile uint8_t keyVal;
extern  volatile  UART_RX_FRAME g_uart_rx_frame;
extern   UART_HandleTypeDef g_uart_handle;
void wifi_test(void);

#endif // WIFI_TEST_H



