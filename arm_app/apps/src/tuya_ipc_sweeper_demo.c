/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_ipc_p2p_demo
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h> 

#include "tuya_ipc_common_demo.h"
#include "tuya_ipc_media.h"
#include "tuya_ipc_p2p.h"
#include "tuya_ipc_stream_storage.h"

#include "tuya_iot_com_api.h"
#include "tuya_iot_sweeper_api.h"
#include "tuya_ipc_sweeper_demo.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_error_code.h"
#include "uni_time.h"
#include "tuya_hal_fs.h"
#include "tuya_hal_memory.h"
#include "tuya_devos_netlink.h"


#define SWEEPER_ALBUM_FILE_MAX (SWEEPER_ALBUM_FILE_TYPE_MAX - SWEEPER_ALBUM_FILE_TYPE_MIN + 1) //�ļ�����

typedef enum {
    SWEEPER_DOWNLOAD_STATUS_NULL,
    SWEEPER_DOWNLOAD_STATUS_START, //��ʼ
    SWEEPER_DOWNLOAD_STATUS_STOP, //����
} SWEEPER_DOWNLOAD_STATUS_E;

typedef struct _sweeper_ctrl {
    int used; // 0 idle ,1 used
    int session_id; //ɨ�ػ��������client

    // download parm
    SWEEPER_DOWNLOAD_STATUS_E album_status; //ɨ�ػ��������״̬
    SWEEPER_TRANS_MODE_E transmode;
    int targetfileArr[SWEEPER_ALBUM_FILE_ALL_TYPE_COUNT];
    int fileIndex[SWEEPER_ALBUM_FILE_MAX]; // curent send file index

} SWEEPER_CTRL;
#define SWEEPER_MAX_TRANS_NUM 10 // sugget: same to p2p max num
SWEEPER_CTRL g_sweeper_ctrl[SWEEPER_MAX_TRANS_NUM];

typedef struct {
    CHAR_T fileName[48]; //�48�ֽڣ���Ϊnull������SDK�ڲ�����
    INT_T len;
    CHAR_T* buff;
} SWEEPER_FILE_INFO;



int g_file_arr[SWEEPER_ALBUM_FILE_ALL_TYPE_COUNT] = { 1, 1, 1, 1, 1, 1 };
int g_support_file = 0x3F;
int g_sleep_time = 1;
char g_file_path_name[3][256] = {"/tmp/map.bin", "/tmp/cleanPath.bin", "/tmp/navPath.bin"}; 



SWEEPER_CTRL* __get_sweeper_handle(int session_id)
{
    if (session_id < 0 || session_id >= SWEEPER_MAX_TRANS_NUM) {
        printf("session_id %d err\n", session_id);
        return NULL;
    }
    return &g_sweeper_ctrl[session_id];
}



