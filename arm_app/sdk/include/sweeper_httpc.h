#ifndef _SWEEPER_HTTPC_H
#define _SWEEPER_HTTPC_H

#include "tuya_cloud_types.h"
#include "cJSON.h"
#include "gw_intf.h"
#include <stdarg.h>

#ifdef __cplusplus
	extern "C" {
#endif

/* 扫地机配置*/
OPERATE_RET httpc_cloud_sweeper_cfg_get(OUT ty_cJSON **result);

/* 文件加密上传 */
OPERATE_RET httpc_cloud_storage_common_cfg_get_V4(IN CONST UINT_T file_size, OUT ty_cJSON **result);

OPERATE_RET httpc_cloud_upload_complete_V30(IN CONST UINT_T time, IN CONST CHAR_T *p_filetype, IN CONST CHAR_T *p_extend,
                                        IN CONST CHAR_T *p_bucket, IN CONST CHAR_T *p_file, IN CONST CHAR_T *p_secret_key, IN CONST UINT_T fie_size);

/* 多地图管理接口 */
/*上传文件通知*/
OPERATE_RET httpc_cloud_upload_complete_v20(IN CONST UINT_T time, IN CONST CHAR_T *p_bucket, IN CONST CHAR_T *p_file[], IN CONST CHAR_T *extend_msg, OUT UINT_T *map_id);


/*更新文件通知*/
OPERATE_RET httpc_cloud_update_file_complete(IN CONST UINT_T time,  IN CONST UINT_T map_id);

/* 删除文件 */
OPERATE_RET httpc_cloud_file_delete(IN CONST UINT_T map_id);

/* 获取文件列表 */
OPERATE_RET httpc_cloud_recode_file_list(IN CONST UINT_T time, IN CONST UINT_T id, OUT CHAR_T *map_url);

/*获取所有文件信息*/
OPERATE_RET httpc_cloud_recode_all_file_list(OUT ty_cJSON **result);

/*查询单次流式记录详情*/
OPERATE_RET httpc_cloud_get_media_detail_v30(IN INT_T sub_record_id, IN INT_T map_id, IN CHAR_T *start, IN INT_T size, OUT ty_cJSON **result);

/*查询最新一次流式记录详情*/
OPERATE_RET httpc_cloud_get_media_latest_v30(IN CHAR_T *start, IN INT_T size, OUT ty_cJSON **result);




#ifdef __cplusplus
}
#endif
#endif

