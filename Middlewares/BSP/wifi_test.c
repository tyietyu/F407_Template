#include "wifi_test.h"
#include "mqtt.h"


void wifiAT_test(void)
{
    esp8266_init();
	uint8_t re = esp8266_config_network();
	if(re==0)
	{
		printf("OK\n");
		
	}
	else
	{
		printf("error\n");
	}

	
}

