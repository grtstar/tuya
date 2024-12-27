/*********************************************************************************
 *Copyright(C),2015-2020, TUYA www.tuya.comm
 *FileName:    tuya_sdk_floor_map_demo
  *brief：1.This article primarily introduces the implementation of floor map and
            cleaning record reporting features, including floor map reporting,
            floor map updating, floor map deletion, floor map file retrieval,
            floor map cloud list retrieval, and cleaning record reporting.
          2.If developers want to run the following functionalities on Ubuntu,
            they can refer to the user_main file and manually operate floor map
            and cleaning record reporting as required.
          3.Developers can refer to the following interface calls to implement
            functional development.
 **********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <uni_thread.h>
#include <unistd.h>

#include "tuya_error_code.h"
#include "tuya_hal_fs.h"
#include "tuya_hal_semaphore.h"
#include "tuya_iot_com_api.h"
#include "tuya_sdk_common.h"
#include "tuya_sdk_dp_demo.h"
#include "tuya_sdk_floor_map_demo.h"
#include "tuyaos_sweeper_api.h"
#include "uni_time.h"

#define FLOOR_MAP_TRANS_SLEEP_TIME 0xffffffff //永久阻塞，需要等待事件触发

/**
 * @brief  扫地机楼层地图上报类型
 */
typedef enum {
    E_FLOOR_MAP_NULL = 0,
    E_FLOOR_MAP_UPLOAD, //地图上报
    E_FLOOR_MAP_UPDATE, //地图更新
    E_FLOOR_MAP_DELETE, //地图删除
    E_FLOOR_MAP_GET, //获取地图
    E_FLOOR_MAP_GET_ALL_MAP_INFO, //获取地图列表
    E_FLOOR_MAP_UPLOAD_RECORD_DATA, //清扫记录上报
    E_FLOOR_MAP_MAX,
} SWEEPER_FLOOR_MAP_TYPE;
/**
 * @brief  扫地机楼层地图资源管理结构体
 */
typedef struct {
    char floor_map_record_num; //类型的记录(清扫记录+楼层地图)条数
    SWEEPER_FLOOR_MAP_TYPE floor_map_type; // 楼层地图上传类型
    void* floor_map_sem; /* 楼层地图发送任务处理信号量 */
} SWEEPER_FLOOR_MAP_CTRL;

/**
 * @brief  扫地机楼层地图资源全局变量
 */
SWEEPER_FLOOR_MAP_CTRL g_sweeper_floor_map_ctrl = { 0 };

/**
 * @brief  楼层地图的六种功能提供选择
 * @param  [*]
 * @param  [*]
 * @return [*]
 */
STATIC VOID printf_multi_map_oper_type_help(VOID)
{
    PR_DEBUG("operate type:\r\n");
    PR_DEBUG("1: upload new map\r\n"); // 上报地图到云端
    PR_DEBUG("2: update map\r\n"); // 更新地图到云端
    PR_DEBUG("3: delete map\r\n"); // 让云端删除地图
    PR_DEBUG("4: get map\r\n"); // 获取地图文件保存到设备端
    PR_DEBUG("5: get all map info\r\n"); // 获取云端地图列表
    PR_DEBUG("6: upload record\r\n"); // 上报清扫记录
    /*楼层地图及清扫记录上报时，如果网络环境有抖动，容易上报失败，开发者根据自身需要做好重传逻辑。
      开发者尽量将以上接口的调用，放在网络相对空闲的时候上报。*/
}

