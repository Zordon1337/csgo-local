

#include <windows.h>
#include "console/console.h"
#include <vector>
#include <string>
#include <iostream>

#include "protos/ProtoParse.h"
#include "protos/Messages.h"

#include "sdk/eventlistener.h"
#include "sdk/igameevent.h"
#include "globals.h"
#include "Minhook/MinHook.h"

#include <format>

#include "memory.h"
#include "sdk/networking.h"
#include "sdk/recv.h"
#include "vars.h"
enum ClientFrameStage {
    FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
    FRAME_START,

    // A network packet is being recieved
    FRAME_NET_UPDATE_START,
    // Data has been received and we're going to start calling PostDataUpdate
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    // Data has been received and we've called PostDataUpdate on all data recipients
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    // We've received all packets, we can now do interpolation, prediction, etc..
    FRAME_NET_UPDATE_END,

    // We're about to start rendering the scene
    FRAME_RENDER_START,
    // We've finished rendering the scene.
    FRAME_RENDER_END
};
CPlayerResource* GetPlayerResourcePointer()
{
    static auto uAddress = M::PatternScan(G::bIsPanoramaDll ? "client_panorama.dll" : "client.dll", "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7") + 2;
    return **reinterpret_cast<CPlayerResource***>(uAddress);
}
using FrameStageFn = void(__stdcall*)(ClientFrameStage stage);
FrameStageFn oFrameStage = nullptr;
void __stdcall FrameStage(ClientFrameStage stage) {
    switch (stage) {
        case FRAME_NET_UPDATE_POSTDATAUPDATE_START: {
            // skins
            CEntity* local = G::g_EntityList->GetEntityFromIndex(G::g_EngineClient->GetLocalPlayerIndex());
            if (!local || local->m_lifeState() != 0 ) {
                return;
            }

            auto weapons = local->m_hMyWeapons();
            for (int i = 0; weapons[i]; i++) {

                CBaseAttributableItem* weapon = (CBaseAttributableItem*)G::g_EntityList->GetClientEntityFromHandle(weapons[i]);
                if (!weapon) break;

                int idx = weapon->m_iItemDefinitionIndex();

                auto skin = CInventory::GetItem(local->m_iTeamNum(), CInventory::GetSlotID(idx), idx);
                switch (idx) {
                case WEAPON_KNIFE_T: {
                    weapon->m_iItemDefinitionIndex() = CInventory::GetKnifeEquipped(2);
                    weapon->m_nModelIndex() = G::g_modelinfo->GetModelIndex(CInventory::FindKnifeModel((ItemDefinitionIndex)weapon->m_iItemDefinitionIndex()));
                    skin = CInventory::GetItem(2, 0, weapon->m_iItemDefinitionIndex());
                    break;
                }
                case WEAPON_KNIFE: {
                    weapon->m_iItemDefinitionIndex() = CInventory::GetKnifeEquipped(3);
                    weapon->m_nModelIndex() = G::g_modelinfo->GetModelIndex(CInventory::FindKnifeModel((ItemDefinitionIndex)weapon->m_iItemDefinitionIndex()));
                    skin = CInventory::GetItem(3, 0, weapon->m_iItemDefinitionIndex());

                    break;
                }
                default: {

                    break;
                }
                }
                weapon->m_nFallbackPaintKit() = (int)skin.flPaintKit;
                weapon->m_iEntityQuality() = (int)skin.iQuality;
                weapon->m_flFallbackWear() = skin.flWear;
                weapon->m_nFallbackSeed() = skin.iPattern;
                if (skin.bHasStattrack) {
                    weapon->m_nFallbackStatTrak() = skin.flStattrack;
                    weapon->m_iEntityQuality() = 2;
                    if(skin.iRarity == 6) weapon->m_iEntityQuality() = 3;
                }
                weapon->m_iAccountID() = G::g_SteamUser->GetSteamID().GetAccountID();
                weapon->m_iItemIDHigh() = -1;

            }
            CPlayerResource* g_player_resource = GetPlayerResourcePointer();

            // V::netvars[hash::CompileTime(var)]
            auto offset = V::netvars[hash::CompileTime("CCSPlayerResource->m_nMusicID")];
            int playerIndex = G::g_EngineClient->GetLocalPlayerIndex();

            int* musicID = reinterpret_cast<int*>(
                reinterpret_cast<uintptr_t>(g_player_resource) + offset + playerIndex * 4
                );

            *musicID = CInventory::GetCurrentMusicKit();
        }
    }
    oFrameStage(stage);
}

