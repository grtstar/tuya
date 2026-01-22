#include <stdint.h>
#include <netinet/in.h>

#include <vector>
#include <map>
#include <fstream>

#include <utils/json.hpp>
#include "utils/log_.h"
#include "lz4/lz4.h"

#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"
#include "tuya_iot_wifi_api.h"
#define __NLOHMANN_JSON_CPP
#include "robot_msg.h"
#include "voice.h"
#include "tuya.h"
#include "tuya_robot.h"
#include "tuya_enums.h"
#include "tuya_dp_function.h"
#include "tuya_utils.h"
#include "timed_task.hpp"

#define TAG "TUY"

#define APP_VERSION_FILE "/userdata/version/soft_version.json" 

tuya_message::RobotState sweeper = {0};
AppMap nowAppMap = {0};

// 1. 清扫记录
void ReportCleanRecord(uint16_t recordId, std::string beginTime, int cleanTimeMinute,
                       int cleanArea, int mapLen, int pathLen, int virtualLen,
                       int cleanMode, int workMode, int cleanningResult, int startMethod)
{
}

// 2. 多层地图

// 3. 语音包
// 4. 设备信息
enum TuyaType
{
    TuyaTypeBool = 0,
    TuyaTypeEnum = 1,
    TuyaTypeValue = 2,
    TuyaTypeBitmap = 3,
    TuyaTypeRaw = 4,
};
std::map<int, TuyaType> DpType =
{
    {1, TuyaTypeBool},
    {2, TuyaTypeBool},
    {3, TuyaTypeBool},
    {4, TuyaTypeEnum},
    {5, TuyaTypeEnum},
    {6, TuyaTypeValue},
    {7, TuyaTypeValue},
    {8, TuyaTypeValue},
    {9, TuyaTypeEnum},
    {10, TuyaTypeEnum},
    {11, TuyaTypeBool},
    {12, TuyaTypeEnum},
    {13, TuyaTypeBool},
    {14, TuyaTypeRaw},
    {15, TuyaTypeRaw},
    {16, TuyaTypeEnum},
    {17, TuyaTypeValue},
    {18, TuyaTypeBool},
    {19, TuyaTypeValue},
    {20, TuyaTypeBool},
    {21, TuyaTypeValue},
    {22, TuyaTypeBool},
    {23, TuyaTypeValue},
    {24, TuyaTypeBool},
    {25, TuyaTypeBool},
    {26, TuyaTypeValue},
    {27, TuyaTypeBool},
    {28, TuyaTypeBitmap},   
    {29, TuyaTypeValue},
    {30, TuyaTypeValue},
    {31, TuyaTypeValue},
    {32, TuyaTypeRaw},
    {33, TuyaTypeRaw},
    {34, TuyaTypeRaw},
    {35, TuyaTypeRaw},
    {36, TuyaTypeEnum},
    {37, TuyaTypeValue},
    {38, TuyaTypeBool},
    {39, TuyaTypeBool},
    {40, TuyaTypeEnum},
    {41, TuyaTypeEnum},
    {42, TuyaTypeEnum},
    {43, TuyaTypeValue},
    {44, TuyaTypeEnum},
    {45, TuyaTypeBool},
    {46, TuyaTypeBool},
    {47, TuyaTypeBool},
    {48, TuyaTypeBool},
    {49, TuyaTypeBool},
    {50, TuyaTypeBool},
    {51, TuyaTypeValue},
    {101, TuyaTypeEnum},
    {102, TuyaTypeBool},
    {103, TuyaTypeBool},
    {107, TuyaTypeValue},
    {147, TuyaTypeBool},
    {149, TuyaTypeBool},
    {151, TuyaTypeBool},
    {166, TuyaTypeBool},
};

void TuyaHandleDPQuery(int dpId)
{
    LOGD(TAG, "DP query: {}", dpId);
    switch (dpId)
    {
    case 1:
        break;
    case 4:
        break;
    default:
        break;
    }
}

struct TuyaDPRawHandler
{
    int dpId;
    void (*dpHanlder)(int dpId, uint8_t *data, int len);
};

TuyaDPRawHandler _dpRawHandler[] =
{
    {14, DPRaw_HandlePathData},
    {15, DPRaw_HandleCommand},
    {32, DPRaw_HandleDeviceTimer},
    {33, DPRaw_HandleDisturbTimeSet},
    {35, DPRaw_HandleVoiceData},
};