/* Callback functions for transporting events */
INT_T tuya_ipc_sweeper_album_cb(IN CONST TRANSFER_EVENT_E event, IN CONST PVOID_T args)
{
    int ret = 0;
    int session_id;
    int i;
    printf("p2p sweeper rev event cb=[%d] ", event);
    SWEEPER_CTRL* pSweepHandle = NULL;
    switch (event) {
    case TRANS_ALBUM_QUERY: /* query album */
    {
        C2C_QUERY_ALBUM_REQ* pSrcType = (C2C_QUERY_ALBUM_REQ*)args;
        pSweepHandle = __get_sweeper_handle(pSrcType->channel);
        if (NULL == pSweepHandle) {
            break;
        }

        printf("TRANS_ALBUM_QUERY %s session_id [%d]\n", IPC_SWEEPER_ROBOT, pSrcType->channel);
        pSweepHandle->used = 1;
        pSweepHandle->session_id = pSrcType->channel;

        printf("g_support_file = [%d]\n", g_support_file);
        for (int i= 0; i<6; i++) {
            if ( g_support_file & (0x01 << i)) {
                g_file_arr[i] = 1;
            } else {
                g_file_arr[i] = 0;
            }
            printf("%d = [%d]\n", i, g_file_arr[i]);
        }
        
        tuya_ipc_sweeper_convert_file_info(g_file_arr, &(pSrcType->pIndexFile), &(pSrcType->fileLen)); // notice: ����ָ�룬 SDK�ڲ������ڴ桢�ڲ��ͷ�

        C2C_ALBUM_INDEX_HEAD *head = pSrcType->pIndexFile;
        C2C_ALBUM_INDEX_ITEM *item = head->itemArr;
        int count = head->itemCount;
        printf("query result count [%d]\n", count);
        for(i=0; i<count; i++) {
            printf("file name:[%s]\n", item->filename);
            item++;
        }
        break;
    }
    case TRANS_ALBUM_DOWNLOAD_START: /* start download album */
    {
        C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START* pSrcType = (C2C_CMD_IO_CTRL_ALBUM_DOWNLOAD_START*)args;
        pSweepHandle = __get_sweeper_handle(pSrcType->channel);
        if (NULL == pSweepHandle) {
            break;
        }

        printf("session[%d] start download\n", pSrcType->channel);
        memset(pSweepHandle->targetfileArr, 0x00, sizeof(pSweepHandle->targetfileArr));

        SWEEPER_TRANS_MODE_E trans_mode = SWEEPER_TRANS_NULL;
        if (0 != tuya_ipc_sweeper_parse_file_info(pSrcType, pSweepHandle->targetfileArr, SWEEPER_ALBUM_FILE_ALL_TYPE_COUNT)) {
            break;
        }
        for(int i = 0; i<6; i++) {
            printf("download i = %d\n", pSweepHandle->targetfileArr[i]);
        }

        int fileType;
        for (fileType = SWEEPER_ALBUM_FILE_TYPE_MIN; fileType <= SWEEPER_ALBUM_FILE_TYPE_MAX; fileType++) {
            if (pSweepHandle->targetfileArr[fileType] == 0) {
                trans_mode = SWEEPER_TRANS_FILE;
                break;
            }
        }
        for (fileType = SWEEPER_ALBUM_STREAM_TYPE_MIN; fileType <= SWEEPER_ALBUM_STREAM_TYPE_MAX; fileType++) {
            if (pSweepHandle->targetfileArr[fileType] == 1) {
                trans_mode = SWEEPER_TRANS_STREAM;
                break;
            }
        }
        if (trans_mode != SWEEPER_TRANS_NULL) {
            pSweepHandle->transmode = trans_mode;
            pSweepHandle->album_status = SWEEPER_DOWNLOAD_STATUS_START;
            printf("session_id %d transmode %d\n", session_id, trans_mode);
        }

        break;
    }
    case TRANS_ALBUM_DOWNLOAD_CANCEL: // cancel download
    {
        C2C_ALBUM_DOWNLOAD_CANCEL* pSrcType = (C2C_ALBUM_DOWNLOAD_CANCEL*)args;
         printf("session[%d] download cancel\n", pSrcType->channel);
        pSweepHandle = __get_sweeper_handle(pSrcType->channel);
        if (NULL == pSweepHandle) {
            break;
        }
        pSweepHandle->album_status = SWEEPER_DOWNLOAD_STATUS_STOP;
        break;
    }
    case TRANS_ALBUM_DELETE: //delete, not supported
    {
        printf("session recv cb delete\n");
        C2C_CMD_IO_CTRL_ALBUM_DELETE* pSrcType = (C2C_CMD_IO_CTRL_ALBUM_DELETE*)args;
        break;
    }
    default:
        ret = -1;
        break;
    }
    return ret;
}

extern CHAR_T s_raw_path[128];
#define ALBUM_FILE_SIZE (200 * 1000)

// UserTodoL: get file demo
int demo_sweeper_read_file(int session_id, SWEEPER_TRANS_MODE_E transmode,
    SWEEPER_ALBUM_FILE_TYPE_E fileType, SWEEPER_FILE_INFO* pStrFileInfo)
{
    typedef struct {
        int fileIndex[SWEEPER_ALBUM_FILE_MAX]; // curent send file index
    } DEMO_SWEEPER_FILE;
    static DEMO_SWEEPER_FILE sweeper_file[SWEEPER_MAX_TRANS_NUM];
    char srcFileName[SWEEPER_ALBUM_FILE_MAX][16] = { "map.bin", "cleanPath.bin", "navPath.bin" };

    if (!pStrFileInfo) {
        printf("pp null\n");
        return -1;
    }
    int FileIdx = sweeper_file[session_id].fileIndex[fileType];
    memset(pStrFileInfo->fileName, 0, sizeof(pStrFileInfo->fileName));
    FileIdx = (FileIdx + 1) % 3;
//    sweeper_file[session_id].fileIndex[fileType] = FileIdx;


    char fullpath[128] = { 0 };
    bzero(fullpath, sizeof(fullpath));
    if (session_id % 2 == 0) {
        sprintf(fullpath, "%s/resource/%s/%s%d", s_raw_path, IPC_SWEEPER_ROBOT, srcFileName[fileType], FileIdx);
    } else {
        sprintf(fullpath, "%s/resource/%s/data2/%s%d", s_raw_path, IPC_SWEEPER_ROBOT, srcFileName[fileType], FileIdx);
    }

    printf("read file :%s\n", fullpath);
    FILE* pFile = fopen(fullpath, "rb");
    if (pFile == NULL) {
        printf("can't read map file %s\n", fullpath);
        return -1;
    }

    int size = fread(pStrFileInfo->buff, 1, ALBUM_FILE_SIZE, pFile);
    fclose(pFile);
    pFile = NULL;
    if (size <= 0) {
        printf("read len err %d\n", size);
        return -1;
    }

    pStrFileInfo->len = size;
    return 0;
}



