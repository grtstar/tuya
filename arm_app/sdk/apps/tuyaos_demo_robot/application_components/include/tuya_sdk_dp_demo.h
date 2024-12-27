/*********************************************************************************
  *Copyright(C),2015-2020, TUYA www.tuya.comm
  *FileName:    tuya_sdk_dp_demo
**********************************************************************************/

#ifndef INCLUDE_TUYA_SDK_DP_DEMO_H_
#define INCLUDE_TUYA_SDK_DP_DEMO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "tuya_cloud_types.h"
#include "tuya_ipc_api.h"
#include "tuya_cloud_com_defs.h"
#include "tuya_sdk_common.h"

/**
 * @brief  command raw命令，协议指令头根据协议具体内容而定
 */
#define COMMAND_TRANS_MAP_HEAD           0xAB //激光扫地机地图协议帧头
#define COMMAND_TRANS_HEAD               0xAA  //激光扫地机协议帧头
#define COMMAND_VERSION                  0x01 //协议版本
#define DISTURB_LEN                      10   //勿扰数据长度
#define COMMAND_TRANS_HEAD_LEN           6    // 0xAA 0x01 CMD_LEN(4)

#define RAW_DATA_MAX_LEN                 256  //一般raw数据最大数据长度
#define MSG_LOCAL_RAW_DATA_MAX_LEN       512  //本地定时raw数据最大数据长度


/**
 * @brief  设定的dp点集合
 */
#define TUYA_DP_SWITCH_GO                  1          /* 清扫开关 */
#define TUYA_DP_PAUSE                      2          /* 清扫暂停/继续 */
#define TUYA_DP_SWITCH_CHARGE              3          /* 回充开关 */
#define TUYA_DP_WORK_MODE                  4          /* 清扫模式 */
#define TUYA_DP_STATUS                     5          /* 机器状态 */
#define TUYA_DP_CLEAN_TIME                 6          /* 清扫时间 */ 
#define TUYA_DP_CLEAN_AREA                 7          /* 清扫面积 */ 
#define TUYA_DP_BATTERY_PERCENTAGE         8          /* 剩余电量 */
#define TUYA_DP_SUCTION                    9          /* 吸力选择 */
#define TUYA_DP_CISTERN                    10         /* 水量选择 */
#define  TUYA_DP_DIRECTION_CONTROL         12         /* 手动控制方向 */
#define TUYA_DP_MAP_RESET                  13         /* 地图重置 */
#define TUYA_DP_COMMAND_TRANS              15         /* RAW数据指令传输 */ 
#define TUYA_DP_EDGE_BRUSH_LIEF            18         /* 边刷剩余寿命 */
#define TUYA_DP_ROLL_BRUSH_LIEF            19         /* 滚刷剩余寿命 */
#define TUYA_DP_FILTER_LIEF                21         /* 滤网剩余寿命 */
#define TUYA_DP_RAG_LIEF                   23         /* 拖布剩余寿命 */
#define TUYA_DP_FAULT                      28         /* 故障告警 */ 
#define TUYA_DP_DEVICE_TIMER               32         /* 设备定时 */ 
#define TUYA_DP_DISTURB_TIMER_SET          33         /* 勿扰时间设置 */
#define TUYA_DP_DEVICE_INFO                34         /* 设备信息 */
#define TUYA_DP_CUSTOMIZE_MODE_SWITCH      39         /* 个性化清洁偏好开关 */
#define TUYA_DP_CLEAN_MODE                 41         /* 清洁工作模式 */

