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

#include "utils/log_.h"

using namespace mars_message;

class TuyaComm
{
private:
    SweeperStatus sweeperStatus;
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
        lcm_->subscribe("AS_Map", &TuyaComm::OnMap, this);
        lcm_->subscribe("AS_Path", &TuyaComm::OnPath, this);
        lcm_->subscribe("AS_VirtualWall", &TuyaComm::OnVirtualWall, this);
        lcm_->subscribe("AS_RestrictedArea", &TuyaComm::OnRestrictedArea, this);
        lcm_->subscribe("AS_CleanRecord", &TuyaComm::OnCleanRecord, this);

        //lcm_->subscribe("APPCMD", &TuyaComm::OnCmd, this);
        lcm_->subscribe("AC_Status", &TuyaComm::OnSweeperStatus, this);
        lcm_->subscribe("AC_PartsLife", &TuyaComm::OnPartsLife, this);
        lcm_->subscribe("AC_CleanInfo", &TuyaComm::OnCleanInfo, this);
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

    template <class MessageType>
    inline bool Send(const std::string &channel, MessageType *msg, int timeoutMs = 100)
    {
        int threadId = pthread_self();
        if(handlerId == 0)
        {
            handlerId = threadId;
        }
        bool b = false;
        int r = lcm_->publish(channel, msg);
        lcm::LCM::HandlerFunction<MessageType> handler = [&b, &msg](const lcm::ReceiveBuffer *rbuf,
                                               const std::string &channel, const MessageType *ret){
            *msg = *ret;                                    
            b = true;                                        
        };
        auto sub = lcm_->subscribe(channel + "_Ack", handler);
        int64_t startTime = TimeTick::Ms();
        while(TimeTick::Ms() - startTime < timeoutMs)
        {
            if(threadId == handlerId)
            {
                lcm_->handleTimeout(1);
            }
            if(b == true)
            {
                break;
            }
        }
        lcm_->unsubscribe(sub);
        LOGD("COMM", "Send return: {}", b);
        return b;
    }

    template <class MessageTypeReq, class MessageTypeRsp>
    inline bool Call(const std::string &channel, const MessageTypeReq *req, MessageTypeRsp *rsp, int timeoutMs)
    {
        int threadId = pthread_self();
        if(handlerId == 0)
        {
            handlerId = threadId;
        }
        bool b = false;
        int r = lcm_->publish(channel, req);
        lcm::LCM::HandlerFunction<MessageTypeRsp> handler =[&b, &rsp](const lcm::ReceiveBuffer *rbuf,
                                               const std::string &channel, const MessageTypeRsp *ret){
            *rsp = *ret;                                    
            b = true;                                        
        };
        auto sub = lcm_->subscribe(channel + "_Rsp", handler);
        int64_t startTime = TimeTick::Ms();
        while(TimeTick::Ms() - startTime < timeoutMs)
        {
            if(threadId == handlerId)
            {
                lcm_->handleTimeout(1);
            }
            if(b == true)
            {
                LOGD("COMM", "Get response {}", channel);
                break;
            }
        }
        lcm_->unsubscribe(sub);
        return b;
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
    void OnSweeperStatus(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const SweeperStatus *msg);
    void OnPartsLife(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppPartsLife *msg);
    void OnCleanInfo(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppCleanInfo *msg);
    void OnVirtualWall(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppVirtualWall *msg);
    void OnRestrictedArea(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppRestrictedArea *msg);
    void OnCleanRecord(const lcm::ReceiveBuffer *rbuf, const std::string &channel, const AppCleanRecord *msg);
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