int GetRawDpId(void (*dpHanlder)(int dpId, uint8_t *data, int len))
{
    for(int i=0; i<sizeof(_dpRawHandler)/sizeof(_dpRawHandler[0]); i++)
    {
        if(dpHanlder == _dpRawHandler[i].dpHanlder)
        {
            return _dpRawHandler[i].dpId;
        }
    }
    return 0;
}

void TuyaHandleRawDPCmd(int dpId, uint8_t *data, int len)
{
    LOGD(TAG, "Raw DP cmd: {}", dpId);
    for(int i=0; i<sizeof(_dpRawHandler)/sizeof(_dpRawHandler[0]); i++)
    {
        if(dpId == _dpRawHandler[i].dpId)
        {
            _dpRawHandler[i].dpHanlder(dpId, data, len);
        }
    }
}


struct TuyaDPHandler
{
    int dpId;
    void (*dpHanlder)(TY_OBJ_DP_S*);
};

TuyaDPHandler _dpHandler[] = 
{
    {1, DP_HandleSwichGo},
    {2, DP_HandlePause},
    {3, DP_HandleSwichCharge},
    {4, DP_HandleMode},
    {9, DP_HandleSuction},
    {10, DP_HandleCistern},
    {11, DP_HandleSeek},
    {12, DP_HandleDirectionControl},
    {13, DP_HandleMapReset},
    {16, DP_HandleRequest},
    {17, DP_HandleEdgeBrushLife},
    {18, DP_HandleEdgeBrushLifeReset},
    {19, DP_HandleRollBrushLife},
    {20, DP_HandleRollBrushLifeReset},
    {22, DP_HandleFilterLifeReset},
    {24, DP_HandleRagLifeReset},
    {25, DP_HandleDonotDisturb},
    {26, DP_HandleVolumeSet},
    {27, DP_HandleBreakClean},
    {28, DP_HandleFault},
    {36, DP_HandleLanguage},
    {37, DP_HandleDustCollectionNum},
    {38, DP_HandleDustCollectionSwitch},
    {39, DP_HandleCustomizeModeSwitch},
    {41, DP_HandleWorkMode},
    {42, DP_HandleUnitSet},
    {44, DP_HandleCarpetCleanPrefer},
    {45, DP_HandleAutoBoost},
    {46, DP_HandleCruiseSwitch},
    {47, DP_HandleChildLock},
    {48, DP48_HandleYMop},
    {49, DP_HandleSelfCleanMop},
    {50, DP_HandleDryingSwitch},
    {51, DP_HandleMopSlefCleaningFrequency},
    {52, DP_HandleMopSlefCleaningStrength},
    {53, DP_HandleWipingStrength},
    {101, DP_HandleAvoidObsMode},
    {102, DP_HandleStationLightDisplaySwitch},
    {103, DP_HandleKeyLightDisplaySwitch},
    {107, DP_HandleStationHotFanTime},
    {147, DP_HandleStationHotFan},
    {149, DP_HandleStationHotWater},
    {151, DP_HandleSmartEdge},
    {166, DP_HandleUploadLogs},

};

int GetDpId(void (*dpHanlder)(TY_OBJ_DP_S*))
{
    for(int i=0; i<sizeof(_dpHandler)/sizeof(_dpHandler[0]); i++)
    {
        if(dpHanlder == _dpHandler[i].dpHanlder)
        {
            return _dpHandler[i].dpId;
        }
    }
    return 0;
}

extern BOOL_T s_mqtt_online_status;
void TuyaHandleDPCmd(TY_OBJ_DP_S *dp)
{
    LOGD(TAG, "DP cmd: {}", dp->dpid);
    s_mqtt_online_status = true;
    for(int i=0; i<sizeof(_dpHandler)/sizeof(_dpHandler[0]); i++)
    {
        if(dp->dpid == _dpHandler[i].dpId)
        {
            _dpHandler[i].dpHanlder(dp);
        }
    }
}

