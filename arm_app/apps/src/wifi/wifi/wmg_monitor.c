#include "wifi/wmg_monitor.h"
#include "wifi/event.h"
#include "wifi/linux_nl.h"

#include <string.h>

static wmg_monitor_object_t monitor_object;

static void monitor_nl_data_notify(wifi_monitor_data_t *frame)
{
	wifi_msg_data_t msg;

	if (monitor_object.monitor_msg_cb) {
		msg.id = WIFI_MSG_ID_MONITOR;
		msg.data.frame = frame;
		monitor_object.monitor_msg_cb(&msg);
	}
	monitor_object.pkt_cnt++;
}

static int monitor_enable(void *param, void *cb_msg)
{
	wmg_status_t ret;
	wifi_msg_data_t msg;

	uint8_t *channel = (uint8_t *)param;
	wmg_status_t *cb_args = (wmg_status_t *)cb_msg;

	if(monitor_object.init_flag == WMG_FALSE) {
		LOGE("monitor has not been initialized\n");
		*cb_args = WMG_STATUS_FAIL;
		return WMG_STATUS_FAIL;
	}

	if(!monitor_object.nl_run) {
		LOGW("has not connect to nl\n");
		*cb_args = WMG_STATUS_FAIL;
		return WMG_STATUS_FAIL;
	}

	if (monitor_object.monitor_state == WIFI_MONITOR_ENABLE) {
		LOGW("wifi monitor is already enabled\n");
		*cb_args = WMG_STATUS_SUCCESS;
		return WMG_STATUS_SUCCESS;
	}

	LOGD("wifi monitor enabling...\n");

	ret = monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_enable(*channel);
	if (ret) {
		LOGE("failed to enable monitor mode\n");
		*cb_args = WMG_STATUS_FAIL;
		return WMG_STATUS_FAIL;
	}
	monitor_object.monitor_state = WIFI_MONITOR_ENABLE;
	if (monitor_object.monitor_msg_cb) {
		msg.id = WIFI_MSG_ID_MONITOR;
		msg.data.mon_state = WIFI_MONITOR_ENABLE;
		monitor_object.monitor_msg_cb(&msg);
	}

	LOGD("wifi monitor enable success\n");

	*cb_args = ret;
	return ret;
}

static int monitor_set_channel(void *param, void *cb_msg)
{
	wmg_status_t ret;
	wifi_msg_data_t msg;

	uint8_t *channel = (uint8_t *)param;
	wmg_status_t *cb_args = (wmg_status_t *)cb_msg;

	if(monitor_object.init_flag == WMG_FALSE) {
		LOGE("monitor has not been initialized\n");
		*cb_args = WMG_STATUS_FAIL;
		return WMG_STATUS_FAIL;
	}

	if(!monitor_object.nl_run) {
		LOGW("has not connect to nl\n");
		*cb_args = WMG_STATUS_FAIL;
		return WMG_STATUS_FAIL;
	}

	if (monitor_object.monitor_state != WIFI_MONITOR_ENABLE) {
		LOGW("wifi monitor is not enabled\n");
		*cb_args = WMG_STATUS_FAIL;
		return WMG_STATUS_FAIL;
	}

	ret = monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_set_channel(*channel);
	if (ret) {
		LOGE("monitor mode failed to set channel\n");
		*cb_args = WMG_STATUS_FAIL;
		return WMG_STATUS_FAIL;
	}

	LOGD("wifi monitor set channel success\n");

	*cb_args = ret;
	return ret;
}

