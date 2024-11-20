/*
 * Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
 *
 * File: wifimg.c
 *
 * Description:
 *		This file implements the core APIs of wifi manager.
 */
#include "wifi/wifimg.h"
#include "wifi/wmg_common.h"

#include <string.h>

act_handle_t wmg_act_handle;

wmg_status_t wifimanager_init(void)
{
	return __wifimanager_init();
}

wmg_status_t wifimanager_deinit(void)
{
	return __wifimanager_deinit();
}

wmg_status_t wifi_on(wifi_mode_t mode)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_ON,1,1,&mode,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_off(void)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_OFF,1,1,NULL,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_sta_connect(wifi_sta_cn_para_t *cn_para)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_CONNECT,1,1,cn_para,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_sta_disconnect(void)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_DISCONNECT,1,1,NULL,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_sta_auto_reconnect(wmg_bool_t enable)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_AUTO_RECONNECT,1,1,&enable,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_sta_get_info(wifi_sta_info_t *sta_info)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_GET_INFO,1,1,sta_info,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_sta_list_networks(wifi_sta_list_t *sta_list_networks)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_LIST_NETWORKS,1,1,sta_list_networks,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_sta_remove_networks(char *ssid)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_REMOVE_NETWORKS,1,1,ssid,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_ap_enable(wifi_ap_config_t *ap_config)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_AP_ID,WMG_AP_ACT_ENABLE,1,1,ap_config,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_ap_disable(void)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_AP_ID,WMG_AP_ACT_DISABLE,1,1,NULL,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_ap_get_config(wifi_ap_config_t *ap_config)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_AP_ID,WMG_AP_ACT_GET_CONFIG,1,1,ap_config,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_monitor_enable(uint8_t channel)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_MONITOR_ID,WMG_MONITOR_ACT_ENABLE,1,1,&channel,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_monitor_set_channel(uint8_t channel)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_MONITOR_ID,WMG_MONITOR_ACT_SET_CHANNEL,1,1,&channel,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_monitor_disable(void)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_MONITOR_ID,WMG_MONITOR_ACT_DISABLE,1,1,NULL,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_register_msg_cb(wifi_msg_cb_t msg_cb)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_REGISTER_MSG_CB,1,1,&msg_cb,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_set_scan_param(wifi_scan_param_t *scan_param)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_SCAN_PARAM,1,1,scan_param,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_get_scan_results(wifi_scan_result_t *scan_results, uint32_t *bss_num, uint32_t arr_size)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;

	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_STA_ID,WMG_STA_ACT_SCAN_RESULTS,3,1,
				(void *)scan_results,(void *)bss_num,(void *)&arr_size,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_set_mac(const char *ifname, uint8_t *mac_addr)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;

	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_SET_MAC,2,1,
				(void *)ifname,(void *)mac_addr,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_get_mac(const char *ifname, uint8_t *mac_addr)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;

	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_GET_MAC,2,1,
				(void *)ifname,(void *)mac_addr,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_send_80211_raw_frame(uint8_t *data, uint32_t len, void *priv)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_SEND_80211_RAW_FRAME,1,1,NULL,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_set_country_code(const char *country_code)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_SET_COUNTRY_CODE,1,1,country_code,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_get_country_code(char *country_code)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_GET_COUNTRY_CODE,1,1,country_code,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_set_ps_mode(wmg_bool_t enable)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_SET_PS_MODE,1,1,&enable,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}

wmg_status_t wifi_linkd_protocol(wifi_linkd_mode_t mode, void *params, int second, wifi_linkd_result_t *linkd_result)
{
	wmg_status_t cb_msg = WMG_STATUS_FAIL;
	if(act_transfer(&wmg_act_handle,WMG_ACT_TABLE_PLA_ID,WMG_PLA_ACT_WIFI_LINKD_PROTOCOL,4,1,
				(void *)&mode,(void *)params,(void *)&second,(void *)linkd_result,&cb_msg)){
		LOGE("act_transfer fail\n");
		return WMG_STATUS_FAIL;
	} else {
		return cb_msg;
	}
}