int sweeper_read_file(SWEEPER_ALBUM_FILE_TYPE_E fileType, SWEEPER_FILE_INFO* pStrFileInfo)
{
    char fullpath[256] = { 0 };
    strcpy(fullpath, g_file_path_name[fileType]);

    printf("file type [%d], read file :[%s]\n", fileType, fullpath);


    TUYA_FILE *fp = tuya_hal_fopen(fullpath, "rb");
    if (fp == NULL) {
        PR_ERR("File Open Failed:%s", fullpath);
        return -1;
    }

    tuya_hal_fseek(fp, 0, SEEK_END);
    int len = tuya_hal_ftell(fp);  
    if(len <= 0) {
        PR_ERR("file len is :%d", len);
        tuya_hal_fclose(fp);
        return -1;
    }
    tuya_hal_fseek(fp, 0, SEEK_SET);


    pStrFileInfo->buff = (char *)tuya_hal_system_malloc(len);
    if(pStrFileInfo->buff == NULL) {
        PR_ERR("malloc failed :%d", len);
        tuya_hal_fclose(fp);
        return -1;
    }

    int read_len = tuya_hal_fread(pStrFileInfo->buff, len, fp);
    if (read_len != len) {
        PR_ERR("tuya_hal_fread failed!");
        tuya_hal_fclose(fp);
        tuya_hal_system_free(pStrFileInfo->buff);
        return -1;
    }

    tuya_hal_fclose(fp);
    fp = NULL;

    pStrFileInfo->len = len;
    return 0;
}

void* thread_album_send(void* arg)
{
    printf("THREAD_ALBUM_SEND start...\n");

    char dataBuff[ALBUM_FILE_SIZE];
    SWEEPER_TRANS_MODE_E tmp_mode = SWEEPER_TRANS_NULL;
    int ret = 0;
    int i = 0;
    while (1) {
        for (i = 0; i< SWEEPER_MAX_TRANS_NUM; i++) {
            SWEEPER_CTRL* pSweeper = &g_sweeper_ctrl[i];
            if (pSweeper->used == 0) {
                continue;
            }
            if (SWEEPER_DOWNLOAD_STATUS_NULL == pSweeper->album_status) {
                continue;
            }

            if (SWEEPER_DOWNLOAD_STATUS_START == pSweeper->album_status) // send
            {
                SWEEPER_ALBUM_FILE_TYPE_E fileType = 0;
                for (fileType = SWEEPER_ALBUM_FILE_TYPE_MIN; fileType < SWEEPER_ALBUM_FILE_MAX; fileType++) {

                    if (pSweeper->targetfileArr[fileType] == 0 && pSweeper->targetfileArr[fileType + SWEEPER_ALBUM_FILE_MAX] == 0) {
                        printf("type %d not needed\n", fileType);
                        continue;
                    }

                    SWEEPER_FILE_INFO strFileInfo = { 0 };
                    ret = sweeper_read_file(fileType, &strFileInfo);
                    if (0 != ret) {
                        printf("__get_sweeper_file_bufferr %d\n", ret);
                        continue;
                    }

                    ret = tuya_ipc_sweeper_send_data_with_buff(pSweeper->session_id, fileType, strFileInfo.len, strFileInfo.buff);
                    printf("tuya_ipc_sweeper_send_data_with_buff ret %d\n", ret);
                    if (0 == ret) {
                        pSweeper->fileIndex[fileType]++;
                    }

                    free(strFileInfo.buff);
                    strFileInfo.buff = NULL;
                }
                if (SWEEPER_TRANS_FILE == pSweeper->transmode) {
                    printf("session_id %d tuya_ipc_sweeper_send_finish_2_app file over\n", pSweeper->session_id);
                    tuya_ipc_sweeper_send_finish_2_app(pSweeper->session_id);
                    pSweeper->album_status = SWEEPER_DOWNLOAD_STATUS_NULL;
                } else {
                    sleep(g_sleep_time); 
                }

            } else if (SWEEPER_DOWNLOAD_STATUS_STOP == pSweeper->album_status) { // send over
                printf("session_id %d tuya_ipc_sweeper_send_finish_2_app\n", pSweeper->session_id);
                tuya_ipc_sweeper_send_finish_2_app(pSweeper->session_id);
                pSweeper->album_status = SWEEPER_DOWNLOAD_STATUS_NULL;
            }
        }
        usleep(1000);
    }
    printf("thread_album_send exit...\n");
    pthread_exit(0);
}


STATIC VOID printf_multi_map_oper_type_help(VOID)
{
    PR_DEBUG("operate type:\r\n");
    PR_DEBUG("1: upload new maps\r\n");
    PR_DEBUG("2: update maps\r\n");
    PR_DEBUG("3: delete maps\r\n");
    PR_DEBUG("4: get maps\r\n");
    PR_DEBUG("5: get all map info\r\n");
    PR_DEBUG("6: upload record\r\n");
}


STATIC VOID get_all_map_info(VOID)
{
    M_MAP_INFO map_info[MAX_M_MAP_INFO_NUM] = {0};
    int i = 0;
    char len = MAX_M_MAP_INFO_NUM;

    int op_ret = tuya_iot_get_all_maps_info(map_info, &len);
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_get_all_maps_info err");
        return;
    }

    for (i = 0; i < len; i++) {
        PR_DEBUG("id: %d\r\n", map_info[i].map_id);
        PR_DEBUG("time: %d\r\n", map_info[i].time);
        PR_DEBUG("extend: %s\r\n", map_info[i].descript);
        PR_DEBUG("data file: %s\r\n", map_info[i].datamap_file);
        PR_DEBUG("bitmap file: %s\r\n", map_info[i].bitmap_file);
        PR_DEBUG("<================>\r\n");
    }
    PR_DEBUG("tuya_iot_get_all_maps_info ok\r\n");
}