#include "tuya_hal_wifi.h"
extern "C"
{
    OPERATE_RET tuya_adapter_wifi_get_ip(IN CONST WF_IF_E wf, OUT NW_IP_S *ip);
    OPERATE_RET tuya_adapter_wifi_get_mac(IN CONST WF_IF_E wf, INOUT NW_MAC_S *mac);
    OPERATE_RET tuya_adapter_wifi_station_get_conn_ap_rssi(OUT SCHAR_T *rssi);
    OPERATE_RET tuya_adapter_wifi_get_ssid(char *ssid);
}

TY_OBJ_DP_S DPReport(int dpId, int32_t value)
{
    TuyaType tuyaType = DpType[dpId];
    switch (tuyaType)
    {
    case TuyaTypeBool:
        return DPReportBool(dpId, value);
    case TuyaTypeEnum:
        return DPReportEnum(dpId, value);
    case TuyaTypeValue:
        return DPReportValue(dpId, value);
    case TuyaTypeBitmap:
        return DPReportBitmap(dpId, value);
    default:
        break;
    }
    return DPReportValue(dpId, value);
}


void TuyaReportStatus(tuya_message::RobotState status)
{
    nlohmann::json j = status;
    LOGD(TAG, "ReportStatus: {}", j.dump(2));
    sweeper = status;

    std::vector<TY_OBJ_DP_S> dps;
    // 1 清扫开关
    dps.emplace_back(DPReport(1, status.switch_go));
    // 2 暂停
    dps.emplace_back(DPReport(2, status.pause));
    // 3 回充开关
    dps.emplace_back(DPReport(3, status.switch_charge));
    // 4 工作模式
    dps.emplace_back(DPReport(4, status.mode));
    // 5 状态
    dps.emplace_back(DPReport(5, status.status));
    // 6 清扫时间
    // 7 清扫面积
    // 8 电量百分比
    dps.emplace_back(DPReport(8, status.battery));
    // 9 吸力
    dps.emplace_back(DPReport(9, status.suction));
    // 10 出水量
    dps.emplace_back(DPReport(10, status.cistern));
    // 11 寻找机器人
    // 12 方向控制
    // 13 地图重置
    // 17 边刷寿命
    // 18 边刷寿命复位
    // 19 滚刷寿命
    // 20 滚刷寿命复位
    // 21 滤网寿命
    // 22 滤网寿命复位
    // 23 抹布寿命
    // 24 抹布寿命复位
    // 25 勿扰模式
    dps.emplace_back(DPReport(25, status.do_not_disturb));
    // 26 音量设置
    dps.emplace_back(DPReport(26, status.volume_set));
    // 27 断点清扫
    dps.emplace_back(DPReport(27, status.break_clean));
    // 28 故障
    dps.emplace_back(DPReport(28, status.fault));
    // 29 清扫面积总计
    // 30 清扫次数总计
    // 31 清扫时间总计
    // 32 定时  raw
    // 33 勿扰时间  raw
    // 34 设备信息  raw
    // 36 语言
    dps.emplace_back(DPReport(36, status.language));
    // 37 集尘频率
    dps.emplace_back(DPReport(37, status.dust_collection_num));
    // 38 集尘开关
    dps.emplace_back(DPReport(38, status.dust_collection_switch));
    // 39 自定义模式切换
    dps.emplace_back(DPReport(39, status.costomize_mode_switch));
    // 40 拖地安装状态
    dps.emplace_back(DPReport(40, status.mop_state));
    // 41 工作模式：扫拖，仅扫，仅拖
    dps.emplace_back(DPReport(41, status.work_mode));
    // 42 单位设置
    dps.emplace_back(DPReport(42, status.unit_set));
    // 43 预估面积
    // 44 地毯清扫偏好
    dps.emplace_back(DPReport(44, status.carpet_clean_prefer));
    // 45 地毯增压
    dps.emplace_back(DPReport(45, status.auto_boost));
    // 46 巡航开关 未支持
    dps.emplace_back(DPReport(46, status.cruise_switch));
    // 47 儿童锁 未支持
    dps.emplace_back(DPReport(47, status.child_lock));
    // 48 拖地模式 未支持
    dps.emplace_back(DPReport(48, status.y_mop));
    // 49 自洁拖布 按键
    dps.emplace_back(DPReport(49, status.self_clean));
    // 50 烘干开关
    dps.emplace_back(DPReport(50, status.drying));
    // 51 拖地自洁频率
    dps.emplace_back(DPReport(51, status.self_clean_frequency));
    // 52 拖地自洁强度
    dps.emplace_back(DPReport(52, status.self_clean_strength));
    // 53 擦地强度
    dps.emplace_back(DPReport(53, status.land_strength));
    // 101 避障模式
    dps.emplace_back(DPReport(101, status.smart_collision));
    // 102
    dps.emplace_back(DPReport(102, status.extra_dp[1]));
    // 103
    dps.emplace_back(DPReport(103, status.extra_dp[0]));
    // 107 基站热风时间
    dps.emplace_back(DPReport(107, status.mop_drying_timee));
    // 147 基站热风
    dps.emplace_back(DPReport(147, status.base_heat));
    // 149 基站热水
    dps.emplace_back(DPReport(149, status.base_mop_hot));
    // 151 智能沿墙
    dps.emplace_back(DPReport(151, status.smart_along_wall));
    // 166 上传日志
    dps.emplace_back(DPReport(166, status.log_upload));

    for(int i=0; i<dps.size(); i++)
    {
        dev_report_dp_json_async(NULL, &dps[i], 1);
    }
 
}

