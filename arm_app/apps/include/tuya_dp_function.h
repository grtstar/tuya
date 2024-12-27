#pragma once
#include <stdint.h>
#include <string>

#include "tuya_cloud_com_defs.h"
#include "tuya_iot_com_api.h"

#include "mars_message/AppPath.hpp"
#include "mars_message/AppPartitionDivision.hpp"
#include "mars_message/AppPartitionMerge.hpp"
#include "mars_message/AppRestrictedArea.hpp"
#include "mars_message/AppRoomClean.hpp"
#include "mars_message/AppSetCleaningSequence.hpp"
#include "mars_message/AppSetRoomName.hpp"
#include "mars_message/AppSetRoomProperties.hpp"
#include "mars_message/AppSpotClean.hpp"
#include "mars_message/AppVirtualWall.hpp"
#include "mars_message/AppZoneClean.hpp"
#include "mars_message/AppLocalAlert.hpp"
#include "mars_message/AppNotDisturbTime.hpp"
#include "mars_message/AppPartsLife.hpp"
#include "mars_message/AppCleanInfo.hpp"

using namespace mars_message;

TY_OBJ_DP_S DPReportBool(int dpId, bool b);
TY_OBJ_DP_S DPReportEnum(int dpId, int e);
TY_OBJ_DP_S DPReportValue(int dpId, int v);
TY_OBJ_DP_S DPReportBitmap(int dpId, uint32_t b);

