#include "wifi/wifi.h"
#include "voice.h"
#include <pthread.h>


#define RESTART_AP_TIME 600
typedef struct {
	pthread_t ap_restart_id;
	wifi_ap_config_t ap_config;
	char ssid_buf[SSID_MAX_LEN];
	char psk_buf[PSK_MAX_LEN];
	int restart_ap_flag;
} restart_ap_t;

static wifimg_object_t *g_object_operation = NULL;

#ifdef __cplusplus
extern "C"{
#endif
	extern void robot_send_wifi_status(enum wifi_status status);
#ifdef __cplusplus
}
#endif








void wifi_msg_cb(wifi_msg_data_t *msg)
{
//	enum wifi_status status = ROBOT_WIFI_ALL;
	switch(msg->id) {
		case WIFI_MSG_ID_DEV_STATUS:
			switch(msg->data.d_status) {
				case WLAN_STATUS_DOWN:
					LOGD("***** wifi msg cb: dev status down *****\n");
					break;
				case WLAN_STATUS_UP:
					LOGD("***** wifi msg cb: dev status up *****\n");
					break;
				default:
					LOGD("***** wifi msg cb: dev status unknow *****\n");
					break;
			}
//			status = ROBOT_WIFI_NOT_CONNECT;
			break;
		case WIFI_MSG_ID_STA_CN_EVENT:
			switch(msg->data.event) {
				case WIFI_DISCONNECTED:
//					status = ROBOT_WIFI_NOT_CONNECT;
					LOGD("***** wifi msg cb: sta event disconnect *****\n");
					break;
				case WIFI_SCAN_STARTED:
					LOGD("***** wifi msg cb: sta event scan started *****\n");
					break;
				case WIFI_SCAN_FAILED:
					LOGD("***** wifi msg cb: sta event scan failed *****\n");
					break;
				case WIFI_SCAN_RESULTS:
					LOGD("***** wifi msg cb: sta event scan results *****\n");
					break;
				case WIFI_NETWORK_NOT_FOUND:
//					status = ROBOT_WIFI_NOT_CONNECT;
					PlayVoice(V_WIFI_NOTFOUND, 0);
					LOGD("***** wifi msg cb: sta event network not found *****\n");
					break;
				case WIFI_PASSWORD_INCORRECT:
					PlayVoice(V_WIFI_FAIL, 0);
//					status = ROBOT_WIFI_NOT_CONNECT;
					LOGD("***** wifi msg cb: sta event password incorrect *****\n");
					break;
				case WIFI_AUTHENTIACATION:
					LOGD("***** wifi msg cb: sta event authentiacation *****\n");
					break;
				case WIFI_AUTH_REJECT:
//					status = ROBOT_WIFI_NOT_CONNECT;
					LOGD("***** wifi msg cb: sta event auth reject *****\n");
					break;
				case WIFI_ASSOCIATING:
					LOGD("***** wifi msg cb: sta event associating *****\n");
					break;
				case WIFI_ASSOC_REJECT:
//					status = ROBOT_WIFI_NOT_CONNECT;
					LOGD("***** wifi msg cb: sta event assoc reject *****\n");
					break;
				case WIFI_ASSOCIATED:
					LOGD("***** wifi msg cb: sta event associated *****\n");
					break;
				case WIFI_4WAY_HANDSHAKE:
					LOGD("***** wifi msg cb: sta event 4 way handshake *****\n");
					break;
				case WIFI_GROUNP_HANDSHAKE:
					LOGD("***** wifi msg cb: sta event grounp handshake *****\n");
					break;
				case WIFI_GROUNP_HANDSHAKE_DONE:
					LOGD("***** wifi msg cb: sta event handshake done *****\n");
					break;
				case WIFI_CONNECTED:
//					status = ROBOT_WIFI_CONNECTED;
					LOGD("***** wifi msg cb: sta event connected *****\n");
					break;
				case WIFI_CONNECT_TIMEOUT:
					LOGD("***** wifi msg cb: sta event connect timeout *****\n");
					break;
				case WIFI_DEAUTH:
					LOGD("***** wifi msg cb: sta event deauth *****\n");
					break;
				case WIFI_DHCP_START:
					LOGD("***** wifi msg cb: sta event dhcp start *****\n");
					break;
				case WIFI_DHCP_TIMEOUT:
					LOGD("***** wifi msg cb: sta event dhcp timeout *****\n");
					break;
				case WIFI_DHCP_SUCCESS:
//					status = ROBOT_WIFI_CONNECTED;
					LOGD("***** wifi msg cb: sta event dhcp success *****\n");
					break;
				case WIFI_TERMINATING:
					LOGD("***** wifi msg cb: sta event terminating *****\n");
					break;
				case WIFI_UNKNOWN:
//					status = ROBOT_WIFI_ERROR;
				default:
					LOGD("***** wifi msg cb: sta event unknow *****\n");
					break;
			}
			break;
		case WIFI_MSG_ID_STA_STATE_CHANGE:
			switch(msg->data.state) {
				case WIFI_STA_IDLE:
					LOGD("***** wifi msg cb: sta state idle *****\n");
					break;
				case WIFI_STA_CONNECTING:
//					status = ROBOT_WIFI_CONNECTING;
					LOGD("***** wifi msg cb: sta state connecting *****\n");
					break;
				case WIFI_STA_CONNECTED:
//					status = ROBOT_WIFI_CONNECTED;
					LOGD("***** wifi msg cb: sta state connected *****\n");
					break;
				case WIFI_STA_OBTAINING_IP:
					LOGD("***** wifi msg cb: sta state obtaining ip *****\n");
					break;
				case WIFI_STA_NET_CONNECTED:
					LOGD("***** wifi msg cb: sta state net connected *****\n");
					break;
				case WIFI_STA_DHCP_TIMEOUT:
					LOGD("***** wifi msg cb: sta state dhcp timeout *****\n");
					break;
				case WIFI_STA_DISCONNECTING:
					LOGD("***** wifi msg cb: sta state disconnecting *****\n");
					break;
				case WIFI_STA_DISCONNECTED:
//					status = ROBOT_WIFI_NOT_CONNECT;
					LOGD("***** wifi msg cb: sta state disconnected *****\n");
					break;
				default:
					LOGD("***** wifi msg cb: sta state unknow *****\n");
					break;
			}
			break;
		case WIFI_MSG_ID_AP_CN_EVENT:
			switch(msg->data.ap_event) {
				case WIFI_AP_ENABLED:
//					status = ROBOT_WIFI_SOFT_AP;
					LOGD("***** wifi msg cb: ap enable *****\n");
					break;
				case WIFI_AP_DISABLED:
					LOGD("***** wifi msg cb: ap disable *****\n");
					break;
				case WIFI_AP_STA_DISCONNECTED:
					LOGD("***** wifi msg cb: ap sta disconnected *****\n");
					break;
				case WIFI_AP_STA_CONNECTED:
					LOGD("***** wifi msg cb: ap sta connected *****\n");
					break;
				case WIFI_AP_UNKNOWN:
//					status = ROBOT_WIFI_ERROR;
				default:
					LOGD("***** wifi msg cb: ap event unknow *****\n");
					break;
			}
			break;
		case WIFI_MSG_ID_MONITOR:
			switch(msg->data.mon_state) {
				case WIFI_MONITOR_DISABLE:
					LOGD("***** wifi msg cb: monitor disable *****\n");
					break;
				case WIFI_MONITOR_ENABLE:
					LOGD("***** wifi msg cb: monitor enable *****\n");
					break;
				default:
					LOGD("***** wifi msg cb: monitor state unknow *****\n");
					break;
			}
			break;
		default:
			break;
	}

//	robot_send_wifi_status(status);	


}

