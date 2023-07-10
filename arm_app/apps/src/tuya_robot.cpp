#include <stdint.h>
#include <netinet/in.h>

#include <vector>
#include <map>

#include "utils/log_.h"
#include "lz4/lz4.h"

#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"

#include "robot_msg.h"
#include "tuya_robot.h"
#include "tuya_enums.h"
#include "tuya_dp_function.h"
#include "tuya_utils.h"
#include "clean_param.h"
#include "timed_task.hpp"

#define TAG "TUY"

#define APP_VERSION_FILE "/userdata/version/soft_version.json"

// 1. 清扫记录
void ReportCleanRecord(uint16_t recordId, std::string beginTime, int cleanTimeMinute,
                       int cleanArea, int mapLen, int pathLen, int virtualLen,
                       int cleanMode, int workMode, int cleanningResult, int startMethod)
{
}

// 2. 多层地图

// 3. 语音包
// 4. 设备信息

std::map<void*, int> ReportFuns =
{
    {(void*)&DP_ReportSwichGo, 1},
    {(void*)&DP_ReportPause, 2},
    {(void*)&DP_ReportSwichCharge, 3},
    {(void*)&DP_ReportMode, 4},
    {(void*)&DP_ReportStatus, 5},
    {(void*)&DP_ReportCleanTime, 6},
    {(void*)&DP_ReportCleanArea, 7},
    {(void*)&DP_ReportBatteryPrecentage, 8},
    {(void*)&DP_ReportSuction, 9},
    {(void*)&DP_ReportCistern, 10},
    {(void*)&DP_ReportSeek, 11},
    {(void*)&DP_ReportDirectionControl, 12},
    {(void*)&DP_ReportMapReset, 13},
    {(void*)&DP_ReportEdgeBrushLife, 17},
    {(void*)&DP_ReportEdgeBrushLifeReset, 18},
    {(void*)&DP_ReportRollBrushLife, 19},
    {(void*)&DP_ReportRollBrushLifeReset, 20},
    {(void*)&DP_ReportFilterLife, 21},
    {(void*)&DP_ReportFilterLifeReset, 22},
    {(void*)&DP_ReportRagLife, 23},
    {(void*)&DP_ReportRagLifeReset, 24},
    {(void*)&DP_ReportDonotDisturb, 25},
    {(void*)&DP_ReportVolumeSet, 26},
    {(void*)&DP_ReportBreakClean, 27},
    {(void*)&DP_ReportFault, 28},
    {(void*)&DP_ReportCleanAreaTotal, 29},
    {(void*)&DP_ReportCleanCountTotal, 30},
    {(void*)&DP_ReportCleanTimeTotal, 31},
    {(void*)&DP_ReportDeviceInfo, 34},
    {(void*)&DP_ReportMopState, 40},
    {(void*)&DP_ReportWorkMode, 41},
    {(void*)&DP_ReportUnitSet, 42},
    {(void*)&DP_ReportEstimatedArea, 43},
    {(void*)&DP_ReportCarpetCleanPrefer, 44},
    {(void*)&DP_ReportAutoBoost, 45},
    {(void*)&DP_ReportChildLock, 47},
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
    {53, DP_HandleWipingStrength}
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


void TuyaHandleDPCmd(TY_OBJ_DP_S *dp)
{
    LOGD(TAG, "DP cmd: {}", dp->dpid);
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

void TuyaReportStatus(SweeperStatus status)
{
    static SweeperStatus sweeper = {0};
    RobotStatus &mcu = status.mcuStatus;
    LOGD(TAG, "runningState:{}\n\
                pauseStatus:{}\n\
                chargeStatus:{}\n\
                cleanMode:{}\n\
                cleanStatus:{}\n\
                cleanTime:{}\n \
                cleanArea:{}\n \
                battLevel:{}\n \
                suctionPower:{}\n\
                cistern:{}\n\
                error:{}",
         status.runningState,
         status.pauseStatus,
         status.chargeStatus,
         M2S(status.cleanMode),
         M2S(status.cleanStatus),
         status.cleanTime,
         status.cleanArea,
         mcu.battLevel,
         M2S(status.suctionPower),
         M2S(status.cistern),
         status.mcuStatus.error);
    std::vector<TY_OBJ_DP_S> dps;
    dps.emplace_back(DP_ReportSwichGo(ReportFuns[(void*)&DP_ReportSwichGo], status.runningState));
    dps.emplace_back(DP_ReportPause(ReportFuns[(void*)&DP_ReportPause], status.pauseStatus));
    dps.emplace_back(DP_ReportSwichCharge(ReportFuns[(void*)&DP_ReportSwichCharge], status.chargeStatus));
    dps.emplace_back(DP_ReportMode(ReportFuns[(void*)&DP_ReportMode], MarsModeToTuya(status.cleanMode)));
    dps.emplace_back(DP_ReportStatus(ReportFuns[(void*)&DP_ReportStatus], MarsStatusToTuya(status.cleanStatus)));
    dps.emplace_back(DP_ReportMopState(ReportFuns[(void*)&DP_ReportMopState], 1));

    LOGD(TAG, "status = {}", MarsStatusToTuya(status.cleanStatus));
    // dps.emplace_back(DP_ReportCleanTime(ReportFuns[(void*)&DP_ReportCleanTime], status.cleanTime));
    // dps.emplace_back(DP_ReportCleanArea(ReportFuns[(void*)&DP_ReportCleanArea], status.cleanArea));
    dps.emplace_back(DP_ReportBatteryPrecentage(ReportFuns[(void*)&DP_ReportBatteryPrecentage], mcu.battLevel));
    dps.emplace_back(DP_ReportSuction(ReportFuns[(void*)&DP_ReportSuction], MarsSuctionToTuya(status.suctionPower)));
    dps.emplace_back(DP_ReportCistern(ReportFuns[(void*)&DP_ReportCistern], MarsCisternToTuya(status.cistern)));
    // if (mcu.error)
    // {
    //     dps.emplace_back(DP28_Fault(MarsFaultToTuya(mcu.error)));
    // }
    // else
    // {
    //     dps.emplace_back(DP28_Fault(0));
    // }
    dev_report_dp_json_async(NULL, &dps[0], dps.size());

    static int64_t lastDeviceInfoTime = 0;
    if (TimeTick::Ms() - lastDeviceInfoTime > 3600 * 1000)
    {
        lastDeviceInfoTime = TimeTick::Ms();

        char ssid[128] = {0};
        tuya_adapter_wifi_get_ssid(ssid);
        std::string wifiName = ssid;

        int rssi = 0;
        SCHAR_T rssiT;
        tuya_adapter_wifi_station_get_conn_ap_rssi(&rssiT);

        NW_IP_S ips = {0};
        tuya_adapter_wifi_get_ip(WF_STATION, &ips);
        std::string ip = ips.ip;

        NW_MAC_S macs = {0};
        char macStr[32] = {0};
        tuya_adapter_wifi_get_mac(WF_STATION, &macs);
        sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", macs.mac[0], macs.mac[1], macs.mac[2], macs.mac[3], macs.mac[4], macs.mac[5]);
        std::string mac = macStr;

        std::string soft_version;
        std::string MCU_version;
        std::string system_version;
        SoftVersion(APP_VERSION_FILE, soft_version, MCU_version, system_version);
        if (soft_version.empty())
        {
            soft_version = "1.0.0";
        }

        std::string mcuVersion = "0.0.0";
        std::string firmwareVersion = soft_version;
        std::string deviceSN = mac;
        std::string moduleUUID = mac + "_UUID";
        std::string baseStationSN = mac + "_BASE";
        std::string baseStationVersion = "0.0.0";
        std::string baseStationLocalVersion = "0.0.0";
        DP_ReportDeviceInfo(ReportFuns[(void*)&DP_ReportDeviceInfo], wifiName, rssi, ip, mac, mcuVersion, firmwareVersion, deviceSN, moduleUUID, baseStationSN, baseStationVersion, baseStationLocalVersion);
    }
}

void TuyaComm::ReportStatus()
{
    if(Send("AQ_Status", &sweeperStatus))
    {
        TuyaReportStatus(sweeperStatus);
    }
}

void TuyaComm::OnMap(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppMap *msg)
{
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
void TuyaComm::OnSweeperStatus(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const SweeperStatus *msg)
{
    TuyaReportStatus(*msg);
}

void TuyaComm::OnPartsLife(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppPartsLife *msg)
{
    std::vector<TY_OBJ_DP_S> dps;
    if (partsLife.edgeBrushLifeMinutes != msg->edgeBrushLifeMinutes)
    {
        dps.emplace_back(DP_ReportEdgeBrushLife(ReportFuns[(void*)&DP_ReportEdgeBrushLife], msg->edgeBrushLifeMinutes));
    }
    if (partsLife.haipaLifeMinutes != msg->haipaLifeMinutes)
    {
        dps.emplace_back(DP_ReportFilterLife(ReportFuns[(void*)&DP_ReportFilterLife], msg->haipaLifeMinutes));
    }
    if (partsLife.mainBrushLifeMinutes != msg->mainBrushLifeMinutes)
    {
        dps.emplace_back(DP_ReportRollBrushLife(ReportFuns[(void*)&DP_ReportRollBrushLife], msg->mainBrushLifeMinutes));
    }
    if (partsLife.ragLifeMinutes != msg->ragLifeMinutes)
    {
        dps.emplace_back(DP_ReportRagLife(ReportFuns[(void*)&DP_ReportRagLife], msg->ragLifeMinutes));
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
            msg->cleanTimeSecond, msg->cleanArea, msg->cleanAreaTotal, msg->cleanCountTotal, msg->cleanTimeTotalSecond);
    {
        std::vector<TY_OBJ_DP_S> dps;
        if (cleanInfo.cleanTimeSecond != msg->cleanTimeSecond)
        {
            dps.emplace_back(DP_ReportCleanTime(ReportFuns[(void*)&DP_ReportCleanTime], msg->cleanTimeSecond));
        }
        if (cleanInfo.cleanArea != msg->cleanArea)
        {
            dps.emplace_back(DP_ReportCleanArea(ReportFuns[(void*)&DP_ReportCleanArea], msg->cleanArea));
        }
        if (cleanInfo.cleanAreaTotal != msg->cleanAreaTotal)
        {
            dps.emplace_back(DP_ReportCleanAreaTotal(ReportFuns[(void*)&DP_ReportCleanAreaTotal], msg->cleanAreaTotal));
        }
        if (cleanInfo.cleanCountTotal != msg->cleanCountTotal)
        {
            dps.emplace_back(DP_ReportCleanCountTotal(ReportFuns[(void*)&DP_ReportCleanCountTotal], msg->cleanCountTotal));
        }
        if (cleanInfo.cleanTimeTotalSecond != msg->cleanTimeTotalSecond)
        {
            dps.emplace_back(DP_ReportCleanTimeTotal(ReportFuns[(void*)&DP_ReportCleanTimeTotal], msg->cleanTimeTotalSecond));
        }
        if (cleanInfo.esimateArea != msg->esimateArea)
        {
            dps.emplace_back(DP_ReportEstimatedArea(ReportFuns[(void*)&DP_ReportEstimatedArea], msg->esimateArea));
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
    ReportCleanRecords(msg->recordId, msg->cleanTimeSecond/60.0 + 0.5, msg->cleanArea, MarsModeToTuya(msg->cleanMode), MarsCleanMethodToTuya((CleanMethod)msg->cleanMethod), msg->finishResult, msg->startReason);
}

void TuyaComm::ReportPartsLife()
{
    LOGD(TAG, "ReportPartsLife");
    std::vector<TY_OBJ_DP_S> dps;
    AppPartsLife life;
    if(CleanParam::Get()->Load(&life, "PartsLife.msg"))
    {
        AppPartsLife *msg = &life;
        dps.emplace_back(DP_ReportEdgeBrushLife(ReportFuns[(void*)&DP_ReportEdgeBrushLife], msg->edgeBrushLifeMinutes));
        dps.emplace_back(DP_ReportFilterLife(ReportFuns[(void*)&DP_ReportFilterLife], msg->haipaLifeMinutes));
        dps.emplace_back(DP_ReportRollBrushLife(ReportFuns[(void*)&DP_ReportRollBrushLife], msg->mainBrushLifeMinutes));
        dps.emplace_back(DP_ReportRagLife(ReportFuns[(void*)&DP_ReportRagLife], msg->ragLifeMinutes));
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
    AppCleanInfo info;
    AppCleanInfo *msg = &info;
    if(CleanParam::Get()->Load(&info, "CleanInfo.msg"))
    {
        dps.emplace_back(DP_ReportCleanTime(ReportFuns[(void*)&DP_ReportCleanTime], msg->cleanTimeSecond));
        dps.emplace_back(DP_ReportCleanArea(ReportFuns[(void*)&DP_ReportCleanArea], msg->cleanArea));
        dps.emplace_back(DP_ReportCleanAreaTotal(ReportFuns[(void*)&DP_ReportCleanAreaTotal], msg->cleanAreaTotal));
        dps.emplace_back(DP_ReportCleanCountTotal(ReportFuns[(void*)&DP_ReportCleanCountTotal], msg->cleanCountTotal));
        dps.emplace_back(DP_ReportCleanTimeTotal(ReportFuns[(void*)&DP_ReportCleanTimeTotal], msg->cleanTimeTotalSecond));
        dps.emplace_back(DP_ReportEstimatedArea(ReportFuns[(void*)&DP_ReportEstimatedArea], msg->esimateArea));
        if (!dps.empty())
        {
            dev_report_dp_json_async(NULL, &dps[0], dps.size());
        }
    }
    else
    {
        LOGE(TAG, "无法获取到 CleanInfo");
    }
}

void TuyaComm::ReportCleanRecords(int recordId, int cleanTime, int cleanArea, int cleanMode, int workMode, int cleaningResult, int startMethod) 
{
    time_t nowTime = time(NULL);
    struct tm *local = localtime(&nowTime);
    char endTime[50] = {0};
    strftime(endTime, 50, "%Y%m%d_%H%M%S", local);
    LOGD(TAG, "{}", endTime);

    unsigned int mapId = 0;
    char descript[128] = {0};
    int actualRead = 0;
    int virtualLen = 0;
    //获取地图数据
    struct stat statbuff;
    unsigned int len = 0;
    std::string mapBin = "/tmp/map.bin";
    FILE_INFO strMapInfo;
    bool ret = ReadFile(mapBin.c_str(), &strMapInfo);
    if (!ret) 
    {
        LOGE(TAG, "get file {} fail", mapBin.c_str());
        strMapInfo.len = 0;
        return;
    } else 
    {
        actualRead += strMapInfo.len;
    }
    //获取清洁路径数据
    FILE_INFO strcleanPathInfo;
    std::string cleanPathBin = "/tmp/cleanPath.bin";
    ret = ReadFile(cleanPathBin.c_str(), &strcleanPathInfo);
    if (!ret) 
    {
        LOGE(TAG, "get file {} fail", cleanPathBin.c_str());
        strcleanPathInfo.len = 0;
    } else 
    {
        actualRead += strcleanPathInfo.len;
    }
    // 获取虚拟墙数据
    AppVirtualWall wall;
    wall.version = 0;
    wall.count = 0;
    std::vector<uint8_t> tuyaVirtualWall;
    if (CleanParam::Get()->Load(&wall, "VirtualWall.msg"))
    {
        AppVirtualWall *msg = &wall;
        if (msg->version == 0) 
        {
            ret = ToTuyaVirtualWall(wall, 0x13, tuyaVirtualWall);
        } else 
        {
            ret = ToTuyaVirtualWall(wall, 0x49, tuyaVirtualWall);
        }
        if (!ret) 
        {
            LOGE(TAG, "get virtual wall fail");
        } else 
        {
            actualRead += tuyaVirtualWall.size();
            virtualLen += tuyaVirtualWall.size();
            LOGD(TAG, "tuyaVirtualWall size: {}", tuyaVirtualWall.size());
        }
    } else 
    {
        LOGE(TAG, "无法获取到虚拟墙信息");
    }
    // 获取禁区数据
    AppRestrictedArea area;
    std::vector<uint8_t> tuyaRestrictedArea;
    if (CleanParam::Get()->Load(&area, "RestrictedArea.msg")) 
    {
        AppRestrictedArea *msg = &area;
        if (msg->version == 0) 
        {
            ret = ToTuyaRestrictedArea(area, 0x1B, tuyaRestrictedArea);
        } else 
        {
            ret = ToTuyaRestrictedArea(area, 0x39, tuyaRestrictedArea);
        }
        if (!ret) 
        {
            LOGE(TAG, "get restricted area fail");
        } else {
            actualRead += tuyaRestrictedArea.size();
            virtualLen += tuyaRestrictedArea.size();
            LOGD(TAG, "tuyaRestrictedArea size: {}", tuyaRestrictedArea.size());
        }
    } else 
    {
        LOGE(TAG, "无法获取到禁区信息");
    }
    
    sprintf(descript, "%d_%s_%d_%d_%d_%d_%d_%d_%d_%d_%d", recordId, endTime,
            cleanTime, cleanArea, strMapInfo.len, strcleanPathInfo.len,
            virtualLen, cleanMode, workMode, cleaningResult, startMethod);
    LOGD(TAG, "upload_record map id: {} , descript: {}", mapId, descript);
    LOGD(TAG, "actualRead: {}", actualRead);
    BYTE_T *buf = (BYTE_T *)malloc(actualRead);
    if (buf == NULL) 
    {
        LOGE(TAG, "Malloc Fail {}", actualRead);
        free(strMapInfo.buff);
        strMapInfo.buff = NULL;
        free(strcleanPathInfo.buff);
        strcleanPathInfo.buff = NULL;
        return;
    }
    memcpy(buf, strMapInfo.buff, strMapInfo.len);
    memcpy(buf + strMapInfo.len, strcleanPathInfo.buff, strcleanPathInfo.len);
    memcpy(buf + strMapInfo.len + strcleanPathInfo.len, &tuyaVirtualWall[0], tuyaVirtualWall.size());
    memcpy(buf + strMapInfo.len + strcleanPathInfo.len + tuyaVirtualWall.size(), &tuyaRestrictedArea[0], tuyaRestrictedArea.size());
    free(strMapInfo.buff);
    strMapInfo.buff = NULL;
    free(strcleanPathInfo.buff);
    strcleanPathInfo.buff = NULL;

    OPERATE_RET op_ret = tuya_iot_map_record_upload_buffer(
        mapId, buf, actualRead, ((strlen(descript) == 0) ? NULL : descript));
    if (op_ret == OPRT_OK) 
    {
        LOGD(TAG, "upload map id:{} record files OK", mapId);
    } else 
    {
        LOGE(TAG, "upload map id:{} record files fail, ret {}", mapId,
           op_ret);
    }
    free(buf);
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
    AppVirtualWall wall;
    wall.version = 0;
    wall.count = 0;
    if(CleanParam::Get()->Load(&wall, "VirtualWall.msg"))
    {
        AppVirtualWall *msg = &wall;
        if(msg->version == 0)
        {
            TuyaReportVirtualWall(GetRawDpId(DPRaw_HandleCommand), 0x13, (AppVirtualWall *)msg);
        }
        else
        {
            TuyaReportVirtualWall(GetRawDpId(DPRaw_HandleCommand), 0x49, (AppVirtualWall* )msg);
        }
    }
    else
    {
        LOGE(TAG, "无法获取到虚拟墙信息");
    }
    AppRestrictedArea area;
    if(CleanParam::Get()->Load(&area, "RestrictedArea.msg"))
    {
        AppRestrictedArea *msg = &area;
        if(msg->version == 0)
        {
            TuyaReportRestrictedArea(GetRawDpId(DPRaw_HandleCommand), 0x1B, (AppRestrictedArea *)msg);

        }
        else
        {
            TuyaReportRestrictedArea(GetRawDpId(DPRaw_HandleCommand), 0x39, (AppRestrictedArea *)msg);

        }
    }
    else
    {
        LOGE(TAG, "无法获取到禁区信息");
    }
}

void TuyaComm::ReportAll()
{
    // 上报定时
    AppLocalAlert alert;
    alert.verison = -1;
    alert.number = 1;
    alert.timedTaskInfo.resize(1);
    alert.timedTaskInfo[0].roomId.resize(1);
    alert.timedTaskInfo[0].zoodId.resize(1);
    if(Send("AC_LocalAlert", &alert) && alert.verison >= 0)
    {
        TimedTask::GetTimedTask()->AddTask(alert);
        TuyaReportLocalAlert(32, alert.verison == 0 ? 0x31 : 0x45, &alert);
    }
    else
    {
        LOGE(TAG, "无法获取到定时任务信息");
    }

    // 上报勿扰时间
    AppNotDisturbTime disturb;
    disturb.version = -1;
    if(Send("AC_NotDisturbTime", &disturb) && disturb.version >= 0)
    {   
        TimedTask::GetTimedTask()->SetNotDisturbTime(disturb);
        TuyaReportNotDisturbTime(33, disturb.version==0 ? 0x33 : 0x41, &disturb);
    } 
    else
    {
        LOGE(TAG, "无法获取到勿扰时间信息");
    }

    // 全量地图数据 机器电量、边刷、滚刷、滤网寿命 音量设置等
    ReportStatus();
    ReportPartsLife();
    ReportCleanInfo();
    ReportMapAll();
}