/* APP储存卡设置页面 */
#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE==1)
#define TUYA_DP_SD_STATUS_ONLY_GET         110         /*存储卡状态 1-正常，2-异常，3-空间不足，4-正在格式化，5-无SD卡*/
#define TUYA_DP_SD_STORAGE_ONLY_GET        109         /* 获取存储卡容量, unit: kb */
#define TUYA_DP_SD_RECORD_ENABLE           150         /* SD卡录像开关 true为打开，false为关闭 功能关闭时，不录像到SD卡*/
#define TUYA_DP_SD_RECORD_MODE             151         /* 录像模式, 设备设置页面-存储卡设置 SD卡录像模式选择，1为事件录像，2为连续录像*/
#define TUYA_DP_SD_UMOUNT                  112         /* 卸载存储卡 */
#define TUYA_DP_SD_FORMAT                  111         /* 存储卡格式化 */
#define TUYA_DP_SD_FORMAT_STATUS_ONLY_GET  117         /* 格式化状态 返回错误码： -2000：SD卡正在格式化 -2001：SD卡格式化异常 -2002：无SD卡 -2003：SD卡错误 //正数为格式化进度*/
#endif


/**
 * @brief  TUYA_DP_COMMAND_TRANS 协议中命令字集合
 */
#define CMD_ROOM_CLEAN                     0x14         //选区清扫
#define CMD_ROOM_CLEAN_REQ                 0x15         //选区清扫信息获取

#define CMD_POSE_CLEAN_V1                  0x3E         //定点清扫V1
#define CMD_POSE_CLEAN_REQ                 0x3F         //定点清扫信息获取V1

#define CMD_AREA_CLEAN_V2_CUSTOMSIZE       0x3A         //划区清扫V2版本(个性版本)
#define CMD_AREA_CLEAN_REQ                 0x3B         //划区清扫信息查询

#define CMD_LOCAL_TIME_SET                 0x30         //本地定时功能
#define CMD_LOCAL_TIME_SET_REQ             0x31         //本地定时功能信息获取

#define CMD_DONT_DISTURB_SET_V1            0x40         //勿扰时间设置v1
#define CMD_DONT_DISTURB_SET_REQ           0x41         //勿扰时间设置信息获取v1



/**
 * @brief 机器执行的模式：根据自身业务决定（与IOT后台的枚举值对应）
 */
typedef enum {
    ROBOT_MODE_SMART = 0,
    ROBOT_MODE_ZONE,
    ROBOT_MODE_POSE,
    ROBOT_MODE_SELECT_ROOM,
    ROBOT_MODE_REMOTE,
    ROBOT_DP_SET_MAX, 
    ROBOT_MODE_GOTO_CHARGE,
    ROBOT_MODE_CHARGING,
    ROBOT_MODE_MAPPING, //DP 模式中并不会主动设置
    ROBOT_MODE_SMART_RESERVE, //预约清扫中的模式
    ROBOT_MODE_CURRENT_POSE, //当前点定点
    ROBOT_MODE_MAX
}ROBOT_WORK_MODE_E;

/**
 * @brief  机器当前的状态：根据自身业务决定（与IOT后台的枚举值对应）
 */
typedef enum {
    DP_ST_STANDBY = 0,
    DP_ST_SMART,
    DP_ST_ZONE_CLEAN,
    DP_ST_PART_CLEAN,
    DP_ST_CLEANING,
    DP_ST_PAUSED,
    DP_ST_GOTO_POS,
    DP_ST_POS_ARRIVED,
    DP_ST_POS_UNARRIVE,
    DP_ST_GOTO_CHARGE,
    DP_ST_CHARGING,
    DP_ST_CHAGR_DONE,
    DP_ST_SLEEP,
    DP_ST_SELECT_ROOM,
    DP_ST_SEEK_DUST_BUCKET,
    DP_ST_COLLECTING_DUST,   
    DP_ST_SELF_CLEAN,
    DP_ST_MAPPING,          
    DP_ST_MAPPING_DONE,      
    DP_ST_MANUAL, 
    DP_ST_BREAKPOINT_CHARGING,         
    DP_ST_STATUS_MAX
}ROBOT_STATUS_E;

/**
 * @brief 存储卡状态 1-正常，2-异常，3-空间不足，4-正在格式化，5-无SD卡
 */  
typedef enum {
    DP_SD_ST_NULL = 0,
    DP_SD_ST_NORMAL,
    DP_SD_ST_ANOMALY,
    DP_SD_ST_INSUFFICIENT_SPACE,
    DP_SD_ST_FORMATTING,  
    DP_NO_SD_CARD,        
    DP_SD_ST_MAX
}ROBOT_SD_STATUS_E;
/**
 * @brief  清扫模式：仅拖、仅扫、自适应等，根据自身业务决定（与IOT后台的枚举值对应）
 */