VOID upload_encrpt_file(VOID)
{
    OPERATE_RET op_ret = 0;
    char map[128] = {0};
    PR_DEBUG("upload file: ");
    scanf("%s", map);
    op_ret = tuya_sweeper_upload_file(map, map, "descript");
    PR_DEBUG("upload encript file. ret: %s\r\n", (op_ret == 0) ? "OK" : "FAIL");
}


VOID upload_record(VOID)
{
    unsigned int map_id = 0;
    char local_name[128] = {0};
    char descript[128] = {0};
    UINT_T actual_read = 0;

    PR_DEBUG("map id: ");
    scanf("%d", &map_id);
    PR_DEBUG("local_name file: ");
    scanf("%s", local_name);
    PR_DEBUG("descript: ");
    scanf("%s", descript);
    PR_DEBUG("map id: %d, record file:%s, descript:%s\r\n",  map_id, local_name, descript);

    struct stat statbuff;
    FILE *fp = fopen(local_name, "rb");
    if(fp == NULL) {
        PR_ERR("File Open Fails:%s\r\n", local_name);
        return;
    }
    if(stat(local_name, &statbuff) < 0) {
        PR_ERR("File Not Exist:%s\r\n", local_name);
        fclose(fp);
    }
    UINT_T size = statbuff.st_size;
    BYTE_T *buf = (BYTE_T *)malloc(size);
    if(buf == NULL) {
        PR_ERR("Malloc Fail %d\r\n", size);
        fclose(fp);
    }
    actual_read = fread(buf, 1, size, fp);
    OPERATE_RET op_ret = tuya_iot_map_record_upload_buffer(map_id, buf, actual_read, ((strlen(descript) == 0) ? NULL : descript));
    if (op_ret == OPRT_OK) {
        PR_DEBUG("upload map id:%d record files OK\r\n", map_id);
    } else {
        PR_ERR("upload map id:%d record files fail\r\n", map_id);
    }
    free(buf);
    fclose(fp);
}


VOID upload_map_file(VOID)
{
    OPERATE_RET op_ret = 0;
    CHAR_T buf[256] = {0};

    GW_NW_STAT_T nw_stat = get_gw_nw_status();
    if (nw_stat != GNS_WAN_VALID) {
        PR_ERR("dev offline\r\n");
        return;
    }
    int type  = 0;
    unsigned int map_id = 0;
    PR_DEBUG("map id: ");
    scanf("%d", &map_id);
    PR_DEBUG("type: <0: layout, 1: route>");
    scanf("%d", &type);
    PR_DEBUG("map file <such as: map/map.bin>: ");
    scanf("%s", buf);
    PR_DEBUG("map_id: %d, type: %d, local file: %s\r\n", map_id, type, buf);
    if (type == 0) {
        op_ret = op_ret = tuya_iot_upload_layout_file(map_id, buf);
        PR_DEBUG("upload layout file. ret: %s\r\n", (op_ret == 0) ? "OK" : "FAIL");
    } else if (type == 1) {
        op_ret = op_ret = tuya_iot_upload_route_file(map_id, buf);
        PR_DEBUG("upload route file. ret: %s\r\n", (op_ret == 0) ? "OK" : "FAIL");
    }
}

VOID multi_map_oper(VOID)
{
    char line[512] = {0};
    OPERATE_RET op_ret = 0;

    GW_NW_STAT_T nw_stat = get_gw_nw_status();
    if (nw_stat != GNS_WAN_VALID) {
        PR_ERR("dev offline\r\n");
        return;
    }
    unsigned int map_id = 0;
    int oper_type = 0;
    printf_multi_map_oper_type_help();
    scanf("%d", &oper_type);
    //printf("<-----oper type:%d----->\r\n", oper_type);
    if (oper_type == 1) {
        char map1[128] = {0};
        char map2[128] = {0};
        char descript[128] = {0};
        PR_DEBUG("bitmap file: ");
        scanf("%s", map1);
        PR_DEBUG("datamap file: ");
        scanf("%s", map2);
        PR_DEBUG("descript: ");
        scanf("%s", descript); 
        
        // /home/zhangyp/code/sdk/sweeper/ubuntu_demo/components/demo_tuya_ipc/resource/ipc_sweeper_robot/map.bin0
        PR_DEBUG("bitmap file: %s, datamap file:  %s, descript:%s\r\n",  map1, map2, descript);
        op_ret = tuya_iot_map_upload_files(map1, map2, ((strlen(descript) == 0) ? NULL : descript), &map_id);
        if (op_ret == OPRT_OK) {
            PR_DEBUG("upload files OK, get map id: %d\r\n", map_id);
        } else {
            PR_ERR("upload files fail\r\n");
        }
    } else if (oper_type == 2 || oper_type == 3 || oper_type == 4 ) {
        PR_DEBUG("map id: ");
        scanf("%d", &map_id);
        if (oper_type == 2) {
            char map1[128] = {0};
            char map2[128] = {0};
            PR_DEBUG("bitmap ifle: ");
            scanf("%s", map1);
            PR_DEBUG("data ifle: ");
            scanf("%s", map2);
            PR_DEBUG("map id: %d,  files:%s %s\r\n", map_id, map1, map2);
            op_ret = tuya_iot_map_update_files(map_id, map1, map2);
            PR_DEBUG("update map files: map id: %d, ret:%i\r\n", map_id, op_ret);
        }else if (oper_type == 3) {
            op_ret = tuya_iot_map_delete(map_id);
            PR_DEBUG("delet map id: %d, ret:%i\r\n", map_id, op_ret);
        } else if (oper_type == 4) {
            PR_DEBUG("store maps path <such as ./>: ");
            scanf("%s", line);
            PR_DEBUG("map id: %d, store map path:%s\r\n", map_id, line);
            op_ret = tuya_iot_get_map_files(map_id, line);
            PR_DEBUG("get map file: map id: %d, ret:%i\r\n", map_id, op_ret);
        }
    }else if (oper_type == 5) {
        PR_DEBUG("to get all map info\r\n");
        get_all_map_info();
    }else if (oper_type == 6) {
        upload_record();
    }
  
}