uint8_t wifi_init_flag = 0;


int wifi_deinit()
{

	LOGD("wifi_deinit.\n");
	if(wifi_init_flag){
		wifimanager_deinit();
		wifi_init_flag = 0;
	}
	exit(0);
}


int wifi_init()
{


	wifimanager_init();

	wifi_init_flag = 1;
	g_object_operation = get_wifimg_object();
	
	if(!g_object_operation->is_init() && g_object_operation->init()){
		LOGE("wifi manager is not running\n");
		return WMG_STATUS_NOT_READY;
	}


	if(g_object_operation->is_init() && wifi_register_msg_cb(wifi_msg_cb)){
		LOGE("wifi register msg cb fail.\n");
	}




	return WMG_STATUS_SUCCESS;
}


void wifi_scan()
{

	int ret = 0;
	char reply[4096] = {'\0'};
	char scan_cmd[8] = {"SCAN"};
//	char scan_cmd[8] = {"LIST_NETWORKS"};
	if(!g_object_operation)
		 ret = (wmg_sta_object_t *)((wmg_sta_object_t *)g_object_operation->mode_object[0]->private_data)->platform_inf[0]->sta_wpa_command(scan_cmd, reply, sizeof(reply));
	LOGD("SCAN result : %s.\n", reply);

}



