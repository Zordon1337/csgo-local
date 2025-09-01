

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);

        CreateThread(nullptr, 0, [](LPVOID) -> DWORD
            {

                console::init();

                auto steamModule = (uintptr_t)GetModuleHandleA("steam_api.dll");
                if (!steamModule) {
                    MessageBoxA(NULL, "Failed to get steamModule\nInjected too quickly?", "Init Error", 1);
                    return -1;
                }
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
                MH_CreateHook(M::PatternScan("client_panorama.dll", "55 8B EC 83 E4 ? 83 EC ? 53 8B D9 56 57 8B 4B ? 85 C9"),
                    &CNetworking::hkCheckForMessages,
                    reinterpret_cast<void**>(&CNetworking::oCheckForMessages));

                void* g_pGameEventManager = nullptr;

                using CreateInterfaceFn = void* (*)(const char* name, int* returnCode);
                CreateInterfaceFn EngineFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface");
                CreateInterfaceFn ClientFactory = (CreateInterfaceFn)GetProcAddress(GetModuleHandleA("client_panorama.dll"), "CreateInterface");

                G::g_EventManager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", nullptr);
                while (!G::g_EventManager) {

                    G::g_EventManager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", nullptr);
                }

                V::LoadConfig();

                E::g_EventListener = new EventListener;

                G::g_EventManager->AddListener(E::g_EventListener, "cs_win_panel_match", false);
                G::g_EventManager->AddListener(E::g_EventListener, "cs_win_panel_round", false);
                G::g_EventManager->AddListener(E::g_EventListener, "cs_game_disconnected", false);
                G::g_EventManager->AddListener(E::g_EventListener, "player_death", false);

                G::g_EngineClient = (IVEngineClient*)EngineFactory("VEngineClient014", nullptr);
                void* client = ClientFactory("VClient018", nullptr);
                G::g_GlobalVars = **reinterpret_cast<IGlobalVars***>((*reinterpret_cast<uintptr_t**>(client))[11] + 10);

                MH_EnableHook(MH_ALL_HOOKS);


                V::PENDING_UPDATE = true;

                V::STEAM_ID = G::g_SteamUser->GetSteamID().GetAccountID();

                G::versionString = *(const char**)(M::PatternScan("engine.dll", "68 ? ? ? ? FF D7 83 C4 ? FF 15 ? ? ? ? 8B F0") + 1);
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
                    else {
                        // not expected
                        G::gameVer = 0;
                        MessageBoxA(NULL, "Your Version is unsupported\r\n It might work but isn't officially supported", "CSGO-LOCAL", 0);
                    }
                }

                console::log(std::format("Welcome back, {}", V::STEAM_ID).c_str());
                console::log(std::format("Game Version: {}", G::versionString).c_str()); // draws \n but fuck it

                while (true) {
                    if (V::PENDING_UPDATE) {
                        CNetworking::SendClientHello();
                        V::PENDING_UPDATE = false;
                    }

                    CMatchmaking::Refresh(G::g_GlobalVars->currentTime);
                    CNetworking::SyncGC();
                    Sleep(50);
                    
                }

            }, nullptr, 0, nullptr);
    }

    return TRUE;
}
