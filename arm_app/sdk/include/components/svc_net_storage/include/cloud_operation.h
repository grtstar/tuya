/**
* @file      cloud_operation.h
* @brief     cloud operation api
* @version   1.0
* @date      2021-11-17
*
* copyright  Copyright (c) tuya.inc 2021
*/
#ifndef __CLOUD_OPERATION_H__
#define __CLOUD_OPERATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ty_cJSON.h"
#include "tuya_cloud_types.h"
#include "tuya_cloud_com_defs.h"

#define VALUE_LEN       128
#define TOKEN_LEN_MAX   2048
#define FILE_NAME_LEN   64
#define URL_MAX_LEN 256
#define CA_TYPE_CLOUD_STROAGE 1
#define CA_TYPE_CLOUD_NOTIFICATION 2
#define CA_TYPE_ECHO_SHOW 3
#define CA_TYPE_CHROMECAST 4
#define CA_TYPE_PRESET 5
#define CA_TYPE_AI_DETECT 6
#define CA_TYPE_AI_SERVER   7
#define CA_TYPE_AI_FACE   8
#define MAX_CA_NUM  9

/**
* @enum MSG_TYPE_E
* @brief message type
*/
typedef enum {
    E_MSG_TXT = 0,
    E_MSG_PICTURE = 1,
    E_MSG_AUDIO = 2,
    E_MSG_VIDEO = 4,
    E_MSG_INVALID = 0xFFFF
}MSG_TYPE_E;

/**
* @enum    CFG_MODULE_E
* @brief   cloud config module
*/
typedef enum {
    CFG_MODULE_NOTIFY,
    CFG_MODULE_CLOUD_STG,
    CFG_MODULE_MAX
}CFG_MODULE_E;

#define AI_ALG_NAME_LEN 16
typedef struct {
    INT_T id;
    CHAR_T name[AI_ALG_NAME_LEN];
}AI_ALG_T;

typedef struct {
    INT_T cnt;
    AI_ALG_T ai_alg[0];
}AI_ALG_INFO_T;

/**
* @struct CLOUD_CONFIG_T
* @brief  config for cloud operation
*/
typedef struct {
    CHAR_T provider[VALUE_LEN];
    CHAR_T ak[VALUE_LEN];
    CHAR_T sk[VALUE_LEN];
    CHAR_T expiration[VALUE_LEN];
    CHAR_T bucket[VALUE_LEN];
    CHAR_T endpoint[VALUE_LEN];
    CHAR_T region[VALUE_LEN];
    CHAR_T content_type[VALUE_LEN];
    CHAR_T secret_key[VALUE_LEN];

    CHAR_T media_path[VALUE_LEN];
    CHAR_T log_path[VALUE_LEN];
    CHAR_T detect_path[VALUE_LEN];
    CHAR_T common_path[VALUE_LEN];
    CHAR_T preset_path[VALUE_LEN];
    CHAR_T unify_path[VALUE_LEN];
    CHAR_T ai_detect_path[VALUE_LEN];
    CHAR_T ai_face_path[VALUE_LEN];

    CHAR_T object[VALUE_LEN*2];
    CHAR_T token[TOKEN_LEN_MAX];
    CHAR_T store_mode[VALUE_LEN];
    CHAR_T mode[VALUE_LEN];
    INT_T  msg_type;
    UINT_T position;
    INT_T  post_mode;
    UINT_T video_time;
    UINT_T mtu;    
    UINT_T life_cycle;
    INT_T  security_level;
    AI_ALG_INFO_T *ai_alg_infos;
}CLOUD_CONFIG_T;

/**
* @enum CLOUD_UPLOAD_PATH_E
* @brief cloud upload path
*/
typedef enum {
    UPLOAD_PATH_MEDIA = 0,
    UPLOAD_PATH_LOG,
    UPLOAD_PATH_DETECT,
    UPLOAD_PATH_COMMON,
    UPLOAD_PATH_PRESET,
    UPLOAD_PATH_FACE,
    UPLOAD_PATH_UNIFY,
    UPLOAD_PATH_AI_DETECT,
    UPLOAD_PATH_MAX,
}CLOUD_UPLOAD_PATH_E;