int wifi_connected(int is_bind)
{
	
	char status_cmd[8] = {"STATUS"};
	char reconnect_cmd[16] = {"RECONNECT"};
	char reply[256] = {'\0'};
	int ret = 0;
	static int log_flag = 0;
	static int disconnect_count = 0;
	
//	LOGD("wifi mode : %d.\n", g_object_operation->get_current_mode());
	if(!g_object_operation || g_object_operation->get_current_mode() != WIFI_STATION){
		
		if(g_object_operation->get_current_mode() == WIFI_MODE_UNKNOWN && is_bind == 5){
		//	LOGD("wifi mode : %d, is_bind : %d.\n", g_object_operation->get_current_mode(), is_bind);
		//	wifi_close();
			LOGW("wifi work on sta.\n");
			wifi_work_on_sta();
		}

		return -1;
	}

	
	ret = (wmg_sta_object_t *)((wmg_sta_object_t *)g_object_operation->mode_object[0]->private_data)->platform_inf[0]->sta_wpa_command(status_cmd, reply, sizeof(reply));


	if (ret) {
		LOGE("failed to get sta status\n");
		sleep(1);
		return -1;
	}

			
/* 			LOGD("network status : %s.\n", reply);
			LOGD("system : /etc/wifi/udhcpc_wlan0 start >/dev/null.\n");
			system("/etc/wifi/udhcpc_wlan0 start >/dev/null"); */
		

	if (strstr(reply, "wpa_state=COMPLETED") != NULL && strstr(reply, "ip_address=") != NULL) {
			disconnect_count = 0;
			if(log_flag != 1){
				LOGD("sta status is: connection\n");
				log_flag = 1;
				robot_send_wifi_status(ROBOT_WIFI_CONNECTED);
			}
			return 0;
	}else{
		if(strstr(reply, "wpa_state=COMPLETED") != NULL && strstr(reply, "ip_address=") == NULL && is_bind >= 4){
			LOGD("network status : %s\nis_bind : %d.\n", reply, is_bind);
 			system("/etc/wifi/udhcpc_wlan0 restart >/dev/null");
			sleep(10); 

		}
		disconnect_count++;
		if(log_flag != 2){
			LOGE("sta status is: disconnect\n");
			log_flag = 2;
		}
		if(disconnect_count >= 5){
			LOGW("wifi work reconnect.\n");
			ret = (wmg_sta_object_t *)((wmg_sta_object_t *)g_object_operation->mode_object[0]->private_data)->platform_inf[0]->sta_wpa_command(reconnect_cmd, reply, sizeof(reply));
			disconnect_count = 0;
		}

		return -1;
	}
} 

int wifi_close()
{
    wifi_off();
}



int wifi_work_on_sta()
{

    int ret = wifi_on(WIFI_STATION);
    if(ret == 0){
        LOGD("wifi on sta mode success.\n");

    }else{
        LOGE("wifi on sta mode fail.\n");
        ret = -1;
    }
    return ret;
}