/**
 * @brief  楼层地图的云端地图列表获取
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
STATIC OPERATE_RET get_all_map_info(VOID)
{
    M_MAP_INFO map_info[MAX_M_MAP_INFO_NUM] = { 0 };
    int i = 0;
    unsigned char len = MAX_M_MAP_INFO_NUM; //云端保存的地图数量最多5张，只能最多拉取5张

    int op_ret = tuya_iot_get_all_maps_info(map_info, &len); //获取云端地图列表接口
    if (OPRT_OK != op_ret) {
        PR_ERR("tuya_iot_get_all_maps_info err");
        return op_ret;
    }

    for (i = 0; i < len; i++) { //打印出获取到的云端地图列表数据
        PR_DEBUG("id: %d\r\n", map_info[i].map_id);
        PR_DEBUG("time: %d\r\n", map_info[i].time);
        PR_DEBUG("extend: %s\r\n", map_info[i].descript);
        PR_DEBUG("data file: %s\r\n", map_info[i].datamap_file);
        PR_DEBUG("bitmap file: %s\r\n", map_info[i].bitmap_file);
        PR_DEBUG("<================>\r\n");
    }
    PR_DEBUG("tuya_iot_get_all_maps_info ok\r\n");
    //注意：开发者使用该接口，获取云端的地图数量及云端地图id，一般是为了与本地的地图对比较，确认云端与本地的地图数量和id是否一致。
    return op_ret;
}

/**
 * @brief  清扫记录功能测试入口
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
STATIC OPERATE_RET upload_record(VOID)
{
    OPERATE_RET ret = 0;
    unsigned int map_id = 1;   //本地地图id
    static UINT16_T total_clean_count = 0; //当前记录ID 开发者可以用总清扫次数来填入
    char local_name[128] = { 0 };
    char descript[128] = { 0 };

    extern char s_raw_path[];
    sprintf(local_name, "%s%s", s_raw_path, "/resource/tuya_sweeper_robot/map.bin1"); //清扫记录数据文件这个是测试用的，开发者根据自己的代码的架构来，传入对应的数据，比如数据块的方式

    // 生成 descript 字段
    POSIX_TM_S CurrentTm = { 0 };
    ret = uni_local_time_get(&CurrentTm); //该获取本地时间（带时区和夏令时）。如果要获取utc时间（不带时区和夏令时），可以替换成uni_time_get接口
    if (ret != OPRT_OK) {
        PR_ERR("uni_local_time_get->CurrentTm ret:%d\n", ret);
        return ret;
    }
    CurrentTm.tm_year = CurrentTm.tm_year + 1900;
    CurrentTm.tm_mon = CurrentTm.tm_mon + 1;
    PR_DEBUG("CurrentTm:%d-%d-%d %d:%d:%d", CurrentTm.tm_year, CurrentTm.tm_mon + 1, CurrentTm.tm_mday,
        CurrentTm.tm_hour, CurrentTm.tm_min, CurrentTm.tm_sec);

    sprintf(descript, "%05d_%04d%02d%02d_%02d%02d%02d_%03d_%03d_%05d_%05d_", total_clean_count,
        CurrentTm.tm_year, CurrentTm.tm_mon, CurrentTm.tm_mday,
        CurrentTm.tm_hour, CurrentTm.tm_min, CurrentTm.tm_sec,
        10, 5, 2738, 561); //文件描述中包括当前记录id+日期+清扫时长+清扫面积+地图长度+路径长度
    //虚拟信息长度 + 清扫模式 + 工作模式 + 清扫结果 + 启动方式
    sprintf(descript + 42, "%05d_%02d_%02d_%02d_%02d", 0, CLEAN_MODE_SMART, CLEAN_ONLY_SWEEP, CLEAN_RESULT_OK, START_METHOD_APP);
    //该文件表述一定要按照激光扫地机的格式来写，这个参数需要上报到云端保存，后续面板根据该文件表述来解析
    PR_DEBUG("map id: %d, record file:%s, descript:%s\r\n", map_id, local_name, descript);
    /**********对文件打开及读取的流程****************/
    struct stat statbuff;
    FILE* fp = fopen(local_name, "rb");
    if (fp == NULL) {
        PR_ERR("File Open Fails:%s\r\n", local_name);
        return -1;
    }
    if (stat(local_name, &statbuff) < 0) {
        PR_ERR("File Not Exist:%s\r\n", local_name);
        fclose(fp);
    }
    UINT_T size = statbuff.st_size;
    BYTE_T* buf = (BYTE_T*)malloc(size);
    if (buf == NULL) {
        PR_ERR("Malloc Fail %d\r\n", size);
        fclose(fp);
    }
    UINT_T actual_read = fread(buf, 1, size, fp);
    /**********对文件打开及读取的流程****************/
    OPERATE_RET op_ret = tuya_iot_map_record_upload_buffer(map_id, buf, actual_read, ((strlen(descript) == 0) ? NULL : descript)); //清扫记录上报接口
    if (op_ret == OPRT_OK) {
        PR_DEBUG("upload map id:%d record files OK\r\n", map_id);
    } else {
        PR_ERR("upload map id:%d record files fail\r\n", map_id);
    }
    free(buf);
    fclose(fp);
    return op_ret;
}