/**
* @enum CLOUD_CFG_TYPE_E
* @brief cloud config type
*/
typedef enum {
    E_CLOUD_CFG_MEDIA = 0,
    E_CLOUD_CFG_DETECT,
    E_CLOUD_CFG_COMON,
    E_CLOUD_CFG_FACE,
    E_CLOUD_CFG_UINFY,
    E_CLOUD_CFG_AI_DETECT,
    E_CLOUD_CFG_MAX,
}CLOUD_CFG_TYPE_E;

/**
* @enum CLOUD_PUT_METHOD_E
* @brief cloud put method, such as http/https
*/
typedef enum {
    CLOUD_PUT_METHOD_HTTPS,
    CLOUD_PUT_METHOD_HTTP,
}CLOUD_PUT_METHOD_E;

/**
* @struct STG_RESTRICTION_CFG_T
* @brief cloud restriction config
*/
typedef struct {
    UINT_T msg_v_duration;
    UINT_T msg_v_interval;
    UINT_T msg_a_duration;
    UINT_T msg_a_interval;
    UINT_T stg_mtu;
}STG_RESTRICTION_CFG_T;

/**
* @brief    cloud operation init
* @return   OPERATE_RET
* - OPRT_OK init success
* - Others  init failed
*/
OPERATE_RET cloud_operation_init(VOID);

/**
* @brief      set log path callback
* @param[in]  cb    log path callback
* @return     VOID
*/
VOID cloud_operation_set_log_path_cb(GW_APP_LOG_PATH_CB cb);

/**
* @brief      get log upload ability
* @return     BOOL_T
* - TRUE      support log upload
* - FALSE     not support log upload
*/
BOOL_T tuya_cloud_get_log_upload_ability(VOID);

/**
* @brief        get common storage config v2
* @param[in]    file_size    file_size
* @param[inout] config       common storage config v2
* @return       OPERATE_RET
* - OPRT_OK     init success
* - Others      init failed
*/
OPERATE_RET tuya_cloud_get_common_storage_config_v2(IN CONST UINT_T file_size, INOUT CLOUD_CONFIG_T *config);

/**
* @brief        set preset storage config from result
* @param[inout] config    preset storage config
* @param[in]    result    result
* @return       OPERATE_RET
* - OPRT_OK     init success
* - Others      init failed
*/
OPERATE_RET tuya_cloud_set_preset_storage_config(INOUT CLOUD_CONFIG_T *config, IN ty_cJSON *result);

/**
* @brief        get ai face storage config
* @param[inout] config    ai face storage config
* @return       OPERATE_RET
* - OPRT_OK     init success
* - Others      init failed
*/
OPERATE_RET tuya_cloud_get_ai_face_storage_config(INOUT CLOUD_CONFIG_T *config);

/**
* @brief      get cloud config by config type
* @param[in]  config    cloud config pointer
* @param[in]  type      cloud config type
* @param[in]  update    TRUE: update config directly from cloud, this will cost time and performance FALSE: get config from SDK buffer, SDK can update config in rotation
* @return     OPERATE_RET
* - OPRT_OK   get cloud config success
* - Others    get cloud config failed
*/
OPERATE_RET tuya_cloud_get_config_by_type(INOUT CLOUD_CONFIG_T *config, IN CONST CLOUD_CFG_TYPE_E type, BOOL_T update);

/**
* @brief      require ca certificate by url
* @param[in]  p_url         url
* @param[in]  ca_type       ca certificate type
* @return     OPERATE_RET
* - OPRT_OK   require ca certificate success
* - Others    require ca certificate failed
*/
OPERATE_RET tuya_cloud_require_new_ca(IN CONST CHAR_T *p_url,IN UINT_T ca_type);

/**
* @brief      put file to cloud
* @param[in]  config        cloud config
* @param[in]  method        put method (http/https)
* @param[in]  path          upload path
* @param[in]  file          file name
* @param[in]  content_type  content type
* @param[in]  data          upload data pointer
* @param[in]  len           upload data length
* @return     OPERATE_RET
* - OPRT_OK   put file to cloud success
* - Others    put file to cloud failed
*/
OPERATE_RET tuya_cloud_put_data_to_file(IN CLOUD_CONFIG_T *config,
                                                    IN CHAR_T *method,
                                                    IN CLOUD_UPLOAD_PATH_E path,
                                                    IN CHAR_T *file,
                                                    IN CHAR_T *content_type,
                                                    IN CHAR_T *data,
                                                    IN INT_T len);

