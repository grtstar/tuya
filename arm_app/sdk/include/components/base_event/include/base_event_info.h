/**
 * @file base_event_info.h
 * @brief tuya simple event module
 * @version 1.0
 * @date 2019-10-30
 *
 * @copyright Copyright (c) tuya.inc 2019
 *
 */

#ifndef __BASE_EVENT_INFO_H__
#define __BASE_EVENT_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief the event define
 *
 */
#define EVENT_RESET             "dev.reset"         // device reset
#define EVENT_INIT              "dev.init"          // sdk init finished, device init start
#define EVENT_RUN               "dev.run"           // sdk active finished, device run start
#define EVENT_REBOOT_REQ        "dev.reboot.req"    // device health check reboot request, application should subscribe it if needed
#define EVENT_REBOOT_ACK        "dev.reboot.ack"    // device health check reboot ack, application should publish when it ready
#define EVENT_CAD_VER_UPDATE    "cad.update"        // cad protocol update
#define EVENT_FW_VER_UPDATE     "fw.update"         // firmware version update
#define EVENT_POST_ACTIVATE     "activate.post"     // active
#define EVENT_PROD_INFO_CHANGED "prod.chg"          //
#define EVENT_HTTP_FAILED       "http.failed"       // http request failed
#define EVENT_SDK_EARLY_INIT_OK "early.init"        // before kv flash init
#define EVENT_SDK_DB_INIT_OK    "db.init"           // before db init
#define EVENT_SCHEMA_UPDATE     "schema.update"     // schema update
#define EVENT_NETCFG_ERROR      "netcfg.error"      // error happened when netcfg
#define EVENT_NETCFG_DATA       "netcfg.data"       // netcfg raw data(ty_cJSON*)
#define EVENT_NETCFG_USER       "netcfg.user"       // netcfg start(user)
#define EVENT_MQTT_CONNECTED    "mqtt.con"          // mqtt connect
#define EVENT_MQTT_DISCONNECTED "mqtt.disc"         // mqtt disconnect
#define EVENT_LAN_DISABLE       "lan.disable"       // lan disable
#define EVENT_LAN_ENABLE        "lan.enable"        // lan enable
#define EVENT_LAN_CLIENT_CLOSE  "lan.cli.close"     // lan client close
#define EVENT_LAN_LINK_CHNAGED  "lan.linkage"       // lan linkage changed
#define EVENT_SUMMER_TZ         "summer.tz"         // summer time zone update
#define EVENT_RECOVER_DB        "dev.recover.db"    // db is lost, device need recover from cloud
#define EVENT_HEALTH_ALERT      "health.alert"      // health alert
#define EVENT_RSC_UPDATE        "rsc.update"        // rsc update
#define EVENT_TIME_SYNC         "time.sync"         // time is sync
#define EVENT_SENIOR_ABILITY    "senior.ability"    // senior ability
#define EVENT_REBOOT            "dev_reboot"        // device reboot

#define EVENT_WIFI_PRE_DISCON   "wifi.pre.discon"   // wifi pre disconent event
#define EVENT_WIFI_PRE_CONN     "wifi.pre.conn"     // wifi pre connect event

#define EVENT_WIFI_STATUS_UPDATE    "wifiStatUpdate"      // wifi status update event
#define EVENT_WIFI_CONNECT_START    "wifi.con.start" // start to connect wifi
#define EVENT_WIFI_CONNECT_SUCCESS  "wifi.con.ok" // succeed to connect wifi
#define EVENT_WIFI_CONNECT_FAILED   "wifi.con.fail" // failed to connect wifi

#define EVENT_BT_LINK_SWITCH     "bt_link"      // bt link switch event

#define EVENT_BT_V2_CRPT         "bt_v2_crpt"       // bt v2 crpt event
#define EVENT_AP_CFG_4G          "ap_cfg_4g"       // AP cfg 4G event


// ota
// #define EVENT_OTA_START_NOTIFY      "ota.start"
#define EVENT_OTA_PROCESS_NOTIFY    "ota.process"
#define EVENT_OTA_FAILED_NOTIFY     "ota.failed"
#define EVENT_OTA_FINISHED_NOTIFY   "ota.finished"

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  /*__BASE_EVENT_H__ */


