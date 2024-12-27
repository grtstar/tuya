#ifndef __TUYA_P2P_MODULE_UPNP_H__
#define __TUYA_P2P_MODULE_UPNP_H__

#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C"
{
#endif
typedef enum{
	UPNP_MODULE_READY = 0,
	UPNP_MODULE_RUNNING,
	UPNP_MODULE_DONE
}tuya_upnp_module_status;

typedef enum{
	UPNP_DEC = 0,
	UPNP_STRING
}tuya_upnp_log_seq_type;

typedef enum {
    UPNP_PORT_PROTOCOL_UDP,
    UPNP_PORT_PROTOCOL_TCP,
    UPNP_PORT_PROTOCOL_MAX = 0XFFFFFFFF,
}tuya_upnp_port_protocol;

#define TUYA_UPNP_ADDRESS_LENGTH 16     //IP地址长度
#define TUYA_UPNP_PROTOCOL_LENGTH 4     //协议长度 tcp or udp

typedef struct tuya_upnp_port_link
{
    char protocol[TUYA_UPNP_PROTOCOL_LENGTH];
    int external_port;
    char remount_host[TUYA_UPNP_ADDRESS_LENGTH];
    int internal_port;
    char internal_client[TUYA_UPNP_ADDRESS_LENGTH];
    int route_level;
    int index;
    struct tuya_upnp_port_link* next;
}tuya_upnp_port_link;


typedef int (*TUYA_UPNP_CONFIG_REPORT_CB)(char *config);

typedef void (*TUYA_UPNP_LOG_SEQ_CB)(tuya_upnp_log_seq_type type, unsigned char id, void* log);

typedef struct tuya_upnp_module_cb{
	TUYA_UPNP_CONFIG_REPORT_CB report_cb;
	TUYA_UPNP_LOG_SEQ_CB log_seq_cb;
}tuya_upnp_module_cb;

////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************
*  Function: tuya_upnp_module_init
*  Desc:     UPNP模块初始化，注册相关回调
*  Input:    session_num，会话数.
*  Input:    request，云端配置信息.
*  Input:    module_cb，注册的回调.
*  Return:   0代表成功，<0代表失败.
***********************************************************/
int tuya_upnp_module_init(int session_num, unsigned char* request, tuya_upnp_module_cb module_cb);

/***********************************************************
*  Function: tuya_upnp_module_deinit
*  Desc:     UPNP模块反初始化，释放线程，清除资源
*  Input:    void.
*  Return:   void.
***********************************************************/
void tuya_upnp_module_deinit(void);

/***********************************************************
*  Function: tuya_upnp_module_modify
*  Desc:     支持外部修改upnp功能模块接口.此接口还无法实现下发新的修改配置来生成新的UPnP端口
*  Input:    session_num，会话数.
*  Input:    request，云端配置信息.
*  Return:   0代表成功，<0代表失败.
***********************************************************/
int tuya_upnp_module_modify(int session_num, unsigned char* request);

/***********************************************************
*  Function: tuya_upnp_module_request_port
*  Desc:     申请使用对应端口协议的端口、外网地址、外网端口信息
*  Input:    protocol，整型协议.
*  Input:    local_port，本地端口地址.
*  Input:    ext_address，外部地址.
*  Input:    ext_port，外部端口地址.
*  Return:   0代表成功，<0代表失败.
***********************************************************/
int tuya_upnp_module_request_port(tuya_upnp_port_protocol protocol, int *local_port, char *ext_address, int *ext_port);

/***********************************************************
*  Function: tuya_upnp_module_release_port
*  Desc:     释放申请使用对应端口协议的端口
*  Input:    protocol，整型协议.
*  Input:    local_port，本地端口.
*  Return:   0代表成功，<0代表失败.
***********************************************************/
int tuya_upnp_module_release_port(tuya_upnp_port_protocol protocol, int local_port);

/***********************************************************
*  Function: tuya_upnp_module_bind_result
*  Desc:     端口绑定设置
*  Input:    protocol，整型协议.
*  Input:    local_port，本地端口值.
*  Input:    error_code，错误码.
*  Return:   0代表成功.
***********************************************************/
int tuya_upnp_module_bind_result(tuya_upnp_port_protocol protocol, int local_port, int error_code);

/***********************************************************
*  Function: tuya_upnp_module_request_port_list
*  Desc:     获取指定的协议端口相关链表
*  Input:    protocol，整型协议.
*  Input:    port，端口号.
*  Return:   tuya_upnp_port_link,端口结构体, NULL代表失败
***********************************************************/
tuya_upnp_port_link* tuya_upnp_module_request_port_list(tuya_upnp_port_protocol protocol,unsigned int port);

/***********************************************************
*  Function: tuya_upnp_module_get_status
*  Desc:     获取UPNP模块状态
*  Input:    void.
*  Return:   模块状态.
***********************************************************/
tuya_upnp_module_status tuya_upnp_module_get_status(void);

/***********************************************************
*  Function: tuya_get_upnp_capability
*  Desc:     UPNP模块反初始化，释放线程，清除资源
*  Input:    NULL
*  Return:   返回UPnP组件的能力集.
***********************************************************/
unsigned int tuya_get_upnp_capability(void);

////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
}
#endif
#endif