/**
 * @brief 楼层地图的功能测试入口
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET multi_map_oper(VOID)
{
    OPERATE_RET ret = 0;
    int oper_type = 0;

    printf_multi_map_oper_type_help();
    scanf("%d", &oper_type);//输入map接口类型
    if (oper_type >= E_FLOOR_MAP_MAX) { //超限 不处理
        return OPRT_COM_ERROR;
    }
    g_sweeper_floor_map_ctrl.floor_map_record_num++; //记录数累加
    g_sweeper_floor_map_ctrl.floor_map_type = oper_type; //测试的map接口类型
    ret = tuya_hal_semaphore_post(g_sweeper_floor_map_ctrl.floor_map_sem);
    if (ret != 0) { //信号量发送失败。
        PR_ERR("post ctrl deal sem err %d", ret);
        g_sweeper_floor_map_ctrl.floor_map_record_num--; //信号量发送错误，数量减掉之前增加的
        goto records_error;
    }
    PR_DEBUG("semaphore post ok");
    return OPRT_OK;
records_error:
    g_sweeper_floor_map_ctrl.floor_map_type = E_FLOOR_MAP_NULL;
    return OPRT_COM_ERROR;
}

/**
 * @brief  楼层地图接口数据组装&发送处理
 * @param  [SWEEPER_FLOOR_MAP_TYPE] floor_map_type接口类型
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
STATIC OPERATE_RET floor_map_map_interface_deal(SWEEPER_FLOOR_MAP_TYPE floor_map_type)
{
    char line[256] = { 0 };
    OPERATE_RET op_ret = 0;
    static unsigned int cloud_map_id = 0;   //云端地图id
    if (floor_map_type == E_FLOOR_MAP_UPLOAD) {

        char map1[128] = { 0 };
        char map2[128] = { 0 };
        char descript[32] = { 0 };
        extern char s_raw_path[];
        sprintf(map1, "%s%s", s_raw_path, "/resource/tuya_sweeper_robot/map.bin0"); //上报的地图文件去本地临时读取，这里是测试用的
        sprintf(map2, "%s%s", s_raw_path, "/resource/tuya_sweeper_robot/map.bin1"); //上报的地图文件去本地临时读取，这里是测试用的
        sprintf(descript, "mm_bin_0_2023_10_27.bin"); //文件描述格式不强制，APP只透传文件，比如测试中使用mm_bin+当前地图id+日期

        PR_DEBUG("bitmap file: %s, datamap file:  %s, descript:%s", map1, map2, descript);
        op_ret = tuya_iot_map_upload_files(map1, map2, ((strlen(descript) == 0) ? NULL : descript), &cloud_map_id); // 地图上报,一般是新建地图完成之后调用该接口上报，存在云端。
        if (op_ret == OPRT_OK) {
            PR_DEBUG("upload files OK, get cloud map id: %d", cloud_map_id);
        } else {
            PR_ERR("upload files fail");
        }
    } else if (floor_map_type == E_FLOOR_MAP_UPDATE || floor_map_type == E_FLOOR_MAP_DELETE || floor_map_type == E_FLOOR_MAP_GET) {
        if (floor_map_type == E_FLOOR_MAP_UPDATE) {
            char map1[128] = { 0 };
            char map2[128] = { 0 };
            extern char s_raw_path[];
            sprintf(map1, "%s%s", s_raw_path, "/resource/tuya_sweeper_robot/map.bin0"); //上报的地图文件去本地临时读取，这里是测试用的
            sprintf(map2, "%s%s", s_raw_path, "/resource/tuya_sweeper_robot/map.bin1"); //上报的地图文件去本地临时读取，这里是测试用的
            PR_DEBUG("map id: %d,  files:%s %s", cloud_map_id, map1, map2);
            op_ret = tuya_iot_map_update_files(cloud_map_id, map1, map2); // 地图更新，这里需要之前上报过地图到云端，云端返回的cloud_map_id；这里要注意更新地图数据，是根据cloud_map_id来，而不是本地的地图id
            if (op_ret == OPRT_OK) { //一般是地图上的内容有新增或者减少才调用该接口，比如有房间分割、合并、房间名称等操作
                PR_DEBUG("update map files map id = %d", cloud_map_id);
            } else {
                PR_ERR("update files fail");
            }
        } else if (floor_map_type == E_FLOOR_MAP_DELETE) {
            op_ret = tuya_iot_map_delete(cloud_map_id); // 地图删除，这里需要之前上报过地图到云端，云端返回的cloud_map_id；这里要注意删除地图数据，是根据cloud_map_id来，而不是本地的地图id
            if (op_ret == OPRT_OK) { //一般是用户有在APP端将地图管理中的地图删除操作，会调用
                PR_DEBUG("delet file ok map_id = %d", cloud_map_id);
            } else {
                PR_ERR("delet map file fail");
            }
        } else if (floor_map_type == E_FLOOR_MAP_GET) {
            extern char s_raw_path[];
            sprintf(line, "%s%s", s_raw_path, "/resource/tuya_sweeper_robot"); //下载到的地图文件保存路径，开发者根据自己的设备存储路径来
            PR_DEBUG("map id: %d, store map path:%s", cloud_map_id, line);
            op_ret = tuya_iot_get_map_files(cloud_map_id, line); // 地图下载地址获取，这里需要之前上报过地图到云端，云端返回的cloud_map_id；这里要注意下载地图数据，是根据cloud_map_id来，而不是本地的地图id
            if (op_ret == OPRT_OK) { //下载成功
                PR_DEBUG("get map id ok = %d", cloud_map_id);
            } else {
                PR_ERR("get map file fail");
            }
        }
    } else if (floor_map_type == E_FLOOR_MAP_GET_ALL_MAP_INFO) {
        PR_DEBUG("to get all map info");
        op_ret = get_all_map_info(); // 地图云端列表获取，一般调用该接口去云端获取地图列表，与本地的地图做对比，来检查设备端与云端地图是否对齐。
    } else if (floor_map_type == E_FLOOR_MAP_UPLOAD_RECORD_DATA) {
        op_ret = upload_record(); //清扫记录上报，一般是清扫结束后，调用该接口上报地图、路径及相关参数给云端，后再面板上展示。
    }
    return op_ret;
}

/**
 * @brief  业务楼层地图启动的逻辑
 * @param  [void*] arg
 * @return [*]
 */