static int monitor_disable(void *param, void *cb_msg)
{
	wmg_status_t ret;
	wifi_msg_data_t msg;
	wmg_status_t *cb_args = (wmg_status_t *)cb_msg;

	if(monitor_object.init_flag == WMG_FALSE) {
		LOGW("wifi monitor is not initialized\n");
		*cb_args = WMG_STATUS_UNHANDLED;
		return WMG_STATUS_UNHANDLED;
	}

	if (monitor_object.monitor_state == WIFI_MONITOR_DISABLE) {
		LOGW("wifi monitor already disabled\n");
		*cb_args = WMG_STATUS_SUCCESS;
		return WMG_STATUS_SUCCESS;
	}

	LOGD("wifi monitor disabling...\n");

	ret = monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_disable();
	if (ret){
		LOGE("failed to disable wifi monitor\n");
	}
    else{
		LOGD("wifi monitor disabled\n");
	}

	monitor_object.monitor_state = WIFI_MONITOR_DISABLE;
	if (monitor_object.monitor_msg_cb) {
		msg.id = WIFI_MSG_ID_MONITOR;
		msg.data.mon_state = WIFI_MONITOR_DISABLE;
		monitor_object.monitor_msg_cb(&msg);
	}

	*cb_args = ret;
	return ret;
}

static int monitor_register_msg_cb(void *param, void *cb_msg)
{
	if(monitor_object.init_flag == WMG_FALSE) {
		LOGE("monitor has not been initialized\n");
		return WMG_STATUS_FAIL;
	}

	wifi_msg_cb_t *msg_cb = (wifi_msg_cb_t *)param;
	wmg_status_t *cb_args = (wmg_status_t *)cb_msg;

	if (*msg_cb == NULL) {
		LOGW("message callback is NULL\n");
		*cb_args = WMG_STATUS_UNHANDLED;
		return WMG_STATUS_UNHANDLED;
	}

	monitor_object.monitor_msg_cb = *msg_cb;

	*cb_args = WMG_STATUS_SUCCESS;
	return WMG_STATUS_SUCCESS;
}

static int monitor_set_mac(void *param,void *cb_msg)
{
	wmg_status_t ret;

	if(monitor_object.init_flag == WMG_FALSE) {
		LOGE("monitor has not been initialized\n");
		return WMG_FALSE;
	}

	common_mac_para_t *common_mac_para = (common_mac_para_t *)param;
	wmg_status_t *cb_args = (wmg_status_t *)cb_msg;

	ret = monitor_object.platform_inf[PLATFORM_LINUX]->monitor_platform_extension(NL_CMD_SET_MAC, (void *)common_mac_para, NULL);

	*cb_args = ret;
	return ret;
}

static int monitor_get_mac(void *param,void **cb_msg)
{
	wmg_status_t ret;

	if(monitor_object.init_flag == WMG_FALSE) {
		LOGE("monitor has not been initialized\n");
		return WMG_FALSE;
	}

	common_mac_para_t *common_mac_para = (common_mac_para_t *)param;
	wmg_status_t *cb_args = (wmg_status_t *)cb_msg;

	ret = monitor_object.platform_inf[PLATFORM_LINUX]->monitor_platform_extension(NL_CMD_GET_MAC, (void *)common_mac_para, NULL);

	*cb_args = ret;
	return ret;
}

