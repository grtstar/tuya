#include "tuya_enums.h"

TuyaFault MarsFaultToTuya(int fault)
{
    // todo
    return (TuyaFault)0;
}

TuyaMode MarsModeToTuya(int mode)
{
    TuyaMode v = DP4_SMART;
    switch (mode)
    {
    case CM_AUTO:
        v = DP4_SMART;
        break;
    case CM_GOTO_CHARGE:
        v = DP4_CHARGE_GO;
        break;
    case CM_ZONE:
        v = DP4_ZONE;
        break;
    case CM_POSE:
        v = DP4_POSE;
        break;
    case CM_PART:
        v = DP4_PART;
        break;
    case CM_ROOM:
        v = DP4_SELECT_ROOM;
        break;
    default:
        break;
    }
    return v;
}

int TuyaModeToMars(TuyaMode mode)
{
    int v = 0;
    switch (mode)
    {
    case DP4_SMART:
        v = CM_AUTO;
        break;
    case DP4_CHARGE_GO:
        v = CM_GOTO_CHARGE;
        break;
    case DP4_ZONE:
        v = CM_ZONE;
        break;
    case DP4_POSE:
        v = CM_POSE;
        break;
    case DP4_PART:
        v = CM_PART;
        break;
    case DP4_SELECT_ROOM:
        v = CM_ROOM;
        break;
    default:
        break;
    }
    return v;
}

TuyaStatus MarsStatusToTuya(int status)
{
    // todo
    TuyaStatus v = DP5_STANDBY;
    switch (status)
    {
    case CS_STANDBY:
        v = DP5_STANDBY;
        break;
    case CS_AUTO:
        v = DP5_SMART;
        break;
    case CS_ZONE_CLEAN:
        v = DP5_ZONE_CLEAN;
        break;
    case CS_PART_CLEAN:
        v = DP5_PART_CLEAN;
        break;
    case CS_GOTO_POS:
        v = DP5_GOTO_POS;
        break;
    case CS_POS_ARRIVED:
        v = DP5_POS_ARRIVED;
        break;
    case CS_POS_UNARRIVE:
        v = DP5_POS_UNARRIVE;
        break;
    case CS_PAUSE:
        v = DP5_PAUSED;
        break;
    case CS_GOTO_CHARGE:
        v = DP5_GOTO_CHARGE;
        break;
    case CS_CHARGING:
        v = DP5_CHARGING;
        break;
    case CS_ROOM:
        v = DP5_SELECT_ROOM;
        break;
    case CS_COLLECTING_DUST:
        v = DP5_COLLECTING_DUST;
        break;
    case CS_SELF_CLEAN:
        v = DP5_SELF_CLEAN;
        break;
    case CS_MAPPING:
        v = DP5_MAPPING;
        break;
    case CS_MAPPING_DONE:
        v = DP5_MAPPING_DONE;
        break;
    default:
        break;
    }
    return (TuyaStatus)v;
}

// int TuyaSuctionToMars(TuyaSuction suction)
// {
//     int v = PWR_CLOSED;
//     switch (suction)
//     {
//     case DP9_CLOSED:
//         v = PWR_CLOSED;
//         break;
//     case DP9_GENTLE:
//         v = PWR_SOFT;
//         break;
//     case DP9_NORMAL:
//         v = PWR_NORMAL;
//         break;
//     case DP9_STRONG:
//         v = PWR_STRONG;
//         break;
//     case DP9_MAX:
//         v = PWR_MAX;
//         break;
//     default:
//         break;
//     }
//     return v;
// }

// TuyaSuction MarsSuctionToTuya(int suction)
// {
//     TuyaSuction v = DP9_CLOSED;
//     switch (suction)
//     {
//     case PWR_CLOSED:
//         v = DP9_CLOSED;
//         break;
//     case PWR_SOFT:
//         v = DP9_GENTLE;
//         break;
//     case PWR_NORMAL:
//         v = DP9_NORMAL;
//         break;
//     case PWR_STRONG:
//         v = DP9_STRONG;
//         break;
//     case PWR_MAX:
//         v = DP9_MAX;
//         break;
//     default:
//         break;
//     }
//     return v;
// }

// TuyaCistern MarsCisternToTuya(int cistern)
// {
//     TuyaCistern v = DP10_CLOSED;
//     switch (cistern)
//     {
//     case PWR_CLOSED:
//         v = DP10_CLOSED;
//         break;
//     case PWR_SOFT:
//         v = DP10_LOW;
//         break;
//     case PWR_NORMAL:
//         v = DP10_MIDDLE;
//         break;
//     case PWR_STRONG:
//         v = DP10_HIGH;
//         break;
//     default:
//         break;
//     }
//     return v;
// }

// int TuyaCisternToMars(TuyaCistern cistern)
// {
//     int v = PWR_CLOSED;
//     switch (cistern)
//     {
//     case DP10_CLOSED:
//         v = PWR_CLOSED;
//         break;
//     case DP10_LOW:
//         v = PWR_SOFT;
//         break;
//     case DP10_MIDDLE:
//         v = PWR_NORMAL;
//         break;
//     case DP10_HIGH:
//         v = PWR_STRONG;
//         break;
//     default:
//         break;
//     }
//     return v;
// }

TuyaWorkMode MarsCleanMethodToTuya(CleanMethod method)
{
    TuyaWorkMode mode = WORK_BOTH;
    switch (method)
    {
    case CleanMethod::CM_BOTH:
        mode = WORK_BOTH;
        break;
    case CleanMethod::CM_MOP:
        mode = WORK_MOP;
        break;
    case CleanMethod::CM_SWEEP:
        mode = WORK_SWEEP;
        break;
    default:
        mode = WORK_AUTO;
        break;
    }
    return mode;
}

CleanMethod TuyaWorkModeToMars(int workMode)
{
    CleanMethod m = CleanMethod::CM_BOTH;
    switch (workMode)
    {
    case TuyaWorkMode::WORK_BOTH:
        m = CleanMethod::CM_BOTH;
        break;
    case TuyaWorkMode::WORK_SWEEP:
        m = CleanMethod::CM_SWEEP;
        break;
    case TuyaWorkMode::WORK_MOP:
        m = CleanMethod::CM_MOP;
        break;
    case TuyaWorkMode::WORK_AUTO:
    default:
        break;
    }
    return m;
}

int TuyaRoomIdToMars(int roomId)
{
    return -(roomId + 10);
}

int MarRoomIdToTuya(int roomId)
{
    return -(roomId + 10);
}