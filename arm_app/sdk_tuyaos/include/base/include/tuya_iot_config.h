/*
tuya_iot_config.h
Copyright(C),2018-2020, 涂鸦科技 www.tuya.comm
*/

/* AUTO-GENERATED FILE. DO NOT MODIFY !!!
*
* This config file is automatically generated by tuya cross-build system.
* It should not be modified by hand.
*/

#ifndef TUYA_IOT_CONFIG_H
#define TUYA_IOT_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* default definitons */

#define SYSTEM_SMALL_MEMORY_BEGIN       0       /*small memory systems begin */
#define SYSTEM_REALTEK8710_1M           1
#define SYSTEM_REALTEK8710_2M           2
#define SYSTEM_NON_OS					3		/*no operating system*/
#define SYSTEM_FREERTOS                 98
#define SYSTEM_SMALL_MEMORY_END         99      /*small memory systems end */
#define SYSTEM_LINUX                    100
#define SYSTEM_LITEOS                   120

#define TLS_DISABLE                     0       /* disable tls function */
#define TLS_TUYA_PSK_ONLY               2       /* only enable ciper 0xAE */
#define TLS_TUYA_ECC_PSK                3       /* enable ciper 0xAE && 0xC027 */
#define TLS_TUYA_ECC_ONLY               4       /* only enable ciper 0xC027 */
#define TLS_TUYA_ECC_ONLY_NOSTRIP       5       /* only enable ciper 0xC027, but enable most of mbed-tls configs */
#define TLS_TUYA_ECC_CLIENT_AUTH        6       /* TLS Client Authentication and Server Authentication mode */

#define TUYA_SL_0           0       /* security level 0,Applies to: Resource-constrained devices;Feature: Can only access Tuya Cloud */
#define TUYA_SL_1           1       /* security level 1,Applies to: Resource-constrained devices;Feature: One-way authentication */
#define TUYA_SL_2           2       /* security level 2,Applies to: Resource-rich equipment;Feature: Two-way authentication */
#define TUYA_SL_3           3       /* security level 3,Applies to: Resource-rich equipment;Feature: Two-way authentication,Devices use security chips to protect sensitive information */
#define TUYA_MODIFY_MBEDTLS             1

#define TUYA_OPERATOR_DISABLE           0x0
#define TUYA_OPERATOR_CUCC              0x01
#define TUYA_OPERATOR_CTCC              0x02
#define TUYA_OPERATOR_CMCC              0x04
#define TUYA_OPERATOR_CMCC_ANDLINK      0x08
#define TUYA_OPERATOR_ALL               (TUYA_OPERATOR_CUCC | TUYA_OPERATOR_CTCC | TUYA_OPERATOR_CMCC)


#define TUYA_LAN_V31              0x1
#define TUYA_LAN_V32              0x2
#define TUYA_LAN_V33              0x4
#define TUYA_LAN_V35              0x8
#define TUYA_LAN_ALL              (TUYA_LAN_V31 | TUYA_LAN_V32 | TUYA_LAN_V33 | TUYA_LAN_V35)

#define TUYA_MQ_V21               0x1
#define TUYA_MQ_V22               0x2
#define TUYA_MQ_V23               0x4
#define TUYA_MQ_ALL               (TUYA_MQ_V21 | TUYA_MQ_V22 | TUYA_MQ_V23)

/* custom settings */
#define BUILD_DATE           "2024_12_24"
#define BUILD_TIME           "14_17_34"
#define GIT_USER             "ci_manage"
#define IOT_SDK_VER          "0.0.4"
#define SDK_BETA_VER         "beta.13"
#define TARGET_PLATFORM      "aarch64-openwrt-linux-gnu-gcc-10.3.0"
#define PROJECT_NAME         "tuyaos_robot_sweeper_sdk"
#define SDK_ID               "tuyaos_robot_sweeper_sdk_0.0.4-beta.13_aarch64-openwrt-linux-gnu-gcc-10.3.0_linux_wifi_ble_stream_0.0.2"
#define KERNEL_VER	        ""
// Generated by Tuya (https://www.tuya.com)
// 基于项目 tuyaos_robot_sweeper_sdk (1.0.0)
// 基于工具链 x86_64 (0.0.25)


// CONFIG_ENABLE_MONITOR_WORK is not set