VOID dp_oper(VOID)
{
    GW_NW_STAT_T nw_stat = get_gw_nw_status();
    if (nw_stat != GNS_WAN_VALID) {
        printf("dev offline\r\n");
        return;
    }
    char line[512] = {0};
    int dpid = 0;
    int dp_type = 0;
    int type = 0;
    uint32_t value = 0;
    char *dp_str = NULL;
    OPERATE_RET op_ret = 0;

    printf("DP id:");
    scanf("%d", &dpid);
    printf("DP type <0: raw, 1: obj> :");
    scanf("%d", &dp_type);
    if (dp_type == 0) {
        printf("DP value:");
        memset(line, 0, sizeof(line));
        scanf("%s", line);
        dp_str = (char *)calloc(strlen(line) + 1, sizeof(char));
        strcpy(dp_str, line);
        uint8_t *hex = (uint8_t *)calloc(strlen(line)/2, sizeof(char));
        ascs2hex(hex, dp_str, strlen(dp_str));
        op_ret = dev_report_dp_raw_sync(NULL, (BYTE_T)dpid, hex, strlen(dp_str)/2, 0);
        if (op_ret == 0) {
            printf("report dp raw sync OK\r\n");
        } else {
            printf("report dp raw ssync FAIL\r\n");
        }

        uint8_t i = 0;
        printf("raw dp data tx:\r\n");
        for (i = 0; i < strlen(dp_str)/2; i++)
            printf("%02x ", hex[i]);
        printf("\r\n\r\n\r\n");
        free(hex);
        free(dp_str);
        return;
    }
    printf("OBJ DP type <0: BOOL, 1: VALUE, 2: STR, 3: ENUM, 4:BITMAP> :");
    scanf("%d", &type);
    printf("DP value:");
    if (type != PROP_STR) {
        scanf("%d", &value);
        printf("dp id:<%d>, type: <%d>, dp value: <%d>\r\n", dpid, type, value);
    }
    else {
        memset(line, 0, sizeof(line));
        scanf("%s", line);
        dp_str = (char *)calloc(strlen(line) + 1, sizeof(char));
        strcpy(dp_str, line);
        printf("dp id:<%d>, type: <%d>, dp value: <%s>\r\n", dpid, type, dp_str);
    }

    TY_OBJ_DP_S dp[1] = {0};
    dp[0].dpid = dpid;
    dp[0].type = (DP_PROP_TP_E)type;
    dp[0].time_stamp = 0;
    switch (dp[0].type) {
        case PROP_BOOL:
            dp[0].value.dp_bool = value;
            break;
        case PROP_VALUE:
            dp[0].value.dp_value = value;
            break;
        case PROP_STR:
            dp[0].value.dp_str = dp_str;
            break;
        case PROP_ENUM:
            dp[0].value.dp_enum = value;
            break;
        case PROP_BITMAP:
            dp[0].value.dp_bitmap = value;
            break;
    }

    op_ret = dev_report_dp_json_async(NULL, dp, 1);
    if (op_ret == 0) {
        printf("report dp json async OK\r\n");
    } else {
        printf("report dp json async FAIL\r\n");
    }
    free(dp_str);
}



typedef enum
{
    MAP_START_STEP = 0,     /*single map began to transport*/
    MAP_TRANSFER_STEP = 1,  /*single map data transmission*/
    MAP_END_STEP = 2,       /*Single map data transmission is over*/
}TY_MAP_STEP_E;

typedef struct
{
    TY_MAP_STEP_E step;   /*single map step of map transmission*/
    USHORT_T map_id;      /*single map id */
    UINT_T   offset;      /*single map data offset */
    UINT_T   len;         /*single map data len */
    UCHAR_T  data[0];      /*single map data pointser */
}TY_SINGLE_MAP_S;


