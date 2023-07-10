#pragma once
#include <vector>

#include "robot_msg.h"

#pragma pack(1)
struct TuyaRoomProperty
{
    int16_t room_id;
    int16_t clean_order;
    int16_t clean_repeat;
    int16_t mop_repeat;
    int8_t color_order;
    int8_t donot_sweep;
    int8_t donot_mop;
    int8_t fan_power;
    int8_t water_level;
    int8_t enable_ymop;
    int8_t nop[12];
};
#pragma pack()
struct TuyaVertex
{
    int16_t x;
    int16_t y;
};
struct TuyaRoom
{
    TuyaRoomProperty room_propeties;
    uint8_t room_name[20];
    uint8_t vertices_num;
    std::vector<TuyaVertex> room_vertices;
};

#pragma pack(1)
struct TuyaMapHeader
{
    uint8_t version;
    uint16_t map_id;
    uint8_t type;
    uint16_t map_width;
    uint16_t map_height;
    uint16_t map_ox;
    uint16_t map_oy;
    uint16_t map_resolution;
    uint16_t charge_x;
    uint16_t charge_y;
    uint32_t pix_len;
    uint16_t pix_lz4len;
};
#pragma pack()

struct TuyaMap
{
    TuyaMapHeader header;
    std::vector<uint8_t> pix;
    uint16_t region_num;
    std::vector<TuyaRoom> rooms;
};

#pragma pack(1)
struct TuyaPathHeader
{
    uint8_t version;
    uint16_t path_id;
    uint8_t init_flag;
    uint8_t type;
    uint32_t count;
    uint16_t direction;
    uint16_t lz4len;
};
#pragma pack()

struct TuyaPath
{
    TuyaPathHeader header;
    std::vector<uint8_t> points;
};


/*
默认值（不可修改删除，否则影响控制面板逻辑）：
smart - 自动清扫模式/全屋清扫模式
chargego - 自动回充模式（需要兼容：goto_charge）
zone - 划区清扫模式/矩形清扫模式
pose - 指哪扫哪模式/定点清扫模式
part - 局部清扫模式
select_room - 选区清扫模式
*/

enum TuyaMode
{
    DP4_SMART,
    DP4_CHARGE_GO,
    DP4_ZONE,
    DP4_POSE,
    DP4_PART,
    DP4_SELECT_ROOM
};

TuyaMode MarsModeToTuya(int mode);
int TuyaModeToMars(TuyaMode mode);

/*
默认值（不可修改删除，否则影响控制面板逻辑）：
standby - 待机中
smart - 自动清扫中
zone_clean - 划区清扫中
part_clean - 局部清扫中
cleaning - 清扫中(备选) paused - 已暂停
goto_pos - 前往目标点中
pos_arrived - 目标点已到达 4-
pos_unarrive - 目标点不可达
goto_charge - 寻找充电座中
charging - 充电中
charge_done - 充电完成
sleep - 休眠
select_room - 选区清扫中
seek_dust_bucket - 寻找集尘桶中
collecting_dust - 集尘中 可在后面增加其他状态值，状态值尽量是状态的英文小写单词，如需要增加故障中状态，则为in_trouble
-*/

enum TuyaStatus
{
    DP5_STANDBY,
    DP5_SMART,
    DP5_ZONE_CLEAN,
    DP5_PART_CLEAN,
    DP5_CLEANING,
    DP5_PAUSED,
    DP5_GOTO_POS,
    DP5_POS_ARRIVED,
    DP5_POS_UNARRIVE,
    DP5_GOTO_CHARGE,
    DP5_CHARGING,
    DP5_CHARGE_DONE,
    DP5_SLEEP,
    DP5_SELECT_ROOM,
    DP5_SEEK_DUST_BUCKET,
    DP5_COLLECTING_DUST,
    DP5_SELF_CLEAN,
    DP5_MAPPING,
    DP5_MAPPING_DONE
};

TuyaStatus MarsStatusToTuya(int status);
int TuyaStatusToMars(TuyaStatus);

/*
默认值（可修改、删除、增加）：
closed - 关闭
gentle - 安静
normal - 正常
strong - 强劲
max - 超强
选项排列与此处值的排列顺序一致，
可调整排列顺序，可删减值，可增加其他状态值，状态值尽量是状态的英文小写单词，
如需要增加自动状态，则为auto
*/
enum TuyaSuction
{
    DP9_CLOSED,
    DP9_GENTLE,
    DP9_NORMAL,
    DP9_STRONG,
    DP9_MAX
};

TuyaSuction MarsSuctionToTuya(int suction);
int TuyaSuctionToMars(TuyaSuction suction);

/*
默认值（可修改、删除、增加）：
closed - 关闭
low - 低
middle - 中
high - 高
选项排列与此处值的排列顺序一致，
可调整排列顺序，可删减值，可增加其他状态值，状态值尽量是状态的英文小写单词，
如需要增加自动状态，则为auto
*/
enum TuyaCistern
{
    DP10_CLOSED,
    DP10_LOW,
    DP10_MIDDLE,
    DP10_HIGH,
};

TuyaCistern MarsCisternToTuya(int cistern);
int TuyaCisternToMars(TuyaCistern cistern);

/*
默认值（不可修改删除，否则影响控制面板逻辑）：
edge_sweep_fault - 边刷故障
middle_sweep_fault - 滚刷故障
left_wheel_fault - 左轮故障
right_wheel_fault - 右轮故障
garbage_box_fault - 尘盒故障
land_check_fault - 地检故障
collision_fault - 碰撞传感器故障
可在后面增加其他状态值，状态值尽量是状态的英文小写单词，
如需要增加故障中状态，则为in_trouble
*/

enum TuyaFault
{
    FAULT_EDGE_BRUSH,
    FAULT_MAIN_BRUSH,
    FAULT_LEFT_WHEEL,
    FAULT_RIGHT_WHEEL,
    FAULT_GARBAGE_BOX,
    FAULT_CLIFF_SENSOR,
    FAULT_BUMPER
};

TuyaFault MarsFaultToTuya(int fault);

enum TuyaWorkMode
{
    WORK_BOTH,
    WORK_SWEEP,
    WORK_MOP,
    WORK_AUTO
};

TuyaWorkMode MarsCleanMethodToTuya(CleanMethod method);
CleanMethod TuyaWorkModeToMars(int workMode);

int TuyaRoomIdToMars(int roomId);
int MarRoomIdToTuya(int roomId);