#ifndef MY_WIFI_H
#define MY_WIFI_H

#include "esp_err.h"

#define WIFI_SSID      "WsyiPhone"
#define WIFI_PASS      "wangWANG1234"
#define MAX_RETRY      5

void wifi_init_sta(void);

#endif // MY_WIFI_H
