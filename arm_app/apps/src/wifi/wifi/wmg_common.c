/*
 * Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
 */

#include "wifi/wifimg.h"
#include "wifi/wmg_common.h"
#include "wifi/wmg_sta.h"
#include "wifi/wmg_ap.h"
#include "wifi/wmg_monitor.h"
//#include "linkd.h"

#define UNK_BITMAP        0x0
#define STA_BITMAP        0x1
#define AP_BITMAP         0x2
#define MONITOR_BITMAP    0x4

#define AP_STA_BITMAP     0x3
#define AP_MONITOR_BITMAP 0x6
#define BITMAP_MARK       0xF

#define STA_MODE_NUM      0
#define AP_MODE_NUM       1
#define MONITOR_MODE_NUM  2
#define P2P_MODE_NUM      3

static wifimg_object_t wifimg_object;

int __wifi_on(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifimg_object_t* wifimg_object = get_wifimg_object();

	wifi_mode_t *mode = (wifi_mode_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		ret = wifimg_object->init();
		if (ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
	}

	ret = wifimg_object->switch_mode(*mode);
	if ((ret == WMG_STATUS_UNHANDLED) || (ret == WMG_STATUS_SUCCESS)) {
		LOGD("switch wifi mode success\n");
		ret = WMG_STATUS_SUCCESS;
	} else {
		LOGE("failed to switch wifi mode\n");
	}

	*cb_status = ret;
	return ret;
}

int __wifi_off(void **call_argv,void **cb_argv)
{
	wifimg_object_t* wifimg_object = get_wifimg_object();

	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(wifimg_object->is_init()){
		wifimg_object->deinit();
	} else {
		LOGD("wifimg is already deinit\n");
	}

	*cb_status = WMG_STATUS_SUCCESS;
	return WMG_STATUS_SUCCESS;
}

int __wifi_sta_connect(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_sta_cn_para_t *cn_para = (wifi_sta_cn_para_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_STATION) {
		LOGE("wifi station mode is not enabled\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if (cn_para == NULL) {
		LOGE("invalid connect parameters\n");
		*cb_status = WMG_STATUS_INVALID;
		return WMG_STATUS_INVALID;
	}

	ret = wifimg_object->sta_connect(cn_para);
	if (!ret){
		LOGD("wifi station connect success\n");
	}
	else{
		LOGE("wifi station connect fail\n");
	}

	*cb_status = ret;
	return ret;
}

int __wifi_sta_disconnect(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGD("wifi manager is not running\n");
	}

	ret = wifimg_object->sta_disconnect();
	*cb_status = ret;
	return ret;
}

int __wifi_sta_auto_reconnect(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_bool_t *enable = (wmg_bool_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_STATION) {
		LOGE("wifi station mode is not enabled\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	ret = wifimg_object->sta_auto_reconnect(*enable);
	*cb_status = ret;
	return ret;
}

int __wifi_sta_get_info(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_sta_info_t *sta_info = (wifi_sta_info_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_STATION) {
		LOGE("wifi station mode is not enabled\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if (sta_info == NULL) {
		LOGE("invalid station info parameters\n");
		*cb_status = WMG_STATUS_INVALID;
		return WMG_STATUS_INVALID;
	}

	ret = wifimg_object->sta_get_info(sta_info);
	*cb_status = ret;
	return ret;
}

int __wifi_sta_list_networks(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_sta_list_t *sta_list_networks = (wifi_sta_list_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_STATION) {
		LOGE("wifi station mode is not enabled\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if (sta_list_networks == NULL) {
		LOGE("invalid list networks parameters\n");
		*cb_status = WMG_STATUS_INVALID;
		return WMG_STATUS_INVALID;
	}

	ret = wifimg_object->sta_list_networks(sta_list_networks);
	*cb_status = ret;
	return ret;
}

int __wifi_sta_remove_networks(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	char *ssid = (char *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_STATION) {
		LOGE("wifi station mode is not enabled\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	ret = wifimg_object->sta_remove_networks(ssid);
	*cb_status = ret;
	return ret;
}

int __wifi_ap_enable(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_ap_config_t *ap_config = (wifi_ap_config_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_AP) {
		LOGE("wifi ap mode is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if (ap_config == NULL) {
		LOGE("invalid ap config parameters\n");
		*cb_status = WMG_STATUS_INVALID;
		return WMG_STATUS_INVALID;
	}

	ret = wifimg_object->ap_enable(ap_config);
	*cb_status = ret;
	return ret;
}

int __wifi_ap_disable(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGD("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_AP) {
		LOGE("wifi ap mode is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	ret = wifimg_object->ap_disable();
	*cb_status = ret;
	return ret;
}

int __wifi_ap_get_config(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_ap_config_t *ap_config = (wifi_ap_config_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_AP) {
		LOGE("wifi ap mode is not enabled\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	ret = wifimg_object->ap_get_config(ap_config);
	*cb_status = ret;
	return ret;
}

int __wifi_monitor_enable(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	uint8_t *channel = (uint8_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_MONITOR) {
		LOGE("wifi monitor mode is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	ret = wifimg_object->monitor_enable(*channel);
	*cb_status = ret;
	return ret;
}

int __wifi_monitor_set_channel(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	uint8_t *channel = (uint8_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_MONITOR) {
		LOGE("wifi monitor mode is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	ret = wifimg_object->monitor_set_channel(*channel);
	*cb_status = ret;
	return ret;
}

int __wifi_monitor_disable(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGD("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_MONITOR) {
		LOGE("wifi monitor mode is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	ret = wifimg_object->monitor_disable();
	*cb_status = ret;
	return ret;
}

int __wifi_register_msg_cb(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_msg_cb_t *msg_cb = (wifi_msg_cb_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if (msg_cb == NULL) {
		LOGE("invalid parameters, msg_cb is NULL\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_INVALID;
	}

	ret = wifimg_object->register_msg_cb(*msg_cb);
	if (!ret){
		LOGD("register msg cb success\n");
	}
	else{
		LOGE("failed to register msg cb\n");
	}

	*cb_status = ret;
	return ret;
}

int __wifi_set_scan_param(void **call_argv,void **cb_argv)
{
	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_scan_param_t *msg_cb = (wifi_scan_param_t *)call_argv[0];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	*cb_status = WMG_STATUS_FAIL;
	return WMG_STATUS_FAIL;
}

int __wifi_get_scan_results(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifi_mode_t current_mode;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	wifi_scan_result_t *scan_results = (wifi_scan_result_t *)call_argv[0];
	uint32_t *bss_num = (uint32_t *)call_argv[1];
	uint32_t *arr_size = (uint32_t *)call_argv[2];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		return WMG_STATUS_NOT_READY;
	}

	current_mode = wifimg_object->get_current_mode();
	if (current_mode != WIFI_STATION) {
		LOGE("wifi station is not enabled\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if (scan_results == NULL || arr_size == 0 || bss_num == NULL) {
		LOGE("invalid parameters\n");
		*cb_status = WMG_STATUS_INVALID;
		return WMG_STATUS_INVALID;
	}

	ret = wifimg_object->get_scan_results(scan_results, bss_num, *arr_size);
	if (!ret){
		LOGD("get scan results success\n");
	}
	else{
		LOGE("failed to get scan results\n");
	}

	*cb_status = ret;
	return ret;
}

int __wifi_set_mac(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	char *ifname = (char *)call_argv[0];
	uint8_t *mac_addr = (uint8_t *)call_argv[1];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	char wmg_ifname[32];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if(ifname == NULL) {
		strncpy(wmg_ifname, WMG_DEFAULT_INF, 6);
	} else {
		if(strlen(ifname) > 32){
			LOGE("infname longer than 32\n");
			*cb_status = WMG_STATUS_FAIL;
			return WMG_STATUS_FAIL;
		} else {
			strncpy(wmg_ifname, ifname, strlen(ifname));
		}
	}

	ret = wifimg_object->set_mac(wmg_ifname, mac_addr);
	if (!ret){
		LOGD("set mac addr success\n");
	}
	else{
		LOGE("failed to set mac addr\n");
	}

	*cb_status = ret;
	return ret;
}

int __wifi_get_mac(void **call_argv,void **cb_argv)
{
	wmg_status_t ret = WMG_STATUS_FAIL;

	wifimg_object_t* wifimg_object = get_wifimg_object();
	char *ifname = (char *)call_argv[0];
	uint8_t *mac_addr = (uint8_t *)call_argv[1];
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	char wmg_ifname[32];

	if(!wifimg_object->is_init()){
		LOGE("wifi manager is not running\n");
		*cb_status = WMG_STATUS_NOT_READY;
		return WMG_STATUS_NOT_READY;
	}

	if(ifname == NULL) {
		strncpy(wmg_ifname, WMG_DEFAULT_INF, 6);
	} else {
		if(strlen(ifname) > 32){
			LOGE("infname longer than 32\n");
			*cb_status = WMG_STATUS_FAIL;
			return WMG_STATUS_FAIL;
		} else {
			strncpy(wmg_ifname, ifname, strlen(ifname));
		}
	}

	ret = wifimg_object->get_mac(wmg_ifname, mac_addr);
	if (!ret){
		LOGD("get mac addr success\n");
	}
	else{
		LOGE("failed to get mac addr\n");
	}

	*cb_status = ret;
	return ret;
}

int __wifi_send_80211_raw_frame(void **call_argv,void **cb_argv)
{
	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];
	*cb_status = WMG_STATUS_FAIL;
	return WMG_STATUS_FAIL;
}

int __wifi_set_country_code(void **call_argv,void **cb_argv)
{
	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];
	*cb_status = WMG_STATUS_FAIL;
	return WMG_STATUS_FAIL;
}

int __wifi_get_country_code(void **call_argv,void **cb_argv)
{
	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];
	*cb_status = WMG_STATUS_FAIL;
	return WMG_STATUS_FAIL;
}

int __wifi_set_ps_mode(void **call_argv,void **cb_argv)
{
	wifimg_object_t* wifimg_object = get_wifimg_object();
	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];
	*cb_status = WMG_STATUS_FAIL;
	return WMG_STATUS_FAIL;
}

int __wifi_linkd_protocol(void **call_argv,void **cb_argv)
{
	wmg_status_t ret;
	wifimg_object_t* wifimg_object = get_wifimg_object();

	wifi_linkd_mode_t *mode = (wifi_mode_t *)call_argv[0];
	wifi_ap_config_t *params = call_argv[1];
	int second = *(int *)call_argv[2];
	wifi_linkd_result_t *linkd_result = (wifi_linkd_result_t *)call_argv[3];

	wifi_ap_config_t *ap_config = params;

	wmg_status_t *cb_status = (wmg_status_t *)cb_argv[0];

	if(*mode == WMG_LINKD_MODE_SOFTAP) {
		if(!wifimg_object->is_init()){
			ret = wifimg_object->init();
			if (ret != WMG_STATUS_SUCCESS) {
				*cb_status = ret;
				return ret;
			}
		}

		ret = wifimg_object->switch_mode(WIFI_AP);
		if ((ret == WMG_STATUS_UNHANDLED) || (ret == WMG_STATUS_SUCCESS)) {
			LOGD("switch wifi ap mode for config network success\n");
			ret = WMG_STATUS_SUCCESS;
		} else {
			LOGE("failed to switch wifi ap mode for config network\n");
			*cb_status = ret;
			return ret;
		}

		if(ap_config == NULL) {
			char ssid_buf[SSID_MAX_LEN] = "Aw-wifimg-Test";
			char psk_buf[PSK_MAX_LEN] = "aw123456";
			wifi_ap_config_t ap_config_default;
			ap_config_default.ssid = ssid_buf;
			ap_config_default.psk = psk_buf;
			ap_config_default.sec = WIFI_SEC_WPA2_PSK;
			ap_config_default.channel = 6;
			ap_config = &ap_config_default;
		}

		ret = wifimg_object->ap_enable(ap_config);
		if (ret == WMG_STATUS_SUCCESS) {
			LOGD("wifi ap enable success\n");
			ret = WMG_STATUS_SUCCESS;
		} else {
			LOGE("wifi ap enable failed\n");
			*cb_status = ret;
			return ret;
		}
	}

	ret = wifimg_object->linkd_protocol(*mode, params, second, linkd_result);
	if(!ret) {
		LOGD("Get ssid and psk success, now connect to ap:%s with psk:%s\n",
				linkd_result->ssid, linkd_result->psk);
	}

	*cb_status = ret;
	return ret;
}

act_func_t pla_action_table[] = {
	[WMG_PLA_ACT_WIFI_ON] = {__wifi_on, "__wifi_on"},
	[WMG_PLA_ACT_WIFI_OFF] = {__wifi_off, "__wifi_off"},
	[WMG_PLA_ACT_WIFI_REGISTER_MSG_CB] = {__wifi_register_msg_cb, "__wifi_register_msg_cb"},
	[WMG_PLA_ACT_WIFI_SET_MAC] = {__wifi_set_mac, "__wifi_set_mac"},
	[WMG_PLA_ACT_WIFI_GET_MAC] = {__wifi_get_mac, "__wifi_get_mac"},
	[WMG_PLA_ACT_WIFI_SEND_80211_RAW_FRAME] = {__wifi_send_80211_raw_frame, "__wifi_send_80211_raw_frame"},
	[WMG_PLA_ACT_WIFI_SET_COUNTRY_CODE] = {__wifi_set_country_code, "__wifi_set_country_code"},
	[WMG_PLA_ACT_WIFI_GET_COUNTRY_CODE] = {__wifi_get_country_code, "__wifi_get_country_code"},
	[WMG_PLA_ACT_WIFI_SET_PS_MODE] = {__wifi_set_ps_mode, "__wifi_set_ps_mode"},
	[WMG_PLA_ACT_WIFI_LINKD_PROTOCOL] = {__wifi_linkd_protocol, "__wifi_linkd_protocol"},
};

act_func_t sta_action_table[] = {
	[WMG_STA_ACT_CONNECT] = {__wifi_sta_connect, "__wifi_sta_connect"},
	[WMG_STA_ACT_DISCONNECT] = {__wifi_sta_disconnect, "__wifi_sta_disconnect"},
	[WMG_STA_ACT_AUTO_RECONNECT] = {__wifi_sta_auto_reconnect, "__wifi_sta_auto_reconnect"},
	[WMG_STA_ACT_GET_INFO] = {__wifi_sta_get_info, "__wifi_sta_get_info"},
	[WMG_STA_ACT_LIST_NETWORKS] = {__wifi_sta_list_networks, "__wifi_sta_list_networks"},
	[WMG_STA_ACT_REMOVE_NETWORKS] = {__wifi_sta_remove_networks, "__wifi_sta_remove_networks"},
	[WMG_STA_ACT_SCAN_PARAM] = {__wifi_set_scan_param, "__wifi_set_scan_param"},
	[WMG_STA_ACT_SCAN_RESULTS] = {__wifi_get_scan_results, "__wifi_get_scan_results"},
};

act_func_t ap_action_table[] = {
	[WMG_AP_ACT_ENABLE] = {__wifi_ap_enable, "__wifi_ap_enable"},
	[WMG_AP_ACT_DISABLE] = {__wifi_ap_disable, "__wifi_ap_disable"},
	[WMG_AP_ACT_GET_CONFIG] = {__wifi_ap_get_config, "__wifi_ap_get_config"},
};

act_func_t monitor_action_table[] = {
	[WMG_MONITOR_ACT_ENABLE] = {__wifi_monitor_enable, "__wifi_monitor_enable"},
	[WMG_MONITOR_ACT_SET_CHANNEL] = {__wifi_monitor_set_channel, "__wifi_monitor_set_channel"},
	[WMG_MONITOR_ACT_DISABLE] = {__wifi_monitor_disable, "__wifi_monitor_disable"},
};

wmg_status_t __wifimanager_init(void)
{
	if(act_init(&wmg_act_handle,"wmg_act_handle",true) != OS_NET_STATUS_OK) {
		LOGE("act init failed.\n");
		return WMG_STATUS_FAIL;
	}

	act_register_handler(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,pla_action_table);

	act_register_handler(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,sta_action_table);

	act_register_handler(&wmg_act_handle,WMG_ACT_TABLE_AP_ID,ap_action_table);

	act_register_handler(&wmg_act_handle,WMG_ACT_TABLE_MONITOR_ID,monitor_action_table);

	return WMG_STATUS_SUCCESS;
}

wmg_status_t __wifimanager_deinit(void)
{
	act_deinit(&wmg_act_handle);
	return WMG_STATUS_SUCCESS;
}

const char *wmg_sec_to_str(wifi_secure_t sec)
{
	switch (sec) {
		case WIFI_SEC_NONE:
		return "NONE";
		case WIFI_SEC_WEP:
		return "WEP";
		case WIFI_SEC_WPA_PSK:
		return "WPAPSK";
		case WIFI_SEC_WPA2_PSK:
		return "WPA2PSK";
		default:
		return "UNKNOWN";
	}
}

const char *wmg_wifi_mode_to_str(wifi_mode_t mode)
{
	switch (mode) {
	case WIFI_STATION:
		return "station";
	case WIFI_AP:
		return "ap";
	case WIFI_MONITOR:
		return "monitor";
	case WIFI_AP_STATION:
		return "ap-station";
	case WIFI_AP_MONITOR:
		return "ap-monitor";
	default:
		return "unknown";
	}
}

uint8_t wmg_wifi_mode_to_bitmap(wifi_mode_t mode)
{
	switch (mode) {
	case WIFI_MODE_UNKNOWN:
		return UNK_BITMAP;
	case WIFI_STATION:
		return STA_BITMAP;
	case WIFI_AP:
		return AP_BITMAP;
	case WIFI_MONITOR:
		return MONITOR_BITMAP;
	case WIFI_AP_STATION:
		return AP_STA_BITMAP;
	case WIFI_AP_MONITOR:
		return AP_MONITOR_BITMAP;
	default:
		return UNK_BITMAP;
	}
}

wifi_mode_t wmg_wifi_bitmap_to_mode(uint8_t bitmap)
{
	switch (bitmap) {
	case UNK_BITMAP:
		return WIFI_MODE_UNKNOWN;
	case STA_BITMAP:
		return WIFI_STATION;
	case AP_BITMAP:
		return WIFI_AP;
	case MONITOR_BITMAP:
		return WIFI_MONITOR;
	case AP_STA_BITMAP:
		return WIFI_AP_STATION;
	case AP_MONITOR_BITMAP:
		return WIFI_AP_MONITOR;
	default:
		return WIFI_MODE_UNKNOWN;
	}
}

mode_object_t* __attribute__((weak)) wmg_sta_register_object(void)
{
	printf("***** WARN(weak):sta mode is not supported *****\n");
	return NULL;
}

mode_object_t* __attribute__((weak)) wmg_ap_register_object(void)
{
	printf("***** WARN(weak):ap mode is not supported *****\n");
	return NULL;
}

mode_object_t* __attribute__((weak)) wmg_monitor_register_object(void)
{
	printf("***** WARN(weak):monitor mode is not supported *****\n");
	return NULL;
}

/* init wifimg */
static wmg_status_t wifimg_init(void)
{
	wmg_status_t ret = WMG_STATUS_FAIL;

	if(wifimg_object.init_flag == WMG_FALSE) {
		os_net_mutex_create(&wifimg_object.mutex_lock);

		//===这里需要实现wifi支持什么模式===/
		//===这里需要优化注册方式===/
		wifimg_object.mode_object[STA_MODE_NUM] = NULL;
		wifimg_object.mode_object[AP_MODE_NUM] = NULL;
		wifimg_object.mode_object[MONITOR_MODE_NUM] = NULL;
		wifimg_object.mode_object[P2P_MODE_NUM] = NULL;
	
		wifimg_object.mode_object[STA_MODE_NUM] = wmg_sta_register_object();
		if(wifimg_object.mode_object[STA_MODE_NUM] != NULL) {
			if(wifimg_object.mode_object[STA_MODE_NUM]->init != NULL) {
				if(wifimg_object.mode_object[STA_MODE_NUM]->init()){
					LOGE("sta mode init faile.\n");
					return WMG_STATUS_FAIL;
				}
			}
			wifimg_object.support_mode_bitmap |= STA_BITMAP;
		}

		wifimg_object.mode_object[AP_MODE_NUM] = wmg_ap_register_object();
		if(wifimg_object.mode_object[AP_MODE_NUM] != NULL) {
			if(wifimg_object.mode_object[AP_MODE_NUM]->init != NULL) {
				if(wifimg_object.mode_object[AP_MODE_NUM]->init()){
					LOGE("ap mode init faile.\n");
					return WMG_STATUS_FAIL;
				}
			}
			wifimg_object.support_mode_bitmap |= AP_BITMAP;
		}

		wifimg_object.mode_object[MONITOR_MODE_NUM] = wmg_monitor_register_object();
		if(wifimg_object.mode_object[MONITOR_MODE_NUM] != NULL) {
			if(wifimg_object.mode_object[MONITOR_MODE_NUM]->init != NULL) {
				if(wifimg_object.mode_object[MONITOR_MODE_NUM]->init()){
					LOGE("monitor mode init faile.\n");
					return WMG_STATUS_FAIL;
				}
			}
			wifimg_object.support_mode_bitmap |= MONITOR_BITMAP;
		}

		wifimg_object.init_flag = WMG_TRUE;
		LOGD("wifi_manager initialize, version: %s\n%s\n",
			WMG_VERSION, WMG_DECLARE);
		LOGD("************************************************\n");
		if((wifimg_object.support_mode_bitmap & STA_BITMAP) != 0) {
			LOGD("wifi_manager: support STA mode\n");
		} else {
			LOGD("wifi_manager: does not support STA mode\n");
		}
		if((wifimg_object.support_mode_bitmap & AP_BITMAP) != 0) {
			LOGD("wifi_manager: support AP mode\n");
		} else {
			LOGD("wifi_manager: does not support AP mode\n");
		}
		if((wifimg_object.support_mode_bitmap & MONITOR_BITMAP) != 0) {
			LOGD("wifi_manager: support MONITOR mode\n");
		} else {
			LOGD("wifi_manager: does not support MONITOR mode\n");
		}
		LOGD("************************************************\n");

		ret = WMG_STATUS_SUCCESS;
	} else {
		LOGD("wifimg is already initialized\n");
		ret = WMG_STATUS_SUCCESS;
	}

	return ret;
}

static void wifimg_deinit(void)
{
	uint8_t current_mode_bitmap;
	int erro_code;

	if(wifimg_object.init_flag == WMG_TRUE) {
		LOGD("wifimg deinit now\n");
		current_mode_bitmap = wifimg_object.current_mode_bitmap;

		if (current_mode_bitmap & STA_BITMAP) {
			if (wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_disable(&erro_code)) {
				LOGE("wifi sta mode disable faile\n");
				return;
			} else {
				wifimg_object.current_mode_bitmap &= (!STA_BITMAP);
				LOGD("wifi sta mode disable success\n");
			}
		}
		if (current_mode_bitmap & AP_BITMAP) {
			if (wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_disable(&erro_code)) {
				LOGE("wifi ap mode disable faile\n");
				return;
			} else {
				wifimg_object.current_mode_bitmap &= (!AP_BITMAP);
				LOGD("wifi ap mode disable success\n");
			}
		}
		if (current_mode_bitmap & MONITOR_BITMAP) {
			if (wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_disable(&erro_code)) {
				LOGE("wifi monitor mode disable faile\n");
				return;
			} else {
				wifimg_object.current_mode_bitmap &= (!MONITOR_BITMAP);
				LOGD("wifi monitor mode disable success\n");
			}
		}
		wifimg_object.current_mode_bitmap = UNK_BITMAP;

		if((wifimg_object.mode_object[STA_MODE_NUM] != NULL) &&
				(wifimg_object.mode_object[STA_MODE_NUM]->deinit != NULL)) {
			LOGD("deinit sta mode now\n");
			wifimg_object.mode_object[STA_MODE_NUM]->deinit();
		}
		if((wifimg_object.mode_object[AP_MODE_NUM] != NULL) &&
				(wifimg_object.mode_object[AP_MODE_NUM]->deinit != NULL)) {
			LOGD("deinit ap mode now\n");
			wifimg_object.mode_object[AP_MODE_NUM]->deinit();
		}
		if((wifimg_object.mode_object[MONITOR_MODE_NUM] != NULL) &&
				(wifimg_object.mode_object[MONITOR_MODE_NUM]->deinit != NULL)) {
			LOGD("deinit monitor mode now\n");
			wifimg_object.mode_object[MONITOR_MODE_NUM]->deinit();
		}
		if((wifimg_object.mode_object[P2P_MODE_NUM] != NULL) &&
			(wifimg_object.mode_object[P2P_MODE_NUM]->deinit != NULL)) {
			LOGD("deinit p2p mode now\n");
			wifimg_object.mode_object[P2P_MODE_NUM]->deinit();
		}

		wifimg_object.init_flag = WMG_FALSE;
		wifimg_object.wifi_status = WLAN_STATUS_DOWN;
		wifimg_object.current_mode_bitmap = UNK_BITMAP;
		wifimg_object.support_mode_bitmap = UNK_BITMAP;

		LOGD("wifimg deinit success\n");
		return;
	} else {
		LOGD("wifimg already deinit\n");
		return;
	}
}

static wmg_status_t wifimg_switch_mode(wifi_mode_t switch_mode)
{
	/* ------------------------------ */
	/* 这里还要判断是否支持切换的模式 */
	wmg_status_t ret;
	wmg_bool_t mode_ret;
	int erro_code;
	uint8_t switch_mode_bitmap, current_mode_bitmap, disable_mode_bitmap, enable_mode_bitmap;
	wifi_mode_t current_mode = wmg_wifi_bitmap_to_mode(wifimg_object.current_mode_bitmap);

	if (switch_mode < WIFI_STATION || switch_mode >= WIFI_MODE_UNKNOWN) {
		LOGE("unknown wifi mode\n");
		return WMG_STATUS_INVALID;
	}

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if(switch_mode == current_mode) {
		LOGD("switch_mode '%s' equals to current_mode '%s'"
			"do not need switch wifi mode\n",
			wmg_wifi_mode_to_str(current_mode),
			wmg_wifi_mode_to_str(switch_mode));
		return WMG_STATUS_UNHANDLED;
	}

	LOGD("switch wifi mode from current_mode '%s' to new_mode '%s'\n",
		wmg_wifi_mode_to_str(current_mode),
		wmg_wifi_mode_to_str(switch_mode));

	switch_mode_bitmap = wmg_wifi_mode_to_bitmap(switch_mode);
	current_mode_bitmap = wmg_wifi_mode_to_bitmap(current_mode);
	LOGD("switch_mode_bitmap:0x%x\n",switch_mode_bitmap);
	LOGD("current_mode_bitmap:0x%x\n",current_mode_bitmap);

	if((wifimg_object.support_mode_bitmap & switch_mode_bitmap) == 0) {
		LOGE("switch mode fail, %s mode does not support\n", wmg_wifi_mode_to_str(switch_mode));
		return WMG_STATUS_UNSUPPORTED;
	}

	disable_mode_bitmap = (switch_mode_bitmap | current_mode_bitmap) ^ switch_mode_bitmap;
	LOGD("disable_mode_bitmap:0x%x\n",disable_mode_bitmap);

	os_net_mutex_lock(&wifimg_object.mutex_lock);

	if (disable_mode_bitmap & STA_BITMAP) {
		if (wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_disable(&erro_code)) {
			LOGE("wifi sta mode disable faile\n");
		} else {
			wifimg_object.current_mode_bitmap &= (!STA_BITMAP);
			LOGD("wifi sta mode disable success\n");
		}
	}
	if (disable_mode_bitmap & AP_BITMAP) {
		if (wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_disable(&erro_code)) {
			LOGE("wifi ap mode disable faile\n");
		} else {
			wifimg_object.current_mode_bitmap &= (!AP_BITMAP);
			LOGD("wifi ap mode disable success\n");
		}
	}
	if (disable_mode_bitmap & MONITOR_BITMAP) {
		if (wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_disable(&erro_code)) {
			LOGE("wifi monitor mode disable\n");
		} else {
			wifimg_object.current_mode_bitmap &= (!MONITOR_BITMAP);
			LOGD("wifi monitor mode disable success\n");
		}
	}

	enable_mode_bitmap = (switch_mode_bitmap | current_mode_bitmap) ^ current_mode_bitmap;
	LOGD("enable_mode_bitmap:0x%x\n",enable_mode_bitmap);
	if (enable_mode_bitmap & STA_BITMAP) {
		if(wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_enable(&erro_code)){
			LOGE("wifi sta mode enable faile\n");
		} else {
			wifimg_object.current_mode_bitmap |= STA_BITMAP;
			LOGD("wifi sta mode enable success\n");
		}
	}
	if (enable_mode_bitmap & AP_BITMAP) {
		if(wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_enable(&erro_code)){
			LOGE("wifi ap mode enable faile\n");
		} else {
			wifimg_object.current_mode_bitmap |= AP_BITMAP;
			LOGD("wifi ap mode enable success\n");
		}
	}
	if (enable_mode_bitmap & MONITOR_BITMAP) {
		if(wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_enable(&erro_code)){
			LOGE("wifi monitor mode enable faile\n");
		} else {
			wifimg_object.current_mode_bitmap |= MONITOR_BITMAP;
			LOGD("wifi monitor mode enable success\n");
		}
	}

	LOGD("switch after current_mode_bitmap:0x%x\n",wifimg_object.current_mode_bitmap);

	os_net_mutex_unlock(&wifimg_object.mutex_lock);

	if(switch_mode_bitmap == wifimg_object.current_mode_bitmap){
		return WMG_STATUS_SUCCESS;
	} else {
		LOGE("switch mode faile\n");
		return WMG_STATUS_FAIL;
	}
}

static wmg_bool_t wifimg_is_init(void)
{
	return wifimg_object.init_flag;
}

static wifi_mode_t wifimg_get_current_mode(void)
{
	return wmg_wifi_bitmap_to_mode(wifimg_object.current_mode_bitmap);
}

static wmg_status_t wifimg_sta_connect(wifi_sta_cn_para_t *cn_para)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_STATION) {
		LOGE("wifi manager is not sta mode, need to switch sta mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_CONNECT, (void *)cn_para, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_sta_disconnect(void)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_STATION) {
		LOGE("wifi manager is not sta mode, need to switch sta mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_DISCONNECT, NULL, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_sta_auto_reconnect(wmg_bool_t enable)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_STATION) {
		LOGE("wifi manager is not sta mode, need to switch sta mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_AUTO_RECONNECT, (void *)&enable, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_sta_get_info(wifi_sta_info_t *sta_info)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_STATION) {
		LOGE("wifi manager is not sta mode, need to switch sta mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_GET_INFO, (void *)sta_info, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_sta_list_networks(wifi_sta_list_t *sta_list)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_STATION) {
		LOGE("wifi manager is not sta mode, need to switch sta mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_LIST_NETWORKS, (void *)sta_list, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_sta_remove_networks(char *ssid)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_STATION) {
		LOGE("wifi manager is not sta mode, need to switch sta mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_REMOVE_NETWORKS, (void *)ssid, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_ap_enable(wifi_ap_config_t *ap_config)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_AP) {
		LOGE("wifi manager is not ap mode, need to switch ap mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_ctl(WMG_AP_CMD_ENABLE, (void *)ap_config, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_ap_disable(void)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_AP) {
		LOGE("wifi manager is not ap mode, need to switch ap mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_ctl(WMG_AP_CMD_DISABLE, NULL, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_ap_get_config(wifi_ap_config_t *ap_config)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_AP) {
		LOGE("wifi manager is not ap mode, need to switch ap mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_ctl(WMG_AP_CMD_GET_CONFIG, (void *)ap_config, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_monitor_enable(uint8_t channel)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_MONITOR) {
		LOGE("wifi manager is not monitor mode, need to switch monitor mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_ctl(WMG_MONITOR_CMD_ENABLE, (void *)&channel, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_monitor_set_channel(uint8_t channel)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_MONITOR) {
		LOGE("wifi manager is not monitor mode, need to switch monitor mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_ctl(WMG_MONITOR_CMD_SET_CHANNEL, (void *)&channel, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_monitor_disable(void)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() != WIFI_MONITOR) {
		LOGE("wifi manager is not monitor mode, need to switch monitor mode first\n");
		return WMG_STATUS_FAIL;
	}

	ret = wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_ctl(WMG_MONITOR_CMD_DISABLE, NULL, &cb_msg);
	if(ret != WMG_STATUS_SUCCESS) {
		return ret;
	}
	if(cb_msg != WMG_STATUS_SUCCESS) {
		return cb_msg;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_register_msg_cb(wifi_msg_cb_t msg_cb)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if(wifimg_object.wmg_msg_cb != NULL) {
		LOGD("wifi manager register new msg cb\n");
	}

	if(wifimg_object.mode_object[STA_MODE_NUM] != NULL) {
		ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_REGISTER_MSG_CB, (void *)&msg_cb, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	}
	if(wifimg_object.mode_object[AP_MODE_NUM] != NULL) {
		ret = wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_ctl(WMG_AP_CMD_REGISTER_MSG_CB, (void *)&msg_cb, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	}
	if(wifimg_object.mode_object[MONITOR_MODE_NUM] != NULL) {
		ret = wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_ctl(WMG_MONITOR_CMD_REGISTER_MSG_CB, (void *)&msg_cb, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_set_scan_param(wifi_scan_param_t *scan_param)
{
	return WMG_STATUS_FAIL;
}

static wmg_status_t wifimg_get_scan_results(wifi_scan_result_t *wifi_scan_results, uint32_t *wifi_bss_num, uint32_t wifi_arr_size)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_SUCCESS;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	if (wifimg_object.get_current_mode() == WIFI_STATION) {
		int erro_code = 0;
		sta_get_scan_results_para_t cmd_para = {
			.scan_results = wifi_scan_results,
			.bss_num = wifi_bss_num,
			.arr_size = wifi_arr_size,
		};
		ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_SCAN_RESULTS, (void *)&cmd_para, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_set_mac(const char *ifname, uint8_t *mac_addr)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_FAIL;
	common_mac_para_t common_mac_para;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	common_mac_para.ifname = ifname;
	common_mac_para.mac_addr = mac_addr;

	if (wifimg_object.get_current_mode() == WIFI_STATION) {
		ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_SET_MAC, (void *)&common_mac_para, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	} else if (wifimg_object.get_current_mode() == WIFI_AP) {
		ret = wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_ctl(WMG_AP_CMD_SET_MAC, (void *)&common_mac_para, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	} else if (wifimg_object.get_current_mode() == WIFI_MONITOR) {
		ret = wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_ctl(WMG_MONITOR_CMD_SET_MAC, (void *)&common_mac_para, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	} else {
		LOGE("wifi manager mode(%s) not support set mac\n", wmg_wifi_mode_to_str(wifimg_object.get_current_mode()));
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_get_mac(const char *ifname, uint8_t *mac_addr)
{
	wmg_status_t ret, cb_msg;
	cb_msg = WMG_STATUS_FAIL;
	common_mac_para_t common_mac_para;

	if(wifimg_object.init_flag == WMG_FALSE) {
		LOGE("wifi manager is not init\n");
		return WMG_STATUS_NOT_READY;
	}

	common_mac_para.ifname = ifname;
	common_mac_para.mac_addr = mac_addr;

	if (wifimg_object.get_current_mode() == WIFI_STATION) {
		ret = wifimg_object.mode_object[STA_MODE_NUM]->mode_opt->mode_ctl(WMG_STA_CMD_GET_MAC, (void *)&common_mac_para, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	} else if (wifimg_object.get_current_mode() == WIFI_AP) {
		ret = wifimg_object.mode_object[AP_MODE_NUM]->mode_opt->mode_ctl(WMG_AP_CMD_GET_MAC, (void *)&common_mac_para, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	} else if (wifimg_object.get_current_mode() == WIFI_MONITOR) {
		ret = wifimg_object.mode_object[MONITOR_MODE_NUM]->mode_opt->mode_ctl(WMG_MONITOR_CMD_GET_MAC, (void *)&common_mac_para, &cb_msg);
		if(ret != WMG_STATUS_SUCCESS) {
			return ret;
		}
		if(cb_msg != WMG_STATUS_SUCCESS) {
			return cb_msg;
		}
	} else {
		LOGE("wifi manager mode(%s) not support get mac\n", wmg_wifi_mode_to_str(wifimg_object.get_current_mode()));
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t wifimg_linkd_protocol(wifi_linkd_mode_t mode, void *params, int second, wifi_linkd_result_t *linkd_result)
{
	return wmg_linkd_protocol(mode, params, second, linkd_result);
}

static wifimg_object_t wifimg_object = {
	.init_flag = WMG_FALSE,
	.wifi_status = WLAN_STATUS_DOWN,
	.current_mode_bitmap = UNK_BITMAP,
	.support_mode_bitmap = UNK_BITMAP,
	.connect_timeout = 0,
	.wmg_msg_cb = NULL,

	.init = wifimg_init,
	.deinit = wifimg_deinit,
	.switch_mode = wifimg_switch_mode,

	.is_init = wifimg_is_init,
	.get_current_mode = wifimg_get_current_mode,

	.sta_connect =  wifimg_sta_connect,
	.sta_disconnect =  wifimg_sta_disconnect,
	.sta_auto_reconnect =  wifimg_sta_auto_reconnect,
	.sta_get_info =  wifimg_sta_get_info,
	.sta_list_networks =  wifimg_sta_list_networks,
	.sta_remove_networks =  wifimg_sta_remove_networks,

	.ap_enable = wifimg_ap_enable,
	.ap_disable = wifimg_ap_disable,
	.ap_get_config = wifimg_ap_get_config,

	.monitor_enable = wifimg_monitor_enable,
	.monitor_set_channel = wifimg_monitor_set_channel,
	.monitor_disable = wifimg_monitor_disable,

	.register_msg_cb = wifimg_register_msg_cb,
	.set_scan_param = wifimg_set_scan_param,
	.get_scan_results =  wifimg_get_scan_results,
	.set_mac =  wifimg_set_mac,
	.get_mac =  wifimg_get_mac,

	.linkd_protocol = wifimg_linkd_protocol,
};

wifimg_object_t* get_wifimg_object(void)
{
	return &wifimg_object;
}