void ViewModelIndexProxy(const CRecvProxyData* data, void* struc, void* Out) {

    auto dat = const_cast<CRecvProxyData*>(data);


    auto iCustomCtKnife = G::g_modelinfo->GetModelIndex(CInventory::FindKnifeModel(CInventory::GetKnifeEquipped(3)));
    auto iCustomTKnife = G::g_modelinfo->GetModelIndex(CInventory::FindKnifeModel(CInventory::GetKnifeEquipped(2)));
    auto iOrginalCtKnife = G::g_modelinfo->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
    auto iOrginalTKnife = G::g_modelinfo->GetModelIndex("models/weapons/v_knife_default_t.mdl");

    if (dat->m_Value.m_Int == iOrginalCtKnife) {
        dat->m_Value.m_Int = iCustomCtKnife;
    }
    if (dat->m_Value.m_Int == iOrginalTKnife) {
        dat->m_Value.m_Int = iCustomTKnife;
    }



    V::oViewModelProxy(data, struc, Out);
}
void Dump(const char* base, RecvTable* table, const std::uint32_t offset) noexcept
{
    // loop through props
    for (auto i = 0; i < table->m_nProps; ++i) {
        RecvProp* prop = &table->m_pProps[i];

        if (!prop)
            continue;

        if (std::isdigit(prop->m_pVarName[0]))
            continue;

        if ((prop->m_pVarName) == ("baseclass"))
            continue;

        if (prop->m_pDataTable &&
            prop->m_pDataTable->m_pNetTableName[0] == 'D')
            Dump(base, prop->m_pDataTable, offset + prop->m_Offset);

        V::netvars[hash::RunTime(std::format("{}->{}", base, prop->m_pVarName).c_str())] = offset + prop->m_Offset;
        

        if (strcmp(prop->m_pVarName, "m_nModelIndex") || strcmp(base, "CBaseViewModel"))
            continue;

        V::oViewModelProxy = prop->m_ProxyFn;
        prop->m_ProxyFn = (RecvVarProxyFn)ViewModelIndexProxy;
    }
}
void Setup() noexcept
{
    auto vtables = *(void***)(G::g_VClient);
    using GetAllClassesFn = ClientClass * (__thiscall*)(void*);
    GetAllClassesFn oGetAllClasses = (GetAllClassesFn)vtables[8];

    for (ClientClass* client = oGetAllClasses(G::g_VClient); client; client = client->m_pNext)
        if (RecvTable* table = client->m_pRecvTable)
            Dump(client->m_pNetworkName, table, 0);
}

