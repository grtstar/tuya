#include <stdint.h>
#include <vector>
#include <fstream>
#include <netinet/in.h>

#include "tuya_robot.h"

#include "utils/log_.h"
#include "utils/json.hpp"
#include "lz4/lz4.h"

#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_sweeper_api.h"
#include "tuya_devos_netlink.h"
#include "tuya_iot_com_api.h"

#include "tuya.h"
#include "tuya_enums.h"
#include "tuya_utils.h"
#include "tuya_dp_function.h"
#include "robot_msg.h"
#include "voice.h"
#include "timed_task.hpp"

// DP 点
#undef TAG
#define TAG "DP"

using json = nlohmann::json;

TY_OBJ_DP_S DPReportBool(int dpId, bool b)
{
    TY_OBJ_DP_S dp = {0};
    dp.dpid = dpId;
    dp.type = PROP_BOOL;
    dp.value.dp_bool = b;
    dp.time_stamp = 0;
    return dp;
}

TY_OBJ_DP_S DPReportEnum(int dpId, int e)
{
    TY_OBJ_DP_S dp = {0};
    dp.dpid = dpId;
    dp.type = PROP_ENUM;
    dp.value.dp_enum = e;
    dp.time_stamp = 0;
    return dp;
}

TY_OBJ_DP_S DPReportValue(int dpId, int v)
{
    TY_OBJ_DP_S dp = {0};
    dp.dpid = dpId;
    dp.type = PROP_VALUE;
    dp.value.dp_value = v;
    dp.time_stamp = 0;
    return dp;
}

TY_OBJ_DP_S DPReportBitmap(int dpId, uint32_t b)
{
    TY_OBJ_DP_S dp = {0};
    dp.dpid = dpId;
    dp.type = PROP_BITMAP;
    dp.value.dp_bitmap = b;
    dp.time_stamp = 0;
    return dp;
}