typedef enum {
    CLEAN_BOTH = 0,
    CLEAN_ONLY_SWEEP,
    CLEAN_ONLY_MOP,
    CLEAN_ADAPTIVE,     //自适应
    CLEAN_MODE_MAX
}ROBOT_CLEAN_WORK_MODE_E;

/**
 * @brief  风机挡位，根据自身业务决定 （与IOT后台的枚举值对应）
 */
typedef enum {
    E_MOTOR_VACCUM_SHUT_DOWN = 0,
    E_MOTOR_VACCUM_6PA, //安静
    E_MOTOR_VACCUM_12PA, //标准
    E_MOTOR_VACCUM_25PA, //强力
    E_MOTOR_VACCUM_40PA, //超强
    E_MOTOR_VACCUM_STATE_MAX, //dp 上报/设置 上限
}MOTOR_VACCUM_STATE_E; //风机挡位

/**
 * @brief  水泵挡位，根据自身业务决定（与IOT后台的枚举值对应）
 */
typedef enum {
    E_MOTOR_PUMPER_LOW= 0,
    E_MOTOR_PUMPER_MID,
    E_MOTOR_PUMPER_HIGH,
    E_MOTOR_PUMPER_STATE_MAX, //dp 上报/设置 上限
}MOTOR_PUMPER_STATE_E; //水泵挡位


/**
 * @brief  设备异常类型（与IOT后台的枚举值对应）
 */
typedef enum {	
    E_MSG_FT_NO_ERR = 0,            
    E_MSG_FT_BUMER_ERR,         //E01 机器碰撞开关异常
    E_MSG_FT_WHEEL_LOCK,        //E02 机器轮子卡住异常
    E_MSG_FT_BRUSH_LOCK,        //E03 机器边刷卡住
    E_MSG_FT_ROLL_LOCK,         //E04 机器滚刷卡住
    E_MSG_FT_ROBOT_TRAPPED,     //E05 机器受困
    E_MSG_FT_CLIFF_ERR,         //E06 机器悬崖传感器检测到受困
    E_MSG_FT_ROBOT_VACANT,      //E07 机器抱起
    E_MSG_FT_LOW_POWER,         //E08 机器低电量（电池电量低于5%，且不在充电台上）
    E_MSG_FT_LOW_POWER_DUCK,    //E10 机器低电量（电池电量低于5%，且不在集尘桶上）
    E_MSG_FT_FORNT_SENSOER_ERR, //E11 机器前方距离传感器异常
    E_MSG_FT_EDGE_SENSOER_ERR,  //E12 机器沿边传感器异常
    E_MSG_FT_SLOPE_ERR,         //E13 斜坡启动异常
    E_MSG_FT_DUST_BOX_ERR,      //E14 机器清扫时无尘盒
    E_MSG_FT_ROBOT_IN_RESTRICTED_AREAS, //E17 机器在禁区启动
    E_MSG_FF_MAX,                   
}FAULT_TYPE_E ;

/**
 * @brief  手动遥控控制msg消息（与IOT后台的枚举值对应）
 */
typedef enum
{
    CTRL_FORWARD = 0,      //前进
    CTRL_BACKWARD,         //后退
    CTRL_TRUN_LEFT,        //左转
    CTRL_TURN_RIGHT,       //右转
    TRL_STOP,              //暂停
    CTRL_EXIT,             //退出
    CTRL_STATE_MAX    
}E_ROBOTICS_DIRECT_TYPE;

/**
 * @brief  选区清扫参数结构体：根据自身协议定义
 */
typedef struct
{
    int room_count;      //房间数量
    int clean_times;     //清扫次数
    char room_ids[32];   //房间号
    char raw_data[256];  //raw数据缓存（保存起来，待面板来查询时，回复面板）
    int raw_data_len;    //raw数据长度
}ROBOT_WORK_ROOM_EXTRA_S;