int RunLoop() {
    console::init();

    auto steamModule = (uintptr_t)GetModuleHandleA("steam_api.dll");
    while (!steamModule) {
        steamModule = (uintptr_t)GetModuleHandleA("steam_api.dll");
    }
    while(!(uintptr_t)GetModuleHandleA("serverbrowser.dll")) {}
    auto hSteamUser = ((HSteamUser(__cdecl*)(void))GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamAPI_GetHSteamUser"))();
    auto hSteamPipe = ((HSteamPipe(__cdecl*)(void))GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamAPI_GetHSteamPipe"))();
    G::g_SteamClient = ((ISteamClient * (__cdecl*)(void))GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamClient"))();
    if (!G::g_SteamClient) {
        MessageBoxA(NULL, "Failed to get g_SteamClient\nInjected too quickly?", "Init Error", 1);
        return -1;
    }
    G::g_SteamHTTP = G::g_SteamClient->GetISteamHTTP(hSteamUser, hSteamPipe, "STEAMHTTP_INTERFACE_VERSION002");
    if (!G::g_SteamHTTP) {
        MessageBoxA(NULL, "Failed to get g_SteamHTTP\nInjected too quickly?", "Init Error", 1);
        return -1;
    }
    G::g_SteamUser = G::g_SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
    if (!G::g_SteamUser) {
        MessageBoxA(NULL, "Failed to get g_SteamUser\nInjected too quickly?", "Init Error", 1);
        return -1;
    }
    G::g_SteamFriends = G::g_SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
    if (!G::g_SteamFriends) {
        MessageBoxA(NULL, "Failed to get g_SteamFriends\nInjected too quickly?", "Init Error", 1);
        return -1;
    }
    G::g_SteamInventory = G::g_SteamClient->GetISteamInventory(hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002");
    if (!G::g_SteamInventory) {
        MessageBoxA(NULL, "Failed to get g_SteamInventory\nInjected too quickly?", "Init Error", 1);
        return -1;
    }
    G::g_GameCoordinator = (ISteamGameCoordinator*)G::g_SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamGameCoordinator001");
    if (!G::g_GameCoordinator) {
        MessageBoxA(NULL, "Failed to get g_GameCoordinator\nInjected too quickly?", "Init Error", 1);
        return -1;
    }
    G::g_SteamMatchMaking = G::g_SteamClient->GetISteamMatchmaking(hSteamUser, hSteamPipe, STEAMMATCHMAKING_INTERFACE_VERSION);
    if (!G::g_SteamMatchMaking) {
        MessageBoxA(NULL, "Failed to get g_SteamMatchMaking\nInjected too quickly?", "Init Error", 1);
        return -1;
    }


    auto offset = M::PatternScan("engine.dll", "68 ? ? ? ? FF D7 83 C4 ? FF 15 ? ? ? ? 8B F0");

    if (!offset) {
        offset = M::PatternScan("engine.dll", "68 ? ? ? ? FF D6 83 C4 ? 5E C3 CC CC CC 80 3D");
        if (!offset)
        {
            //MessageBoxA(0,"Failed to get version string", "CSGO_LOCAL", 1);
            G::versionString = "";
        }
    }
    if (offset) {
        G::versionString = *(const char**)(offset + 1);
    }
    {
        // shit code lol
        if (strstr(G::versionString, "2019")) {
            G::gameVer = 2019;
        }
        else if (strstr(G::versionString, "2020")) {
            G::gameVer = 2020;
        }
        else if (strstr(G::versionString, "2021")) {
            G::gameVer = 2021;
        }
        else if (strstr(G::versionString, "2022")) {
            G::gameVer = 2022;
        }
        else if (strstr(G::versionString, "2023")) {
            G::gameVer = 2023;
        }
        else if (strstr(G::versionString, "2018")) {
            G::gameVer = 2018;
        }
        else if (strstr(G::versionString, "2017")) {
            G::gameVer = 2017;
        }
        else if (strstr(G::versionString, "2016")) {
            G::gameVer = 2016;
        }
        else {
            // not expected
            G::gameVer = 0;
            MessageBoxA(NULL, "Your Version is unsupported\r\nIt might work but isn't officially supported", "CSGO-LOCAL", 0);
        }
    }
    if (GetModuleHandleA("client.dll") != nullptr)
        G::bIsPanoramaDll = false;
    else if (GetModuleHandleA("client_panorama.dll"))
        G::bIsPanoramaDll = true;
    else
        console::log("error occurred in initialization, SEE dllmain.cpp");
    MH_Initialize();

    auto vt = *(void***)G::g_GameCoordinator;

    auto res = MH_CreateHook(
        vt[0],
        &CNetworking::hkSendMsg,
        reinterpret_cast<void**>(&CNetworking::oSendMsg)
    );
    auto res3 = MH_CreateHook(
        vt[1],
        &CNetworking::hkIsMessageAvailable,
        reinterpret_cast<void**>(&CNetworking::oIsMessageAvailable)
    );
    auto res2 = MH_CreateHook(
        vt[2],
        &CNetworking::hkRetrieveMessage,
        reinterpret_cast<void**>(&CNetworking::oRetrieveMessage)
    );



    void* g_pGameEventManager = nullptr;

    using CreateInterfaceFn = void* (*)(const char* name, int* returnCode);
    CreateInterfaceFn EngineFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
    CreateInterfaceFn ClientFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA(G::bIsPanoramaDll ? "client_panorama.dll" : "client.dll"), "CreateInterface");
    while (!ClientFactory) {
        ClientFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client_panorama.dll"), "CreateInterface");

    }
    G::g_EventManager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", nullptr);
    while (!G::g_EventManager) {

        G::g_EventManager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", nullptr);
    }

    CCaseOpening::CreateCrates();
    V::LoadConfig();

    

    E::g_EventListener = new EventListener;

    G::g_EventManager->AddListener(E::g_EventListener, "cs_win_panel_match", false);
    G::g_EventManager->AddListener(E::g_EventListener, "cs_win_panel_round", false);
    G::g_EventManager->AddListener(E::g_EventListener, "cs_game_disconnected", false);
    G::g_EventManager->AddListener(E::g_EventListener, "player_death", false);
    G::g_EventManager->AddListener(E::g_EventListener, "round_mvp", false);

    G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient014", nullptr);
    while (!G::g_EngineClient) {
        G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient014", nullptr);
        if(!G::g_EngineClient)
            G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient013", nullptr);
    }
    G::g_VClient = ClientFactory("VClient018", nullptr);

    while (!G::g_VClient) {
        G::g_VClient = ClientFactory("VClient018", nullptr);
        if(!G::g_VClient) G::g_VClient = ClientFactory("VClient017", nullptr);
    }
    G::g_GlobalVars = **reinterpret_cast<IGlobalVars***>((*reinterpret_cast<uintptr_t**>(G::g_VClient))[11] + 10);

    G::g_EntityList = (IClientEntityList*)ClientFactory("VClientEntityList003", nullptr);
    G::g_modelinfo = (IVModelInfoClient*)EngineFactory("VModelInfoClient004", nullptr);
    G::g_MemAlloc = *(IMemAlloc**)(GetProcAddress(GetModuleHandleA("tier0.dll"), "g_pMemAlloc"));

    MH_CreateHook((*(void***)(G::g_VClient))[(G::gameVer > 2018 || (G::gameVer == 2018 && G::bIsPanoramaDll)) ? 37 : 36], &FrameStage, reinterpret_cast<void**>(&oFrameStage));


    MH_EnableHook(MH_ALL_HOOKS);


    V::STEAM_ID = G::g_SteamUser->GetSteamID().GetAccountID();


    console::log(std::format("Welcome back, {}", V::STEAM_ID).c_str());
    console::log(std::format("Game Version: {}", G::gameVer).c_str());

   

    
    Setup(); // setup netvars / TODO: MOVE TO OTHER FILE
    bool bSaveRequired = false;
    for (auto medal : V::othermedals) {
        CItem item;

        item.bHasStattrack = false;
        item.flPaintKit = 0;
        item.flStattrack = 0;
        item.flWear = 0;
        item.iDefIdx = medal;
        item.iItemId = rand() % 10000;
        item.iPattern = 0;
        item.iFlag = 0;
        item.iQuality = 4;
        item.iRarity = 6;
        V::items.push_back(item);
        V::othermedals[medal] = NULL;
        bSaveRequired = true;
    }
    for (auto it = V::items.begin(); it != V::items.end();) {
        if (it->iDefIdx == 0) {
            it = V::items.erase(it);
            bSaveRequired = true;
        }
        else {
            ++it;
        }
    }
    if (bSaveRequired) V::SaveConfig();



    V::PENDING_UPDATE = true;




    while (true) {
        if (V::PENDING_UPDATE) {
            CNetworking::SendClientHello();
            V::PENDING_UPDATE = false;

            
        }

        CMatchmaking::Refresh(G::g_GlobalVars->currentTime, G::bIsPanoramaDll, G::g_VClient, G::gameVer);
        CNetworking::SyncGC();
        Sleep(1);

    }
}
extern "C" __declspec(dllexport) int ExportedMain() {

    CreateThread(nullptr, 0, [](LPVOID) -> DWORD {G::bignoreWelcome = true; return RunLoop(); }, nullptr, 0, nullptr);
    return 0;
}
extern "C" __declspec(dllexport) void InstallGC(bool a1) {
    if (a1) return; // ignore dedicated
    CreateThread(nullptr, 0, [](LPVOID) -> DWORD {G::bignoreWelcome = true; return RunLoop(); }, nullptr, 0, nullptr);

}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        CreateThread(nullptr, 0, [](LPVOID) -> DWORD {return RunLoop();}, nullptr, 0, nullptr);
    }

    return TRUE;
}
