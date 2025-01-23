#include <stdint.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <netinet/in.h>
#include <openssl/md5.h>
#include "tuya_robot.h"

#include "utils/log_.h"
#include "utils/json.hpp"
#include "lz4/lz4.h"

#include "tuya_cloud_com_defs.h"
#include "tuyaos_sweeper_api.h"

#include "tuya.h"
#include "tuya_enums.h"
#include "tuya_utils.h"
#include "tuya_dp_function.h"
#include "robot_msg.h"
#include "voice.h"
#include "curl/curl.h"
// DP 点
#undef TAG
#define TAG "DP"


#include "voice.h"
#include "mars_message/Event.hpp"
void PlayVoice(int v, int param)
{
    Event voice;
    voice.event = v;
    voice.param = param;
    TuyaComm::Get()->Publish("Voice", &voice);
}

using json = nlohmann::json;

typedef std::function<int(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)> curl_info_cb;

curl_info_cb global_progress_callback;
// 包装函数，用于调用 std::function 对象
int progress_callback_wrapper(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    if (global_progress_callback) {
        return global_progress_callback(clientp, dltotal, dlnow, ultotal, ulnow);
    }
    return 0;
}

int download_file(const char *file_name, const char *url, curl_info_cb info_cb);

// 计算文件的 MD5 哈希值
std::string md5sum(const std::string& file_path) {
    unsigned char c[MD5_DIGEST_LENGTH];
    MD5_CTX mdContext;
    MD5_Init(&mdContext);

    std::ifstream file(file_path, std::ifstream::binary);
    if (!file) {
        LOGD(TAG, "Failed to open file: {}", file_path);
        return "";
    }

    char buffer[1024];
    while (file.good()) {
        file.read(buffer, sizeof(buffer));
        MD5_Update(&mdContext, buffer, file.gcount());
    }

    MD5_Final(c, &mdContext);

    std::ostringstream md5_string;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        md5_string << std::hex << std::setw(2) << std::setfill('0') << (int)c[i];
    }

    return md5_string.str();
}

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