#define ENABLE_LOCAL_STORAGE 1

#define STACK_SIZE_LOCAL_STORAGE 8192

#define STACK_SIZE_LOCAL_STORAGE_MONITOR 8192

#define STACK_SIZE_LOCAL_STORAGE_PLAYBACK 262144

#define STACK_SIZE_LOCAL_STORAGE_DOWNLOAD 524288

#define STACK_SIZE_ALBUM_CAPTURE_IMAGE 131072

#define STACK_SIZE_ALBUM_DOWNLOAD 524288


#define ENABLE_AI_DETECT 1

#define ENABLE_FACE_DETECTION 1


// CONFIG_LOW_POWER_ENABLE is not set

// CONFIG_DB_EN_OLD_VER is not set

// CONFIG_ENABLE_IPC_GW_BOTH is not set

#define STACK_SIZE_IPC_ALGO_MODEL 16384

#define STACK_SIZE_IPC_EVENT 24576

// CONFIG_HARDWARE_INFO_CHECK is not set


#define STACK_SIZE_VIDEO_MSG 131072


#define ENABLE_REMOTE_EVENT 1

#define ENABLE_CLOUD_STORAGE 1

#define STACK_SIZE_CLOUD_STORAGE 65536


#define SUPPORT_RTSP 1

#define STACK_SIZE_IPC_RTSP_SERVER 16384

#define STACK_SIZE_IPC_RTSP_CLIENT 16384

#define STACK_SIZE_IPC_RTP_SERVER 16384

#define STACK_SIZE_IPC_RTSP_TIMER 16384


#define TUYA_P2P 4

#define ENABLE_MQTT_P2P 1

#define ENABLE_P2P_MODULE 1


#define ENABLE_IPC_P2P 1

// CONFIG_ENABLE_XVR_P2P is not set

#define STACK_SIZE_P2P_MEDIA_SEND 65536

#define STACK_SIZE_P2P_MEDIA_RECV 65536

#define STACK_SIZE_P2P_CMD_SEND 65536

#define STACK_SIZE_P2P_CMD_RECV 65536

#define STACK_SIZE_P2P_DETECT 65536

#define STACK_SIZE_P2P_LISTEN 131072

#define STACK_SIZE_IPC_UPNP 65536


#define ENABLE_CLOUD_STREAM 1

#define STACK_SIZE_CLOUD_STREAM 8192


#define ENABLE_MQTT_WEBRTC 1

#define STACK_SIZE_WEBRTC_PLAYBACK_QUERY 262144

#define STACK_SIZE_WEBRTC 131072

#define STACK_SIZE_WEBRTC_DETECH 65536


#define IPC_CHANNEL_NUM 1


#define SUPPORT_RTP 1


#define ENABLE_VIDEO_MSG 1

#define ENABLE_IPC 1


#define ENABLE_TMM_LINK 1


#define ENABLE_UPNP 1

#define STACK_SIZE_MID_UPNP 65536


#define ENABLE_CLOUD_OPERATION 1

// CONFIG_NEW_HTTP_MANAGER is not set


// CONFIG_ENABLE_XVR_MODE is not set

#define DISABLE_MBEDTLS_SRTP 0


#define ENABLE_LOG 1

#define STACK_SIZE_TIMERQ 4096

// CONFIG_ENABLE_PRINTF_CHECK is not set

#define ENABLE_SET_SOCK_OPT 1

// CONFIG_ENABLE_NETWORK_OPTIONS is not set


#define STACK_SIZE_HEALTH_MONITOR 1536

#define ENABLE_OSS_COMPILE_WARNINGS 1


#define KV_PROTECTED_ENABLE 1

#define ENABLE_KV_FILE 1

#define ENABLE_KV_PROTECTED 1

// CONFIG_ENABLE_KV_KEY_SEED is not set


// CONFIG_ENABLE_UF_HUGE_FILE is not set

#define MAX_LENGTH_OF_FILE_FULL_PATH 128


#define EVENT_NAME_MAX_LEN 16

#define ENABLE_TUYA_LAN 1

#define TUYA_LAN_VERSION 0x8

#define ENABLE_LAN_ENCRYPTION 1

// CONFIG_ENABLE_RTSP_SERVER is not set

