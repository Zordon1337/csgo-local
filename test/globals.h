#pragma once
#include "steamsdk/isteamgamecoordinator.h"
#include "steamsdk/isteammatchmaking.h"
#include "steamsdk/isteamuser.h"
#include "sdk/igameevent.h"
#include "sdk/ivengineclient.h"
#include "matchmaking.h"
#include "sdk/iglobalvars.h"

namespace G {

    ISteamClient* g_SteamClient = nullptr;
    ISteamHTTP* g_SteamHTTP = nullptr;
    ISteamUser* g_SteamUser = nullptr;
    ISteamFriends* g_SteamFriends = nullptr;
    ISteamInventory* g_SteamInventory = nullptr;
    ISteamGameCoordinator* g_GameCoordinator = nullptr;
    ISteamMatchmaking* g_SteamMatchMaking = nullptr;
    IGameEventManager2* g_EventManager = nullptr;
    IVEngineClient* g_EngineClient = nullptr;
    IGlobalVars* g_GlobalVars = nullptr;

#define MEDAL_BASE_ID 10000

    const char* versionString = "";
    int gameVer = 0;

}