typedef enum
{
    MAP_ADD_TYPE = 0,  /*The map data keeps accumulating*/
    MAP_ALL_TYPE = 1,  /*Map data is uploaded once*/
}TY_MAP_TYPE_E;
    

typedef struct
{
    USHORT_T session_id;     /*A map shows the number*/
    UCHAR_T sub_map_id;      /*Displays the submap number at once*/
    UCHAR_T sub_clean_flag;  /*The submap clears flag bits,0:Keep adding up the data,1:Clear all current data of the corresponding submap ID*/
    TY_MAP_TYPE_E type;      /*The map type*/
}TY_MANY_MAP_HEAD_S;


typedef struct
{
    TY_MANY_MAP_HEAD_S head;   /*many map Map attribute structure*/
    UINT_T   offset;           /*many map data offset */
    UINT_T   len;              /*many map data len */
    UCHAR_T  data[0];          /*many map data pointser */
}TY_MANY_MAP_S;


#define TY_SVC_MAP_MQTT_TIMEROUT_S      (5)               /*Map data MQTT upload failure timeout S*/


STATIC OPERATE_RET tuya_svc_map_stream_single_map_data_pack(IN TY_SINGLE_MAP_S *p_single_map,OUT FLOW_BODY_ST **pp_pack)
{
    if(NULL == p_single_map) {
        PR_ERR("in parameter is null");
        return OPRT_INVALID_PARM;
    }
    
    if( (p_single_map->step > MAP_END_STEP) || (p_single_map->step < MAP_START_STEP) ) {
        PR_ERR("sigle step rang is err:%d",p_single_map->step);
        return OPRT_INVALID_PARM;
    }

    PR_DEBUG("map_setp:%d,map_id:%d,offset:%d,len:%d",p_single_map->step,p_single_map->map_id,p_single_map->offset,p_single_map->len);

    FLOW_BODY_ST  *p_map = NULL; 
    UINT_T malloc_len = 0;

    if( (MAP_START_STEP == p_single_map->step) || (MAP_END_STEP == p_single_map->step) ) {
        malloc_len = SIZEOF(FLOW_BODY_ST);
    }

    if(MAP_TRANSFER_STEP == p_single_map->step) {
         malloc_len = SIZEOF(FLOW_BODY_ST) + p_single_map->len;
    }

    if(0 == malloc_len) {
        PR_ERR("sigle map pack malloc_len is 0,step is:%d",p_single_map->step);
        return OPRT_INVALID_PARM;
    }

    p_map = (FLOW_BODY_ST *)Malloc(malloc_len);
    if(NULL == p_map) {
        PR_ERR("malloc %d err",malloc_len);
        return OPRT_MALLOC_FAILED;
    }
    memset(p_map,0,malloc_len);

    p_map->id= p_single_map->map_id;
    p_map->posix = uni_time_get_posix();
    p_map->step = p_single_map->step;
    if(MAP_TRANSFER_STEP == p_single_map->step) {
        p_map->offset = p_single_map->offset;
        p_map->len = p_single_map->len;
        if(p_map->len > 0) {
            memcpy(p_map->data, p_single_map->data, p_single_map->len);
        }
    }else {
        p_map->offset = 0;
        p_map->len = 0;
    }

    *pp_pack = p_map;

    return OPRT_OK;
}



STATIC OPERATE_RET tuya_svc_map_stream_single_data_mqtt_push(IN TY_SINGLE_MAP_S *p_single_map)
{
    if(NULL == p_single_map) {
        PR_ERR("in parameter is null");
        return OPRT_INVALID_PARM;
    }

    OPERATE_RET op_ret = OPRT_OK;
    FLOW_BODY_ST *p_map = NULL;

    op_ret = tuya_svc_map_stream_single_map_data_pack(p_single_map,&p_map);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_svc_map_stream_single_map_data_pack err:%d",op_ret);
        return op_ret;
    }

    op_ret = tuya_iot_media_data_report(p_map,TY_SVC_MAP_MQTT_TIMEROUT_S);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_media_data_report err:%d",op_ret);
    }

    Free(p_map);
    p_map = NULL;

    return op_ret;
}


STATIC OPERATE_RET tuya_svc_map_stream_many_map_data_pack(IN TY_MANY_MAP_S *p_many_map,OUT FLOW_BODY_V2_ST **pp_pack)
{
    if(NULL == p_many_map) {
        PR_ERR("in parameter is null");
        return OPRT_INVALID_PARM;
    }

    FLOW_BODY_V2_ST  *p_map = NULL; 
    UINT_T malloc_len = 0;

    malloc_len = SIZEOF(FLOW_BODY_V2_ST) + p_many_map->len;

    p_map = (FLOW_BODY_V2_ST *)Malloc(malloc_len);
    if(NULL == p_map) {
        PR_ERR("malloc %d err",malloc_len);
        return OPRT_MALLOC_FAILED;
    }
    memset(p_map,0,malloc_len);

    p_map->id= p_many_map->head.session_id;
    p_map->map_id= p_many_map->head.sub_map_id;
    p_map->clear_type= p_many_map->head.sub_clean_flag;
    p_map->data_type= p_many_map->head.type;
    p_map->posix = uni_time_get_posix();
    p_map->offset = p_many_map->offset;
    p_map->len = p_many_map->len;
    if(p_many_map->len > 0) {
        memcpy(p_map->data,p_many_map->data,p_many_map->len);
    }

    *pp_pack = p_map;

    return OPRT_OK;
}