#define STACK_SIZE_LAN 4096


#define STACK_SIZE_UPGRADE 4096

// CONFIG_ENABLE_FIRMWARE_SIGN_TUYA is not set

// CONFIG_ENABLE_FIRMWARE_SIGN_USER is not set


// CONFIG_ENABLE_PRODUCT_AUTOTEST is not set

#define STACK_SIZE_MF_TEST 5120

// CONFIG_ENABLE_LWIP is not set

#define ENABLE_BT_SERVICE 1

#define ENABLE_BT_NETCFG 1

// CONFIG_ENABLE_BT_ACTIVE is not set

// CONFIG_ENABLE_BT_CTRL is not set

#define BT_ADV_INTERVAL_MIN 30

#define BT_ADV_INTERVAL_MAX 60

// CONFIG_ENABLE_BT_REMOTE_CTRL is not set

#define BT_REMOTE_CTRL_SCAN_INTERVAL 30

#define BT_REMOTE_CTRL_SCAN_WINDOWS 10

// CONFIG_ENABLE_BT_TIMER is not set

// CONFIG_ENABLE_BT_MF is not set

// CONFIG_ENABLE_MF_BY_BT is not set

// CONFIG_ENABLE_BT_VIRTUAL is not set

#define TUYA_BLE_VER 4

#define ENABLE_TAL_BLUETOOTH 1

#define TAL_BLUETOOTH 1

#define TAL_BLE_SERVICE_VERSION 2

// CONFIG_ENABLE_TUYA_BLE_STACK is not set

#define TY_BT_MOD 1

#define BT_NETCFG 1

// CONFIG_BT_ACTIVE is not set

// CONFIG_BT_CTRL is not set

// CONFIG_BT_REMOTE_CTRL is not set

// CONFIG_BT_TIMER is not set

// CONFIG_MF_BT is not set

// CONFIG_BT_VIRTUAL is not set

#define WIFI_GW 1

// CONFIG_TUYA_HOSTAPD_SUPPORT is not set

#define ENABLE_WIFI_SERVICE 1

// CONFIG_ENABLE_WIFI_EZ is not set

// CONFIG_ENABLE_WIFI_FFS is not set

// CONFIG_ENABLE_WIFI_PEGASUS is not set

// CONFIG_ENABLE_WIFI_PROTECT is not set

// CONFIG_ENABLE_WIFI_FFC is not set

// CONFIG_ENABLE_HOSTAPD is not set

#define ENABLE_STATION_AP_MODE 0

#define WIFI_AP_BUF_SIZE 10240

#define QRCODE_ACTIVE_MODE 1

#define ENABLE_TUYA_CLOUD 1

// CONFIG_ENABLE_ASTRO_TIMER is not set

// CONFIG_ENABLE_CALENDAR_TIMER is not set

#define ENABLE_QRCODE_ACTIVE 1

// CONFIG_ENABLE_DEVICE_DATA_COLLECT is not set

// CONFIG_ENABLE_LOCAL_KEY_UPDATE is not set

// CONFIG_ENABLE_COMMUNICATE_PRIORITY is not set


// CONFIG_ENABLE_BLACK_WHITE_LIST is not set

#define MAX_SECURITY_LEVEL 1

#define TUYA_SECURITY_LEVEL 1

// CONFIG_ENABLE_MBEDTLS_DEBUG is not set

#define ENABLE_CUSTOM_CONFIG 1

// CONFIG_ENABLE_MBEDTLS_SSL_MAX_FRAGMENT_LENGTH is not set

#define ENABLE_MBEDTLS_CLIENT_SSL_SESSION_TICKETS 1

#define ENABLE_MBEDTLS_SSL_MAX_CONTENT_LEN 16384

// CONFIG_ENABLE_MBEDTLS_HAVE_TIME is not set

// CONFIG_ENABLE_MBEDTLS_FS_IO is not set

// CONFIG_ENABLE_MBEDTLS_NET_C is not set

// CONFIG_ENABLE_MBEDTLS_HAVEGE_C is not set

#define ENABLE_MBEDTLS_TIMING_C 1

#define ENABLE_MBEDTLS_ARC4_C 1


// CONFIG_ENABLE_MBEDTLS_DES_C is not set