int wifi_get_mode()
{
	/*
	typedef enum {
	WIFI_STATION,
	WIFI_AP,
	WIFI_MONITOR,
	WIFI_AP_STATION,
	WIFI_AP_MONITOR,
	WIFI_MODE_UNKNOWN,
	} wifi_mode_t;
	*/
	return g_object_operation ? g_object_operation->get_current_mode() : -1;
}

int wifi_work_on_ap(char *ssid)
{

    restart_ap_t restart_ap;
    wifi_ap_config_t ap_config;
    int ret = 0;
    char pssid[64] = {'\0'};
    memcpy(pssid, ssid, strlen(ssid));
    ap_config.ssid = pssid;
    ap_config.psk = NULL;
    ap_config.sec = WIFI_SEC_NONE;
    ap_config.channel = 6;
    ret = wifi_on(WIFI_AP);
    if(ret == 0){
        LOGD("wifi on ap mode success.\n");
/*         if(wifi_register_msg_cb(wifi_msg_cb)){
            LOGE("wifi register msg cb fail.\n");
        } */
    }else{
        LOGE("wifi on ap mode fail.\n");
        ret = -1;
    }

    ret = wifi_ap_enable(&ap_config);
    if (ret == 0) {
        LOGD("ap enable success, ssid=%s, psk=%s, sec=%d, channel=%d\n", ap_config.ssid, ap_config.psk, ap_config.sec, ap_config.channel);
		robot_send_wifi_status(ROBOT_WIFI_SOFT_AP);
    }else{
		robot_send_wifi_status(ROBOT_WIFI_ERROR);
	}
}

int wifi_work_on_monitor(wifi_msg_handle msg_handle)
{

    int ret = wifi_on(WIFI_MONITOR);
    if(ret == 0){
        LOGD("wifi on monitor mode success.\n");
		if(msg_handle && wifi_register_msg_cb(msg_handle)){
            LOGE("wifi register msg cb fail.\n");
        } 
		ret = wifi_monitor_enable(6);
		if (ret == 0) {
			LOGD("monitor enable success, channel=%d\n", 6);
		} else {
			LOGE("ap enable failed\n");
			return -1;
		}
    }else{
        LOGE("wifi on monitor mode fail.\n");
        ret = -1;
    }
	return ret;
}


int wifi_set_channel(const int ch)
{

	if(wifi_monitor_set_channel((uint8_t)(ch))){
		LOGE("set monitor mode channel %d failed\n",ch);
		return -1;
	} else {
		LOGD("set monitor mode channel %d success\n",ch);
	}
	return 0;
}


int wifi_connect_network(char *ssid, char *pwd)
{

    int ret = 0;
    wifi_sta_cn_para_t cn_para;
    cn_para.ssid = ssid;
    cn_para.password = pwd;
    cn_para.fast_connect = 0;
    cn_para.sec = WIFI_SEC_WPA2_PSK;
    LOGD("wifi connect use wpa2 psk.\n");
	robot_send_wifi_status(ROBOT_WIFI_CONNECTING);
    ret = wifi_sta_connect(&cn_para);
    if(ret != 0){
        if(cn_para.sec == WIFI_SEC_WPA2_PSK){
            cn_para.sec = WIFI_SEC_WPA_PSK;
            LOGD("wifi connect use wpa psk.\n");
            ret = wifi_sta_connect(&cn_para);
            if(ret != 0){
                if(cn_para.sec == WIFI_SEC_WPA_PSK){
                    cn_para.sec = WIFI_SEC_WEP;
                    LOGD("wifi connect use wep.\n");
                    ret = wifi_sta_connect(&cn_para);
                }
            }
        }
    }

    if(ret == 0){
        LOGD("wifi connect ap success.\n");
		robot_send_wifi_status(ROBOT_WIFI_CONNECTED);
    }else{
		robot_send_wifi_status(ROBOT_WIFI_ERROR);
        LOGE("wifi connect ap fail.\n");
    }
    return ret;
}