// true：清扫启动 false：清扫停止
TY_OBJ_DP_S DP_ReportSwichGo(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleSwichGo(TY_OBJ_DP_S *dp)
{
    LOGD(TAG, "SwichGo:{}", dp->value.dp_bool);
    tuya_message::Request req = {};
    tuya_message::Result res = {};
    if (dp->value.dp_bool)
    {
        TuyaComm::Get()->Send("ty_start", &req, &res);
    }
    else
    {
        TuyaComm::Get()->Send("ty_stop", &req, &res);
    }
}

// true：暂停当前工作状态 false：继续当前工作状态
TY_OBJ_DP_S DP_ReportPause(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandlePause(TY_OBJ_DP_S *dp)
{
    tuya_message::Request req = {};
    tuya_message::Result res = {};
    if (dp->value.dp_bool)
    {
        TuyaComm::Get()->Send("ty_pause", &req, &res);
    }
    else
    {
        TuyaComm::Get()->Send("ty_start", &req, &res);
    }
}

// true：回充启动 false：回充停止
TY_OBJ_DP_S DP_ReportSwichCharge(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleSwichCharge(TY_OBJ_DP_S *dp)
{
    tuya_message::Request req = {};
    tuya_message::Result res = {};
    if (dp->value.dp_bool)
    {
        TuyaComm::Get()->Send("ty_charge", &req, &res);
    }
    else
    {
        TuyaComm::Get()->Send("ty_pause", &req, &res);
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
    // 这个功能没有意义
    LOGD(TAG, "mode = {}", mode);
}

/*
默认值（不可修改删除，否则影响控制面板逻辑）：
standby - 待机中
smart - 自动清扫中
zone_clean - 划区清扫中
part_clean - 局部清扫中
cleaning - 清扫中 (备选) paused - 已暂停
goto_pos - 前往目标点中
pos_arrived - 目标点已到达 4-
pos_unarrive - 目标点不可达
goto_charge - 寻找充电座中
charging - 充电中
charge_done - 充电完成
sleep - 休眠
select_room - 选区清扫中
seek_dust_bucket - 寻找集尘桶中
collecting_dust - 集尘中 可在后面增加其他状态值，状态值尽量是状态的英文小写单词，如需要增加故障中状态，则为 in_trouble
-*/

TY_OBJ_DP_S DP_ReportStatus(int dpId, int status)
{
    return DPReportEnum(dpId, status);
}

/*
默认区间为 0min—9999 分钟（没有小数点），
如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为 1，上报的数值会经过计算，保留一位小数，
例如上报 589，经过换算后为 58.9min
*/
TY_OBJ_DP_S DP_ReportCleanTime(int dpId, int second)
{
    return DPReportValue(dpId, second / 60);
}

/*
默认区间为 0㎡—9999㎡（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为 1，上报的数值会经过计算，保留一位小数，
例如上报 589，经过换算后为 58.9㎡
*/
TY_OBJ_DP_S DP_ReportCleanArea(int dpId, float cleanArea)
{
    return DPReportValue(dpId, cleanArea);
}

/*
默认区间为 0%—100%（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为 1，上报的数值会经过计算，保留一位小数，
例如上报 589，经过换算后为 58.9%
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
如需要增加自动状态，则为 auto
*/
TY_OBJ_DP_S DP_ReportSuction(int dpId, int power)
{
    return DPReportEnum(dpId, power);
}
void DP_HandleSuction(TY_OBJ_DP_S *dp)
{
    int suction = dp->value.dp_enum;
    tuya_message::Request req = {suction};
    tuya_message::Result res = {};
    TuyaComm::Get()->Send("ty_set_fan", &req, &res);
}

/*
默认值（可修改、删除、增加）：
closed - 关闭
low - 低
middle - 中
high - 高
选项排列与此处值的排列顺序一致，
可调整排列顺序，可删减值，可增加其他状态值，状态值尽量是状态的英文小写单词，
如需要增加自动状态，则为 auto
*/
TY_OBJ_DP_S DP_ReportCistern(int dpId, int cistern)
{
    return DPReportEnum(dpId, cistern);
}
void DP_HandleCistern(TY_OBJ_DP_S *dp)
{
    int cistern = dp->value.dp_enum;
    tuya_message::Request req = {cistern};
    tuya_message::Result res = {};
    TuyaComm::Get()->Send("ty_set_watertank", &req, &res);
}

TY_OBJ_DP_S DP_ReportSeek(int dpId, bool b)
{
    return DPReportBool(dpId, b);
}
void DP_HandleSeek(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    PlayVoice(V_SEEK_ROBOT, 3);

    TY_OBJ_DP_S d = DP_ReportSeek(dp->dpid, 0);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
枚举值：forward, backward, turn_left, turn_right, stop, exit
默认值（不可修改、删除、增加），按键按下下发对应的方向值，松手下发停
*/
TY_OBJ_DP_S DP_ReportDirectionControl(int dpId, int direction)
{
    return DPReportEnum(dpId, direction);
}
void DP_HandleDirectionControl(TY_OBJ_DP_S *dp)
{
    int direction = dp->value.dp_enum;
    tuya_message::Request req = {direction};
    tuya_message::Result res = {};
    TuyaComm::Get()->Send("ty_direction", &req, &res);

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
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_reset_map", &req, &res))
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
    if (data[0] == 0xAA)
    {
        TuyaHandleStandardFunction(dpId, data, len);
    }
    if (data[0] == 0xAB)
    {
        TuyaHandleExtentedFuction(dpId, data, len);
    }
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
默认区间为 0min—9000min（不支持小数点）注意：选择了该 DP 点，必须选择对应的重置 DP 点
*/
TY_OBJ_DP_S DP_ReportEdgeBrushLife(int dpId, int life)
{
    return DPReportValue(dpId, life);
}
void DP_HandleEdgeBrushLife(TY_OBJ_DP_S *dp)
{
    int life = dp->value.dp_value;
    tuya_message::Request req = {};
    mars_message::AppPartsLife parts = {0};
    // todo
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
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_parts_life_reset", &parts))
    {
        TY_OBJ_DP_S d = DP_ReportEdgeBrushLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能未实现");
    }
}

/*
滚刷剩余寿命
默认区间为 0min—18000min（不支持小数点）注意：选择了该 DP 点，必须选择对应的重置 DP 点
*/
TY_OBJ_DP_S DP_ReportRollBrushLife(int dpId, int life)
{
    return DPReportValue(dpId, life);
}
void DP_HandleRollBrushLife(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    AppPartsLife parts = {0};
    parts.mainBrushLifeMinutes = 18000;
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_parts_life_reset", &parts))
    {
        TY_OBJ_DP_S d = DP_ReportRollBrushLife(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能未实现");
    }
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
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_parts_life_reset", &parts, &res))
    {
        TY_OBJ_DP_S d = DP_ReportRollBrushLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能未实现");
    }
}

/*
滤网/海帕剩余寿命
默认区间为 0min—9000min（不支持小数点）注意：选择了该 DP 点，必须选择对应的重置 DP 点
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
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_parts_life_reset", &parts, &res))
    {
        TY_OBJ_DP_S d = DP_ReportEdgeBrushLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能未实现");
    }
}

/*
拖布剩余寿命
默认区间为 0min—9000min（不支持小数点）注意：选择了该 DP 点，必须选择对应的重置 DP 点
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
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_parts_life_reset", &parts, &res))
    {
        TY_OBJ_DP_S d = DP_ReportEdgeBrushLifeReset(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
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
    tuya_message::Result res = {};
    tuya_message::Request req = {b};
    if (TuyaComm::Get()->Send("ty_dnd_on", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
默认区间为 0—100，可自行修改区间，不支持小数点
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
    evt.param = value;
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("Volume", &evt, &res))
    {
        TY_OBJ_DP_S d = DPReportValue(dp->dpid, value);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能未实现");
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
    LOGD(TAG, "{}断点续扫", b ? "开启" : "关闭");
    tuya_message::Request evt = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_continue_clean", &evt, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能未实现");
    }
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
如需要增加故障中状态，则为 in_trouble
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
默认区间为 0㎡—99999㎡（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为 1，上报的数值会经过计算，保留一位小数，
例如上报 589，经过换算后为 58.9㎡
*/
TY_OBJ_DP_S DP_ReportCleanAreaTotal(int dpId, int cleanAreaTotal)
{
    return DPReportValue(dpId, cleanAreaTotal);
}

/*
默认区间为 0—99999（没有小数点）
*/
TY_OBJ_DP_S DP_ReportCleanCountTotal(int dpId, int cleanCountTotal)
{
    return DPReportValue(dpId, cleanCountTotal);
}

/*
默认区间为 0min—99999min（没有小数点），如果需要显示为小数点，可以修改倍数参数，
例如需要支持一位小数点，则倍数参数为 1，上报的数值会经过计算，保留一位小数，
例如上报 589，经过换算后为 58.9.min
*/
TY_OBJ_DP_S DP_ReportCleanTimeTotal(int dpId, int cleanTimeTotalSecond)
{
    return DPReportValue(dpId, cleanTimeTotalSecond / 60);
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

// status: 0x00 为下载失败、0x01 为下载安装中、0x02 为安装成功、0x03 为使用中
void TuyaReportVoiceDownLoadResult(int dpId, uint8_t version, uint32_t langId, uint32_t fileVersion, uint8_t status, uint8_t schedu)
{
   std::vector<uint8_t> raw;
    raw.push_back(0xAB);
    raw.push_back(version);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(0x35);
    raw.push_back(langId >> 24);
    raw.push_back(langId >> 16);
    raw.push_back(langId >> 8);
    raw.push_back(langId);
    if(version == 0x01)
    {
        raw.push_back(fileVersion >> 24);
        raw.push_back(fileVersion >> 16);
        raw.push_back(fileVersion >> 8);
        raw.push_back(fileVersion);
    }
    raw.push_back(status);
    raw.push_back(schedu);
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

void DPRaw_HandleVoiceData(int dpId, uint8_t *data, int len)
{
    LOGD(TAG, "DPRaw_HandleVoiceData : {}", spdlog::to_hex(data, data + len));
    DPRaw_HandleCommand(dpId, data, len);
}

void DP_HandleLanguage(TY_OBJ_DP_S *dp)
{
    int lang = dp->value.dp_enum;
}

/*
集尘频率设置
默认区间为 0—99999（没有小数点），该功能不支持小数点
*/
void DP_HandleDustCollectionNum(TY_OBJ_DP_S *dp)
{
    int value = dp->value.dp_value;
    LOGD(TAG, "集尘频率 {}", value);
    tuya_message::Request req = {value};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_set_auto_dust", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportValue(dp->dpid, value);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
集尘宝
点击立即集尘，下发 true
*/
void DP_HandleDustCollectionSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "集尘宝 {}", b);
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_dust_collect", &req, &res)) // todo
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
自定义模式
用于设置是否需要使用自定义的房间模式进行工作，如果需要使用自定义模式，则需要打开该开关
*/
TY_OBJ_DP_S DP_ReportCustomizeModeSwitch(int dpId, bool state)
{
    return DPReportBool(dpId, state);
}
void DP_HandleCustomizeModeSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "自定义清扫 {}", b);
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_customize_clean", &req, &res)) // todo
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
枚举值：none, installed
*/
TY_OBJ_DP_S DP_ReportMopState(int dpId, int state)
{
    return DPReportEnum(dpId, state);
}

/*
枚举值：both_work, only_sweep, only_mop
*/
TY_OBJ_DP_S DP_ReportWorkMode(int dpId, int workMode)
{
    return DPReportEnum(dpId, workMode);
}
void DP_HandleWorkMode(TY_OBJ_DP_S *dp)
{
    int workMode = dp->value.dp_enum;
    LOGD(TAG, "工作模式 {}", workMode);
    Event evt;
    evt.event = workMode;
    if (TuyaComm::Get()->Send("AC_WorkMode", &evt)) // 不变
    {
        TY_OBJ_DP_S d = DPReportEnum(dp->dpid, workMode);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
单位设置
枚举值：square_meter, square_foot
*/
TY_OBJ_DP_S DP_ReportUnitSet(int dpId, int unit)
{
    return DPReportEnum(dpId, unit);
}
void DP_HandleUnitSet(TY_OBJ_DP_S *dp)
{
    int unit = dp->value.dp_enum;
    LOGD(TAG, "单位设置：{} ", unit == 0 ? "平方米" : "平方英尺");
    Event evt;
    evt.event = unit;
    TY_OBJ_DP_S d = DPReportEnum(dp->dpid, unit);
    dev_report_dp_json_async(NULL, &d, 1);
}

/*
仅在激光扫地机公版面板-V2 使用
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
如需要增加故障中状态，则为 in_trouble
*/
TY_OBJ_DP_S DP_ReportCarpetCleanPrefer(int dpId, int prefer)
{
    return DPReportEnum(dpId, prefer);
}
void DP_HandleCarpetCleanPrefer(TY_OBJ_DP_S *dp)
{
    int prefer = dp->value.dp_enum;
    LOGD(TAG, "地毯清洁偏好 {}", prefer);
    tuya_message::Request req = {prefer};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_carpet_clean_prefer", &req, &res)) // todo
    {
        if(res.code == 0)
        {
            TY_OBJ_DP_S d = DPReportEnum(dp->dpid, prefer);
            dev_report_dp_json_async(NULL, &d, 1);
        }
        
    }
    else
    {
        LOGE(TAG, "功能无实现");
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
    LOGD(TAG, "{}地毯增压", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_carpet_pressure", &req, &res))
    {
        TY_OBJ_DP_S d = DP_ReportAutoBoost(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
巡航
*/
void DP_HandleCruiseSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    // 没有这个功能
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
    LOGD(TAG, "{}童锁", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_set_dust_child_lock", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
Y 字型拖地
*/
void DP48_HandleYMop(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    // 没有这个功能
}

/*
拖布自清洁
*/
void DP_HandleSelfCleanMop(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}拖布自清洁", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_turnon_clean_mop", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
烘干开关
*/
void DP_HandleDryingSwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}烘干", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_turnon_dry_mop", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, res.a);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
拖布自清洁频率
数值范围：1-10, 间距：1, 倍数：0, 单位：㎡
*/
void DP_HandleMopSlefCleaningFrequency(TY_OBJ_DP_S *dp)
{
    int value = dp->value.dp_value;
    LOGD(TAG, "拖布自清洁频率：{}", value);
    tuya_message::Request req = {value};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_set_wash_mop_frequency", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportValue(dp->dpid, value);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
拖布自清洁强度
fast, daily, depth
*/
void DP_HandleMopSlefCleaningStrength(TY_OBJ_DP_S *dp)
{
    int prefer = dp->value.dp_enum;
    LOGD(TAG, "拖布自清洁强度：{}", prefer);
    tuya_message::Request req = {prefer};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_set_wash_mop_strength", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportEnum(dp->dpid, prefer);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
擦地强度
slow, normal, fast
*/
void DP_HandleWipingStrength(TY_OBJ_DP_S *dp)
{
    int prefer = dp->value.dp_enum;
    LOGD(TAG, "擦地强度：{}", prefer);
    tuya_message::Request req = {prefer};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_set_wipe_strength", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportEnum(dp->dpid, prefer);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

void DP_HandleSmartEdge(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}智能沿边", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_smart_edge", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

void DP_HandleUploadLogs(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}上传日志", b ? "开启" : "关闭");
    if(b)
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
        d = DPReportBool(dp->dpid, 0);
        // tuya_message::Request req = {b};
        // tuya_message::Result res = {};
        // TuyaComm::Get()->Send("ty_upload_logs", &req, &res);
        std::function<void()> f = [d]() {
            system("./logupdate.sh");
            sleep(3);
            dev_report_dp_json_async(NULL, &d, 1);
        };
        std::thread(f).detach();
    }
}

/*
少碰撞，标准覆盖，多覆盖
*/
void DP_HandleAvoidObsMode(TY_OBJ_DP_S *dp)
{
    int mode = dp->value.dp_enum;
    LOGD(TAG, "避障模式：{}", mode);
    tuya_message::Request req = {mode};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_set_obs_mode", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportEnum(dp->dpid, mode);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

/*
*/
void DP_HandleKeyLightDisplaySwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}按键灯显", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if(TuyaComm::Get()->Send("ty_key_light_onoff", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

void DP_HandleStationLightDisplaySwitch(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}基站灯显", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if(TuyaComm::Get()->Send("ty_station_light_onoff", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

void DP_HandleStationHotFan(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}基站热风", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_turnon_dry_station", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

void DP_HandleStationHotWater(TY_OBJ_DP_S *dp)
{
    bool b = dp->value.dp_bool;
    LOGD(TAG, "{}基站热水", b ? "开启" : "关闭");
    tuya_message::Request req = {b};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_turnon_station_hotwater", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportBool(dp->dpid, b);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

void DP_HandleStationHotFanTime(TY_OBJ_DP_S *dp)
{
    int value = dp->value.dp_value;
    LOGD(TAG, "基站热风时间：{}", value);
    tuya_message::Request req = {value};
    tuya_message::Result res = {};
    if (TuyaComm::Get()->Send("ty_set_sttaion_hotfan_time", &req, &res))
    {
        TY_OBJ_DP_S d = DPReportValue(dp->dpid, value);
        dev_report_dp_json_async(NULL, &d, 1);
    }
    else
    {
        LOGE(TAG, "功能无实现");
    }
}

// 标准功能
void DPRaw_Send(int dpId, uint8_t *data, int len, int timeoutMs);

std::vector<uint8_t> ToVirtualWallData(uint8_t cmd, AppVirtualWall *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    if (msg->version == 0)
    {
        // LOGD(TAG, "上报虚拟墙设置 v1.0.0");
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
    return raw;
}

void TuyaReportVirtualWall(int dpId, uint8_t cmd, AppVirtualWall *msg)
{
    auto raw = ToVirtualWallData(cmd, msg);
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
}

std::vector<uint8_t> ToRestrictedAreaData(uint8_t cmd, AppRestrictedArea *msg)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);

    LOGD(TAG, "msg->version = {}", msg->version);
    if (msg->version == 0)
    {
        LOGD(TAG, "上报禁区设置 v1.1.0");
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
        }
        raw[2] = raw.size() - 3;
        uint8_t sum = 0;
        for (int i = 3; i < raw.size(); i++)
        {
            sum += raw[i];
        }
        raw.push_back(sum);

        // LOGD(TAG, "Data: {}", spdlog::to_hex(raw.begin(), raw.end()));
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
    }
    return raw;
}
void TuyaReportRestrictedArea(int dpId, uint8_t cmd, AppRestrictedArea *msg)
{
    auto raw = ToRestrictedAreaData(cmd, msg);
    DPRaw_Send(dpId, &raw[0], raw.size(), 100);
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
        for (auto const &task_info : msg->timedTaskInfo)
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
        for (auto const &task_info : msg->timedTaskInfo)
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
        auto d = tonb16(msg->starTime.hour << 8 | msg->starTime.minute);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->startWeekDay);
        d = tonb16(msg->endTime.hour << 8 | msg->endTime.minute);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->endWeekDay);
    }
    if (msg->version == 1)
    {
        LOGD(TAG, "上报勿扰时间设置 v1.1.0");
        raw.push_back(msg->version);
        raw.push_back(msg->timeZone);
        raw.push_back(msg->onoff);
        auto d = tonb16(msg->starTime.hour << 8 | msg->starTime.minute);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->startWeekDay);
        d = tonb16(msg->endTime.hour << 8 | msg->endTime.minute);
        raw.insert(raw.end(), d.begin(), d.end());
        raw.push_back(msg->endWeekDay);
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

void TuyaReportPartitionDivision(int dpId, uint8_t cmd, int ret)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(ret);

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

void TuyaReportPartitionRestoreDefault(int dpId, uint8_t cmd, int ret)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(ret);

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

void TuyaReportPartitionMerge(int dpId, uint8_t cmd, int ret)
{
    std::vector<uint8_t> raw;
    raw.push_back(0xAA);
    raw.push_back(0x00);
    raw.push_back(0x00);
    raw.push_back(cmd);
    raw.push_back(ret);

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
    if (ret = 0x00)
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
    if (ret = 0x00)
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

void TuyaHandleStandardFunction(int dpId, uint8_t *d, int len)
{
    LOGD(TAG, "TuyaHandleStandardFunction len={}", len);
    LOGD(TAG, "Recv: {}", spdlog::to_hex(d, d + len));
    std::vector<uint8_t> data(d, d + len);
    // check data
    try
    {
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
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_virtual_wall", &wall, &res))
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
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_virtual_wall", &wall, &res))
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
                if (room.cleanRepeat > 2)
                {
                    room.cleanRepeat = 1;
                }
                room.count = data[5];
                for (int i = 0; i < room.count; i++)
                {
                    room.id.push_back((data[6 + i]));
                }
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_clean_blocks", &room, &res))
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
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_point_point", &sc, &res))
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
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_point_point", &sc, &res))
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
                uint8_t *start = &data[5];
                for (int i = 0; i < area.count; i++)
                {
                    area.mode.push_back(*start++);
                    area.type.push_back(0);
                    area.name.push_back("");
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
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_restricted_area", &area, &res))
                {
                    TuyaReportRestrictedArea(dpId, 0x1B, &area);
                }
            }
            break;
        case 0x38: // 禁区设置 v1.2.0/1
        case 0x39:
        {
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
            tuya_message::Result res;
            if (TuyaComm::Get()->Send("ty_restricted_area", &area, &res))
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
                partition_division.ret = 0x01; // 0x00 为分割失败、0x01 为分割成功
                LOGD(TAG, "房间 id:{}, x0:{}, y0:{}, x1:{}, y1:{}", partition_division.roomId, x0, y0, x1, y1);

                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_split_partition", &partition_division, &res, 1000))
                {
                    TuyaReportPartitionDivision(dpId, 0x1D, res.code == 0);
                }
            }
            break;
        case 0x1E: // 分区合并
        case 0x1F:
            LOGD(TAG, "分区合并");
            {
                AppPartitionMerge partition_merge;
                partition_merge.count = 2;
                partition_merge.roomId.push_back((data[4]));
                partition_merge.roomId.push_back((data[5]));
                // 0x00 为合并失败、0x01 为合并成功、0x02 为不相邻的房间不可合并
                partition_merge.ret = 0x01;
                LOGD(TAG, "合并的区域 id:{},{}", data[4], data[5]);
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_merge_partition", &partition_merge, &res))
                {
                    TuyaReportPartitionMerge(dpId, 0x1F, res.code == 0);
                }
            }
            break;
        case 0x20: // 分区恢复默认
        case 0x21:
            LOGD(TAG, "分区恢复默认");
            {
                tuya_message::Request req;
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_re_split_region", &req, &res, 3000))
                {
                    TuyaReportPartitionRestoreDefault(dpId, 0x21, res.code == 0);
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
                    room_properties.enable.push_back(1);
                    room_properties.fan.push_back(((TuyaSuction)*start++));
                    room_properties.waterLevel.push_back(((TuyaCistern)*start++));
                    room_properties.yMop.push_back(*start++);
                    room_properties.cleanRepeat.push_back(*start++);
                    LOGD(TAG, "房间 id:{}, 风机档位:{}, 水箱档位:{}, Y 字形拖地:{}, 清扫次数:{}",
                         room_properties.roomId[i], room_properties.fan[i],
                         room_properties.waterLevel[i], room_properties.yMop[i], room_properties.cleanRepeat[i]);
                }
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_set_room_properties", &room_properties, &res))
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
                LOGD(TAG, "房间 id:{} 房间名称:{} 长度:{}", room_info.roomId[i], name, name_len);
            }
            tuya_message::Result res;
            if (TuyaComm::Get()->Send("ty_set_room_name", &room_info, &res))
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
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_clean_sequence", &clean_sequence, &res))
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

                    zone.cleanRepeat.push_back(data[4]);
                    zone.order.push_back(0);
                    zone.mode.push_back(0);
                    zone.suction.push_back(0);
                }

                if (zone.zone.empty())
                    zone.zone.resize(1);
                if (zone.name.empty())
                    zone.name.resize(zone.count);
                if (zone.zoneID.empty())
                    zone.zoneID.resize(zone.count);
                if (zone.localSave.empty())
                    zone.localSave.resize(zone.count);
                if (zone.type.empty())
                    zone.type.resize(zone.count);
                if (zone.mode.empty())
                    zone.mode.resize(zone.count);
                if (zone.suction.empty())
                    zone.suction.resize(zone.count);
                if (zone.cistern.empty())
                    zone.cistern.resize(zone.count);
                if (zone.cleanRepeat.empty())
                    zone.cleanRepeat.resize(zone.count);
                if (zone.order.empty())
                    zone.order.resize(zone.count);
                

                tuya_message::Result res;
                LOGL(TAG);
                if (TuyaComm::Get()->Send("ty_zone_clean", &zone, &res))
                {
                    LOGL(TAG);
                    TuyaReportZoneClean(dpId, 0x29, &zone);
                }
                LOGL(TAG);
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

            tuya_message::Result res;
            if (TuyaComm::Get()->Send("ty_zone_clean", &zone, &res))
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
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_set_m_orders", &alert, &res))
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

                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_set_m_orders", &alert, &res))
                {
                    TuyaReportLocalAlert(dpId, 0x31, &alert);
                }
                else
                {
                    LOGE(TAG, "ty_set_m_orders failed");
                }
            }
            break;
        case 0x32: // 勿扰时间设置 v1.0.0
        case 0x33:
            LOGD(TAG, "勿扰时间设置 v1.0.0");
            {
                AppNotDisturbTime time;
                // TimedTask::GetTimedTask()->GetNotDisturbTime(time);
                time.version = 0;
                time.timeZone = data[4];
                time.starTime.hour = nbto16(&data[5]) >> 8;
                time.starTime.minute = nbto16(&data[5]) & 0xFF;
                time.startWeekDay = data[7];
                time.endTime.hour = nbto16(&data[8]) >> 8;
                time.endTime.minute = nbto16(&data[8]) & 0xFF;
                time.endWeekDay = data[10];
                // TimedTask::GetTimedTask()->SetNotDisturbTime(time);
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_set_dnd", &time, &res))
                {
                    TuyaReportNotDisturbTime(dpId, 0x33, &time);
                }
                else
                {
                    LOGE(TAG, "ty_set_dnd failed");
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
                time.starTime.hour = nbto16(&data[7]) >> 8;
                time.starTime.minute = nbto16(&data[7]) & 0xFF;
                time.startWeekDay = data[9];
                time.endTime.hour = nbto16(&data[10]) >> 8;
                time.endTime.minute = nbto16(&data[10]) & 0xFF;
                time.endWeekDay = data[12];
                // TimedTask::GetTimedTask()->SetNotDisturbTime(time);
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_set_dnd", &time, &res))
                {
                    TuyaReportNotDisturbTime(dpId, 0x41, &time);
                }
                else
                {
                    LOGE(TAG, "ty_set_dnd failed");
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
                tuya_message::Request req;
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_start_quick_mapping", &req, &res))
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
                tuya_message::Request req = {0};
                tuya_message::Result res;
                if (TuyaComm::Get()->Send("ty_reset_map", &req, &res))
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
        case 0x99: // 获取所有配置 (预留)
            LOGD(TAG, "获取所有配置 (预留)");
            break;
        default:
            break;
        }
    }
    catch (const std::exception &e)
    {
        LOGE(TAG, "TuyaHandleStandardFunction error:{}", e.what());
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
int TuyaUpdateMapFile(int currId, uint32_t tuyaMapId)
{
     std::string mapBin = "/tmp/map.bin";
    std::ifstream mapFile(mapBin, std::ios::binary);
    if (!mapFile.is_open())
    {
        LOGE(TAG, "Failed to open map file: {}", mapBin);
        return -1;
    }
    std::vector<uint8_t> mapData((std::istreambuf_iterator<char>(mapFile)), std::istreambuf_iterator<char>());
    mapFile.close();

    // 获取虚拟墙数据
    AppVirtualWall wall;
    std::vector<uint8_t> tuyaVirtualWall;
    wall.version = 0;
    wall.count = 0;
    tuya_message::Request req = {};
    if (TuyaComm::Get()->Send("ty_get_virtual_wall", &req, &wall) && wall.version >= 0)
    {
        tuyaVirtualWall = ToVirtualWallData(wall.version == 0 ? 0x13 : 0x49, &wall);
    }
    else
    {
        LOGE(TAG, "无法获取到虚拟墙信息");
    }

    // 获取禁区数据
    AppRestrictedArea area;
    std::vector<uint8_t> tuyaRestrictedArea;
    if (TuyaComm::Get()->Send("ty_get_restricted_area", &req, &area) && area.version >= 0)
    {
        tuyaRestrictedArea = ToRestrictedAreaData(area.version == 0 ? 0x1B : 0x39, &area);
    }
    else
    {
        LOGE(TAG, "无法获取到禁区/划区信息");
    }

    // 拼接 mapData, tuyaVirtualWall 和 tuyaRestrictedArea
    std::vector<uint8_t> combinedData;
    combinedData.insert(combinedData.end(), mapData.begin(), mapData.end());
    combinedData.insert(combinedData.end(), tuyaVirtualWall.begin(), tuyaVirtualWall.end());
    combinedData.insert(combinedData.end(), tuyaRestrictedArea.begin(), tuyaRestrictedArea.end());

    // 保存到文件
    std::string combinedFile = "/tmp/mm_map_" + std::to_string(currId) + ".bin";
    std::ofstream outFile(combinedFile, std::ios::binary);
    if (!outFile.is_open())
    {
        LOGE(TAG, "Failed to open output file: {}", combinedFile);
        return -1;
    }
    outFile.write(reinterpret_cast<const char *>(combinedData.data()), combinedData.size());
    outFile.close();

    // 将 currId 写入到文件
    std::string currIdFile = "/tmp/mm_bin.txt";
    std::ofstream currIdOut(currIdFile);
    if (!currIdOut.is_open())
    {
        LOGE(TAG, "Failed to open output file: {}", currIdFile);
        return -1;
    }
    currIdOut << currId;
    currIdOut.close();

    // 上传到云端
    time_t t;
    time(&t);
    
    char descript[128];
    snprintf(descript, 128, "Map_%d_%ld", currId, t);
    UINT_T mapId = 0;
    int ret = tuya_iot_map_update_files(tuyaMapId, combinedFile.c_str(), currIdFile.c_str());
    tuyaMapId = mapId;
    system(("rm -f " + combinedFile).c_str());
    system(("rm -f " + currIdFile).c_str());
    LOGD(TAG, "tuya_iot_map_upload_files ret:{}, mapId:{}", ret, mapId);
    return ret;
}

int TuyaUploadMapFile(int currId, uint32_t & tuyaMapId)
{
    // 获取当前地图文件
    std::string mapBin = "/tmp/map.bin";
    std::ifstream mapFile(mapBin, std::ios::binary);
    if (!mapFile.is_open())
    {
        LOGE(TAG, "Failed to open map file: {}", mapBin);
        return -1;
    }
    std::vector<uint8_t> mapData((std::istreambuf_iterator<char>(mapFile)), std::istreambuf_iterator<char>());
    mapFile.close();

    // 获取虚拟墙数据
    AppVirtualWall wall;
    std::vector<uint8_t> tuyaVirtualWall;
    wall.version = 0;
    wall.count = 0;
    tuya_message::Request req = {};
    if (TuyaComm::Get()->Send("ty_get_virtual_wall", &req, &wall) && wall.version >= 0)
    {
        tuyaVirtualWall = ToVirtualWallData(wall.version == 0 ? 0x13 : 0x49, &wall);
    }
    else
    {
        LOGE(TAG, "无法获取到虚拟墙信息");
    }

    // 获取禁区数据
    AppRestrictedArea area;
    std::vector<uint8_t> tuyaRestrictedArea;
    if (TuyaComm::Get()->Send("ty_get_restricted_area", &req, &area) && area.version >= 0)
    {
        tuyaRestrictedArea = ToRestrictedAreaData(area.version == 0 ? 0x1B : 0x39, &area);
    }
    else
    {
        LOGE(TAG, "无法获取到禁区/划区信息");
    }

    // 拼接 mapData, tuyaVirtualWall 和 tuyaRestrictedArea
    std::vector<uint8_t> combinedData;
    combinedData.insert(combinedData.end(), mapData.begin(), mapData.end());
    combinedData.insert(combinedData.end(), tuyaVirtualWall.begin(), tuyaVirtualWall.end());
    combinedData.insert(combinedData.end(), tuyaRestrictedArea.begin(), tuyaRestrictedArea.end());

    // 保存到文件
    std::string combinedFile = "/tmp/mm_map_" + std::to_string(currId) + ".bin";
    std::ofstream outFile(combinedFile, std::ios::binary);
    if (!outFile.is_open())
    {
        LOGE(TAG, "Failed to open output file: {}", combinedFile);
        return -1;
    }
    outFile.write(reinterpret_cast<const char *>(combinedData.data()), combinedData.size());
    outFile.close();

    // 将 currId 写入到文件
    std::string currIdFile = "/tmp/mm_bin.txt";
    std::ofstream currIdOut(currIdFile);
    if (!currIdOut.is_open())
    {
        LOGE(TAG, "Failed to open output file: {}", currIdFile);
        return -1;
    }
    currIdOut << currId;
    currIdOut.close();

    // 上传到云端
    time_t t;
    time(&t);
    
    char descript[128];
    snprintf(descript, 128, "Map_%d_%ld", currId, t);
    UINT_T mapId = 0;
    int ret = tuya_iot_map_upload_files(combinedFile.c_str(), currIdFile.c_str(), descript, &mapId);
    tuyaMapId = mapId;
    system(("rm -f " + combinedFile).c_str());
    system(("rm -f " + currIdFile).c_str());
    LOGD(TAG, "tuya_iot_map_upload_files ret:{}, mapId:{}", ret, mapId);
    return ret;
}

void TuyaHandleExtentedFuction(int dpId, uint8_t *data, int len)
{
    // 校验数据
    if (len <= 5)
    {
        return;
    }
    switch (data[6])
    {
    case 0x2A: // 保存地图
        LOGD(TAG, "保存地图 in {}", data[7]);
        {
            tuya_message::Request req = {0};
            tuya_message::Result res;
            if (TuyaComm::Get()->Send("ty_save_map", &req, &res, 1000) && res.code == 0)
            {
                // Save space：保存的空间，00：保存到本地，01：保存到云端
                if (data[7] == 0)
                {
                }
                if (data[7] == 1)
                {
                    LOGD(TAG, "保存成功");
                    // 保存在云端
                    uint32_t tuyaMapId;
                    int ret = TuyaUploadMapFile(res.a, tuyaMapId); // 上传地图文件
                    if (ret == 0)
                    {
                        req.a = res.a;
                        req.b = tuyaMapId;
                        if(TuyaComm::Get()->Send("ty_connect_tuya_id", &req, &res))
                        {
                            TuyaReportMapSaveResult(dpId, 0x2B, 0x01); // ret：0x00 保存失败、0x01 保存成功、0x02：本地空间已满
                        }
                        else
                        {
                            LOGD(TAG, "连接本地和云端地图失败");
                            TuyaReportMapSaveResult(dpId, 0x2B, 0x00);
                        }
                    }
                }
            }
            else
            {
                LOGD(TAG, "保存失败");
                TuyaReportMapSaveResult(dpId, 0x2B, 0x00);
            }
        }
        break;
    case 0x2C: // 删除地图
    {
        uint32_t mapId = nbto32(&data[7]);
        LOGD(TAG, "删除地图 id:{}", mapId);
        tuya_message::Request req = {(int)mapId};
        tuya_message::Result res;
        if(TuyaComm::Get()->Send("ty_delete_map_by_tuya_id", &req, &res))
        {
            LOGD(TAG, "删除本地地图成功");
        }
        else
        {
            LOGD(TAG, "删除本地地图失败");
        }
        if (tuya_iot_map_delete(mapId) == 0)
        {
            LOGD(TAG, "删除成功");
            TuyaReportMapSaveResult(dpId, 0x2D, 0x01);
        }
        else
        {
            LOGD(TAG, "删除失败");
            TuyaReportMapSaveResult(dpId, 0x2D, 0x00);
        }
    }
    break;
    case 0x2E: // 使用地图
    {
        uint32_t mapId = nbto32(&data[7]);
        uint32_t urlLen = nbto32(&data[11]);
        std::string url((char *)&data[15], urlLen);
        LOGD(TAG, "使用地图 id:{}, url:{}", mapId, url);
        tuya_message::Request req = {(int)mapId};
        tuya_message::Result res;
        if(TuyaComm::Get()->Send("ty_change_map_by_tuya_id", &req, &res, 2000) && res.code == 0)
        {
            LOGD(TAG, "使用地图成功");
            TuyaReportMapSaveResult(dpId, 0x2F, 0x01);
        }
        else
        {
            LOGD(TAG, "使用地图失败");
            TuyaReportMapSaveResult(dpId, 0x2F, 0x00);
        }
    }
    break;
    case 0x34: // 语音包下载
    {
        LOGD(TAG, "语音包下载");
        uint8_t version = data[1];
        uint32_t langId = nbto32(&data[7]);
        uint8_t ds = 11;
        uint32_t fileVersion = 0;
        if(version == 0x01)
        {
            fileVersion = nbto32(&data[ds]);
            ds = 15;
        }
        uint8_t md5Len = data[ds];
        std::string md5((char *)&data[ds+1], md5Len);
        LOGD(TAG, "语音包下载 langId:{}, fileVersion:{}, md5:{}", langId, fileVersion, md5);
        uint32_t urlLen = nbto32(&data[ds+1 + md5Len]);
        std::string url((char *)&data[ds+5 + md5Len], urlLen);
        LOGD(TAG, "语音包下载 url:{}", url);
        // 如果存在这个语音包，直接使用
        if (access(("/data/voice/" + std::to_string(langId)).c_str(), F_OK) == 0) {
            std::ifstream verFile("/data/voice/" + std::to_string(langId) + "/ver.txt");
            if (verFile.is_open()) {
                std::string line;
                std::getline(verFile, line);
                verFile.close();
                if (std::to_string(fileVersion) == line) {
                    LOGD(TAG, "语音包已存在且版本 {} 相同", fileVersion);
                    system("rm -rf /data/voice/now");
                    system(("ln -sf /data/voice/" + std::to_string(langId) + " " + "/data/voice/now").c_str());
                    TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x03, 0);
                    PlayVoice(V_SEEK_ROBOT, 0);
                    return;
                }
            }
            return;
        }

        std::string file_name;
        size_t pos = url.find_last_of("/");
        if (pos != std::string::npos) {
            file_name = url.substr(pos + 1);
        }
        if (file_name.empty()) {
            file_name = "voice.zip";
        }
        system("mkdir -p /data/voice");
        file_name = "/data/voice" + file_name;
        if(download_file(file_name.c_str(), url.c_str(), [dpId, version, langId, fileVersion](void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) -> int
        {
            if(dltotal != 0)
            {
                LOGD(TAG, "{}/{} {}%", dlnow, dltotal, 100 * dlnow / dltotal);
                static int last_process = 0;
                int process = 100 * dlnow / dltotal;
                if(process % 10 == 0 && process != last_process)
                {
                    last_process = process;
                    TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x01, process);
                }
            }
            return 0;
        }) == 0)
        {
            TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x01, 100);
            // 获取文件 md5
            std::string md5_file = md5sum(file_name.c_str());
            if(md5_file == md5)
            {
                LOGD(TAG, "语音包下载成功");
                // 获取文件扩展名
                std::string ext = file_name.substr(file_name.find_last_of(".") + 1);
                int err = 0;
                if(ext == "gz")
                {
                    err += system(("tar -zxf " + file_name + " -C /data/voice/" + std::to_string(langId)).c_str());
                    err += system(("rm " + file_name).c_str());
                    err += system(("mv /data/voice/" + std::to_string(langId) + "/*/* " + "/data/voice/" + std::to_string(langId)).c_str());
                    // 写入版本信息
                    err += system(("echo " + std::to_string(fileVersion) + " > " + "/data/voice/" + std::to_string(langId) + "/ver.txt").c_str());
                    err += system("rm -rf /data/voice/now");
                    err += system(("ln -sf /data/voice/" + std::to_string(langId) + " " + "/data/voice/now").c_str());
                    err += system(("echo " + std::to_string(langId) + " > " + "/data/voice/" + "/now.txt").c_str());
                    //TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x02, 0);
                }
                else if(ext == "zip")
                {
                    err += system(("unzip -o " + file_name + " -d /data/voice/" + std::to_string(langId)).c_str());
                    err += system(("rm " + file_name).c_str());
                    err += system(("mv /data/voice/" + std::to_string(langId) + "/*/* " + "/data/voice/" + std::to_string(langId)).c_str());
                    err += system(("echo " + std::to_string(fileVersion) + " > " + "/data/voice/" + std::to_string(langId) + "/ver.txt").c_str());
                    err += system("rm -rf /data/voice/now");
                    err += system(("ln -sf /data/voice/" + std::to_string(langId) + " " + "/data/voice/now").c_str());
                    err += system(("echo " + std::to_string(langId) + " > " + "/data/voice/" + "/now.txt").c_str());
                    //TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x02, 0);
                }

                if(err == 0)
                {
                    TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x03, 0);
                    PlayVoice(V_SEEK_ROBOT, 0);
                }
                else
                {
                    LOGD(TAG, "语音包解压失败");
                    TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x00, 0);
                }
            }
            else
            {
                LOGD(TAG, "语音包下载失败 md5:{}, {}", md5_file, md5);
                TuyaReportVoiceDownLoadResult(dpId, version, langId, fileVersion, 0x00, 0);
            }
        }
    }
    break;
    default:
        break;
    }
}

/*
* 使用 CURLOPT_XFERINFODATA 属性来传递该函数。
* dltotal：需要下载的总字节数
* ultotal：需要上传的总字节数
* dlnow：已经下载的字节数
* ulnow：已经上传的字节数
* return：返回非 0 值将会中断传输。
*   
*/

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *stream)
{
	return fwrite(ptr, size, nmemb, (FILE*)stream);
}

int download_file(const char *file_name, const char *url, curl_info_cb info_cb)
{
	FILE *fp = fopen(file_name, "wb");
	if(!fp){
		LOGE(TAG, "open file %s fail.\n", file_name);
		return -1;
	}
	CURL *curl = curl_easy_init();
	if(!curl){
		LOGE(TAG, "curl easy init fail.\n");
		fclose(fp);
		return -1;
	}
    global_progress_callback = info_cb;
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(curl, CURLOPT_XFERINFODATA, NULL);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback_wrapper);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    LOGL(TAG);
	CURLcode ret = curl_easy_perform(curl);
	if(ret != CURLE_OK){
		LOGE(TAG, "curl_easy_perform download fail, reason : %s.\n", curl_easy_strerror(ret));
	}
	fclose(fp);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	return ret;
}