/**
* @brief      get content length callback
* @param[in]  pri_data      private data
* @return     content length
* - > 0       valid content length
* - <= 0      invalid content length
*/
typedef UINT_T (*TUYA_CLOUD_GET_CONTENT_LEN_CB)(VOID *pri_data);

/**
* @brief      callback before read content
* @param[in]  pri_data      private data
* @param[out] p_malloc_buffer_size     expected malloc buffer size
* @return     result
* - == 0      success
* - != 0      failed
*/
typedef INT_T (*TUYA_CLOUD_BEFORE_READ_CONTENT_CB)(VOID *pri_data, OUT UINT_T *p_malloc_buffer_size);

/**
* @brief      callback for read content
* @param[in]  p_buffer    read content buffer, put data in this buffer
* @param[in]  buf_size    buffer size
* @param[in]  pri_data    private data
* @return     data size in p_buffer
*/
typedef INT_T (*TUYA_CLOUD_READ_CONTENT_CB)(BYTE_T *p_buffer, INT_T buf_size, VOID *pri_data);

/**
* @brief      callback after read content
* @param[in]  pri_data    private_data
* @return     result
* - == 0      success
* - != 0      failed
*/
typedef INT_T (*TUYA_CLOUD_AFTER_READ_CONTENT_CB)(VOID *pri_data);

/**
* @struct CLOUD_CUSTOM_CONTENT_CTX_T
* @brief custom content upload ctx
*/
typedef struct {
    TUYA_CLOUD_GET_CONTENT_LEN_CB get_content_len_cb;
    TUYA_CLOUD_BEFORE_READ_CONTENT_CB before_read_cb;
    TUYA_CLOUD_READ_CONTENT_CB read_content_cb;
    TUYA_CLOUD_AFTER_READ_CONTENT_CB after_read_cb;
    VOID *pri_data;
}CLOUD_CUSTOM_CONTENT_CTX_T;

/**
* @brief      put custom data to cloud, used in case when file size is unknown
* @param[in]  config        cloud config
* @param[in]  method        put method (http/https)
* @param[in]  path          upload path
* @param[in]  file          file name
* @param[in]  content_type  content type
* @param[in]  p_ctx         custom content ctx
* @return     OPERATE_RET
* - OPRT_OK   put custom data to cloud success
* - Others    put custom data to cloud failed
*/
OPERATE_RET tuya_cloud_put_custom_data(IN CLOUD_CONFIG_T *config,
                                                   IN CHAR_T *method,
                                                   IN CLOUD_UPLOAD_PATH_E path,
                                                   IN CHAR_T *file,
                                                   IN CHAR_T *content_type,
                                                   IN CLOUD_CUSTOM_CONTENT_CTX_T *p_ctx);

/**
* @brief      post file to cloud
* @param[in]  config            cloud config
* @param[in]  method            put method (http/https)
* @param[in]  path              upload path
* @param[in]  file_local_name   file name in local
* @param[in]  file_upload_name  file name in cloud
* @param[in]  content_type      content type
* @param[out] log_suf
* @return     OPERATE_RET
* - OPRT_OK   put custom data to cloud success
* - Others    put custom data to cloud failed
*/
OPERATE_RET tuya_cloud_post_file_to_oss(IN CLOUD_CONFIG_T *config,
                                                    IN CHAR_T *method,
                                                    IN CLOUD_UPLOAD_PATH_E path,
                                                    IN CHAR_T *file_local_name,
                                                    IN CHAR_T *file_upload_name,
                                                    IN CHAR_T *content_type,
                                                    OUT CHAR_T *logSuf);

/**
* @brief      upload file to cloud complete
* @param[in]  p_filetype    upload file type
* @param[in]  p_extend      extend string to cloud
* @param[in]  p_bucket      bucket
* @param[in]  p_file        upload file name
* @return     OPERATE_RET
* - OPRT_OK   success
* - Others    failed
*/
OPERATE_RET tuya_cloud_upload_file_complete(IN CONST CHAR_T *p_filetype,
                                                          IN CONST CHAR_T *p_extend,
                                                          IN CONST CHAR_T *p_bucket,
                                                          IN CONST CHAR_T *p_file);

/**
* @brief      broadcast update status for cloud config
* @reutrn     VOID
*/
VOID tuya_cloud_cfg_broadcast_update_status(VOID);