void* thread_floor_map_send(void* arg)
{
    PR_DEBUG("floor map send thread start...");
    OPERATE_RET op_ret = 0;
    unsigned char i = 0;

    while (1) {
        op_ret = tuya_hal_semaphore_waittimeout(g_sweeper_floor_map_ctrl.floor_map_sem, FLOOR_MAP_TRANS_SLEEP_TIME); //最长无限等待

        if (OPRT_OK == op_ret) { // 等待信号量成功
            PR_DEBUG("recv sem goto floor map send num %d %d", g_sweeper_floor_map_ctrl.floor_map_record_num, g_sweeper_floor_map_ctrl.floor_map_type);
        }
        if (g_sweeper_floor_map_ctrl.floor_map_record_num <= 0) { //判断记录数
            continue;
        }

        for (i = 0; i < g_sweeper_floor_map_ctrl.floor_map_record_num; i++) {
            op_ret = floor_map_map_interface_deal(g_sweeper_floor_map_ctrl.floor_map_type);
        }
        g_sweeper_floor_map_ctrl.floor_map_record_num = 0; //发送完成，记录数清0
    }
    PR_DEBUG("floor map send thread end...");
}

/**
 * @brief  扫地机楼层地图上报服务启动
 * @param  [*]
 * @return OPRT_OK on success. Others on error, please refer to tuya_error_code.h
 */
OPERATE_RET ty_user_sweeper_floor_map_init(void)
{
    OPERATE_RET ret = 0;
    THRD_PARAM_S thrd_param;
    THRD_HANDLE thrd; /* 任务句柄 */

    /* 创建楼层地图的线程任务 */
    thrd_param.stackDepth = 1024 * 1024;
    thrd_param.thrdname = "floor_map_trans_thrd";
    thrd_param.priority = TRD_PRIO_1;

    /* 创建机器状态管理任务 */
    ret = CreateAndStart(&thrd, NULL, NULL, (void*)thread_floor_map_send, NULL, &thrd_param);
    if (OPRT_OK != ret) {
        PR_ERR("floor_map_trans_thrd create failed!");
        return ret;
    }

    ret = tuya_hal_semaphore_create_init(&g_sweeper_floor_map_ctrl.floor_map_sem, 0, 1); //创建信号量
    if (OPRT_OK != ret) {
        PR_ERR("floor map sem create err!");
        return ret;
    }

    return OPRT_OK;
}