OPERATE_RET tuya_svc_map_stream_many_data_mqtt_push(IN TY_MANY_MAP_S *p_many_map)
{
    if(NULL == p_many_map) {
        PR_ERR("in parameter is null");
        return OPRT_INVALID_PARM;
    }

    OPERATE_RET op_ret = OPRT_OK;
    FLOW_BODY_V2_ST *p_map = NULL;

    op_ret = tuya_svc_map_stream_many_map_data_pack(p_many_map,&p_map);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_svc_map_stream_single_map_data_pack err:%d",op_ret);
        return op_ret;
    }

    op_ret = tuya_iot_media_data_report_v2(p_map,TY_SVC_MAP_MQTT_TIMEROUT_S);
    if(OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_media_data_report err:%d",op_ret);
    }

    Free(p_map);
    p_map = NULL;

    return op_ret;
}



VOID upload_flow_data()
{
    OPERATE_RET op_ret = OPRT_OK;
    unsigned int map_id = 0;
    TY_SINGLE_MAP_S single_map = {0};
    PR_DEBUG("map id: (0~255)");
    scanf("%d", &map_id);

    char map[128] = {0};
    PR_DEBUG("data file path: ");
    scanf("%s", map);

    TUYA_FILE *fp = tuya_hal_fopen(map, "rb");
    if (fp == NULL) {
        PR_ERR("File Open Failed:%s", map);
        return;
    }

    tuya_hal_fseek(fp, 0, SEEK_END);
    int len = tuya_hal_ftell(fp);  
    if(len <= 0) {
        PR_ERR("file len is :%d", len);
        tuya_hal_fclose(fp);
        return;
    }
    tuya_hal_fseek(fp, 0, SEEK_SET);

    char *map_buf = single_map.data;
    map_buf = (char *)tuya_hal_system_malloc(len);
    if(map_buf == NULL) {
        PR_ERR("malloc failed :%d", len);
        tuya_hal_fclose(fp);
        return;
    }

    int read_len = tuya_hal_fread(map_buf, len, fp);
    if (read_len != len) {
        PR_ERR("tuya_hal_fread failed!");
        tuya_hal_fclose(fp);
        tuya_hal_system_free(map_buf);
        return;
    }

    tuya_hal_fclose(fp);
    fp = NULL;

#if 0
    // ���õ�ͼ��С����ʼλ��
    unsigned char test_data_para[3] = {0x01, 0x00, 0x03}; 
    op_ret = dev_report_dp_raw_sync(NULL, 19, test_data_para, sizeof(test_data_para), 5);
    if(op_ret != OPRT_OK) {
        PR_ERR("dev_report_dp_raw_sync failed");
        tuya_hal_system_free(map_buf);
        return;
    }
    
    BYTE_T test_map_data_para[] =
    {0x01, 0x01, 0x00, 0x02, 0x01, 0x03, 0x03, 0x01, 0x02, 0x04, 0x01, 0x02, 0x05, 0x01, 0x02, 0x06, 0x01, 0x02,\
     0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x02, 0x02, 0x04, 0x02, 0x02, 0x05, 0x02, 0x02, 0x06, 0x02, 0x02,\
     0x01, 0x03, 0x02, 0x02, 0x03, 0x02, 0x03, 0x03, 0x02, 0x04, 0x03, 0x02, 0x05, 0x03, 0x02, 0x06, 0x03, 0x02,\
     0x01, 0x04, 0x02, 0x02, 0x04, 0x02, 0x03, 0x04, 0x02, 0x04, 0x04, 0x02, 0x05, 0x04, 0x02, 0x06, 0x04, 0x02,\
     0x01, 0x05, 0x02, 0x02, 0x05, 0x02, 0x03, 0x05, 0x02, 0x04, 0x05, 0x02, 0x05, 0x05, 0x02, 0x06, 0x05, 0x02,\
     0x01, 0x06, 0x02, 0x02, 0x06, 0x02, 0x03, 0x06, 0x02, 0x04, 0x06, 0x02, 0x05, 0x06, 0x02, 0x06, 0x06, 0x02};
#endif

    single_map.step = MAP_START_STEP;
    single_map.map_id = map_id;

    op_ret = tuya_svc_map_stream_single_data_mqtt_push(&single_map);
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_svc_map_stream_single_data_mqtt_push failed");
        tuya_hal_system_free(map_buf);
        return;
    }
    
    single_map.step = MAP_TRANSFER_STEP;
    single_map.map_id = map_id;
    single_map.offset = 0;
    single_map.len = len;
    op_ret = tuya_svc_map_stream_single_data_mqtt_push(&single_map);
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_svc_map_stream_single_data_mqtt_push failed");
        tuya_hal_system_free(map_buf);
        return;
    }

    single_map.step = MAP_END_STEP;
    single_map.map_id = map_id;
    op_ret = tuya_svc_map_stream_single_data_mqtt_push(&single_map);
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_svc_map_stream_single_data_mqtt_push failed");
        tuya_hal_system_free(map_buf);
        return;
    }

    tuya_hal_system_free(map_buf);
    map_buf = NULL;

    PR_DEBUG("tuya_svc_map_stream_single_data_mqtt_push success");    
    return;
}