/**
 * @brief  目标点结构体：根据自身协议定义
 */
typedef struct {
    float x;    // 机器人坐标系下的x轴坐标，单位米
    float y;    // 机器人坐标系下的y轴坐标，单位米
}ROBOT_POINT_S;


/**
 * @brief  矩形区域结构体：根据自身协议定义
 */
typedef struct {
    ROBOT_POINT_S vertices[4];      //4个点（x/y）一个区域
}ROBOT_AREA_COORDINATE_S;

/**
 * @brief  定点清扫参数结构体：根据自身协议内容定义
 */
typedef struct
{
    ROBOT_AREA_COORDINATE_S points[1];   //一个x/y坐标点
    char clean_times;                    //清扫次数
    char raw_data[256];                  //raw数据缓存
    int raw_data_len;                    //raw数据长度
}ROBOT_WORK_POSE_EXTRA_S;

/**
 * @brief  区域名称结构体
 */
typedef struct {
    char name_len;      //房间名称长度
    char name[19];      //房间名称内容
}ROBOT_AREA_NAME_S;     //区域名称

/**
 * @brief  选区清扫参数结构体：根据自身协议定义
 */
typedef struct
{
    int area_count;                        //房间数量
    int clean_times;                       //清扫次数
    ROBOT_AREA_COORDINATE_S points[10];    //x/y坐标
    ROBOT_AREA_NAME_S names[10];           //房间名称
    char raw_data[256];                    //raw数据缓存
    int raw_data_len;                      //raw数据长度
}ROBOT_WORK_AREA_EXTRA_S;

/**
 * @brief 本地定时基本单元结构体：根据自身协议定义
 */
typedef struct {
    unsigned char effectiveness;  //定时是否有效
    unsigned char week;           //星期:星期一：0x01;星期二：0x02;星期三：0x04;...
    unsigned char time[2];        //执行时间
    unsigned char clean_mode;     //工作模式，0x00为扫拖、0x01为仅扫、0x02为仅拖
    unsigned char fan_level;      //吸力模式
    unsigned char water_level;    //水量模式
    unsigned char sweep_count;    //扫地清扫次数
    unsigned char rooms;          //房间个数
    unsigned char id[32];         //房间ID
}ROBOT_LOCAL_TIMER_UNIT_S; 

/**
 * @brief  本地定时参数结构体：根据自身协议定义
 */
typedef struct {
    char time_zone;                     //手机系统时区
    unsigned char number;               //定时条数
    ROBOT_LOCAL_TIMER_UNIT_S points[30];//与面板对齐，这里最大支持30条  
    char raw_data[512];                 //raw数据缓存
    int raw_data_len;                   //raw数据长度
}ROBOT_LOCAL_TIMER_SET_S; 

/**
 * @brief  勿扰模式结构体：根据自身协议定义
 */
typedef struct {
    unsigned char version;        //协议版本
    char disturb_switch;          //勿扰开关
    char time_zone;               //手机系统时区
    unsigned char start_time[2];  //开始时间
    unsigned char start_day;      //单天还是隔天
    unsigned char end_time[2];    //结束时间
    unsigned char end_day;        //单天还是隔天
    char raw_data[256];           //raw数据缓存
    int raw_data_len;             //raw数据长度
}ROBOT_DISTURB_TIMER_SET_S;

/**
 * @brief  DP点资源全局状态
 */