void TuyaComm::ReportDeviceInfo(void)
{
    extern std::string id;
    extern std::string uuid;
    extern std::string authKey;

    //app version
    extern std::string soft_version;
    extern std::string MCU_version;
    extern std::string system_version;
    {
        LOGD(TAG, "上传设备信息");
        char ssid[128] = {0};
        tuya_adapter_wifi_get_ssid(ssid);
        std::string wifiName = ssid;

        SCHAR_T rssiT;
        tuya_adapter_wifi_station_get_conn_ap_rssi(&rssiT);
        int rssi = rssiT;

        NW_IP_S ips = {0};
        tuya_adapter_wifi_get_ip(WF_STATION, &ips);
        std::string ip = ips.ip;

        NW_MAC_S macs = {0};
        char macStr[32] = {0};
        tuya_adapter_wifi_get_mac(WF_STATION, &macs);
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", macs.mac[0], macs.mac[1], macs.mac[2], macs.mac[3], macs.mac[4], macs.mac[5]);
        std::string mac = macStr;
        std::string mcuVersion = MCU_version;
        std::string firmwareVersion = soft_version;
        std::string deviceSN = id;
        std::string moduleUUID = uuid;
        std::string baseStationSN = "BASE";
        std::string baseStationVersion = "1.0.0";
        std::string baseStationLocalVersion = "1.0.0";
        DP_ReportDeviceInfo(34, wifiName, rssi, ip, mac, mcuVersion, firmwareVersion, deviceSN, moduleUUID, baseStationSN, baseStationVersion, baseStationLocalVersion);
    }
}

void TuyaComm::ReportStatus()
{
    tuya_message::Request req = {0};
    tuya_message::RobotState state = {0};
    LOGD(TAG, "获取设备状态");
    if(Send("ty_state", &req, &state, 1000))
    {
        TuyaReportStatus(state);
    }
    else
    {
        LOGE(TAG, "无法获取到设备状态");
    }
}


void TuyaComm::OnMap(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppMap *msg)
{
    nowAppMap = *msg;
    TuyaMap map = ToTuyaMap(*msg);
    SaveTuyaMap(map, "/tmp/map.bin");
}
void TuyaComm::OnPath(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppPath *msg)
{
    TuyaPath path = ToTuyaPath(*msg, msg->id, 0);
    SaveTuyaPath(path, "/tmp/cleanPath.bin");
}
void TuyaComm::OnCmd(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const Debug *msg)
{
}
void TuyaComm::OnSweeperStatus(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const tuya_message::RobotState *msg)
{
    TuyaReportStatus(*msg);
}