VOID upload_flow_data_v2()
{
    OPERATE_RET op_ret = OPRT_OK;
    unsigned int map_id = 0;
    TY_MANY_MAP_S many_map = {0};
    
    PR_DEBUG("map id: (0~255)");
    scanf("%d", &map_id);

    unsigned int session_id = 0;
    PR_DEBUG("sub record id: (0~65535)");
    scanf("%d", &session_id);
    

    char map[128] = {0};
    PR_DEBUG("data file path: ");
    scanf("%s", map);

    TUYA_FILE *fp = tuya_hal_fopen(map, "rb");
    if (fp == NULL) {
        PR_ERR("File Open Failed:%s", map);
        return;
    }

    tuya_hal_fseek(fp, 0, SEEK_END);
    int len = tuya_hal_ftell(fp);  
    if(len <= 0) {
        PR_ERR("file len is :%d", len);
        tuya_hal_fclose(fp);
        return;
    }
    tuya_hal_fseek(fp, 0, SEEK_SET);

    UCHAR_T *map_buf = many_map.data;
    map_buf = (UCHAR_T *)tuya_hal_system_malloc(len);
    if(map_buf == NULL) {
        PR_ERR("malloc failed :%d", len);
        tuya_hal_fclose(fp);
        return;
    }

    int read_len = tuya_hal_fread(map_buf, len, fp);
    if (read_len != len) {
        PR_ERR("tuya_hal_fread failed!");
        tuya_hal_fclose(fp);
        tuya_hal_system_free(map_buf);
        return;
    }

    tuya_hal_fclose(fp);
    fp = NULL;

    many_map.head.session_id = session_id;
    many_map.head.sub_map_id = map_id;
    many_map.head.sub_clean_flag = 0;
    many_map.head.type = MAP_ALL_TYPE;
    many_map.offset = 0;
    many_map.len = len;
 
    op_ret = tuya_svc_map_stream_many_data_mqtt_push(&many_map);
    if(op_ret != OPRT_OK) {
        PR_ERR("tuya_svc_map_stream_many_data_mqtt_push failed");
        tuya_hal_system_free(map_buf);
        return;
    }
    
    tuya_hal_system_free(map_buf);
    map_buf = NULL;
    PR_DEBUG("tuya_svc_map_stream_single_data_mqtt_push success");    
    return;
}



VOID sweeper_get_device_media_detail()
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *result = NULL;
    UINT_T map_id = 0;
    UINT_T sub_record_id = 0;
    UINT_T size = 0;
    CHAR_T start[128] = {0};
        
    PR_DEBUG("input map id:");
    scanf("%d", &map_id);

    PR_DEBUG("input sub record id:");
    scanf("%d", &sub_record_id);

    PR_DEBUG("input start:");
    scanf("%s", start);

    PR_DEBUG("input size:");
    scanf("%d", &size);

    if (strcmp("0", start) == 0) {
        memset(start, 0x00, sizeof(start));
    }
    
    op_ret = tuya_iot_sweeper_get_device_media_detail(sub_record_id, map_id, start, size, &result);
    if (OPRT_OK != op_ret) {
        PR_ERR("get media detail failed");
        goto EXIT;
    }
    
    PR_DEBUG("get media detail success!");
    char *in = ty_cJSON_PrintUnformatted(result);
    PR_DEBUG("result:%s\n", in);
  
EXIT:
    if(result != NULL) {
        ty_cJSON_Delete(result);
    }

    return ;
}


VOID sweeper_get_device_media_latest()
{
    OPERATE_RET op_ret = OPRT_OK;
    ty_cJSON *result = NULL;
    UINT_T size = 0;
    CHAR_T start[128] = {0};
        
    PR_DEBUG("input start:");
    scanf("%s", start);

    PR_DEBUG("input size:");
    scanf("%d", &size);
    
    if (strcmp("0", start) == 0) {
        memset(start, 0x00, sizeof(start));
    }

    op_ret = tuya_iot_sweeper_get_device_media_latest(start, size, &result);
    if (OPRT_OK != op_ret) {
        PR_ERR("get media detail failed");
        goto EXIT;
    }
    
    PR_DEBUG("get media detail success!");
    char *in = ty_cJSON_PrintUnformatted(result);
    PR_DEBUG("result:%s\n", in);
  
EXIT:
    if(result != NULL) {
        ty_cJSON_Delete(result);
    }

    return ;
}

