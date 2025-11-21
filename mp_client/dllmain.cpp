

#include <windows.h>
#include "console/console.h"
#include <vector>
#include <string>
#include <iostream>

#include "SDK/Proto/ProtoParse.h"
#include "SDK/Proto/Messages.h"

#include "sdk/eventlistener.h"
#include "sdk/igameevent.h"
#include "globals.h"
#include "Minhook/MinHook.h"

#include <format>

#include "memory.h"
#include "sdk/networking.h"
#include "sdk/recv.h"
#include "vars.h"
#include "Logic/CCaseOpening.h"
#include "SDK/http.h"
#include "SDK/steamsdk/steam_api.h"


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

            
            for (int i = 0; i < 64; i++) {
                CEntity* player = G::g_EntityList->GetEntityFromIndex(i);
                if (!player || player->m_lifeState() != 0 || player == local) {
                    continue;
                }
                PlayerInfo plrinfo{};
                if (!G::g_EngineClient->GetPlayerInfo(i, &plrinfo)) continue;
				if (plrinfo.fakeplayer) continue;
				if (plrinfo.iSteamID < 1) continue;
                CInventory::CRemoteInventory inv;
                bool foundInventory = false;
                for (int i = 0; i < CInventory::remoteInventories.size(); i++) {
					if (CInventory::remoteInventories[i].steamID == plrinfo.iSteamID) {
                        inv = CInventory::remoteInventories[i]; foundInventory = true;
						break;
					}
                }
                if (!foundInventory) {
					inv = http::getRemoteInventory(plrinfo.iSteamID);
                    inv.steamID = plrinfo.iSteamID;

					CInventory::remoteInventories.push_back(inv);

                    CMsgGCCStrike15_v2_ClientRequestPlayersProfile msg;
                    msg.account_id().set(plrinfo.iSteamID);
                    auto packet = msg.serialize();

                    void* ptr = malloc(packet.size() + 8);

                    if (!ptr)
                        break;

                    ((uint32_t*)ptr)[0] = 9127 | ((DWORD)1 << 31);
                    ((uint32_t*)ptr)[1] = 0;

                    memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
                    bool result = G::g_GameCoordinator->SendMsg(9127 | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
                    free(ptr);
                }
                auto weapons = player->m_hMyWeapons();
                for (int i = 0; weapons[i]; i++) {

                    CBaseAttributableItem* weapon = (CBaseAttributableItem*)G::g_EntityList->GetClientEntityFromHandle(weapons[i]);
                    if (!weapon) continue;

                    int idx = weapon->m_iItemDefinitionIndex();

                    auto skin = inv.getEquip(CInventory::GetSlotID(idx), player->m_iTeamNum());
					if (skin.iDefIdx == -1)
						continue;
                    switch (idx) {
                    case WEAPON_KNIFE_T: {
						weapon->m_iItemDefinitionIndex() = inv.getEquip(0, 2).iDefIdx;
                        weapon->m_nModelIndex() = G::g_modelinfo->GetModelIndex(CInventory::FindKnifeModel((ItemDefinitionIndex)weapon->m_iItemDefinitionIndex()));
                        skin = inv.getEquip(0, 2);
                        continue;
                    }
                    case WEAPON_KNIFE: {
                        weapon->m_iItemDefinitionIndex() = inv.getEquip(0, 3).iDefIdx;
                        weapon->m_nModelIndex() = G::g_modelinfo->GetModelIndex(CInventory::FindKnifeModel((ItemDefinitionIndex)weapon->m_iItemDefinitionIndex()));
                        skin = inv.getEquip(0, 3);
                        continue;
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
                        if (skin.iRarity == 6) weapon->m_iEntityQuality() = 3;
                    }
                    weapon->m_iAccountID() = 
                    weapon->m_iItemIDHigh() = -1;

                }
            }


            if (!local || local->m_lifeState() != 0) {
                return;
            }

            PlayerInfo plrinfo{};
            if (!G::g_EngineClient->GetPlayerInfo(G::g_EngineClient->GetLocalPlayerIndex(), &plrinfo)) return;

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
                    if (skin.iRarity == 6) weapon->m_iEntityQuality() = 3;
                }
                weapon->m_iAccountID() = G::g_SteamUser->GetSteamID().GetAccountID();
                weapon->m_iItemIDHigh() = -1;

            }

            CPlayerResource* g_player_resource = GetPlayerResourcePointer();

            // V::netvars[hash::CompileTime(var)]
            for (int i = 0; i <= 64; i++) {
                auto m_nMusicID = V::netvars[hash::CompileTime("CCSPlayerResource->m_nMusicID")];
                auto m_nActiveCoinRank = V::netvars[hash::CompileTime("CCSPlayerResource->m_nActiveCoinRank")];
                auto m_nPersonaDataPublicLevel = V::netvars[hash::CompileTime("CCSPlayerResource->m_nPersonaDataPublicLevel")];
                auto m_iCompetitiveRanking = V::netvars[hash::CompileTime("CCSPlayerResource->m_iCompetitiveRanking")];
                int localplayerIndex = G::g_EngineClient->GetLocalPlayerIndex();
                int playerIndex = i;


                PlayerInfo plrinfo;
				if (!G::g_EngineClient->GetPlayerInfo(playerIndex, &plrinfo)) continue;
				if (plrinfo.fakeplayer) continue;


                if (m_nMusicID > 0) {
                    int* musicID = reinterpret_cast<int*>(
                        reinterpret_cast<uintptr_t>(g_player_resource) + m_nMusicID + playerIndex * 4
                        );

                    if (musicID != nullptr)
                        *musicID = playerIndex == localplayerIndex ? CInventory::GetCurrentMusicKit() : CInventory::GetRemoteInventory(plrinfo.iSteamID).getEquip(54, 0).iDefIdx;
                }

                if (m_nActiveCoinRank > 0) {
                    int* coinID = reinterpret_cast<int*>(
                        reinterpret_cast<uintptr_t>(g_player_resource) + m_nActiveCoinRank + playerIndex * 4
                        );
                    if (coinID != nullptr)
                        *coinID = playerIndex == localplayerIndex ? CInventory::GetCurrentMedal() : CInventory::GetRemoteInventory(plrinfo.iSteamID).getEquip(55, 0).iDefIdx;
                }

                if (m_nPersonaDataPublicLevel > 0) {
                    int* lvlID = reinterpret_cast<int*>(
                        reinterpret_cast<uintptr_t>(g_player_resource) + m_nPersonaDataPublicLevel + playerIndex * 4
                        );
                    if (lvlID != nullptr)
                        *lvlID = playerIndex == localplayerIndex ? V::iLevel : CInventory::GetRemoteInventory(plrinfo.iSteamID).lvl;
                
                }
                // TODO: Move somewhere else
                static auto GetGameMode = M::PatternScan(G::bIsPanoramaDll ? "client_panorama.dll" : "client.dll", "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? CC CC E8");
                if (!GetGameMode) GetGameMode = M::PatternScan(G::bIsPanoramaDll ? "client_panorama.dll" : "client.dll", "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? F3 0F 10 05 ? ? ? ? 0F 2E 05 ? ? ? ? 8B 0D ? ? ? ? 9F F6 C4 ? 7A ? 39 0D ? ? ? ? 75 ? A1 ? ? ? ? 33 05 ? ? ? ? A9 ? ? ? ? 74 ? 8B 15 ? ? ? ? 85 D2 74 ? 8B 02 8B CA 68 ? ? ? ? FF 90 ? ? ? ? 8B 0D ? ? ? ? 81 F1 ? ? ? ? 8B C1 C3 8B 01 FF 60 ? E8");
                static auto GetGameType = M::PatternScan(G::bIsPanoramaDll ? "client_panorama.dll" : "client.dll", "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? CC CC 8B");
                if (!GetGameType) GetGameType = M::PatternScan(G::bIsPanoramaDll ? "client_panorama.dll" : "client.dll", "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? F3 0F 10 05 ? ? ? ? 0F 2E 05 ? ? ? ? 8B 0D ? ? ? ? 9F F6 C4 ? 7A ? 39 0D ? ? ? ? 75 ? A1 ? ? ? ? 33 05 ? ? ? ? A9 ? ? ? ? 74 ? 8B 15 ? ? ? ? 85 D2 74 ? 8B 02 8B CA 68 ? ? ? ? FF 90 ? ? ? ? 8B 0D ? ? ? ? 81 F1 ? ? ? ? 8B C1 C3 8B 01 FF 60 ? 8B 0D ? ? ? ? 81 F9");
                
                if (m_iCompetitiveRanking > 0 && GetGameType && GetGameMode) {
                    int* rankID = reinterpret_cast<int*>(
                        reinterpret_cast<uintptr_t>(g_player_resource) + m_iCompetitiveRanking + playerIndex * 4
                        );

                    bool bIsMM = false;
                    bool bIsRanked = false;

                    if (!reinterpret_cast<int(__thiscall*)(void*)>(GetGameType)(GetGameType)) bIsRanked = true;
                    if (reinterpret_cast<int(__thiscall*)(void*)>(GetGameMode)(GetGameMode) == 1) bIsMM = true;

                    if (rankID != nullptr && bIsRanked)
                        *rankID = playerIndex == localplayerIndex ? (bIsMM ? V::Ranks::Competetive::iCurrentRank : V::Ranks::Wingman::iCurrentRank) : (bIsMM ? CInventory::GetRemoteInventory(plrinfo.iSteamID).mmrank : CInventory::GetRemoteInventory(plrinfo.iSteamID).wmrank);

                }
            }
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
    MH_Initialize();
    
    while(!(uintptr_t)GetModuleHandleA("serverbrowser.dll")) {}

    auto hSteamUser = ((HSteamUser(__cdecl*)(void))GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamAPI_GetHSteamUser"))();
    auto hSteamPipe = ((HSteamPipe(__cdecl*)(void))GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamAPI_GetHSteamPipe"))();
    G::g_SteamClient = ((ISteamClient * (__cdecl*)(void))GetProcAddress(GetModuleHandle(L"steam_api.dll"), "SteamClient"))();
    G::g_SteamHTTP = G::g_SteamClient->GetISteamHTTP(hSteamUser, hSteamPipe, "STEAMHTTP_INTERFACE_VERSION002");
    G::g_SteamUser = G::g_SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
    G::g_SteamFriends = G::g_SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
    G::g_GameCoordinator = (ISteamGameCoordinator*)G::g_SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamGameCoordinator001");
    

    auto offset = M::PatternScan("engine.dll", "68 ? ? ? ? FF D7 83 C4 ? FF 15 ? ? ? ? 8B F0");

    if (!offset) {
        offset = M::PatternScan("engine.dll", "68 ? ? ? ? FF D6 83 C4 ? 5E C3 CC CC CC 80 3D");
        if (!offset)
        {
            G::versionString = "";
        }
        if (!offset) offset = M::PatternScan("engine.dll", "68 ? ? ? ? FF 15 ? ? ? ? 83 C4 ? C3 CC 80 3D ? ? ? ? ? 75");
        if (!offset)
        {
            G::versionString = "";
        }
    }
    if (offset) {
        G::versionString = *(const char**)(offset + 1);
    }
    {
        char* ver = _strdup(G::versionString);
        auto dat = strtok(ver, " ");
        int splitter = 0;
        std::string months[13] = { "None", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        while (dat != NULL) {
            splitter++;
            dat = strtok(0, " ");
            if (!dat) break;
            if (splitter == 3) {
                auto str = std::string(dat);
                // parse month
                for (int i = 1; i < 13; i++) {
                    if (str == months[i]) {
                        G::buildMonth = i;
                    }
                }
            }
            else if (splitter == 4) {
                G::buildDay = std::stoi(dat);
            } 
            else if (splitter == 5) {
                G::gameVer = std::stoi(dat);
            } 

        }
        /*else {
            // not expected
            G::gameVer = 0;
            
            MessageBoxA(NULL, "Your Version is unsupported\r\nIt might work but isn't officially supported", "CSGO-LOCAL", 0);
            // version is unsupported, but still parse it

            if (strstr(G::versionString, "2015")) G::gameVer = 2015;
            if (strstr(G::versionString, "2014")) G::gameVer = 2014;
            if (strstr(G::versionString, "2013")) G::gameVer = 2013;
        }*/
    }
    console::init();
    if (GetModuleHandleA("client.dll") != nullptr)
        G::bIsPanoramaDll = false;
    else if (GetModuleHandleA("client_panorama.dll"))
        G::bIsPanoramaDll = true;
    else
        console::log("error occurred in initialization, SEE dllmain.cpp");

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
    G::g_EventManager->AddListener(E::g_EventListener, "round_end", false);

    G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient014", nullptr);
    while (!G::g_EngineClient) {
        G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient014", nullptr);
        if(!G::g_EngineClient)
            G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient013", nullptr);
        if(!G::g_EngineClient)
            G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient012", nullptr);
    }
    G::g_VClient = ClientFactory("VClient018", nullptr);

    while (!G::g_VClient) {
        G::g_VClient = ClientFactory("VClient018", nullptr);
        if(!G::g_VClient) G::g_VClient = ClientFactory("VClient017", nullptr);
        if(!G::g_VClient) G::g_VClient = ClientFactory("VClient016", nullptr);
        if (!G::g_VClient) G::g_VClient = ClientFactory("VClient015", nullptr);
    }
    G::g_GlobalVars = **reinterpret_cast<IGlobalVars***>((*reinterpret_cast<uintptr_t**>(G::g_VClient))[11] + 10);

    G::g_EntityList = (IClientEntityList*)ClientFactory("VClientEntityList003", nullptr);
    G::g_modelinfo = (IVModelInfoClient*)EngineFactory("VModelInfoClient004", nullptr);

    MH_CreateHook((*(void***)(G::g_VClient))[(G::gameVer > 2018 || (G::gameVer == 2018 && G::bIsPanoramaDll)) ? 37 : 36], &FrameStage, reinterpret_cast<void**>(&oFrameStage));


    MH_EnableHook(MH_ALL_HOOKS);


    V::STEAM_ID = G::g_SteamUser->GetSteamID().GetAccountID();


    console::log(std::format("Welcome back, {}", V::STEAM_ID).c_str());
    console::log(std::format("Game Version: {}.{}.{}", G::buildMonth, G::buildDay, G::gameVer).c_str());

   

    
    Setup(); // setup netvars / TODO: MOVE TO OTHER FILE
    {
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
        if (bSaveRequired) 
            V::SaveConfig();
    }

    V::MainInit = true;
    V::PENDING_UPDATE = true;



   

    if (V::pendingEquipSlots.size() > 0) {
		for (auto& equip : V::pendingEquipSlots) {
			CInventory::EquipSlot(equip.item.iItemId, equip.teamId, equip.slotId);
		}
		V::pendingEquipSlots.clear();
    }
    http::SendUserProfileToServer();

    while (true) {
        if (V::PENDING_UPDATE) {
            CNetworking::SendClientHello();
            V::PENDING_UPDATE = false;
            
        }
        CMatchmaking::Refresh(G::g_GlobalVars->currentTime, G::bIsPanoramaDll, G::g_VClient, G::gameVer);
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