static wmg_status_t monitor_mode_init(void)
{
	if (monitor_object.init_flag == WMG_FALSE) {
		LOGD("monitor mode init now\n");
		monitor_object.platform_inf[PLATFORM_LINUX] = NULL;
		monitor_object.platform_inf[PLATFORM_RTOS] = NULL;
		monitor_object.platform_inf[PLATFORM_LINUX] = monitor_linux_inf_object_register();
		if(monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_init != NULL){
			if(monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_init(monitor_nl_data_notify)){
				return WMG_STATUS_FAIL;
			}
		}
		monitor_object.init_flag = WMG_TRUE;
	} else {
		LOGD("monitor mode already init\n");
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t monitor_mode_deinit(void)
{
	if (monitor_object.init_flag == WMG_TRUE) {
		LOGD("monitor mode deinit now\n");
		if(monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_deinit != NULL){
			monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_deinit();
		}
		monitor_object.init_flag = WMG_FALSE;
		monitor_object.monitor_msg_cb = NULL;
		monitor_object.pkt_cnt = 0;
	} else {
		LOGD("monitor mode already deinit\n");
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_status_t monitor_mode_enable(int* erro_code)
{
	wmg_status_t ret;

	if(monitor_object.init_flag == WMG_FALSE) {
		LOGE("monitor has not been initialized\n");
		return WMG_STATUS_FAIL;
	}
	if (monitor_object.monitor_enable == WMG_TRUE) {
		LOGW("wifi monitor already enabled\n");
		return WMG_STATUS_SUCCESS;
	}

	LOGD("monitor monde enabling...\n");

	ret = monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_connect();
	if (ret) {
		LOGE("failed to connect to nl\n");
		return WMG_STATUS_FAIL;
	}
	monitor_object.nl_run = WMG_TRUE;
	LOGD("start hmonitord success\n");

	monitor_object.monitor_enable = WMG_TRUE;

	LOGD("wifi monitor enable success\n");
	return ret;
}

static wmg_status_t monitor_mode_disable(int* erro_code)
{
	wmg_status_t ret;

	if(monitor_object.init_flag == WMG_FALSE) {
		LOGW("wifi monitor has not been initialized\n");
		return WMG_STATUS_SUCCESS;
	}
	if (monitor_object.monitor_enable == WMG_FALSE) {
		LOGW("wifi monitor already disabled\n");
		return WMG_STATUS_SUCCESS;
	}

	LOGD("wifi monitor disabling...\n");

	monitor_object.platform_inf[PLATFORM_LINUX]->monitor_nl_disable();
	monitor_object.monitor_enable = WMG_FALSE;
	monitor_object.nl_run = WMG_FALSE;

	LOGD("wifi monitor disabled\n");

	return WMG_STATUS_SUCCESS;
}

static wmg_status_t monitor_mode_ctl(int cmd, void *param, void *cb_msg)
{
	if(monitor_object.init_flag == WMG_FALSE) {
		LOGE("wifi monitor has not been initialized\n");
		return WMG_STATUS_FAIL;
	}
	if(monitor_object.monitor_enable == WMG_FALSE) {
		LOGW("wifi monitor already disabled\n");
		return WMG_STATUS_SUCCESS;
	}

	LOGD("=====monitor_mode_ctl  cmd: %d=====\n", cmd);

	switch (cmd) {
		case WMG_MONITOR_CMD_ENABLE:
			if(monitor_enable(param,cb_msg)){
				return WMG_STATUS_FAIL;
			}
			break;
		case WMG_MONITOR_CMD_SET_CHANNEL:
			if(monitor_set_channel(param,cb_msg)){
				return WMG_STATUS_FAIL;
				return WMG_STATUS_FAIL;
			}
			break;
		case WMG_MONITOR_CMD_DISABLE:
			if(monitor_disable(param,cb_msg)){
				return WMG_STATUS_FAIL;
			}
			break;
		case WMG_MONITOR_CMD_REGISTER_MSG_CB:
			if(monitor_register_msg_cb(param,cb_msg)){
				return WMG_STATUS_FAIL;
			}
			break;
		case WMG_MONITOR_CMD_SET_MAC:
			if(monitor_set_mac(param,cb_msg)){
				return WMG_STATUS_FAIL;
			}
			break;
		case WMG_MONITOR_CMD_GET_MAC:
			if(monitor_get_mac(param,cb_msg)){
				return WMG_STATUS_FAIL;
			}
			break;
		default:
		return WMG_STATUS_FAIL;
	}
	return WMG_STATUS_SUCCESS;
}

static wmg_monitor_object_t monitor_object = {
	.init_flag = WMG_FALSE,
	.monitor_enable = WMG_FALSE,
	.nl_run = WMG_FALSE,
	.monitor_state = WIFI_MONITOR_DISABLE,
	.pkt_cnt = 0,
	.monitor_msg_cb = NULL,
};

static mode_opt_t monitor_mode_opt = {
	.mode_enable = monitor_mode_enable,
	.mode_disable = monitor_mode_disable,
	.mode_ctl = monitor_mode_ctl,
};

static mode_object_t monitor_mode_object = {
	.mode_name = "monitor",
	.init = monitor_mode_init,
	.deinit = monitor_mode_deinit,
	.mode_opt = &monitor_mode_opt,
	.private_data = &monitor_object,
};

mode_object_t* wmg_monitor_register_object(void)
{
	return &monitor_mode_object;
}
