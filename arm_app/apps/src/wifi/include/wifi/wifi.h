#ifndef __R329_WIFI_H_
#define __R329_WIFI_H_
#include "wifi/wifimg.h"
#include "wifi/wmg_common.h"
#include "wifi/wmg_sta.h"
enum wifi_status{
    ROBOT_WIFI_NOT_CONNECT = 0,
    ROBOT_WIFI_SOFT_AP,
    ROBOT_WIFI_CONNECTING,
    ROBOT_WIFI_CONNECTED,
    ROBOT_WIFI_ERROR,
    ROBOT_WIFI_ALL
};

typedef void (*wifi_msg_handle)(wifi_msg_data_t *);

void wifi_scan();
int wifi_connected(int);
int wifi_init();
int wifi_deinit();
int wifi_close();
int wifi_work_on_sta();
int wifi_work_on_ap(char *ssid);
int wifi_work_on_monitor(wifi_msg_handle msg_handle);
int wifi_set_channel(const int ch);
int wifi_connect_network(char *ssid, char *pwd);
int wifi_get_mode();
void time_handle(int num);

#endif