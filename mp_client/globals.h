#pragma once
#include "SDK/steamsdk/isteamgamecoordinator.h"
#include "SDK/steamsdk/isteammatchmaking.h"
#include "SDK/steamsdk/isteamuser.h"
#include "SDK/igameevent.h"
#include "SDK/ivengineclient.h"
#include "SDK/iglobalvars.h"
#include "SDK/icliententitylist.h"
#include "SDK/IVModelInfoClient.h"
#include "SDK/IMemAlloc.h"

#include "Logic/CMatchmaking.h"

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
    IClientEntityList* g_EntityList = nullptr;
    IVModelInfoClient* g_modelinfo = nullptr;
    IMemAlloc* g_MemAlloc = nullptr;
    void* g_VClient = nullptr;

#define MEDAL_BASE_ID 10000

    const char* versionString = "";
    int gameVer = 0;
    bool bIsPanoramaDll = false;
    bool bignoreWelcome = true;

}