// CONFIG_ENABLE_MBEDTLS_CAMELLIA_C is not set

// CONFIG_ENABLE_MBEDTLS_BLOWFISH_C is not set

// CONFIG_ENABLE_MBEDTLS_XTEA_C is not set

#define ENABLE_MBEDTLS_CCM_C 1

#define ENABLE_MBEDTLS_GCM_C 1


// CONFIG_ENABLE_MBEDTLS_RSA_C is not set

#define ENABLE_MBEDTLS_DHM_C 1

#define ENABLE_MBEDTLS_ECP_C 1

// CONFIG_ENABLE_MBEDTLS_PK_C is not set

#define ENABLE_MBEDTLS_ECDH_C 1

#define ENABLE_MBEDTLS_ECDSA_C 1

#define ENABLE_MBEDTLS_ECP_DP_SECP192R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_SECP224R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_SECP256R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_SECP384R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_SECP521R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_SECP192K1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_SECP224K1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_SECP256K1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_BP256R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_BP384R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_BP512R1_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_CURVE25519_ENABLED 1

#define ENABLE_MBEDTLS_ECP_DP_CURVE448_ENABLED 1

#define ENABLE_MBEDTLS_ECP_NIST_OPTIM 1

// CONFIG_ENABLE_MBEDTLS_ECP_RESTARTABLE is not set


// CONFIG_ENABLE_MBEDTLS_X509_CRT is not set

#define ENABLE_MBEDTLS_CHACHA20_C 1

// CONFIG_ENABLE_MBEDTLS_CHACHAPOLY_C is not set

// CONFIG_ENABLE_MBEDTLS_CMAC_C is not set

// CONFIG_ENABLE_MBEDTLS_HMAC_DRBG_C is not set

#define ENABLE_MBEDTLS_POLY1305_C 1

// CONFIG_ENABLE_MBEDTLS_RIPEMD160_C is not set

#define MBEDTLS_TLS_MODE ENABLE_MBEDTLS_TLS_CLIENT_ONLY

#define ENABLE_MBEDTLS_TLS_SERVER_AND_CLIENT 1

// CONFIG_ENABLE_MBEDTLS_TLS_SERVER_ONLY is not set

// CONFIG_ENABLE_MBEDTLS_TLS_CLIENT_ONLY is not set

// CONFIG_ENABLE_MBEDTLS_TLS_DISABLED is not set

#define ENABLE_MBEDTLS_SSL_SRV_C 1

#define ENABLE_MBEDTLS_SSL_CLI_C 1

#define MBEDTLS_TLS_ENABLED 1

#define ENABLE_MBEDTLS_SSL_PROTO_TLS1 1

#define ENABLE_MBEDTLS_SSL_PROTO_TLS1_1 1

#define ENABLE_MBEDTLS_SSL_PROTO_TLS1_2 1

#define ENABLE_MBEDTLS_SSL_PROTO_DTLS 1

#define ENABLE_MBEDTLS_SSL_DTLS_HELLO_VERIFY 1

#define ENABLE_MBEDTLS_SSL_DTLS_SRTP 1


#define ENABLE_MBEDTLS_PSK_MODES 1

#define ENABLE_MBEDTLS_KEY_EXCHANGE_PSK 1

// CONFIG_ENABLE_MBEDTLS_KEY_EXCHANGE_DHE_PSK is not set

#define ENABLE_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK 1

// CONFIG_ENABLE_MBEDTLS_KEY_EXCHANGE_RSA_PSK is not set

// CONFIG_ENABLE_MBEDTLS_KEY_EXCHANGE_RSA is not set

// CONFIG_ENABLE_MBEDTLS_KEY_EXCHANGE_DHE_RSA is not set

#define ENABLE_MBEDTLS_KEY_EXCHANGE_ELLIPTIC_CURVE 1

// CONFIG_ENABLE_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA is not set

// CONFIG_ENABLE_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA is not set

#define ENABLE_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA 1

#define ENABLE_MBEDTLS_KEY_EXCHANGE_ECDH_RSA 1


#define HTTP_MAX_REQ_RESP_HDR_SIZE 1024

#define MAX_HTTP_SESSION_NUM 16


#define TUYA_MQ_VERSION 0x4

#define STACK_SIZE_MQTT_CLIENT 8192

