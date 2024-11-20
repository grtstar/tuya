#pragma once

#include <lcm/lcm-cpp.hpp>

#include "mars_message/GridMap.hpp"
#include "mars_message/SweeperStatus.hpp"
#include "mars_message/Debug.hpp"
#include "mars_message/Twist.hpp"
#include "mars_message/Event.hpp"

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
#include "mars_message/AppMap.hpp"
#include "mars_message/AppCleanRecord.hpp"

#include "message/tuya_messages.h"

using namespace mars_message;

class TuyaComm
{
private:
    GridMap         map;
    AppPath         path;
    AppPartsLife    partsLife;
    AppCleanInfo    cleanInfo;
    AppVirtualWall  virtualWall;
    AppRestrictedArea   restrictedArea;
    int handlerId;
    TuyaComm()
    {
        handlerId = 0;
    };
public:
    lcm::LCM * lcm_;

    static TuyaComm* Get()
    {
        static TuyaComm tuyaComm;
        return &tuyaComm;
    }
    void SetLcm(lcm::LCM *lcm)
    {
        lcm_ = lcm;
        lcm_->subscribe("ty_Map", &TuyaComm::OnMap, this);
        lcm_->subscribe("ty_Path", &TuyaComm::OnPath, this);
        lcm_->subscribe("ty_VirtualWall", &TuyaComm::OnVirtualWall, this);
        lcm_->subscribe("ty_RestrictedArea", &TuyaComm::OnRestrictedArea, this);
        lcm_->subscribe("ty_CleanRecord", &TuyaComm::OnCleanRecord, this);

        //lcm_->subscribe("APPCMD", &TuyaComm::OnCmd, this);
        lcm_->subscribe("ty_robot_state", &TuyaComm::OnSweeperStatus, this);
        lcm_->subscribe("ty_PartsLife", &TuyaComm::OnPartsLife, this);
        lcm_->subscribe("ty_CleanInfo", &TuyaComm::OnCleanInfo, this);
        lcm_->subscribe("ty_SaveMap", &TuyaComm::OnSaveMap, this);
        lcm_->subscribe("ty_UpdateMap", &TuyaComm::OnUpdateMap, this);
        lcm_->subscribe("Mevt", &TuyaComm::OnEvent, this);
        lcm_->subscribe("Kevt", &TuyaComm::OnEvent, this);
    }

    template <class MessageType>
    inline int Publish(const std::string &channel, const MessageType *msg)
    {
        return lcm_->publish(channel, msg);
    }

    template <class MessageType, class MessageHandlerClass>
    void* Subscribe(const std::string &channel,
                             void (MessageHandlerClass::*handlerMethod)(const lcm::ReceiveBuffer *rbuf,
                                                                        const std::string &channel,
                                                                        const MessageType *msg),
                             MessageHandlerClass *handler)
    {
        return lcm_->subscribe(channel, handlerMethod, handler);
    }

    template <class MessageType, class MessageTypeRet>
    inline int Send(const std::string &channel, MessageType *msg, MessageTypeRet *ret, int timeoutMs = 100, int retryTimes = 1)
    {
        return lcm_->send(channel, msg, ret, timeoutMs, retryTimes) == 0;
    }

    template <class MessageType>
    inline bool Send(const std::string &channel, MessageType *msg, int timeoutMs = 100, int retryTimes = 1)
    {
        MessageType ret;
        int r = lcm_->send(channel, msg, &ret, timeoutMs, retryTimes);
        *msg = ret;
        return r == 0;
    }

    void HandleForever()
    {
        handlerId = pthread_self();
        while (0 == lcm_->handle());
    }

    int HandleTimeout(int timeoutMs)
    {
        handlerId = pthread_self();
        return lcm_->handleTimeout(timeoutMs);
    }

public:
    void OnMap(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppMap *msg);
    void OnPath(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppPath *msg);
    void OnCmd(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const Debug *msg);
    void OnSweeperStatus(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const tuya_message::RobotState *msg);
    void OnPartsLife(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppPartsLife *msg);
    void OnCleanInfo(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppCleanInfo *msg);
    void OnVirtualWall(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppVirtualWall *msg);
    void OnRestrictedArea(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppRestrictedArea *msg);
    void OnCleanRecord(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppCleanRecord *msg);
    void OnEvent(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const Event *msg);
    void OnSaveMap(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const tuya_message::RobotEvent *msg);
    void OnUpdateMap(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const tuya_message::RobotEvent *msg);
public:
    void ReportStatus();
    void ReportMap();
    void ReportPath();
    void ReportMapAll();
    void ReportAll();
    void ReportPartsLife();
    void ReportCleanInfo();
    void ReportCleanRecords(int recordId, int cleanTime, int cleanArea, int cleanMode, int workMode, int cleaningResult, int startMethod);
};

void TuyaReportStatus(SweeperStatus status);
