#pragma once

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
#include "mars_message/ClearComponent.hpp"
#include "mars_message/AppMisc.hpp"

using namespace mars_message;

#define PARAM_PATH "/oem/mars/datas/"

void VirtualWallSave(const AppVirtualWall *msg);
bool VirutalWallLoad(AppVirtualWall *msg);
void VirtualWallClear(AppVirtualWall *msg);

void RestrictedAreaSave(const AppRestrictedArea *msg);
bool RestrictedAreaLoad(AppRestrictedArea *msg);
void RestrictedAreaClear(AppRestrictedArea *msg);

void LocalAlertSave(const AppLocalAlert *msg);
bool LocalAlertLoad(AppLocalAlert *msg);

void NotDisturbTimeSave(const AppNotDisturbTime *msg);
bool NotDisturbTimeLoad(AppNotDisturbTime *msg);

void PartsLifeSave(const AppPartsLife *msg);
bool PartsLifeLoad(AppPartsLife *msg);

void CleanInfoSave(const AppCleanInfo *msg);
bool CleanInfoLoad(AppCleanInfo *msg);

int PowerToFanPwm(int suction);
int PowerToGTPwm(int power);
int PowerToMBrushPwm(int power);

class CleanParam
{
private:
    CleanParam()
    {
        virtualWall.version = -1;
        restrictedArea.version = -1;
        localAlert.verison = -1;
        notDiturbTime.version = -1;
        partsLife.edgeBrushLifeMinutes = 9000;
        partsLife.haipaLifeMinutes = 9000;
        partsLife.mainBrushLifeMinutes = 18000;
        partsLife.ragLifeMinutes = 9000;
        memset(&cleanInfo, 0, sizeof(cleanInfo));
        appMisc.verison = -1;
        memset(&clearComponent, 0, sizeof(clearComponent));
    }
public:
    static CleanParam * Get()
    {
        static CleanParam CleanParam;
        return &CleanParam;
    }

    AppVirtualWall virtualWall;
    AppRestrictedArea restrictedArea;
    AppLocalAlert   localAlert;
    AppNotDisturbTime   notDiturbTime;
    AppPartsLife    partsLife;
    AppCleanInfo    cleanInfo;
    ClearComponent  clearComponent;
    AppMisc         appMisc;

    template <typename T>
    void Save(T *msg, std::string path)
    {
        uint8_t buf[1024] = {0};
        int len = msg->encode(buf, 0, sizeof(buf));
        FILE *fp = fopen(path.c_str(), "wb");
        if(fp)
        {
            fwrite(buf, len, 1, fp);
            fflush(fp);
            fclose(fp);
        }
    }

    template <typename T>
    bool Load(T *msg, std::string path)
    {
        uint8_t buf[1024] = {0};
        path = (std::string)PARAM_PATH + path;
        FILE *fp = fopen(path.c_str(), "rb");
        if(fp)
        {
            int len = fread(buf, 1, sizeof(buf), fp);
            fclose(fp);
            if(len == msg->decode(buf, 0, len))
            {
                return true;
            }
            else
            {
                 LOGE("CP", "解析文件出错 {}", path);
            }
        }
        else
        {
            LOGE("CP", "打开文件出错 {}", path);
        }
        return false;
    }
};