void TuyaComm::OnPartsLife(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppPartsLife *msg)
{
    std::vector<TY_OBJ_DP_S> dps;
    if (partsLife.edgeBrushLifeMinutes != msg->edgeBrushLifeMinutes)
    {
        dps.emplace_back(DPReport(17, msg->edgeBrushLifeMinutes / 60));
    }
    if (partsLife.haipaLifeMinutes != msg->haipaLifeMinutes)
    {
        dps.emplace_back(DPReport(21, msg->haipaLifeMinutes / 60));
    }
    if (partsLife.mainBrushLifeMinutes != msg->mainBrushLifeMinutes)
    {
        dps.emplace_back(DPReport(19, msg->mainBrushLifeMinutes / 60));
    }
    if (partsLife.ragLifeMinutes != msg->ragLifeMinutes)
    {
        dps.emplace_back(DPReport(23, msg->ragLifeMinutes / 60));
    }
    if (!dps.empty())
    {
        dev_report_dp_json_async(NULL, &dps[0], dps.size());
    }
    partsLife = *msg;
}

void TuyaComm::OnCleanInfo(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppCleanInfo *msg)
{
    LOGD(TAG, "cleanTime:{} cleanArea:{} cleanAreaTotal:{} cleanCountTotal:{} cleanTimeTotal:{}", 
            msg->cleanTimeSecond, msg->cleanArea, msg->cleanAreaTotal, msg->cleanCountTotal, msg->cleanTimeTotalMinutes);
    {
        std::vector<TY_OBJ_DP_S> dps;
        if (cleanInfo.cleanTimeSecond != msg->cleanTimeSecond)
        {
            dps.emplace_back(DPReport(6, msg->cleanTimeSecond/60));
        }
        if (cleanInfo.cleanArea != msg->cleanArea)
        {
            dps.emplace_back(DPReport(7, msg->cleanArea));
        }
        if (cleanInfo.cleanAreaTotal != msg->cleanAreaTotal)
        {
            dps.emplace_back(DPReport(29, msg->cleanAreaTotal));
        }
        if (cleanInfo.cleanCountTotal != msg->cleanCountTotal)
        {
            dps.emplace_back(DPReport(30, msg->cleanCountTotal));
        }
        if (cleanInfo.cleanTimeTotalMinutes != msg->cleanTimeTotalMinutes)
        {
            dps.emplace_back(DPReport(31, msg->cleanTimeTotalMinutes));
        }
        if (cleanInfo.esimateArea != msg->esimateArea)
        {
            dps.emplace_back(DPReport(43, msg->esimateArea));
        }
        if (!dps.empty())
        {
            dev_report_dp_json_async(NULL, &dps[0], dps.size());
        }
        cleanInfo = *msg;
    }
}

void TuyaComm::OnVirtualWall(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppVirtualWall *msg)
{
    nlohmann::json j = *msg;
    LOGD(TAG, "OnVirtualWall: {}", j.dump(2));
    if(msg->version == 0)
    {
        TuyaReportVirtualWall(GetRawDpId(DPRaw_HandleCommand), 0x13, (AppVirtualWall *)msg);
    }
    else
    {
        TuyaReportVirtualWall(GetRawDpId(DPRaw_HandleCommand), 0x49, (AppVirtualWall* )msg);
    }
}

void TuyaComm::OnRestrictedArea(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppRestrictedArea *msg)
{
    nlohmann::json j = *msg;
    LOGD(TAG, "OnRestrictedArea: {}", j.dump(2));
    if(msg->version == 0)
    {
        TuyaReportRestrictedArea(GetRawDpId(DPRaw_HandleCommand), 0x1B, (AppRestrictedArea *)msg);

    }
    else
    {
        TuyaReportRestrictedArea(GetRawDpId(DPRaw_HandleCommand), 0x39, (AppRestrictedArea *)msg);

    }
}

void TuyaComm::OnCleanRecord(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppCleanRecord *msg)
{
    for(int i=0; i<3; i++)
    {
        if(ReportCleanRecords(msg->recordId, msg->cleanTimeSecond/60.0 + 0.5, msg->cleanArea, (msg->cleanMode), ((CleanMethod)msg->cleanMethod), msg->finishResult, msg->startReason))
        {
            break;
        }
    }
}

void TuyaComm::OnSchedule(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppLocalAlert *msg)
{
    TuyaReportLocalAlert(32, msg->verison == 0 ? 0x31 : 0x45, (AppLocalAlert *)msg);
}

void TuyaComm::OnEvent(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const Event *msg)
{
    LOGD(TAG, "Event: {}", M2S(msg->event));
    switch (msg->event)
    {
    case MSG_RESET_WIFI:
        tuya_iot_wf_gw_unactive();
        break;
    }
}

