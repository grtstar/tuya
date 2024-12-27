#ifndef UPLOAD_NOTIFICATION_SWEEPER_H
#define UPLOAD_NOTIFICATION_SWEEPER_H

#include "tuya_cloud_types.h"
#include "tuya_cloud_error_code.h"
#include "cloud_operation.h"
#include "upload_notification.h"


OPERATE_RET upload_map_files(IN CONST CHAR_T *bitmap_file, IN CONST CHAR_T *datamap_file, IN CONST CHAR_T *extend_msg, OUT CONST UINT_T *map_id);

OPERATE_RET upload_map_buffer_subpack(IN CONST UINT_T bitmap_total_len,
                                       IN CONST CHAR_T *bitmap,
                                       IN CONST UINT_T bitmap_len,
                                       IN CONST UINT_T datamap_total_len,
                                       IN CONST CHAR_T *datamap,
                                       IN CONST UINT_T datamap_len,
                                       IN CONST CHAR_T *extend_msg,
                                       OUT CONST UINT_T *map_id);

OPERATE_RET update_map_files(IN CONST UINT_T map_id, IN CONST CHAR_T *bitmap_file, IN CONST CHAR_T *data_map_file,
                             IN CONST CHAR_T *cloud_bitmap_file, IN CONST CHAR_T *cloud_data_map_file);

OPERATE_RET update_map_buffer_subpack(IN CONST UINT_T map_id,
                                       IN CONST UINT_T bitmap_total_len,
                                       IN CONST CHAR_T *bitmap,
                                       IN CONST UINT_T bitmap_len,
                                       IN CONST UINT_T datamap_total_len,
                                       IN CONST CHAR_T *datamap,
                                       IN CONST UINT_T datamap_len);

OPERATE_RET delete_map_files(IN CONST UINT_T map_id);

OPERATE_RET get_map_file_list(IN CONST UINT_T id, OUT CHAR_T *map_url);

OPERATE_RET upload_file_v2(IN CONST CHAR_T *file, IN CONST CHAR_T *cloud_file_name, IN CONST CHAR_T *extend_msg);


#endif//UPLOAD_NOTIFICATION_SWEEPER_H