TY_OBJ_DP_S DP_ReportSwichGo(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleSwichGo(TY_OBJ_DP_S *dp)
{
    LOGD(TAG, "SwichGo:{}", dp->value.dp_bool);
    Event event;
    event.event = dp->value.dp_bool;
    TuyaComm::Get()->Publish("AC_Go", &event);
}



TY_OBJ_DP_S DP_ReportPause(int dpId, bool b)
{
    return DPReportBool(dpId, b);

}
void DP_HandlePause(TY_OBJ_DP_S *dp)
{
    Event event;
    event.event = dp->value.dp_bool;
    event.param = dp->value.dp_bool;
    TuyaComm::Get()->Publish("AC_Pause", &event);
}

TY_OBJ_DP_S DP_ReportSwichCharge(int dpId, bool b)
{
    return DPReportBool(dpId, b);

}
void DP_HandleSwichCharge(TY_OBJ_DP_S *dp)
{
    Event event;
    event.event = dp->value.dp_bool;
    event.param = dp->value.dp_bool;
    if(TuyaComm::Get()->Publish("AC_Charge", &event))
    {
        
    }
}

/*
默认值（不可修改删除，否则影响控制面板逻辑）：
smart - 自动清扫模式/全屋清扫模式
chargego - 自动回充模式（需要兼容：goto_charge）
zone - 划区清扫模式/矩形清扫模式
pose - 指哪扫哪模式/定点清扫模式
part - 局部清扫模式
select_room - 选区清扫模式
*/
TY_OBJ_DP_S DP_ReportMode(int dpId, int mode)
{
    return DPReportEnum(dpId, mode);
}

void DP_HandleMode(TY_OBJ_DP_S *dp)
{
    int mode = dp->value.dp_enum;
    Event evt;
    evt.event = TuyaModeToMars((TuyaMode)mode);
    evt.param = mode;
    TuyaComm::Get()->Publish("AC_Mode", &evt);
    LOGD(TAG, "mode = {}", mode);
}



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

TY_OBJ_DP_S DP_ReportStatus(int dpId, int status)
{
    return DPReportEnum(dpId, status);
}

/*
默认区间为0min—9999分钟（没有小数点），
如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为1，上报的数值会经过计算，保留一位小数，
例如上报589，经过换算后为58.9min
*/
TY_OBJ_DP_S DP_ReportCleanTime(int dpId, int second)
{
    return DPReportValue(dpId, second/60);
}

/*
默认区间为0㎡—9999㎡（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为1，上报的数值会经过计算，保留一位小数，
例如上报589，经过换算后为58.9㎡
*/
TY_OBJ_DP_S DP_ReportCleanArea(int dpId, float cleanArea)
{
    return DPReportValue(dpId, cleanArea);
}

/*
默认区间为0%—100%（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为1，上报的数值会经过计算，保留一位小数，
例如上报589，经过换算后为58.9%
*/
TY_OBJ_DP_S DP_ReportBatteryPrecentage(int dpId, int percent)
{
    return DPReportValue(dpId, percent);
}

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
TY_OBJ_DP_S DP_ReportSuction(int dpId, int power)
{
    return DPReportEnum(dpId, power);
}
void DP_HandleSuction(TY_OBJ_DP_S *dp)
{
    int suction = dp->value.dp_enum;
    Event evt;
    evt.event = TuyaSuctionToMars((TuyaSuction)suction);
    evt.param = suction;
    TuyaComm::Get()->Publish("AC_Suction", &evt);
}

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
TY_OBJ_DP_S DP_ReportCistern(int dpId, int cistern)
{
    return DPReportEnum(dpId, cistern);
}
void DP_HandleCistern(TY_OBJ_DP_S *dp)
{
    int cistern = dp->value.dp_enum;
    Event evt;
    evt.event = TuyaCisternToMars((TuyaCistern)cistern);
    evt.param = cistern;
    TuyaComm::Get()->Publish("AC_Cistern", &evt);
}

TY_OBJ_DP_S DP_ReportSeek(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleSeek(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    PlayVoice(V_SEEK_ROBOT, PLAY_INTERUPT);

    TY_OBJ_DP_S d = DP_ReportSeek(dp->dpid, b);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
枚举值: forward, backward, turn_left, turn_right, stop, exit
默认值（不可修改、删除、增加），按键按下下发对应的方向值，松手下发停
*/
TY_OBJ_DP_S DP_ReportDirectionControl(int dpId, int direction)
{
    return DPReportEnum(dpId, direction);
}
void DP_HandleDirectionControl(TY_OBJ_DP_S *dp)
{
    int direction = dp->value.dp_enum;
    Event evt = {0};
    switch (direction)
    {
    case 0:
        evt.event = MSG_MANUAL_FORWARD;
        break;
    case 1:
        evt.event = MSG_MANUAL_BACK;
        break;
    case 2:
        evt.event = MSG_MANUAL_LEFT;
        break;
    case 3:
        evt.event = MSG_MANUAL_RIGHT;
        break;
    case 4:
        evt.event = MSG_MANUAL_STOP;
        break;
    case 5:
        break;
    case 6:
        break;
    default:
        break;
    }
    TuyaComm::Get()->Publish("AC_Direction", &evt);
    TY_OBJ_DP_S d = DP_ReportDirectionControl(dp->dpid, direction);
    dev_report_dp_json_async(NULL, &d, 1);
}

TY_OBJ_DP_S DP_ReportMapReset(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleMapReset(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "删除当前地图");
    Event evt;
    evt.event = MSG_RESET_MAP;
    evt.param = 0;
    if(TuyaComm::Get()->Send("AC_MapEvent", &evt))
    {
        LOGD(TAG, "删除当前地图成功");
        TY_OBJ_DP_S d = DP_ReportMapReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

// Tuya 暂未使用
void DPRaw_HandlePathData(int dpId, uint8_t *data, int len)
{
}

void DPRaw_HandleCommand(int dpId, uint8_t *data, int len)
{
    TuyaHandleStandardFunction(dpId, data, len);
    TuyaHandleExtentedFuction(dpId, data, len);
}

void DPRaw_Send(int dpId, uint8_t *data, int len, int timeoutMs)
{
    dev_report_dp_raw_sync(NULL, dpId, (const uint8_t *)data, len, timeoutMs);
}

void DP_HandleRequest(TY_OBJ_DP_S *dp)
{
    int request = dp->value.dp_enum;
    switch (request)
    {
    case 0: // get map
    {
        LOGD(TAG, "DP16 get map");
        TuyaComm::Get()->ReportMap();
    }
    break;
    case 1: // get path
    {
        LOGD(TAG, "DP16 get path");
        TuyaComm::Get()->ReportPath();
    }
    break;
    case 2: // get both
    {
        LOGD(TAG, "DP16 get both");
        TuyaComm::Get()->ReportMapAll();
    }
    break;
    default:
        break;
    }
}

/*
边刷剩余寿命
默认区间为0min—9000min（不支持小数点） 注意：选择了该DP点，必须选择对应的重置DP点
*/
TY_OBJ_DP_S DP_ReportEdgeBrushLife(int dpId, int life)
{
    return DPReportValue(dpId, life);
}
void DP_HandleEdgeBrushLife(TY_OBJ_DP_S *dp)
{
    int life = dp->value.dp_value;
}


TY_OBJ_DP_S DP_ReportEdgeBrushLifeReset(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleEdgeBrushLifeReset(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    AppPartsLife parts = {0};
    parts.edgeBrushLifeMinutes = 9000;
    if(TuyaComm::Get()->Send("AC_PartsLifeReset", &parts))
    {
        TY_OBJ_DP_S d = DP_ReportEdgeBrushLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}


/*
滚刷剩余寿命
默认区间为0min—18000min（不支持小数点） 注意：选择了该DP点，必须选择对应的重置DP点
*/
TY_OBJ_DP_S DP_ReportRollBrushLife(int dpId, int life)
{
    return DPReportValue(dpId, life);
}
void DP_HandleRollBrushLife(TY_OBJ_DP_S *dp)
{
    int life = dp->value.dp_value;
}


TY_OBJ_DP_S DP_ReportRollBrushLifeReset(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleRollBrushLifeReset(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    AppPartsLife parts = {0};
    parts.mainBrushLifeMinutes = 18000;
    if(TuyaComm::Get()->Send("AC_PartsLifeReset", &parts))
    {
        TY_OBJ_DP_S d = DP_ReportRollBrushLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}


/*
滤网/海帕剩余寿命
默认区间为0min—9000min（不支持小数点） 注意：选择了该DP点，必须选择对应的重置DP点
*/
TY_OBJ_DP_S DP_ReportFilterLife(int dpId, int life)
{
    return DPReportValue(dpId, life);
}

TY_OBJ_DP_S DP_ReportFilterLifeReset(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleFilterLifeReset(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    AppPartsLife parts = {0};
    parts.haipaLifeMinutes = 9000;
    if(TuyaComm::Get()->Send("AC_PartsLifeReset", &parts))
    {
        TY_OBJ_DP_S d = DP_ReportFilterLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

/*
拖布剩余寿命
默认区间为0min—9000min（不支持小数点） 注意：选择了该DP点，必须选择对应的重置DP点
*/
TY_OBJ_DP_S DP_ReportRagLife(int dpId, int life)
{
    return DPReportValue(dpId, life);
}
TY_OBJ_DP_S DP_ReportRagLifeReset(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleRagLifeReset(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    AppPartsLife parts = {0};
    parts.ragLifeMinutes = 9000;
    if(TuyaComm::Get()->Send("AC_PartsLifeReset", &parts))
    {
        TY_OBJ_DP_S d = DP_ReportRagLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}


TY_OBJ_DP_S DP_ReportDonotDisturb(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleDonotDisturb(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}勿扰模式", b ? "开启" : "关闭");
    TimedTask::GetTimedTask()->SetNotDisturbSwitch(b);
    AppNotDisturbTime time;
    TimedTask::GetTimedTask()->GetNotDisturbTime(time);
    if (time.version >= 0)
    {
        TuyaComm::Get()->Send("AC_NotDisturbTime", &time);
    }
     
    TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
默认区间为0—100，可自行修改区间，不支持小数点
*/
TY_OBJ_DP_S DP_ReportVolumeSet(int dpId, int v)
{
    return DPReportValue(dpId, v);
}
void DP_HandleVolumeSet(TY_OBJ_DP_S *dp)
{
    int value = dp->value.dp_value;
    Event evt;
    evt.event = value;
    if(TuyaComm::Get()->Send("AC_VolumeSet", &evt))
    {
        TY_OBJ_DP_S d = DPReportValue(dp->dpid, value);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

/*
断点续扫
*/
TY_OBJ_DP_S DP_ReportBreakClean(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleBreakClean(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    Event evt;
    evt.event = b;
    b = TuyaComm::Get()->Send("AC_BreakClean", &evt);
    TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
    dev_report_dp_json_async(NULL, &d, 1);
}



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
TY_OBJ_DP_S DP_ReportFault(int dpId, uint32_t fault)
{
    return DPReportBitmap(dpId, fault);
}
void DP_HandleFault(TY_OBJ_DP_S *dp)
{
    uint32_t fault = dp->value.dp_bitmap;
}

/*
默认区间为0㎡—99999㎡（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为1，上报的数值会经过计算，保留一位小数，
例如上报589，经过换算后为58.9㎡
*/
TY_OBJ_DP_S DP_ReportCleanAreaTotal(int dpId, int cleanAreaTotal)
{
    return DPReportValue(dpId, cleanAreaTotal);
}

/*
默认区间为0—99999（没有小数点）
*/
TY_OBJ_DP_S DP_ReportCleanCountTotal(int dpId, int cleanCountTotal)
{
    return DPReportValue(dpId, cleanCountTotal);
}

/*
默认区间为0min—99999min（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为1，上报的数值会经过计算，保留一位小数，
例如上报589，经过换算后为58.9.min
*/
TY_OBJ_DP_S DP_ReportCleanTimeTotal(int dpId, int cleanTimeTotalSecond)
{
    return DPReportValue(dpId, cleanTimeTotalSecond/60);
}

void DPRaw_HandleDeviceTimer(int dpId, uint8_t *data, int len)
{
    TuyaHandleStandardFunction(dpId, data, len);
}

void DPRaw_HandleDisturbTimeSet(int dpId, uint8_t *data, int len)
{
    TuyaHandleStandardFunction(dpId, data, len);
}

void DP_ReportDeviceInfo(int dpId, std::string wifiName, int rssi, std::string ip, std::string mac,
                     std::string mcuVersion, std::string firmwareVersion, std::string deviceSN, std::string moudleUUID,
                     std::string baseStationSN, std::string baseStationVersion, std::string baseStationLocalVersion)
{
    json deviceInfo;
    deviceInfo["WiFi_Name"] = wifiName;
    deviceInfo["RSSI"] = rssi;
    deviceInfo["IP"] = ip;
    deviceInfo["Mac"] = mac;
    deviceInfo["MCU_Version"] = mcuVersion;
    deviceInfo["Firmware_Version"] = firmwareVersion;
    deviceInfo["Device_SN"] = deviceSN;
    deviceInfo["Module_UUID"] = moudleUUID;
    deviceInfo["BaseStationSn"] = baseStationSN;
    deviceInfo["BaseStationVersion"] = baseStationVersion;
    deviceInfo["BaseStationLocalVersion"] = baseStationLocalVersion;
    std::string raw = deviceInfo.dump();
    dev_report_dp_raw_sync(NULL, dpId, (const uint8_t *)raw.c_str(), raw.length(), 200);
}

void DPRaw_HandleVoiceData(int dpId, uint8_t *data, int len)
{
}

void DP_HandleLanguage(TY_OBJ_DP_S *dp)
{
    int lang = dp->value.dp_enum;
}

/*
集尘频率设置
默认区间为0—99999（没有小数点），该功能不支持小数点
*/
void DP_HandleDustCollectionNum(TY_OBJ_DP_S *dp)
{
    int value = dp->value.dp_value;
}

/*
集尘宝
点击立即集尘，下发true
*/
void DP_HandleDustCollectionSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
}

/*
自定义模式
用于设置是否需要使用自定义的房间模式进行工作，如果需要使用自定义模式，则需要打开该开关
*/
void DP_HandleCustomizeModeSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
}

/*
枚举值: none, installed
*/
TY_OBJ_DP_S DP_ReportMopState(int dpId, int state)
{
    return DPReportEnum(dpId, state);
}

/*
枚举值: both_work, only_sweep, only_mop
*/
TY_OBJ_DP_S DP_ReportWorkMode(int dpId, int workMode)
{
    return DPReportEnum(dpId, workMode);
}
void DP_HandleWorkMode(TY_OBJ_DP_S *dp)
{
    int workMode = dp->value.dp_enum;
    Event evt;
    evt.event = workMode;
    if(TuyaComm::Get()->Send("AC_WorkMode", &evt))
    {
        TY_OBJ_DP_S d = DPReportEnum(dp->dpid, workMode);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

/*
单位设置
枚举值: square_meter, square_foot
*/
TY_OBJ_DP_S DP_ReportUnitSet(int dpId, int unit)
{
    return DPReportEnum(dpId, unit);
}
void DP_HandleUnitSet(TY_OBJ_DP_S *dp)
{
    int unit = dp->value.dp_enum;
    LOGD(TAG, "单位设置: {} ", unit == 0 ? "平方米":"平方英尺");
    Event evt;
    evt.event = unit;
    TY_OBJ_DP_S d = DPReportEnum(dp->dpid, unit);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
仅在激光扫地机公版面板-V2使用
*/
TY_OBJ_DP_S DP_ReportEstimatedArea(int dpId, int area)
{
    return DPReportValue(dpId, area);
}

/*
地毯清洁偏好
默认值（不可修改删除，否则影响控制面板逻辑）：
adaptive - 自适应
evade - 规避
ignore - 忽略
可在后面增加其他状态值，状态值尽量是状态的英文小写单词，
如需要增加故障中状态，则为in_trouble
*/
TY_OBJ_DP_S DP_ReportCarpetCleanPrefer(int dpId, int prefer)
{
    return DPReportEnum(dpId, prefer);
}
void DP_HandleCarpetCleanPrefer(TY_OBJ_DP_S *dp)
{
    int prefer = dp->value.dp_enum;
    Event evt;
    evt.event = prefer;
    if(TuyaComm::Get()->Send("AC_CartpetCleanPrefer", &evt))
    {
        TY_OBJ_DP_S d = DPReportEnum(dp->dpid, prefer);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

/*
地毯增压
*/
TY_OBJ_DP_S DP_ReportAutoBoost(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleAutoBoost(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    Event evt;
    evt.event = b;
    if(TuyaComm::Get()->Send("AC_AutoBoost", &evt))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}



/*
巡航
*/
void DP_HandleCruiseSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    Event evt;
    evt.event = b;
    if(TuyaComm::Get()->Send("AC_CruiseSwitch", &evt))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

/*
按键童锁
*/
TY_OBJ_DP_S DP_ReportChildLock(int dpId, bool b)
{
    return DPReportBool(dpId, b);

}
void DP_HandleChildLock(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    Event evt;
    evt.event = b;
    if(TuyaComm::Get()->Send("AC_ChildLock", &evt))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

/*
Y字型拖地
*/
void DP48_HandleYMop(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    Event evt;
    evt.event = b;
    if(TuyaComm::Get()->Send("AC_YMop", &evt))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
}

/*
拖布自清洁
*/
void DP_HandleSelfCleanMop(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}拖布自清洁", b ? "开启": "关闭");
    TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
烘干开关
*/
void DP_HandleDryingSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}烘干", b ? "开启" : "关闭");
    TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
拖布自清洁频率
数值范围: 1-10, 间距: 1, 倍数: 0, 单位: ㎡
*/
void DP_HandleMopSlefCleaningFrequency(TY_OBJ_DP_S *dp)
{
    int value = dp->value.dp_value;
    LOGD(TAG, "拖布自清洁频率: {}", value);
    TY_OBJ_DP_S d = DPReportValue(dp->dpid, value);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
拖布自清洁强度
fast, daily, depth
*/
void DP_HandleMopSlefCleaningStrength(TY_OBJ_DP_S *dp)
{
    int prefer = dp->value.dp_enum;
    LOGD(TAG, "拖布自清洁强度: {}", prefer);
    TY_OBJ_DP_S d = DPReportEnum(dp->dpid, prefer);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
擦地强度
slow, normal, fast
*/
void DP_HandleWipingStrength(TY_OBJ_DP_S *dp)
{
    int prefer = dp->value.dp_enum;
    LOGD(TAG, "擦地强度: {}", prefer);
    TY_OBJ_DP_S d = DPReportEnum(dp->dpid, prefer);
    dev_report_dp_json_async(NULL, &d, 1);
}

// 标准功能
void DPRaw_Send(int dpId, uint8_t *data, int len, int timeoutMs);

void TuyaReportVirtualWall(int dpId, uint8_t cmd, AppVirtualWall *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if (msg->version == 0)
    {
        LOGD(TAG, "上报虚拟墙设置 v1.0.0");
        raw.push_back(msg->count);
        for (int i = 0; i < msg->count; i++)
        {
            int x0 = MarsXToTuya(msg->wall[i].begin);
            int y0 = MarsYToTuya(msg->wall[i].begin);
            int x1 = MarsXToTuya(msg->wall[i].end);
            int y1 = MarsYToTuya(msg->wall[i].end);
            auto xx = tonb16(x0);
            raw.insert(raw.end(), xx.begin(), xx.end());
            xx = tonb16(y0);
            raw.insert(raw.end(), xx.begin(), xx.end());
            xx = tonb16(x1);
            raw.insert(raw.end(), xx.begin(), xx.end());
            xx = tonb16(y1);
            raw.insert(raw.end(), xx.begin(), xx.end());
        }
        raw[2] = raw.size() - 3;
        uint8_t sum = 0;
        for (int i = 3; i < raw.size(); i++)
        {
            sum += raw[i];
        }
        raw.push_back(sum);
    }
    if (msg->version == 1)
    {
    }
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}


void TuyaReportRestrictedArea(int dpId, uint8_t cmd, AppRestrictedArea *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);

    if (msg->version == 0)
    {
        LOGD(TAG, "上报禁区设置 v1.1.0");
    }
    if (msg->version == 1)
    {
        LOGD(TAG, "上报禁区设置 v1.2.0");
        raw.push_back(msg->version);
        raw.push_back(msg->count);
        for (int i = 0; i < msg->count; i++)
        {
            raw.push_back(msg->mode[i]);
            raw.push_back(msg->polygon[i].count);
            for (int j = 0; j < msg->polygon[i].count; j++)
            {
                int x0 = MarsXToTuya(msg->polygon[i].vetex[j]);
                int y0 = MarsYToTuya(msg->polygon[i].vetex[j]);
                auto d = tonb16(x0);
                raw.insert(raw.end(), d.begin(), d.end());
                d = tonb16(y0);
                raw.insert(raw.end(), d.begin(), d.end());
            }

            raw.push_back(msg->name[i].length());
            msg->name[i].resize(19, 0);
            for (int j = 0; j < 19; j++)
            {
                raw.push_back(msg->name[i][j]);
            }
        }
        raw[2] = raw.size() - 3;
        uint8_t sum = 0;
        for (int i = 3; i < raw.size(); i++)
        {
            sum += raw[i];
        }
        raw.push_back(sum);

        // LOGD(TAG, "Data: {}", spdlog::to_hex(raw.begin(), raw.end()));
        DPRaw_Send(dpId, &raw[0], raw.size(), 100);
    }
    if (msg->version == 2)
    {
        LOGD(TAG, "上报禁区设置 v1.2.1");
        raw.push_back(msg->version);
        raw.push_back(msg->count);
        for (int i = 0; i < msg->count; i++)
        {
            raw.push_back(msg->mode[i]);
            raw.push_back(msg->type[i]);
            raw.push_back(msg->polygon[i].count);
            for (int j = 0; j < msg->polygon[i].count; j++)
            {
                int x0 = MarsXToTuya(msg->polygon[i].vetex[j]);
                int y0 = MarsYToTuya(msg->polygon[i].vetex[j]);
                auto d = tonb16(x0);
                raw.insert(raw.end(), d.begin(), d.end());
                d = tonb16(y0);
                raw.insert(raw.end(), d.begin(), d.end());
            }

            raw.push_back(msg->name[i].length());
            msg->name[i].resize(19, 0);
            for (int j = 0; j < 19; j++)
            {
                raw.push_back(msg->name[i][j]);
            }
        }
        raw[2] = raw.size() - 3;
        uint8_t sum = 0;
        for (int i = 3; i < raw.size(); i++)
        {
            sum += raw[i];
        }
        raw.push_back(sum);

        // LOGD(TAG, "Data: {}", spdlog::to_hex(raw.begin(), raw.end()));
        DPRaw_Send(dpId, &raw[0], raw.size(), 100);
    }
}

void TuyaReportSpotClean(int dpId, uint8_t cmd, AppSpotClean *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if (msg->version == 0)
    {
        LOGD(TAG, "上报定点清扫 v1.0.0");
        int x = MarsXToTuya(msg->spot);
        int y = MarsYToTuya(msg->spot);
        auto d = tonb16(x);
        raw.insert(raw.end(), d.begin(), d.end());
        d = tonb16(y);
        raw.insert(raw.end(), d.begin(), d.end());
    }
    if (msg->version == 1)
    {
        LOGD(TAG, "上报定点清扫 v1.1.0");
    }
    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++)
    {
        sum += raw[i];
    }
    raw.push_back(sum);

    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportZoneClean(int dpId, uint8_t cmd, AppZoneClean *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if (msg->version == 0)
    {
        LOGD(TAG, "上报划区清扫 v1.1.0");
        raw.push_back(msg->cleanRepeat[0]);
        raw.push_back(msg->count);
        for (int i = 0; i < msg->count; i++)
        {
            raw.push_back(msg->zone[i].count);
            for (int j = 0; j < msg->zone[i].count; j++)
            {
                int x0 = MarsXToTuya(msg->zone[i].vetex[j]);
                int y0 = MarsYToTuya(msg->zone[i].vetex[j]);
                auto d = tonb16(x0);
                raw.insert(raw.end(), d.begin(), d.end());
                d = tonb16(y0);
                raw.insert(raw.end(), d.begin(), d.end());
            }
        }
    }
    if (msg->version == 1)
    {
        LOGD(TAG, "上报划区清扫 v1.2.0");
        raw.push_back(msg->version);
        raw.push_back(msg->cleanRepeat[0]);
        raw.push_back(msg->count);
        for (int i = 0; i < msg->count; i++)
        {
            raw.push_back(msg->zone[i].count);
            for (int j = 0; j < msg->zone[i].count; j++)
            {
                int x0 = MarsXToTuya(msg->zone[i].vetex[j]);
                int y0 = MarsYToTuya(msg->zone[i].vetex[j]);
                auto d = tonb16(x0);
                raw.insert(raw.end(), d.begin(), d.end());
                d = tonb16(y0);
                raw.insert(raw.end(), d.begin(), d.end());
            }

            raw.push_back(msg->name[i].length());
            msg->name[i].resize(19, 0);
            for (int j = 0; j < 19; j++)
            {
                raw.push_back(msg->name[i][j]);
            }
        }
    }
    if (msg->version == 2)
    {
        LOGD(TAG, "上报划区清扫 v1.2.1");
        raw.push_back(msg->version);
        raw.push_back(msg->mode[0]);
        raw.push_back(msg->suction[0]);
        raw.push_back(msg->cistern[0]);
        raw.push_back(msg->cleanRepeat[0]);
        raw.push_back(msg->count);
        for (int i = 0; i < msg->count; i++)
        {
            raw.push_back(msg->zone[i].count);
            for (int j = 0; j < msg->zone[i].count; j++)
            {
                int x0 = MarsXToTuya(msg->zone[i].vetex[j]);
                int y0 = MarsYToTuya(msg->zone[i].vetex[j]);
                auto d = tonb16(x0);
                raw.insert(raw.end(), d.begin(), d.end());
                d = tonb16(y0);
                raw.insert(raw.end(), d.begin(), d.end());
            }

            raw.push_back(msg->name[i].length());
            msg->name[i].resize(19, 0);
            for (int j = 0; j < 19; j++)
            {
                raw.push_back(msg->name[i][j]);
            }
        }
    }
    if (msg->version == 2)
    {
        LOGD(TAG, "上报划区清扫 v1.2.2");
        raw.push_back(msg->version);
        raw.push_back(msg->count);
        for (int i = 0; i < msg->count; i++)
        {
            auto id = tonb32(msg->zoneID[i]);
            raw.insert(raw.end(), id.begin(), id.end());
            raw.push_back(msg->localSave[i]);
            raw.push_back(msg->type[i]);
            raw.push_back(msg->zone[i].count);
            for (int j = 0; j < msg->zone[i].count; j++)
            {
                int x0 = MarsXToTuya(msg->zone[i].vetex[j]);
                int y0 = MarsYToTuya(msg->zone[i].vetex[j]);
                auto d = tonb16(x0);
                raw.insert(raw.end(), d.begin(), d.end());
                d = tonb16(y0);
                raw.insert(raw.end(), d.begin(), d.end());
            }

            raw.push_back(msg->cleanRepeat[i]);
            raw.push_back(msg->order[i]);
            raw.push_back(msg->mode[i]);
            raw.push_back(msg->suction[i]);
            raw.push_back(0x00);
            raw.push_back(0x00);
            raw.push_back(0x00);
            raw.push_back(0x00);
            raw.push_back(0x00);

            raw.push_back(msg->name[i].length());
            msg->name[i].resize(19, 0);
            for (int j = 0; j < 19; j++)
            {
                raw.push_back(msg->name[i][j]);
            }
        }
    }
    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++)
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    // LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportLocalAlert(int dpId, uint8_t cmd, AppLocalAlert *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if (msg->verison == 0)
    {
        LOGD(TAG, "上报本地定时功能 v1.0.0");
        raw.push_back(msg->timeZone);
        raw.push_back(msg->number);
        for(auto const &task_info: msg->timedTaskInfo)
        {
            raw.push_back(task_info.valid);
            raw.push_back(task_info.weekDayMask);
            auto d = tonb16(task_info.time);
            raw.insert(raw.end(), d.begin(), d.end());
            raw.push_back(task_info.roomN);
            for (int i = 0; i < task_info.roomN; i++) 
            {
                raw.push_back(task_info.roomId[i]);
            }
            raw.push_back(task_info.cleanMode);
            raw.push_back(task_info.fan);
            raw.push_back(task_info.water);
            raw.push_back(task_info.cleanRepeat);
        }
    }
    if (msg->verison == 1)
    {
        LOGD(TAG, "上报本地定时功能 v1.1.0");
        raw.push_back(msg->timeZone);
        raw.push_back(msg->number);
        for(auto const &task_info: msg->timedTaskInfo)
        {
            raw.push_back(task_info.valid);
            raw.push_back(task_info.weekDayMask);
            auto d = tonb16(task_info.time);
            raw.insert(raw.end(), d.begin(), d.end());
            d = tonb16(task_info.mapId);
            raw.insert(raw.end(), d.begin(), d.end());
            raw.push_back(task_info.roomN);
            for (int i = 0; i < task_info.roomN; i++) 
            {
                raw.push_back(task_info.roomId[i]);
            }
            for (int i = 0; i < task_info.zoomN; i++) 
            {
                d = tonb32(task_info.zoodId[i]);
                raw.insert(raw.end(), d.begin(), d.end());
            }
            raw.push_back(task_info.cleanMode);
            raw.push_back(task_info.fan);
            raw.push_back(task_info.water);
            raw.push_back(task_info.cleanRepeat);
        }
    }
    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++)
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportNotDisturbTime(int dpId, uint8_t cmd, AppNotDisturbTime *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if (msg->version == 0)
    {
        LOGD(TAG, "上报勿扰时间设置 v1.0.0");
        raw.push_back(msg->timeZone);
        auto d = tonb16(msg->starTime);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->startDay);
        d = tonb16(msg->endTime);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->endDay);
    }
    if (msg->version == 1)
    {
        LOGD(TAG, "上报勿扰时间设置 v1.1.0");
        raw.push_back(msg->version);
        raw.push_back(msg->timeZone);
        raw.push_back(msg->onoff);
        auto d = tonb16(msg->starTime);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->startDay);
        d = tonb16(msg->endTime);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->endDay);
    }
    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++)
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportRoomClean(int dpId, uint8_t cmd, AppRoomClean *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(msg->cleanRepeat);
    raw.push_back(msg->count);
    raw.insert(raw.end(), msg->id.begin(), msg->id.end());
    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++) 
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportRoomName(int dpId, uint8_t cmd, AppSetRoomName *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(msg->count);

    for (int i = 0; i < msg->count; i++) 
    {
        raw.push_back(msg->roomId[i]);
        raw.push_back(msg->name[i].length());
        for (int j = 0; j < msg->name[i].length(); j++)
        {
            raw.push_back(msg->name[i][j]);
        }
    }

    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++) 
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportCleaningSequence(int dpId, uint8_t cmd, AppSetCleaningSequence *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(msg->count);

    for (int i = 0; i < msg->count; i++) 
    {
        raw.push_back(msg->roomId[i]);
    }

    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++) 
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportPartitionDivision(int dpId, uint8_t cmd, AppPartitionDivision *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(msg->ret);

    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++) 
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportPartitionRestoreDefault(int dpId, uint8_t cmd, Event *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(msg->param);

    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++) 
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportRoomProperties(int dpId, uint8_t cmd, AppSetRoomProperties *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(msg->count);
    raw.insert(raw.end(), msg->roomId.begin(), msg->roomId.end());
    raw.insert(raw.end(), msg->fan.begin(), msg->fan.end());
    raw.insert(raw.end(), msg->waterLevel.begin(), msg->waterLevel.end());
    raw.insert(raw.end(), msg->yMop.begin(), msg->yMop.end());
    raw.insert(raw.end(), msg->cleanRepeat.begin(), msg->cleanRepeat.end());

    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++) 
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportPartitionMerge(int dpId, uint8_t cmd, AppPartitionMerge *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(msg->ret);

    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++) 
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportAutoMappingResult(int dpId, uint8_t cmd, uint8_t ret, uint8_t errCode)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if(ret = 0x00)
    {
        raw.push_back(0x00);
        raw.push_back(errCode);
    }
    else
    {
        raw.push_back(0x01);
    }
    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++)
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaReportMapResetResult(int dpId, uint8_t cmd, uint8_t ret, uint8_t errCode)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if(ret = 0x00)
    {
        raw.push_back(0x00);
        raw.push_back(errCode);
    }
    else
    {
        raw.push_back(0x01);
    }
    raw[2] = raw.size() - 3;
    uint8_t sum = 0;
    for (int i = 3; i < raw.size(); i++)
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

void TuyaHandleStandardFunction(int dpId, uint8_t *data, int len)
{
    LOGD(TAG, "TuyaHandleStandardFunction len={}", len);
    LOGD(TAG, "Recv: {}", spdlog::to_hex(data, data + len));
    // check data
    switch (data[3])
    {
    case 0x12: // 虚拟墙设置 v1.0.0
    case 0x13:
        LOGD(TAG, "虚拟墙设置 v1.0.0");
        {
            AppVirtualWall wall;
            wall.version = 0;
            wall.count = data[4];
            wall.mode.assign(1, 0);
            for (int i = 0; i < wall.count; i++)
            {
                int x0 = nbto16(&data[5 + i * 8]);
                int y0 = nbto16(&data[5 + i * 8 + 2]);
                int x1 = nbto16(&data[5 + i * 8 + 4]);
                int y1 = nbto16(&data[5 + i * 8 + 6]);
                wall.wall.push_back(Segment{TuyaXYToMars(x0, y0), TuyaXYToMars(x1, y1)});
            }
            if (TuyaComm::Get()->Send("AC_VirtualWall", &wall))
            {
                TuyaReportVirtualWall(dpId, 0x13, &wall);
            }
        }
        break;
    case 0x48:
    case 0x49: // 虚拟墙设置 v1.1.0
        LOGD(TAG, "虚拟墙设置 v1.1.0");
        {
            AppVirtualWall wall;
            wall.version = 1;
            wall.count = data[4];
            for (int i = 0; i < wall.count; i++)
            {
                wall.mode.push_back(data[5 + i * 9]);
                int x0 = nbto16(&data[5 + i * 9 + 1]);
                int y0 = nbto16(&data[5 + i * 9 + 3]);
                int x1 = nbto16(&data[5 + i * 9 + 5]);
                int y1 = nbto16(&data[5 + i * 9 + 7]);
                wall.wall.push_back(Segment{TuyaXYToMars(x0, y0), TuyaXYToMars(x1, y1)});
            }
            if (TuyaComm::Get()->Send("AC_VirtualWall", &wall))
            {
                TuyaReportVirtualWall(dpId, 0x49, &wall);
            }
        }
        break;
    case 0x14: // 选区清扫
        LOGD(TAG, "选区清扫");
        {
            AppRoomClean room;
            room.version = 0;
            room.cleanRepeat = data[4];
            if(room.cleanRepeat > 2)
            {
                room.cleanRepeat = 1;
            }
            room.count = data[5];
            for (int i = 0; i < room.count; i++)
            {
                room.id.push_back(data[6 + i]);
            }
            if (TuyaComm::Get()->Send("AC_RoomClean", &room))
            {
                TuyaReportRoomClean(dpId, 0x15, &room);
            }
        }

        break;
    case 0x15:
        break;
    case 0x16: // 定点清扫 v1.0.0
    case 0x17:
        LOGD(TAG, "定点清扫 v1.0.0");
        {
            // LOGD(TAG, "{}", spdlog::to_hex(data, data + len));
            int x = nbto16(&data[4]);
            int y = nbto16(&data[6]);
            LOGD(TAG, "x:{},y:{}", x, y);

            AppSpotClean sc;
            sc.version = 0;
            sc.spot = TuyaXYToMars(x, y);
            if (TuyaComm::Get()->Send("AC_SpotClean", &sc))
            {
                TuyaReportSpotClean(dpId, 0x17, &sc);
            }
        }
        break;
    case 0x3E: // 定点清扫 v1.1.0
    case 0x3F:
        LOGD(TAG, "定点清扫 v1.1.0");
        {
            int mode = data[6];
            int suction = data[7];
            int cistern = data[8];
            int count = data[9];
            int x = nbto16(&data[10]);
            int y = nbto16(&data[12]);
            LOGD(TAG, "mode:{},suction:{},cistern:{},count:{},x:{},y:{}",
                 mode, suction, cistern, count, x, y);
            AppSpotClean sc;
            sc.version = 1;
            sc.mode = mode;
            sc.suction = suction;
            sc.cistern = cistern;
            sc.cleanRepeat = count;
            sc.spot = TuyaXYToMars(x, y);
            if (TuyaComm::Get()->Send("AC_SpotClean", &sc))
            {
                TuyaReportSpotClean(dpId, 0x3F, &sc);
            }
        }

        break;
    case 0x1A: // 禁区设置 v1.1.0
    case 0x1B:
        LOGD(TAG, "禁区设置 v1.1.0");
        {
            AppRestrictedArea area;
            area.version = 0;
            area.count = data[4];
            area.type.push_back(0);
            area.name.push_back("");
            uint8_t *start = &data[5];
            for (int i = 0; i < area.count; i++)
            {
                area.mode.push_back(*start++);
                Polygon polygon;
                polygon.count = *start++;
                for (int j = 0; j < polygon.count; j++)
                {
                    int x = nbto16(start);
                    start += 2;
                    int y = nbto16(start);
                    start += 2;
                    polygon.vetex.push_back(TuyaXYToMars(x, y));
                }
                area.polygon.emplace_back(polygon);
            }
            if (TuyaComm::Get()->Send("AC_RestrictedArea", &area))
            {
                TuyaReportRestrictedArea(dpId, 0x1B, &area);
            }
        }
        break;
    case 0x38: // 禁区设置 v1.2.0/1
    case 0x39:
    {
        LOGD(TAG, "Data: {}", spdlog::to_hex(data, data + len));
        AppRestrictedArea area;
        area.version = data[4];
        if (area.version == 1)
        {
            LOGD(TAG, "禁区设置 v1.2.0");
            area.type.push_back(0);
        }
        if (area.version == 2)
        {
            LOGD(TAG, "禁区设置 v1.2.1");
        }
        area.count = data[5];
        uint8_t *start = &data[6];
        for (int i = 0; i < area.count; i++)
        {
            area.mode.push_back(*start++);
            if (area.version == 2)
            {
                area.type.push_back(*start++);
            }
            Polygon polygon;
            polygon.count = *start++;
            for (int j = 0; j < polygon.count; j++)
            {
                int x = nbto16(start);
                start += 2;
                int y = nbto16(start);
                start += 2;
                polygon.vetex.push_back(TuyaXYToMars(x, y));
            }
            area.polygon.emplace_back(polygon);
            std::string name;
            int nameLen = *start++;
            for (int j = 0; j < nameLen; j++)
            {
                name.push_back(*(start + j));
            }
            start += 19;
            name.push_back('\0');
            area.name.push_back(name);
        }
        if (TuyaComm::Get()->Send("AC_RestrictedArea", &area))
        {
            TuyaReportRestrictedArea(dpId, 0x39, &area);
        }
    }
    break;
    case 0x1C: // 分区分割
    case 0x1D:
        LOGD(TAG, "分区分割");
        {
            AppPartitionDivision partition_division;
            uint8_t *start = &data[4];
            partition_division.roomId = *start++;
            int x0 = nbto16(start);
            start += 2;
            int y0 = nbto16(start);
            start += 2;
            partition_division.begin = TuyaXYToMars(x0, y0);
            int x1 = nbto16(start);
            start += 2;
            int y1 = nbto16(start);
            start += 2;
            partition_division.end = TuyaXYToMars(x1, y1);
            partition_division.ret = 0x01; // 0x00为分割失败、0x01为分割成功
            LOGD(TAG, "房间id:{}, x0:{}, y0:{}, x1:{}, y1:{}", partition_division.roomId, x0, y0, x1, y1);

            if (TuyaComm::Get()->Send("AC_PartitionDivision", &partition_division))
            {   
                TuyaReportPartitionDivision(dpId, 0x1D, &partition_division);
            }
        }
        break;
    case 0x1E: // 分区合并
    case 0x1F:
        LOGD(TAG, "分区合并");
        {
            AppPartitionMerge partition_merge;
            partition_merge.count = 2;
            partition_merge.roomId.push_back(data[4]);
            partition_merge.roomId.push_back(data[5]);
            // 0x00为合并失败、0x01为合并成功、0x02为不相邻的房间不可合并
            partition_merge.ret = 0x01;
            LOGD(TAG, "合并的区域id:{},{}", data[4], data[5]);
            if (TuyaComm::Get()->Send("AC_PartitionMerge", &partition_merge))
            {   
                TuyaReportPartitionMerge(dpId, 0x1F, &partition_merge);
            }
        }
        break;
    case 0x20: // 分区恢复默认
    case 0x21:
        LOGD(TAG, "分区恢复默认");
        {
            Event evt;
            evt.param = 0x01; // 0x00为分割失败、0x01为分割成功
            if (TuyaComm::Get()->Send("AC_PartitionRestoreDefault", &evt))
            {
                TuyaReportPartitionRestoreDefault(dpId, 0x21, &evt);
            }
        }
        break;
    case 0x22: // 设置房间属性
    case 0x23:
        LOGD(TAG, "设置房间属性");
        {
            AppSetRoomProperties room_properties;
            uint8_t *start = &data[4];
            room_properties.count = *start++;
            for (int i = 0; i < room_properties.count; ++i)
            {   
                room_properties.roomId.push_back(*start++);
                room_properties.fan.push_back(TuyaSuctionToMars((TuyaSuction)*start++));
                room_properties.waterLevel.push_back(TuyaCisternToMars((TuyaCistern)*start++));
                room_properties.yMop.push_back(*start++);
                room_properties.cleanRepeat.push_back(*start++);
                LOGD(TAG, "房间id:{}, 风机档位:{}, 水箱档位:{}, Y字形拖地:{}, 清扫次数:{}",
                     room_properties.roomId[i], room_properties.fan[i],
                     room_properties.waterLevel[i], room_properties.yMop[i], room_properties.cleanRepeat[i]);
            }
            if (TuyaComm::Get()->Send("AC_SetRoomProperties", &room_properties))
            {
                TuyaReportRoomProperties(dpId, 0x23, &room_properties);
            }
        }
        break;
    case 0x24: // 设置房间名称
    case 0x25:
    {
        LOGD(TAG, "设置房间名称");
        AppSetRoomName room_info;
        room_info.count = data[4];
        uint8_t *start = &data[5];
        for (int i = 0; i < room_info.count; i++)
        {
            room_info.roomId.push_back(*start++);
            std::string name;
            int name_len = *start++;
            for (int j = 0; j < name_len; j++)
            {
                name.push_back(*(start + j));
            }
            start += 19;
            name.push_back('\0');
            room_info.name.push_back(name);
            LOGD(TAG, "房间id:{} 房间名称:{} 长度:{}", room_info.roomId[i], name, name_len);
        }
        if (TuyaComm::Get()->Send("AC_SetRoomName", &room_info))
        {
            TuyaReportRoomName(dpId, 0x25, &room_info);
        }
    }
        break;
    case 0x26: // 设置清扫顺序
    case 0x27:
        LOGD(TAG, "设置清扫顺序");
        {
            AppSetCleaningSequence clean_sequence;
            clean_sequence.count = data[4];
            uint8_t *start = &data[5];
            for (int i = 0; i < clean_sequence.count; i++)
            {
                clean_sequence.roomId.push_back(*start++);
                LOGD(TAG, "清扫顺序:{} ", clean_sequence.roomId[i]);
            }
            if (TuyaComm::Get()->Send("AC_SetCleaningSequence", &clean_sequence))
            {
                TuyaReportCleaningSequence(dpId, 0x27, &clean_sequence);
            }
        }
        break;
    case 0x28: // 划区清扫 v1.1.0
    case 0x29:
        LOGD(TAG, "划区清扫 v1.1.0");
        {
            AppZoneClean zone;
            zone.version = 0;
            zone.cleanRepeat.push_back(data[4]);
            zone.order.push_back(0);
            zone.mode.push_back(0);
            zone.suction.push_back(0);
            zone.count = data[5];
            uint8_t *start = &data[6];

            for (int i = 0; i < zone.count; i++)
            {
                Polygon polygon;
                polygon.count = *start++;
                for (int j = 0; j < polygon.count; j++)
                {
                    int x = nbto16(start);
                    start += 2;
                    int y = nbto16(start);
                    start += 2;
                    polygon.vetex.push_back(TuyaXYToMars(x, y));
                }
                zone.zone.emplace_back(polygon);
            }

            if (zone.zone.empty())
                zone.zone.resize(1);
            if (zone.name.empty())
                zone.name.resize(1);
            if (zone.zoneID.empty())
                zone.zoneID.resize(1);
            if (zone.localSave.empty())
                zone.localSave.resize(1);
            if (zone.type.empty())
                zone.type.resize(1);
            if (zone.mode.empty())
                zone.mode.resize(1);
            if (zone.suction.empty())
                zone.suction.resize(1);
            if (zone.cistern.empty())
                zone.cistern.resize(1);
            if (zone.cleanRepeat.empty())
                zone.cleanRepeat.resize(1);
            if (zone.order.empty())
                zone.order.resize(1);

            if (TuyaComm::Get()->Send("AC_ZoneClean", &zone))
            {
                TuyaReportZoneClean(dpId, 0x29, &zone);
            }
        }
        break;
    case 0x3A: // 划区清扫 v1.2.0/1/2
    case 0x3B:
    {
        AppZoneClean zone;
        zone.version = data[4];
        if (zone.version == 1)
        {
            LOGD(TAG, "划区清扫 v1.2.0");

            zone.cleanRepeat.push_back(data[5]);
            zone.count = data[6];
            uint8_t *start = &data[7];

            for (int i = 0; i < zone.count; i++)
            {
                Polygon polygon;
                polygon.count = *start++;
                for (int j = 0; j < polygon.count; j++)
                {
                    int x = nbto16(start);
                    start += 2;
                    int y = nbto16(start);
                    start += 2;
                    polygon.vetex.push_back(TuyaXYToMars(x, y));
                }
                zone.zone.emplace_back(polygon);
                std::string name;
                int nameLen = *start++;
                for (int j = 0; j < nameLen; j++)
                {
                    name.push_back(*(start + j));
                }
                start += 19;
                name.push_back('\0');
                zone.name.push_back(name);
            }
        }
        if (zone.version == 2)
        {
            LOGD(TAG, "划区清扫 v1.2.1");

            zone.mode.push_back(data[5]);
            zone.cleanRepeat.push_back(data[6]);
            zone.suction.push_back(data[7]);
            zone.cistern.push_back(data[8]);
            zone.count = data[9];

            uint8_t *start = &data[10];
            for (int i = 0; i < zone.count; i++)
            {
                Polygon polygon;
                polygon.count = *start++;
                for (int j = 0; j < polygon.count; j++)
                {
                    int x = nbto16(start);
                    start += 2;
                    int y = nbto16(start);
                    start += 2;
                    polygon.vetex.push_back(TuyaXYToMars(x, y));
                }
                zone.zone.emplace_back(polygon);
                std::string name;
                int nameLen = *start++;
                for (int j = 0; j < nameLen; j++)
                {
                    name.push_back(*(start + j));
                }
                start += 19;
                name.push_back('\0');
                zone.name.push_back(name);
            }
        }
        if (zone.version == 3)
        {
            LOGD(TAG, "划区清扫 v1.2.2");
            zone.count = data[5];
            uint8_t *start = &data[6];
            for (int i = 0; i < zone.count; i++)
            {
                zone.zoneID.push_back(nbto32(start));
                start += 4;
                zone.localSave.push_back(*start++);
                zone.type.push_back(*start++);
                Polygon polygon;
                polygon.count = *start++;
                for (int j = 0; j < polygon.count; j++)
                {
                    int x = nbto16(start);
                    start += 2;
                    int y = nbto16(start);
                    start += 2;
                    polygon.vetex.push_back(TuyaXYToMars(x, y));
                }
                zone.cleanRepeat.push_back(*start++);
                zone.order.push_back(*start++);
                zone.mode.push_back(*start++);
                zone.suction.push_back(*start++);
                start += 5;
                std::string name;
                int nameLen = *start++;
                for (int j = 0; j < nameLen; j++)
                {
                    name.push_back(*(start + j));
                }
                start += 19;
                name.push_back('\0');
                zone.name.push_back(name);
            }
        }

        if (zone.zone.empty())
            zone.zone.resize(1);
        if (zone.name.empty())
            zone.name.resize(1);
        if (zone.zoneID.empty())
            zone.zoneID.resize(1);
        if (zone.localSave.empty())
            zone.localSave.resize(1);
        if (zone.type.empty())
            zone.type.resize(1);
        if (zone.mode.empty())
            zone.mode.resize(1);
        if (zone.suction.empty())
            zone.suction.resize(1);
        if (zone.cistern.empty())
            zone.cistern.resize(1);
        if (zone.cleanRepeat.empty())
            zone.cleanRepeat.resize(1);
        if (zone.order.empty())
            zone.order.resize(1);

        if (TuyaComm::Get()->Send("AC_ZoneClean", &zone))
        {
            TuyaReportZoneClean(dpId, 0x3B, &zone);
        }
    }
    break;
    case 0x30: // 本地定时功能 v1.0.0
    case 0x31:
        LOGD(TAG, "本地定时功能 v1.0.0");
        {
            AppLocalAlert alert;
            alert.verison = 0;
            alert.timeZone = data[4];
            alert.number = data[5];
            alert.timedTaskInfo.resize(alert.number);

            uint8_t *data_start = &data[6];
            for (int i = 0; i < alert.number; ++i)
            {
                alert.timedTaskInfo[i].valid = *data_start++;
                alert.timedTaskInfo[i].weekDayMask = *data_start++;
                alert.timedTaskInfo[i].time = nbto16(data_start);
                data_start = data_start + 2;
                alert.timedTaskInfo[i].roomN = *data_start++;
                uint8_t *start = data_start;
                for (int j = 0; j < alert.timedTaskInfo[i].roomN; j++)
                {   
                    alert.timedTaskInfo[i].roomId.push_back(*start++);
                }
                alert.timedTaskInfo[i].cleanMode = *start++;
                alert.timedTaskInfo[i].fan = *start++;
                alert.timedTaskInfo[i].water = *start++;
                alert.timedTaskInfo[i].cleanRepeat = *start++;
                data_start = start;

                if (alert.timedTaskInfo[i].roomId.empty())
                {
                    alert.timedTaskInfo[i].roomId.resize(1);
                }
                if (alert.timedTaskInfo[i].zoodId.empty())
                {
                    alert.timedTaskInfo[i].zoodId.resize(1);
                }
            }

            TimedTask::GetTimedTask()->AddTask(alert);

            if (TuyaComm::Get()->Send("AC_LocalAlert", &alert))
            {
                TuyaReportLocalAlert(dpId, 0x31, &alert);
            }
        }
        break;
    case 0x44: // 本地定时功能 v1.1.0
    case 0x45:
        LOGD(TAG, "本地定时功能 v1.1.0");
        {
            AppLocalAlert alert;
            alert.verison = data[4];
            alert.timeZone = data[5];
            alert.number = data[6];
            alert.timedTaskInfo.resize(alert.number);

            uint8_t *data_start = &data[7];
            for (int i = 0; i < alert.number; ++i)
            {
                alert.timedTaskInfo[i].valid = *data_start++;
                alert.timedTaskInfo[i].weekDayMask = *data_start++;
                alert.timedTaskInfo[i].time = nbto16(data_start);
                alert.timedTaskInfo[i].mapId = nbto16(data_start + 2);
                data_start = data_start + 4;
                alert.timedTaskInfo[i].roomN = *data_start++;
                uint8_t *start = data_start;
                for (int j = 0; j < alert.timedTaskInfo[i].roomN; j++)
                {
                    alert.timedTaskInfo[i].roomId.push_back(*start++);
                }
                alert.timedTaskInfo[i].zoomN = *start++;
                for (int j = 0; j < alert.timedTaskInfo[i].zoomN; j++)
                {
                    alert.timedTaskInfo[i].zoodId.push_back(nbto32(start));
                    start += 4;
                }
                alert.timedTaskInfo[i].cleanMode = *start++;
                alert.timedTaskInfo[i].fan = *start++;
                alert.timedTaskInfo[i].water = *start++;
                alert.timedTaskInfo[i].cleanRepeat = *start++;
                data_start = start;

                if (alert.timedTaskInfo[i].roomId.empty())
                {
                    alert.timedTaskInfo[i].roomId.resize(1);
                }
                if (alert.timedTaskInfo[i].zoodId.empty())
                {
                    alert.timedTaskInfo[i].zoodId.resize(1);
                }
            }

            TimedTask::GetTimedTask()->AddTask(alert);

            if (TuyaComm::Get()->Send("AC_LocalAlert", &alert))
            {
                TuyaReportLocalAlert(dpId, 0x45, &alert);
            }
        }
        break;
    case 0x32: // 勿扰时间设置 v1.0.0
    case 0x33:
        LOGD(TAG, "勿扰时间设置 v1.0.0");
        {
            AppNotDisturbTime time;
            TimedTask::GetTimedTask()->GetNotDisturbTime(time);
            time.version = 0;
            time.timeZone = data[4];
            time.starTime = nbto16(&data[5]);
            time.startDay = data[7];
            time.endTime = nbto16(&data[8]);
            time.endDay = data[10];
            TimedTask::GetTimedTask()->SetNotDisturbTime(time);
            if (TuyaComm::Get()->Send("AC_NotDisturbTime", &time))
            {
                TuyaReportNotDisturbTime(dpId, 0x33, &time);
            }
        }
        break;
    case 0x40: // 勿扰时间设置 v1.1.0
    case 0x41:
        LOGD(TAG, "勿扰时间设置 v1.1.0");
        {
            AppNotDisturbTime time;
            time.version = data[4];
            time.onoff = data[5];
            time.timeZone = data[6];
            time.starTime = nbto16(&data[7]);
            time.startDay = data[9];
            time.endTime = nbto16(&data[10]);
            time.endDay = data[12];
            TimedTask::GetTimedTask()->SetNotDisturbTime(time);
            if (TuyaComm::Get()->Send("AC_NotDisturbTime", &time))
            {
                TuyaReportNotDisturbTime(dpId, 0x41, &time);
            }
        }
        break;
    case 0x36: // AI 物体识别上报
    case 0x37:
        LOGD(TAG, "AI 物体识别上报");
        break;
    case 0x3C: // 快速建图
    case 0x3D:
        LOGD(TAG, "快速建图");
        {
            Event event;
            event.event = MSG_QUICK_MAPPING;
            if (TuyaComm::Get()->Send("AC_MapEvent", &event))
            {
                TuyaReportAutoMappingResult(dpId, 0x3D, 0x01, 0x00);
            }
            else
            {
                TuyaReportAutoMappingResult(dpId, 0x3D, 0x00, 0x01);
            }
        }
        break;
    case 0x42: // 重置当前地图
    case 0x43:
        LOGD(TAG, "重置当前地图");
        {
            Event event;
            event.event = MSG_RESET_MAP;
            if (TuyaComm::Get()->Send("AC_MapEvent", &event))
            {
                TuyaReportMapResetResult(dpId, 0x43, 0x01, 0x00);
            }
            else
            {
                TuyaReportMapResetResult(dpId, 0x43, 0x00, 0x01);
            }
        }
        break;
    case 0x46: // 基站控制设置
    case 0x47:
        LOGD(TAG, "基站控制设置");
        break;
    case 0x99: // 获取所有配置(预留)
        LOGD(TAG, "获取所有配置(预留)");
        break;
    default:
        break;
    }
}

void TuyaReportMapSaveResult(int dpId, uint8_t cmd, uint8_t ret)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAB);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(ret);
    raw[5] = raw.size() - 6;
    uint8_t sum = 0;
    for (int i = 6; i < raw.size(); i++)
    {
        sum += raw[i];
    }
    raw.push_back(sum);
    LOGD(TAG, "Send: {}", spdlog::to_hex(raw.begin(), raw.end()));
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

// 拓展功能
void TuyaHandleExtentedFuction(int dpId, uint8_t *data, int len)
{
    // 校验数据

    switch (data[6])
    {
    case 0x2A: // 保存地图
        LOGD(TAG, "保存地图 in {}", data[7]);
        Event event;
        event.event = MSG_SAVE_MAP;
        if (TuyaComm::Get()->Send("AC_MapEvent", &event))
        {
            // Save space：保存的空间，00：保存到本地，01：保存到云端
            // ret：0x00保存失败 、0x01保存成功、0x02：本地空间已满
            LOGD(TAG, "保存成功");
            TuyaReportMapSaveResult(dpId, 0x2B, 0x01);
        }
        else
        {
            LOGD(TAG, "保存失败");
            TuyaReportMapSaveResult(dpId, 0x2B, 0x00);
        }
        break;
    case 0x2C: // 删除地图
        LOGD(TAG, "删除地图");
        break;
    case 0x34: // 语音包下载
        LOGD(TAG, "语音包下载");
        break;
    default:
        break;
    }
}