void TuyaComm::OnRoomClean(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppRoomClean *msg)
{
    TuyaReportRoomClean(GetRawDpId(DPRaw_HandleCommand), 0x15, (AppRoomClean *)msg);
}
int TuyaUploadMapFile(int currId, uint32_t & tuyaMapId);
int TuyaUpdateMapFile(int currId, uint32_t tuyaMapId);
void TuyaReportMapSaveResult(int dpId, uint8_t cmd, uint8_t ret);
void TuyaComm::OnSaveMap(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const tuya_message::RobotEvent *msg)
{
    tuya_message::Request req = {0};
    tuya_message::Result res;
    if (TuyaComm::Get()->Send("ty_save_map", &req, &res, 1000) && res.code == 0)
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
                TuyaReportMapSaveResult(GetRawDpId(DPRaw_HandleCommand), 0x2B, 0x01); // ret：0x00 保存失败、0x01 保存成功、0x02：本地空间已满
            }
            else
            {
                LOGD(TAG, "连接本地和云端地图失败");
                TuyaReportMapSaveResult(GetRawDpId(DPRaw_HandleCommand), 0x2B, 0x00);
            }
        }
    }
    else
    {
        LOGD(TAG, "保存失败");
        TuyaReportMapSaveResult(GetRawDpId(DPRaw_HandleCommand), 0x2B, 0x00);
    }
}

void TuyaComm::OnUpdateMap(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const tuya_message::RobotEvent *msg)
{
    int mapId = msg->a;
    int tuyaMapId = msg->b;
    TuyaUpdateMapFile(mapId, tuyaMapId);
}
void TuyaComm::OnReportSpot(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppSpotClean *msg)
{
    LOGD(TAG, "OnReportSpot: {}", msg->version);
    TuyaReportSpotClean(GetRawDpId(DPRaw_HandleCommand), msg->version==0?0x17:0x3F, (AppSpotClean *)msg);
}

void TuyaComm::ReportPartsLife()
{
    LOGD(TAG, "ReportPartsLife");
    std::vector<TY_OBJ_DP_S> dps;
    AppPartsLife life = {0};
    tuya_message::Request req = {0};
    if(Send("ty_get_parts_life", &req, &life))
    {
        AppPartsLife *msg = &life;
        dps.emplace_back(DPReport(17, msg->edgeBrushLifeMinutes/60));
        dps.emplace_back(DPReport(19, msg->mainBrushLifeMinutes/60));
        dps.emplace_back(DPReport(21, msg->haipaLifeMinutes/60));
        dps.emplace_back(DPReport(23, msg->ragLifeMinutes/60));
        if (!dps.empty())
        {
            dev_report_dp_json_async(NULL, &dps[0], dps.size());
        }
    }
    else
    {
        LOGE(TAG, "无法获取到 PartsLife");
    }
}

void TuyaComm::ReportCleanInfo()
{
    LOGD(TAG, "ReportCleanInfo");
    std::vector<TY_OBJ_DP_S> dps;
    AppCleanInfo info = {};
    tuya_message::Request req = {0};
    if(Send("ty_get_clean_info", &req, &info))
    {
        dps.emplace_back(DPReport(6, info.cleanTimeSecond/60));
        dps.emplace_back(DPReport(7, info.cleanArea));
        dps.emplace_back(DPReport(29, info.cleanAreaTotal));
        dps.emplace_back(DPReport(30, info.cleanCountTotal));
        dps.emplace_back(DPReport(31, info.cleanTimeTotalMinutes));
        dps.emplace_back(DPReport(43, info.esimateArea));

        if (!dps.empty())
        {
            dev_report_dp_json_async(NULL, &dps[0], dps.size());
        }
    }
    else
    {
        LOGE(TAG, "无法获取到 CleanInfo");
    }
    // 上报清扫顺序
    LOGD(TAG, "ReportCleanSequence");
    AppSetCleaningSequence order = {};
    if(Send("ty_get_cleaning_sequence", &req, &order))
    {
        TuyaReportCleaningSequence(GetRawDpId(DPRaw_HandleCommand), 0x27, &order);
    }
    else
    {
        LOGE(TAG, "无法获取到 CleanSequence");
    }

}