TY_OBJ_DP_S DP_ReportSwichGo(int dpId, bool b);
void DP_HandleSwichGo(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportPause(int dpId, bool b);
void DP_HandlePause(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportSwichCharge(int dpId, bool b);
void DP_HandleSwichCharge(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportMode(int dpId, int mode);
void DP_HandleMode(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportStatus(int dpId, int status);

TY_OBJ_DP_S DP_ReportCleanTime(int dpId, int second);

TY_OBJ_DP_S DP_ReportCleanArea(int dpId, float cleanArea);

TY_OBJ_DP_S DP_ReportBatteryPrecentage(int dpId, int percent);

TY_OBJ_DP_S DP_ReportSuction(int dpId, int power);
void DP_HandleSuction(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportCistern(int dpId, int cistern);
void DP_HandleCistern(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportSeek(int dpId, bool b);
void DP_HandleSeek(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportDirectionControl(int dpId, int direction);
void DP_HandleDirectionControl(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportMapReset(int dpId, bool b);
void DP_HandleMapReset(TY_OBJ_DP_S *dp);

void DPRaw_HandlePathData(int dpId, uint8_t *data, int len);

void DPRaw_HandleCommand(int dpId, uint8_t *data, int len);

void DP_HandleRequest(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportEdgeBrushLife(int dpId, int life);
void DP_HandleEdgeBrushLife(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportEdgeBrushLifeReset(int dpId, bool b);
void DP_HandleEdgeBrushLifeReset(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportRollBrushLife(int dpId, int life);
void DP_HandleRollBrushLife(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportRollBrushLifeReset(int dpId, bool b);
void DP_HandleRollBrushLifeReset(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportFilterLife(int dpId, int life);
TY_OBJ_DP_S DP_ReportFilterLifeReset(int dpId, bool b);
void DP_HandleFilterLifeReset(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportRagLife(int dpId, int life);
TY_OBJ_DP_S DP_ReportRagLifeReset(int dpId, bool b);
void DP_HandleRagLifeReset(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportDonotDisturb(int dpId, bool b);
void DP_HandleDonotDisturb(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportVolumeSet(int dpId, int v);
void DP_HandleVolumeSet(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportBreakClean(int dpId, bool b);
void DP_HandleBreakClean(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportFault(int dpId, uint32_t fault);
void DP_HandleFault(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportCleanAreaTotal(int dpId, int cleanAreaTotal);

TY_OBJ_DP_S DP_ReportCleanCountTotal(int dpId, int cleanCountTotal);

TY_OBJ_DP_S DP_ReportCleanTimeTotal(int dpId, int cleanTimeTotalSecond);

void DPRaw_HandleDeviceTimer(int dpId, uint8_t *data, int len);

void DPRaw_HandleDisturbTimeSet(int dpId, uint8_t *data, int len);

void DP_ReportDeviceInfo(int dpId, std::string wifiName, int rssi, std::string ip, std::string mac,
                     std::string mcuVersion, std::string firmwareVersion, std::string deviceSN, std::string moudleUUID,
                     std::string baseStationSN, std::string baseStationVersion, std::string baseStationLocalVersion);

void DPRaw_HandleVoiceData(int dpId, uint8_t *data, int len);

void DP_HandleLanguage(TY_OBJ_DP_S *dp);

void DP_HandleDustCollectionNum(TY_OBJ_DP_S *dp);

void DP_HandleDustCollectionSwitch(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportCustomizeModeSwitch(int dpId, bool state);
void DP_HandleCustomizeModeSwitch(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportMopState(int dpId, int state);

TY_OBJ_DP_S DP_ReportWorkMode(int dpId, int workMode);
void DP_HandleWorkMode(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportUnitSet(int dpId, int unit);
void DP_HandleUnitSet(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportEstimatedArea(int dpId, int area);

TY_OBJ_DP_S DP_ReportCarpetCleanPrefer(int dpId, int prefer);

void DP_HandleCarpetCleanPrefer(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportAutoBoost(int dpId, bool b);
void DP_HandleAutoBoost(TY_OBJ_DP_S *dp);

void DP_HandleCruiseSwitch(TY_OBJ_DP_S *dp);

TY_OBJ_DP_S DP_ReportChildLock(int dpId, bool b);
void DP_HandleChildLock(TY_OBJ_DP_S *dp);

void DP48_HandleYMop(TY_OBJ_DP_S *dp);

void DP_HandleSelfCleanMop(TY_OBJ_DP_S *dp);

void DP_HandleDryingSwitch(TY_OBJ_DP_S *dp);

void DP_HandleMopSlefCleaningFrequency(TY_OBJ_DP_S *dp);

void DP_HandleMopSlefCleaningStrength(TY_OBJ_DP_S *dp);

void DP_HandleWipingStrength(TY_OBJ_DP_S *dp);

void DP_HandleSmartEdge(TY_OBJ_DP_S *dp);
void DP_HandleUploadLogs(TY_OBJ_DP_S *dp);
void DP_HandleAvoidObsMode(TY_OBJ_DP_S *dp);
void DP_HandleKeyLightDisplaySwitch(TY_OBJ_DP_S *dp);
void DP_HandleStationLightDisplaySwitch(TY_OBJ_DP_S *dp);
void DP_HandleStationHotFan(TY_OBJ_DP_S *dp);
void DP_HandleStationHotWater(TY_OBJ_DP_S *dp);
void DP_HandleStationHotFanTime(TY_OBJ_DP_S *dp);

void TuyaReportVirtualWall(int dpId, uint8_t cmd, AppVirtualWall *msg);
void TuyaReportRestrictedArea(int dpId, uint8_t cmd, AppRestrictedArea *msg);
void TuyaReportSpotClean(int dpId, uint8_t cmd, AppSpotClean *msg);
void TuyaReportZoneClean(int dpId, uint8_t cmd, AppZoneClean *msg);
void TuyaReportLocalAlert(int dpId, uint8_t cmd, AppLocalAlert *msg);
void TuyaReportNotDisturbTime(int dpId, uint8_t cmd, AppNotDisturbTime *msg);
void TuyaReportRoomClean(int dpId, uint8_t cmd, AppRoomClean *msg);
void TuyaReportRoomName(int dpId, uint8_t cmd, AppSetRoomName *msg);
void TuyaReportCleaningSequence(int dpId, uint8_t cmd, AppSetCleaningSequence *msg);
void TuyaReportPartitionDivision(int dpId, uint8_t cmd, AppPartitionDivision *msg);
void TuyaReportPartitionRestoreDefault(int dpId, uint8_t cmd, Event *msg);
void TuyaReportRoomProperties(int dpId, uint8_t cmd, AppSetRoomProperties *msg);
void TuyaReportPartitionMerge(int dpId, uint8_t cmd, AppPartitionMerge *msg);

void TuyaHandleStandardFunction(int dpId, uint8_t *data, int len);
void TuyaHandleExtentedFuction(int dpId, uint8_t *data, int len);

std::vector<uint8_t> ToVirtualWallData(uint8_t cmd, AppVirtualWall *msg);
std::vector<uint8_t> ToRestrictedAreaData(uint8_t cmd, AppRestrictedArea *msg);