#define QOS1_PUBACK_TIMEOUT 3

#define QOS1_PUBLISH_SLOT_CNT 8

#define QOS1_PUBACK_TIMEOUT_LIMIT 3

#define MAX_RANDOM_DELAY_BEFORE_RECONNECT 300

#define MQTT_HEATBEAT_INTERVAL 60

#define MQTT_RECV_BUF_SIZE 512

#define MQTT_MAX_MESSGAE_SIZE 4096

#define MQTT_RECV_SELECT_TIMEOUT 3000

// CONFIG_ENABLE_MQTT_QOS1_RESEND is not set

// CONFIG_TUYA_IOT_DEBUG is not set


#define STACK_SIZE_WORK_QUEUE 5120

#define MAX_NODE_NUM_WORK_QUEUE 100

#define STACK_SIZE_MSG_QUEUE 4096

#define MAX_NODE_NUM_MSG_QUEUE 100

// CONFIG_ENABLE_IOT_DEBUG is not set

#define TUYA_REDUCE_BIN 0

// CONFIG_ENABLE_WEBSOCKET is not set


//
// VENDOR
//

#define TUYA_DEFCONFIG ""

#define TUYA_PRODUCT_RELEASE_NAME "x86_64"

// end of VENDOR
// Generated by Tuya (https://www.tuya.com)
// 基于项目 tuyaos-auto-tester (0.0.1-beta.61)
// 基于工具链 fh8636-rtos (0.0.1-beta.1)

// CONFIG_ENABLE_LWIP is not set

// CONFIG_TUYA_HOSTAPD_SUPPORT is not set

// CONFIG_ENABLE_HOSTAPD is not set


#define OPERATING_SYSTEM 100

#define LITTLE_END 1

#define ENABLE_FILE_SYSTEM 1

#define ENABLE_RTC 1

#define ENABLE_ADC 1

#define ENABLE_UART 1

// CONFIG_ENABLE_UART_INTERRUPT is not set

#define ENABLE_WATCHDOG 1

#define ENABLE_FLASH 1

#define TUYA_FLASH_TYPE_MAX_PARTITION_NUM 10

#define ENABLE_PWM 1

#define ENABLE_GPIO 1

#define ENABLE_I2C 1

#define ENABLE_SPI 1

// CONFIG_ENABLE_ZIGBEE is not set

#define ENABLE_WIFI 1

#define ENABLE_WIRED 1

#define ENABLE_TIMER 1

#define ENABLE_BLUETOOTH 1

// CONFIG_ENABLE_BLUETOOTH_MESH is not set

// CONFIG_ENABLE_HCI is not set

#define ENABLE_DISPLAY 1

#define ENABLE_MEDIA 1

#define ENABLE_PM 1

// CONFIG_GPRS_GW is not set

// CONFIG_TUYA_CAT1 is not set

// CONFIG_ENABLE_CELLULAR is not set

#define ENABLE_STORAGE 1

// CONFIG_ENABLE_NB is not set

#define ENABLE_REGISTER 1

#define ENABLE_DAC 1

#define ENABLE_I2S 1


// CONFIG_ENABLE_PLATFORM_AES is not set

// CONFIG_ENABLE_PLATFORM_SHA256 is not set

// CONFIG_ENABLE_PLATFORM_MD5 is not set

// CONFIG_ENABLE_PLATFORM_SHA1 is not set

// CONFIG_ENABLE_PLATFORM_RSA is not set

// CONFIG_ENABLE_PLATFORM_ECC is not set

// CONFIG_ENABLE_WAKEUP is not set
#ifndef TUYA_LAN_VERSION
#define TUYA_LAN_VERSION TUYA_LAN_V35
#endif

#ifndef TUYA_MQ_VERSION
#define TUYA_MQ_VERSION TUYA_MQ_V23
#endif

#ifndef ENABLE_DEVICE_DATA_COLLECT
#define ENABLE_DEVICE_DATA_COLLECT 0
#endif

#ifndef TUYA_WEAK_ATTRIBUTE
#define TUYA_WEAK_ATTRIBUTE __attribute__ ((weak))
#endif

#ifndef KERNEL_VER
#define KERNEL_VER "9.9.9"
#endif

/* custom settings */

#ifdef __cplusplus
}
#endif


#endif