bool TuyaComm::ReportCleanRecords(int recordId, int cleanTime, int cleanArea, int cleanMode, int workMode, int cleaningResult, int startMethod) 
{
    time_t nowTime = time(NULL);
    struct tm *local = localtime(&nowTime);
    char endTime[50] = {0};
    strftime(endTime, 50, "%Y%m%d_%H%M%S", local);
    LOGD(TAG, "{}", endTime);

    char descript[128] = {0};

  // 获取当前地图文件
    if(sweeper.costomize_mode_switch == 0)
    {
        for(auto & roomProp : nowAppMap.roomPropeties)
        {
            roomProp.cleanOrder = 0;
            roomProp.cleanRepeart = 0;
            roomProp.mopRepeat = 0;
            roomProp.colorOrder = -1;
            roomProp.donotSweep = 0;
            roomProp.donotMop = 0;
            roomProp.fanPower = 0xff;
            roomProp.waterLevel = 0xff;
            roomProp.enableYMop = 0xff;
        }
    }
    TuyaMap map = ToTuyaMap(nowAppMap);
    SaveTuyaMap(map, "/tmp/crecord.bin");

    std::string mapBin = "/tmp/crecord.bin";
    std::ifstream mapFile(mapBin, std::ios::binary);
    if (!mapFile.is_open())
    {
        LOGE(TAG, "Failed to open map file: {}", mapBin);
        return false;
    }
    std::vector<uint8_t> mapData((std::istreambuf_iterator<char>(mapFile)), std::istreambuf_iterator<char>());
    mapFile.close();

    // 获取当前地图路径文件
    std::string pathBin = "/tmp/cleanPath.bin";
    std::ifstream pathFile(pathBin, std::ios::binary);
    if (!pathFile.is_open())
    {
        LOGE(TAG, "Failed to open path file: {}", pathBin);
        return false;
    }
    std::vector<uint8_t> pathData((std::istreambuf_iterator<char>(pathFile)), std::istreambuf_iterator<char>());
    pathFile.close();

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
    // RecordId_BeginTime_CleanTime_CleanArea_MapLen_PathLen_VirtualLen_CleanMode_WorkMode_CleaningResult_StartMethod
    
    sprintf(descript, "%d_%s_%d_%d_%d_%d_%d_%d_%d_%d_%d", recordId, endTime,
            cleanTime, cleanArea, mapData.size(), pathData.size(), tuyaVirtualWall.size() + tuyaRestrictedArea.size(), cleanMode, workMode, cleaningResult, startMethod);
    LOGD(TAG, "upload_record record id: {} , descript: {}", recordId, descript);

    // 拼接地图、轨迹、虚拟墙和禁区数据
    std::vector<uint8_t> combinedData;
    combinedData.insert(combinedData.end(), mapData.begin(), mapData.end());
    combinedData.insert(combinedData.end(), pathData.begin(), pathData.end());
    combinedData.insert(combinedData.end(), tuyaVirtualWall.begin(), tuyaVirtualWall.end());
    combinedData.insert(combinedData.end(), tuyaRestrictedArea.begin(), tuyaRestrictedArea.end());

    OPERATE_RET op_ret = tuya_iot_map_record_upload_buffer(
        recordId, combinedData.data(), combinedData.size(), ((strlen(descript) == 0) ? NULL : descript));
    if (op_ret == OPRT_OK) 
    {
        LOGD(TAG, "upload map id:{} record files OK", recordId);
    } else 
    {
        LOGE(TAG, "upload map id:{} record files fail, ret {}", recordId,
           op_ret);
        return false;
    }
    return true;
}

void TuyaComm::ReportMap()
{
}

void TuyaComm::ReportPath()
{
}

