#ifndef _SWEEPER_CLOUD_OPERATION_H_
#define _SWEEPER_CLOUD_OPERATION_H_

#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "cloud_operation.h"

typedef struct {
    UINT64_T upload_interval;
    UINT64_T max_upload_size;
} MAP_DATA_CFG;


OPERATE_RET cloud_upload_file_complete_v20(IN CONST CHAR_T *p_bucket, IN CONST CHAR_T *p_file[], IN CONST CHAR_T *extend_msg, OUT UINT_T *map_id);

OPERATE_RET cloud_file_delete(IN CONST UINT_T map_id);

OPERATE_RET cloud_file_get_list(IN CONST UINT_T id, OUT CHAR_T *map_url);

OPERATE_RET cloud_get_sweeper_config(INOUT MAP_DATA_CFG *route_map_cfg, INOUT MAP_DATA_CFG *layout_map_cfg);

OPERATE_RET cloud_update_file_complete(IN CONST UINT_T map_id);

VOID iot_register_extra_mqt_cb(VOID);



#endif