typedef struct {
    bool switch_go;                     //清扫开关
    bool pause;                         //暂停开关
    bool switch_charge;                 //回充开关
    ROBOT_WORK_MODE_E work_mode;        //清扫模式

    ROBOT_STATUS_E status;              //清扫状态
 
    ROBOT_CLEAN_WORK_MODE_E clean_mode; //清扫模式（扫拖、只扫、只拖） 
    char clean_count;                   //清扫次数
    char suction_select;                //吸力
    char cistern_select;                //水量 

    int total_clean_time;               //总清扫时间
    int total_clean_area;               //总清扫面积
    
    int edge_brush_used_time;           //边刷使用时长
    int roll_brush_used_time;           //滚刷使用时长
    int filter_used_time;               //滤芯使用时长
    int rag_used_time;                  //拖布使用时长

    char battery_percent;                //电池电量

    ROBOT_WORK_ROOM_EXTRA_S room_clean_sets; //选区清扫

    ROBOT_WORK_POSE_EXTRA_S pose_sets;       //定点清扫

    ROBOT_WORK_AREA_EXTRA_S area_clean_sets; //划区清扫

    ROBOT_LOCAL_TIMER_SET_S local_timer_sets; //定时设置
    ROBOT_DISTURB_TIMER_SET_S disturb_timer_sets; //勿扰模式设置

#if defined(TY_ROBOT_MEDIA_ENABLE) && (TY_ROBOT_MEDIA_ENABLE==1)
    ROBOT_SD_STATUS_E sd_status;      //SD卡状态
    BOOL_T sd_record_on_off;          //SD卡录像开关状态
    int sd_record_mode;               //录像模式
    int s_sd_format_progress;         //格式化状态
#endif
} ROBOT_DP_S;

/**
 * @brief  接收到sdk obj dp处理回调，注册到SDK中
 * @param  [TY_SDK_RECV_OBJ_DP_S] *dp_rev
 * @return [*]
 */
VOID ty_cmd_handle_dp_cmd_objs(IN CONST TY_RECV_OBJ_DP_S* dp_rev);

/**
 * @brief  接收到sdk raw dp处理回调，注册到SDK中
 * @param  [TY_SDK_RECV_OBJ_DP_S] *dp_rev
 * @return [*]
 */
VOID ty_cmd_handle_dp_raw_objs(IN CONST TY_RECV_RAW_DP_S* dp_rev);

/**
 * @brief  接收到sdk query dp处理回调，注册到SDK中
 * @param  [TY_SDK_RECV_OBJ_DP_S] *dp_rev
 * @return [*]
 */
VOID ty_cmd_handle_dp_query_objs(IN CONST TY_DP_QUERY_S* dp_query);

/**
 * @brief  保存在flash或内存中的数据加载
 * @return [*]
 */
void dp_handle_all_dp_load(void);

/**
 * @brief  设备上线同步DP
 * @param  [unsigned char*] p_obj_dp
 * @param  [unsigned int] len
 * @return [*]
 */
OPERATE_RET dp_handle_sync_to_cloud(void);

/**
 * @brief  上报机器设备信息
 * @param  [*]
 * @return [*]
 */
OPERATE_RET dp_handle_device_info_respone(void);

#ifdef TUYA_DP_SD_STATUS_ONLY_GET
/**
 * @brief  SD status DP 设置 & 回复
 * @param  [ROBOT_SD_STATUS_E] sd_status
 * @return [*]
 */
void dp_handle_sd_status_response(ROBOT_SD_STATUS_E sd_status);

#endif

#ifdef TUYA_DP_SD_FORMAT_STATUS_ONLY_GET
/**
 * @brief  SD卡式化状态上报
 * @param  [int] status 状态
 * @return [*]
 */
void dp_handle_report_sd_format_status(int status);

#endif

#ifdef TUYA_DP_SD_STORAGE_ONLY_GET
/**
 * @brief  SD卡存储容量上报
 * @param  [] 
 * @return [*]
 */
void dp_handle_sd_storage_response(void);

#endif

/**
 * @brief  上报机器电池电量
 * @param  [int] percent 0~100
 * @return [*]
 */
void ty_cmd_handle_battery_capacity_response(int percent);

/**
 * @brief  上报机器异常错误
 * @param  [FAULT_TYPE_E] errno_type
 * @return [*]
 */
void ty_cmd_handle_fault_response(FAULT_TYPE_E errno_type);

/**
 * @brief  DP点存储值初始化
 * @return [*]
 */
void ty_cmd_dp_init(void) ;

/**
 * @brief  设备上线上报相关信息
 * @param  [*]
 * @return [*]
 */
void ty_cmd_handle_sync_to_cloud(void);

#ifdef __cplusplus
}


#endif
#endif  /*INCLUDE_TUYA_SDK_DP_DEMO_H_*/