/**
* @brief      set cloud config upate by module
* @param[in]  module \n
* @ref        CFG_MODULE_NOTIFY    notify module \n
* @ref        CFG_MODULE_CLOUD_STG cloud storage module
* @return     VOID
*/
VOID tuya_cloud_cfg_set_update(CFG_MODULE_E module);

/**
* @brief      check whether cloud config need update by module
* @ref        CFG_MODULE_NOTIFY    notify module \n
* @ref        CFG_MODULE_CLOUD_STG cloud storage module
* @return     VOID
*/
BOOL_T tuya_cloud_cfg_get_update(CFG_MODULE_E module);

/**
* @brief        get restriction config
* @param[inout] p_restriction    restriction config pointer
* @return       OPERATE_RET
* - OPRT_OK     success
* - Others      failed
*/
OPERATE_RET tuya_cloud_get_spec_restriction(INOUT STG_RESTRICTION_CFG_T *p_restriction);

/**
* @brief        put multimedia file to cloud
* @param[in]    config        cloud config
* @param[in]    cfg_type      cloud config type
* @param[in]    put_method    put method (http/https)
* @param[in]    file          file name
* @param[in]    content_type  content type
* @param[in]    data          upload data pointer
* @param[in]    len           upload data length
* @return       OPERATE_RET
* - OPRT_OK     put file to cloud success
* - Others      put file to cloud failed
*/
OPERATE_RET tuya_imm_cloud_put_data(IN CLOUD_CONFIG_T *config,
                                              IN CLOUD_CFG_TYPE_E cfg_type, 
                                              IN CLOUD_PUT_METHOD_E put_method, 
                                              IN CHAR_T *file,
                                              IN CHAR_T *content_type,
                                              IN CHAR_T *data, 
                                              IN INT_T len);

/**
* @brief        create put handle to cloud
* @param[in]    config        cloud config
* @param[in]    cfg_type      cloud config type
* @param[in]    put_method    put method (http/https)
* @param[in]    file          file name
* @param[in]    content_type  content type
* @param[inout] p_handle      put handle
* @return       OPERATE_RET
* - OPRT_OK     create handle success
* - Others      create handle failed
*/
OPERATE_RET tuya_imm_cloud_put_fragment_data_start(IN CLOUD_CONFIG_T *config,
                                                                    IN CLOUD_CFG_TYPE_E cfg_type,
                                                                    IN CLOUD_PUT_METHOD_E put_method, 
                                                                    IN CHAR_T *file,
                                                                    IN CHAR_T *content_type,
                                                                    IN INT_T content_len,
                                                                    INOUT PVOID_T *p_handle);

/**
* @brief        put multimedia data to cloud
* @param[in]    p_handle    put handle, created by tuya_imm_cloud_put_fragment_data_start api
* @param[in]    data        data pointer
* @param[in]    len         data length
* @return       OPERATE_RET
* - OPRT_OK     put data success
* - Others      put data failed
*/
OPERATE_RET tuya_imm_cloud_put_fragment_data_process(IN PVOID_T *p_handle, IN CONST CHAR_T *data, IN CONST INT_T len);

/**
* @brief        stop putting multimedia data to cloud
* @param[inout] p_handle    put handle, created by tuya_imm_cloud_put_fragment_data_start api
* @return       OPERATE_RET
* - OPRT_OK     put data success
* - Others      put data failed
*/
OPERATE_RET tuya_imm_cloud_put_fragment_data_stop(INOUT PVOID_T *p_handle);

/**
* @brief      xvr get cloud  config
* @param[in]  nodeId    xvr device nodeId
* @param[in]  config    cloud config pointer
* @param[in]  with_viedo_storage    video cloud config requst flag
* @return     OPERATE_RET
* - OPRT_OK   get cloud config success
* - Others    get cloud config failed
*/
OPERATE_RET tuya_cloud_xvr_get_storage_config(IN CHAR_T *nodeId, INOUT CLOUD_CONFIG_T *config, IN CONST BOOL_T with_viedo_storage);

/**
* @brief      xvr get cloud token config
* @param[in]  nodeId    xvr device nodeId
* @param[in]  config    cloud config pointer
* - OPRT_OK   xvr get cloud config success
* - Others    xvr get cloud config failed
*/
OPERATE_RET tuya_cloud_xvr_get_unify_config(IN CHAR_T * nodeId,INOUT CLOUD_CONFIG_T *config);


#ifdef __cplusplus
}
#endif

#endif
