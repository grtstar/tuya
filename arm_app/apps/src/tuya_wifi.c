#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <pthread.h>

#ifdef RK_DEVICEIO
#include <DeviceIo/Rk_wifi.h>
#endif

#include "wifi/wifimg.h"
#include "wifi/wmg_common.h"
#include "wifi/wmg_sta.h"

#include "tuya_iot_config.h"
#if defined(WIFI_GW) && (WIFI_GW == 1)

#include "tuya_hal_wifi.h"
#include "tuya.h"
#include "voice.h"
#define TAG "WIF"
#define WLAN_DEV "wlan0"

#define LOGF printf("%s +++++++\n", __FUNCTION__)

int _wifiMode = 0;
char _ssid[128] = {0};

static void exec_cmd(char *pCmd)
{
    printf("Exec Cmd:%s \r\n", pCmd);
    system(pCmd);
}

#ifdef RK_DEVICEIO
int _RK_wifi_state_callback(RK_WIFI_RUNNING_State_e state)
{
printf("_RK_wifi_state_callback state:%d\n", state);
return 0;
}

#else
void wifi_msg_cb(wifi_msg_data_t *msg)
{
    switch (msg->id)
    {
    case WIFI_MSG_ID_DEV_STATUS:
        switch (msg->data.d_status)
        {
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
        switch (msg->data.event)
        {
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
        switch (msg->data.state)
        {
        case WIFI_STA_IDLE:
            LOGD("***** wifi msg cb: sta state idle *****\n");
            break;
        case WIFI_STA_CONNECTING:
            LOGD("***** wifi msg cb: sta state connecting *****\n");
            break;
        case WIFI_STA_CONNECTED:
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
            LOGD("***** wifi msg cb: sta state disconnected *****\n");
            break;
        default:
            LOGD("***** wifi msg cb: sta state unknow *****\n");
            break;
        }
        break;
    case WIFI_MSG_ID_AP_CN_EVENT:
        switch (msg->data.ap_event)
        {
        case WIFI_AP_ENABLED:
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
        default:
            LOGD("***** wifi msg cb: ap event unknow *****\n");
            break;
        }
        break;
    case WIFI_MSG_ID_MONITOR:
        switch (msg->data.mon_state)
        {
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
}
#endif

int tuya_wifi_init(void)
{
    LOGF;
#ifdef RK_DEVICEIO
    RK_wifi_ble_register_callback(_RK_wifi_state_callback);
#else
    wifimanager_init();
    if (!get_wifimg_object()->is_init() && get_wifimg_object()->init())
    {
        LOGE("wifimg init fail\n");
        return WMG_STATUS_NOT_READY;
    }
    if (wifi_register_msg_cb(wifi_msg_cb))
    {
        LOGE("wifi register msg cb fail\n");
        return WMG_STATUS_FAIL;
    }
#endif
    printf("wifi init success\n");
    return WMG_STATUS_SUCCESS;
}

#define MAX_AP_SEARCH 20
OPERATE_RET tuya_adapter_wifi_all_ap_scan(OUT AP_IF_S **ap_ary, OUT UINT_T *num)
{
    LOGF;
    if (NULL == ap_ary || NULL == num)
    {
        return OPRT_INVALID_PARM;
    }
#ifdef RK_DEVICEIO
    RK_wifi_scan();
    // bssid
    char *r = RK_wifi_scan_r_sec(0x1);
    printf("bssid : %s\n", r);
    // freq
    r = RK_wifi_scan_r_sec(0x2);
    printf("freq : %s\n", r);
    // signal level
    r = RK_wifi_scan_r_sec(0x4);
    printf("signal : %s\n", r);
    // flags
    r = RK_wifi_scan_r_sec(0x8);
    printf("flags : %s\n", r);
    // ssid
    r = RK_wifi_scan_r_sec(0x10);
    printf("ssid : %s\n", r);

#else
    static AP_IF_S s_aps[MAX_AP_SEARCH];

    memset(s_aps, 0, sizeof(s_aps));
    *ap_ary = s_aps;
    *num = 0;

#ifdef __HuaweiLite__
    // todo
    // ap used Liteos system scan to implement
#else
   wifi_scan_result_t scan_result[10] = {0};
   int bss_num = {0};
   int r = wifi_get_scan_results(scan_result, &bss_num, sizeof(scan_result));
   if(r != WMG_STATUS_SUCCESS)
   {
       printf("wifi_get_scan_results failed: %d\n", r);
       return OPRT_COM_ERROR;
   }
    for(int i = 0; i < bss_num; i++)
    {
         memcpy(s_aps[i].bssid, scan_result[i].bssid, sizeof(scan_result[i].bssid));
         s_aps[i].channel = (scan_result[i].freq-2400)/20;
         s_aps[i].rssi = scan_result[i].rssi;
         s_aps[i].s_len = strlen(scan_result[i].ssid);
         strncpy(s_aps[i].ssid, scan_result[i].ssid, sizeof(s_aps[i].ssid));
    }
#endif
    printf("WIFI Scan AP End\r\n");
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_assign_ap_scan(IN CONST CHAR_T *ssid, OUT AP_IF_S **ap)
{
    LOGF;
    if (NULL == ssid || NULL == ap)
    {
        return OPRT_INVALID_PARM;
    }

    /*
   scan all ap and search
    */
    AP_IF_S *pTotalAp = NULL;
    UINT_T tatalNum = 0;
    int index = 0;
    tuya_hal_wifi_all_ap_scan(&pTotalAp, &tatalNum);

    *ap = NULL;

    for (index = 0; index < tatalNum; index++)
    {
        if (memcmp(pTotalAp[index].ssid, ssid, pTotalAp[index].s_len) == 0)
        {
            *ap = pTotalAp + index;
            break;
        }
    }

    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_release_ap(IN AP_IF_S *ap)
{ // Static variables, no need to free
    return OPRT_OK;
}

static int s_curr_channel = 1;
OPERATE_RET tuya_adapter_wifi_set_cur_channel(IN CONST BYTE_T chan)
{
    LOGF;
#ifdef RK_DEVICEIO

#else
    if (wifi_monitor_set_channel((uint8_t)(chan)))
    {
        LOGE("set monitor mode channel %d failed\n", chan);
        return OPRT_COM_ERROR;
    }
    else
    {
        LOGD("set monitor mode channel %d success\n", chan);
    }
#ifdef WIFI_CHIP_7601
    tuya_linux_wf_wk_mode_set(WWM_SNIFFER);
#endif
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_get_cur_channel(OUT BYTE_T *chan)
{
    if (NULL == chan)
    {
        return OPRT_INVALID_PARM;
    }

    printf("WIFI Get Curr Channel:%d \r\n", *chan);

    return OPRT_OK;
}

#pragma pack(1)
/*
http://www.radiotap.org/
*/
typedef struct
{
    /**
     * @it_version: radiotap version, always 0
     */
    BYTE_T it_version;

    /**
     * @it_pad: padding (or alignment)
     */
    BYTE_T it_pad;

    /**
     * @it_len: overall radiotap header length
     */
    USHORT_T it_len;

    /**
     * @it_present: (first) present word
     */
    UINT_T it_present;
} ieee80211_radiotap_header;
#pragma pack()

static volatile SNIFFER_CALLBACK s_pSnifferCall = NULL;
static volatile BOOL_T s_enable_sniffer = FALSE;

static void *func_Sniffer(void *pReserved)
{
    printf("Sniffer Thread Create\r\n");

    int sock = socket(PF_PACKET, SOCK_RAW, htons(0x03)); // ETH_P_ALL
    if (sock < 0)
    {
        printf("Sniffer Socket Alloc Fails %d \r\n", sock);
        perror("Sniffer Socket Alloc");
        return (void *)0;
    }

    { /* Force binding to wlan0, can be considered to remove */
        struct ifreq ifr;
        memset(&ifr, 0x00, sizeof(ifr));
        strncpy(ifr.ifr_name, WLAN_DEV, sizeof(ifr.ifr_name) - 1);
        setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, (char *)&ifr, sizeof(ifr));
    }

#define MAX_REV_BUFFER 512
    BYTE_T rev_buffer[MAX_REV_BUFFER];

    int skipLen = 26; /* Radiotap default length is 26 */

    while ((s_pSnifferCall != NULL) && (TRUE == s_enable_sniffer))
    {
        int rev_num = recvfrom(sock, rev_buffer, MAX_REV_BUFFER, 0, NULL, NULL);
        ieee80211_radiotap_header *pHeader = (ieee80211_radiotap_header *)rev_buffer;
        skipLen = pHeader->it_len;

#ifdef WIFI_CHIP_7601
        skipLen = 144;
#endif
        if (skipLen >= MAX_REV_BUFFER)
        { /* Package is lost directly when its length greater than maximum*/
            continue;
        }

        if (0)
        {
            printf("skipLen:%d ", skipLen);
            int index = 0;
            for (index = 0; index < 180; index++)
            {
                printf("%02X-", rev_buffer[index]);
            }
            printf("\r\n");
        }
        if (rev_num > skipLen)
        {
            s_pSnifferCall(rev_buffer + skipLen, rev_num - skipLen, 2);
        }
    }

    s_pSnifferCall = NULL;

    close(sock);

    printf("Sniffer Proc Finish\r\n");
    return (void *)0;
}

static pthread_t sniffer_thId; // ID of capture thread

static BOOL_T sniffer_set_done = FALSE;
// Prevent duplication calling
int tuya_adapter_wifi_sniffer_set(const bool en, const SNIFFER_CALLBACK cb)
// OPERATE_RET tuya_hal_wifi_sniffer_set(IN CONST BOOL_T en,IN CONST SNIFFER_CALLBACK cb)
{
    LOGF;
    if (en == s_enable_sniffer)
    {
        printf("Already in status %d\r\n", en);
        return OPRT_OK;
    }
    s_enable_sniffer = en;
    if (en == TRUE)
    {
        IPC_APP_Notify_LED_Sound_Status_CB(IPC_START_WIFI_CFG);

        printf("Enable Sniffer\r\n");
        tuya_hal_wifi_set_work_mode(WWM_SNIFFER);

        s_pSnifferCall = cb;
        pthread_create(&sniffer_thId, NULL, func_Sniffer, NULL);

        printf("Enable Qrcode \r\n");
    }
    else
    {
        printf("Disable Sniffer\r\n");
        pthread_join(sniffer_thId, NULL);

        tuya_hal_wifi_set_work_mode(WWM_STATION);

        printf("Disable Qrcode\r\n");

        sniffer_set_done = TRUE;
    }

    return OPRT_OK;
}

int hwl_get_local_ip_info(const char *dev, NW_IP_S *ip)
{
#ifdef RK_DEVICEIO
    RK_WIFI_INFO_Connection_s info;
    if (RK_wifi_running_getConnectionInfo(&info) == 0)
    {
        int ip1, ip2, ip3, ip4;
        sscanf(info.ip_address, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4);
        strcpy(ip->ip, info.ip_address);
        sprintf(ip->gw, "%d.%d.%d.1", ip1, ip2, ip3);
        strcpy(ip->mask, "255.255.255.0");
        return OPRT_OK;
        ;
    }

#else
    wifi_sta_info_t sta_info = {0};
    if(WMG_STATUS_SUCCESS == wifi_sta_get_info(&sta_info))
    {
        char ip_str[32] = {0};
        char gw_str[32] = {0};
        char mask_str[32] = {0};
        if (sta_info.ip_addr[0] != 0)
        {
            sprintf(ip_str, "%d.%d.%d.%d", sta_info.ip_addr[0], sta_info.ip_addr[1], sta_info.ip_addr[2], sta_info.ip_addr[3]);
            sprintf(gw_str, "%d.%d.%d.%d", sta_info.ip_addr[0], sta_info.ip_addr[1], sta_info.ip_addr[2], 1);
            sprintf(mask_str, "%d.%d.%d.%d", 255, 255, 255, 0);
            strcpy(ip->ip, ip_str);
            strcpy(ip->gw, gw_str);
            strcpy(ip->mask, mask_str);
            return OPRT_OK;
        }
    }
    
#endif
    return OPRT_COM_ERROR;
}

OPERATE_RET tuya_adapter_wifi_get_ip(IN CONST WF_IF_E wf, OUT NW_IP_S *ip)
{
    LOGF;
    if (NULL == ip)
    {
        return OPRT_INVALID_PARM;
    }

    int ret = 0;

    if (wf == WF_AP)
    { /* Simple Processing in AP Mode */
        memcpy(ip->ip, "192.168.4.1", strlen("192.168.4.1"));
        memcpy(ip->gw, "192.168.4.1", strlen("192.168.4.1"));
        memcpy(ip->mask, "255.255.255.0", strlen("255.255.255.0"));
        return OPRT_OK;
    }

    if (wf == WF_STATION)
    {
        ret = hwl_get_local_ip_info(WLAN_DEV, ip);
    }

    printf("WIFI[%d] Get IP:%s\r\n", wf, ip->ip);
    return ret;
}

OPERATE_RET tuya_adapter_wifi_set_mac(IN CONST WF_IF_E wf, IN CONST NW_MAC_S *mac)
{
    LOGF;
    if (NULL == mac)
    {
        return OPRT_INVALID_PARM;
    }
    printf("WIFI Set MAC\r\n");
    // reserved
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_get_mac(IN CONST WF_IF_E wf, INOUT NW_MAC_S *mac)
{
    LOGF;

    if (NULL == mac)
    {
        return OPRT_INVALID_PARM;
    }

#ifdef __HuaweiLite__
    // todo
    // Implementing MAC acquisition of liteos system
#else
    FILE *pp = popen("ifconfig " WLAN_DEV, "r");
    if (pp == NULL)
    {
        return OPRT_COM_ERROR;
    }

    char tmp[256];
    memset(tmp, 0, sizeof(tmp));
    while (fgets(tmp, sizeof(tmp), pp) != NULL)
    {
        char *pMACStart = strstr(tmp, "HWaddr ");
        if (pMACStart != NULL)
        {
            int x1, x2, x3, x4, x5, x6;
            sscanf(pMACStart + strlen("HWaddr "), "%x:%x:%x:%x:%x:%x", &x1, &x2, &x3, &x4, &x5, &x6);
            mac->mac[0] = x1 & 0xFF;
            mac->mac[1] = x2 & 0xFF;
            mac->mac[2] = x3 & 0xFF;
            mac->mac[3] = x4 & 0xFF;
            mac->mac[4] = x5 & 0xFF;
            mac->mac[5] = x6 & 0xFF;

            break;
        }
    }
    pclose(pp);
#endif

    printf("WIFI Get MAC %02X-%02X-%02X-%02X-%02X-%02X\r\n",
           mac->mac[0], mac->mac[1], mac->mac[2], mac->mac[3], mac->mac[4], mac->mac[5]);

    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_set_work_mode(IN CONST WF_WK_MD_E mode)
{
    LOGF;
    printf("mode = %d\n", mode);
    char tmpCmd[100] = {0};
    _wifiMode = mode;
    switch (mode)
    {
    case WWM_LOWPOWER:
    {
        // Linux system does not care about low power
        break;
    }
    case WWM_SNIFFER:
    {
        wifi_on(WIFI_MONITOR);
        break;
    }
    case WWM_STATION:
    {
        wifi_on(WIFI_STATION);
        break;
    }
    case WWM_SOFTAP:
    {
        wifi_on(WIFI_AP);
        break;
    }
    case WWM_STATIONAP:
    { 
        wifi_on(WIFI_AP);
        break;
    }
    default:
    {
        break;
    }
    }
    printf("WIFI Set Mode %d\r\n", mode);

    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_get_work_mode(OUT WF_WK_MD_E *mode)
{
    // LOGF;
    if (NULL == mode)
    {
        return OPRT_INVALID_PARM;
    }

    *mode = _wifiMode;
    return OPRT_OK;

#ifdef __HuaweiLite__
    // todo liteos system
    // Implementation of mode acquisition
    char scan_mode[10] = {0};
#else
    // todo
    wifi_mode_t current_mode;
    wifimg_object_t* wifimg_object = get_wifimg_object();
    if(wifimg_object == NULL)
    {
        return OPRT_COM_ERROR;
    }
    current_mode = wifimg_object->get_current_mode();
    switch (current_mode)
    {
    case WIFI_STATION:
        *mode = WWM_STATION;
        break;
    case WIFI_AP:
        *mode = WWM_SOFTAP;
        break;
    case WIFI_MONITOR:
        *mode = WWM_SNIFFER;
        break;
    case WIFI_AP_STATION:
        *mode = WWM_STATIONAP;
        break;
    case WIFI_AP_MONITOR:
        *mode = WWM_SNIFFER;
        break;
    case WIFI_MODE_UNKNOWN:
        *mode = WWM_UNKNOWN;
        break;
    default:
        *mode = WWM_UNKNOWN;
        break;
    }
#endif

    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_ap_start(IN CONST WF_AP_CFG_IF_S *cfg)
{
    LOGF;
    if (NULL == cfg)
    {
        return OPRT_INVALID_PARM;
    }

    printf("Start AP SSID:%s, PASS:%s \r\n", cfg->ssid, cfg->passwd);
    char ssid[128] = {0};
    strncpy(ssid, cfg->ssid, sizeof(ssid));
    char passwd[128] = {0};
    strncpy(passwd, cfg->passwd, sizeof(passwd));
    wifi_ap_config_t ap_config = {0};
    ap_config.ssid = ssid;
    ap_config.psk = passwd;
    ap_config.channel = cfg->chan;
    ap_config.sec = cfg->md;
    int r = wifi_ap_enable(&ap_config);
    if(WMG_STATUS_SUCCESS != r)
    {
        printf("wifi_ap_enable failed: %d\n", r);
        return OPRT_COM_ERROR;
    }
    printf("Start AP Success\n");
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_ap_stop(VOID)
{
    LOGF;
    printf("Stop AP \r\n");
    int r = wifi_ap_disable();
    if(r != WMG_STATUS_SUCCESS)
    {
        printf("wifi_ap_disable failed: %d\n", r);
        return OPRT_COM_ERROR;
    }
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_get_connected_ap_info_v2(FAST_WF_CONNECTED_AP_INFO_V2_S **fast_ap_info)
{
    LOGF;
    return 0;
}

OPERATE_RET tuya_adapter_wifi_fast_station_connect_v2(CONST FAST_WF_CONNECTED_AP_INFO_V2_S *fast_ap_info)
{
    LOGF;
    return 0;
}

OPERATE_RET tuya_adapter_wifi_station_connect(IN CONST CHAR_T *ssid, IN CONST CHAR_T *passwd)
{
    LOGF;
    if (sniffer_set_done)
    {
        sniffer_set_done = FALSE;
        IPC_APP_Notify_LED_Sound_Status_CB(IPC_REV_WIFI_CFG);
        usleep(1000 * 1000);
    }

    IPC_APP_Notify_LED_Sound_Status_CB(IPC_CONNECTING_WIFI);

    if (NULL == ssid)
    {
        // get bind info from ethernet network
        printf("get bind info ...\n");
    }
    else
    {
        // get bind info from ap / wifi-smart / qrcode
        printf("get wifi info ... ssid: %s, password: %s\n", ssid, passwd);
    }

    // Add a blocking operation for the wifi connection here.
    strncpy(_ssid, ssid, sizeof(_ssid));
#endif
#ifdef RK_DEVICEIO
    RK_wifi_enable(1);
    RK_wifi_connect(ssid, passwd);
#else

    wifi_sta_info_t sta_info = {0};
    int rr = wifi_sta_get_info(&sta_info);
    printf("wifi_sta_get_info: %d\n", rr);
    wifi_on(WIFI_STATION);
    wifi_sta_cn_para_t sta = {0};
    sta.ssid = ssid;
    sta.password = passwd;
    if(passwd == NULL || strlen(passwd) == 0)
    {
        sta.sec = WIFI_SEC_NONE;
    }
    else
    {
        sta.sec = WIFI_SEC_WPA2_PSK;
    }
    int r = wifi_sta_connect(&sta);
    if(WMG_STATUS_SUCCESS != r)
    {
        printf("wifi_sta_connect failed: %d\n", r);
        return OPRT_COM_ERROR;
    }
#endif
    printf("STA Connect AP\r\n");
    PlayVoice(V_WIFI_RECV_PWD, PLAY_QUEUE);
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_station_disconnect(VOID)
{
    LOGF;
    printf("STA Disconn AP\r\n");
    // Reserved
#ifdef RK_DEVICEIO
    RK_wifi_disconnect_network();
#else
    int r = wifi_sta_disconnect();
    if(WMG_STATUS_SUCCESS != r)
    {
        printf("wifi_sta_disconnect failed: %d\n", r);
        return OPRT_COM_ERROR;
    }
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_station_get_conn_ap_rssi(OUT SCHAR_T *rssi)
{
    LOGF;
    if (NULL == rssi)
    {
        return OPRT_INVALID_PARM;
    }
    *rssi = 0;

#ifdef __HuaweiLite__
    // todo
    // liteos system
    // Implementation of RSSI acquisition
#else
    
#endif
    printf("Get Conn AP RSSI:%d\r\n", *rssi);

    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_get_bssid(UCHAR_T *mac)
{
    LOGF;
    wifi_sta_info_t sta_info = {0};
    if(WMG_STATUS_SUCCESS == wifi_sta_get_info(&sta_info))
    {
        memcpy(mac, sta_info.bssid, sizeof(sta_info.bssid));
    }
    return 0;
}

OPERATE_RET tuya_adapter_wifi_get_ssid(char *ssid)
{
#ifdef RK_DEVICEIO
    RK_WIFI_INFO_Connection_s info;
    RK_wifi_running_getConnectionInfo(&info);

    strncpy(ssid, info.ssid, sizeof(info.ssid));
#else
    wifi_sta_info_t sta_info = {0};
    if(WMG_STATUS_SUCCESS == wifi_sta_get_info(&sta_info))
    {
        if(sta_info.ssid[0] == '\\' && sta_info.ssid[1] == 'x')
        {
            int i = 0;
            int j = 0;
            for(i = 0; i < strlen(sta_info.ssid); i += 4)
            {
                char tmp[3] = {sta_info.ssid[i+2], sta_info.ssid[i + 3], '\0'};
                ssid[j++] = strtol(tmp, NULL, 16);
            }
            ssid[j] = '\0';
        }
        else
        {
            strncpy(ssid, sta_info.ssid, sizeof(sta_info.ssid));
        }
    }
#endif
    return 0;
}

#ifdef RK_DEVICEIO
RK_WIFI_RUNNING_State_e runningStatus = WSS_IDLE;
int _RK_wifi_state_callback(RK_WIFI_RUNNING_State_e state)
{
    runningStatus = state;
    return 0;
}
#endif

OPERATE_RET tuya_adapter_wifi_station_get_status(OUT WF_STATION_STAT_E *stat)
{
    if (NULL == stat)
    {
        return OPRT_INVALID_PARM;
    }
    NW_IP_S ip;
#ifdef RK_DEVICEIO
    static bool first = true;
    if (first)
    {
        if (RK_wifi_running_getState(&runningStatus))
        {
            return OPRT_COM_ERROR;
        }
        RK_wifi_register_callback(_RK_wifi_state_callback);
        first = false;
    }

    switch (runningStatus)
    {
    case RK_WIFI_State_IDLE:
        *stat = WSS_IDLE;
        break;
    case RK_WIFI_State_CONNECTING:
        *stat = WSS_CONNECTING;
        break;
    case RK_WIFI_State_CONNECTFAILED:
        *stat = WSS_CONN_FAIL;
        break;
    case RK_WIFI_State_CONNECTFAILED_WRONG_KEY:
        *stat = WSS_PASSWD_WRONG;
        break;
    case RK_WIFI_State_CONNECTED:
    {
        if (hwl_get_local_ip_info("wlan0", &ip) == OPRT_OK)
        {
            *stat = WSS_GOT_IP;
        }
        else
        {
            *stat = WSS_CONN_SUCCESS;
        }
    }

    break;
    case RK_WIFI_State_DISCONNECTED:
        *stat = WSS_CONN_FAIL;
        break;
    case RK_WIFI_State_OPEN:
        *stat = WSS_IDLE;
        break;
    case RK_WIFI_State_OFF:
        *stat = WSS_IDLE;
        break;
    default:
        break;
    }

    printf("======> tuya_adapter_wifi_station_get_status %d\n", *stat);
#else
    if (hwl_get_local_ip_info("wlan0", &ip) == OPRT_OK)
    {

        *stat = WSS_GOT_IP; // Be sure to return in real time
    }
    else
    {
        *stat = WSS_IDLE;
    }
#endif
    // Reserved
    return OPRT_OK;
}

OPERATE_RET tuya_adapter_wifi_set_country_code(IN CONST CHAR_T *p_country_code)
{
    LOGF;
    printf("Set Country Code:%s \r\n", p_country_code);

    return OPRT_OK;
}

int tuya_adapter_wifi_send_mgnt(const uint8_t *buf, const uint32_t len)
{
    LOGF;
    // use to send pro pack
    return 0;
}

OPERATE_RET tuya_adapter_wifi_register_recv_mgnt_callback(CONST BOOL_T enable, CONST WIFI_REV_MGNT_CB recv_cb)
{
    LOGF;
    return 0;
}

OPERATE_RET tuya_adapter_wifi_set_lp_mode(CONST BOOL_T en, CONST UCHAR_T dtim)
{
    LOGF;
    return 0;
}

BOOL_T tuya_adapter_wifi_rf_calibrated(VOID_T)
{
    LOGF;
    return 1;
}

int gpio_test_all_cb()
{
	return 0;
}