void TuyaComm::ReportMapAll()
{
    LOGD(TAG, "ReportMapAll");
    // 上报禁区/划区
    AppRestrictedArea area;
    tuya_message::Request req = {};
    if(Send("ty_get_restricted_area", &req, &area) && area.version >= 0)
    {
        TuyaReportRestrictedArea(GetRawDpId(DPRaw_HandleCommand), area.version == 0 ? 0x1B : 0x39, &area);
    }
    else
    {
        LOGE(TAG, "无法获取到禁区信息");
    }
    AppZoneClean zone;
    if(Send("ty_get_zone_clean", &req, &zone) && zone.count > 0)
    {
        TuyaReportZoneClean(GetRawDpId(DPRaw_HandleCommand), zone.version == 0 ? 0x29 : 0x3A, &zone);
    }
    else
    {
        LOGE(TAG, "无法获取到划区信息");
    }

    // AppSpotClean sc;
    // if (TuyaComm::Get()->Send("ty_get_spot", &req, &sc))
    // {
    //     TuyaReportSpotClean(GetRawDpId(DPRaw_HandleCommand), 0x17, &sc);
    // }
    // else
    // {
    //     LOGE(TAG, "无法获取到定点清扫信息");
    // }

    AppRoomClean room;
    if(Send("ty_get_clean_blocks", &req, &room) && room.count > 0)
    {
        TuyaReportRoomClean(GetRawDpId(DPRaw_HandleCommand), 0x15, &room);
    }
    else
    {
        LOGE(TAG, "无法获取到分区清扫信息");
    }
    // 上报虚拟墙
    AppVirtualWall wall;
    if(Send("ty_get_virtual_wall", &req, &wall) && wall.version >= 0)
    {
        TuyaReportVirtualWall(GetRawDpId(DPRaw_HandleCommand), wall.version == 0 ? 0x13 : 0x49, &wall);
    }
    else
    {
        LOGE(TAG, "无法获取到虚拟墙信息");
    }
#if 0
    // 上报清扫顺序
    LOGD(TAG, "ReportCleanSequence");
    AppSetCleaningSequence order = {};
    if(Send("ty_get_cleaning_sequence", &req, &order))
    {
        TuyaReportCleaningSequence(GetRawDpId(DPRaw_HandleCommand), 0x27, &order);
    }
    else
    {
        LOGE(TAG, "无法获取到 CleanSequence");
    }

    // 上报房间属性
    AppSetRoomProperties room_properties;
    if(Send("ty_get_room_properties", &req, &room_properties))
    {
        TuyaReportRoomProperties(GetRawDpId(DPRaw_HandleCommand), 0x23, &room_properties);
    }
    else
    {
        LOGE(TAG, "无法获取到房间属性信息");
    }
#endif
}

void TuyaComm::ReportAll()
{
    // 上报定时
    AppLocalAlert alert;
    tuya_message::Request req = {};
    if(Send("ty_get_m_orders", &req, &alert) && alert.verison >= 0)
    {   
        TuyaReportLocalAlert(32, alert.verison == 0 ? 0x31 : 0x45, &alert);
    } 
    else
    {
        LOGE(TAG, "无法获取到本地定时信息");
    }
    // alert.verison = -1;
    // alert.number = 1;
    // alert.timedTaskInfo.resize(1);
    // alert.timedTaskInfo[0].roomId.resize(1);
    // alert.timedTaskInfo[0].zoodId.resize(1);
    // if(Send("AC_LocalAlert", &alert) && alert.verison >= 0)
    // {
    //     // TimedTask::GetTimedTask()->AddTask(alert);
    //     TuyaReportLocalAlert(32, alert.verison == 0 ? 0x31 : 0x45, &alert);
    // }
    // else
    // {
    //     LOGE(TAG, "无法获取到定时任务信息");
    // }

    // 上报勿扰时间
    AppNotDisturbTime disturb;
    if(Send("ty_get_dnd", &req, &disturb) && disturb.version >= 0)
    {   
        TuyaReportNotDisturbTime(33, disturb.version==0 ? 0x33 : 0x41, &disturb);
    } 
    else
    {
        LOGE(TAG, "无法获取到勿扰时间信息");
    }

    // 全量地图数据 机器电量、边刷、滚刷、滤网寿命 音量设置等
    LOGD(TAG, "ReportStatus ---------------------------------");
    ReportStatus();
    LOGD(TAG, "ReportDeviceInfo ---------------------------------");
    ReportDeviceInfo();
    LOGD(TAG, "ReportPartsLife ---------------------------------");
    ReportPartsLife();
    LOGD(TAG, "ReportCleanInfo ---------------------------------");
    ReportCleanInfo();
    LOGD(TAG, "ReportMapAll ---------------------------------");
    ReportMapAll();
}