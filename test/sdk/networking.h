#pragma once
#include "../protos/ProtoParse.h"
#include "../protos/Messages.h"
#include <queue>
#include <mutex>
#include <string>
#include "globals.h"

class CNetworking {
private:
    static inline void* _ecx;
    static inline int _a2;

    struct GCMessage {
        std::string data;
        int packetId;
        float delay;
    };

    static inline std::deque<GCMessage> msgQueue;
    static inline std::recursive_mutex queueMutex;


public:


    static bool SendClientHello()
    {
        CMsgClientHello msg;
        msg.client_session_need().set(1);
        auto packet = msg.serialize();

        void* ptr = malloc(packet.size() + 8);

        if (!ptr)
            return false;

        ((uint32_t*)ptr)[0] = k_EMsgGCClientHello | ((DWORD)1 << 31);
        ((uint32_t*)ptr)[1] = 0;

        memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
        bool result = G::g_GameCoordinator->SendMsg(k_EMsgGCClientHello | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
        free(ptr);

        return result;
    }

    using CheckForMessagesFn = void(__thiscall*)(void* ecx, int a2); // a2 exists but unused
    static inline CheckForMessagesFn oCheckForMessages;
    static void __fastcall hkCheckForMessages(void* ecx, void* edx, int a2) {
        _ecx = ecx;
        _a2 = a2;
        oCheckForMessages(ecx, a2);

    }

    // holy shit, this is terrible but im not going to spend any more time finding better solution
    static void SyncGC() {
        if (oCheckForMessages &&_ecx  && _a2) {
            oCheckForMessages(_ecx, _a2);
        }
    }

    using SendMsgGC = EGCResults(__fastcall*)(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData);
    static inline SendMsgGC oSendMsg;

    using RetrieveMessageFn = EGCResults(__fastcall*)(void* ecx, void* edx, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
    static inline RetrieveMessageFn oRetrieveMessage;

    using IsMessageAvailableFn = bool(__fastcall*)(void* ecx, void* edx, uint32* pcubMsgSize);
    static inline IsMessageAvailableFn oIsMessageAvailable;

    static inline CMsgClientWelcome::CMsgSOCacheSubscribed BuildSoCache() {
        CMsgClientWelcome::CMsgSOCacheSubscribed msg;
        msg.version().set(7523377975160828514); // https://github.com/mikkokko/csgo_gc/blob/b2fe06da7eeff3d01833464a22c2d2db47a6fce0/csgo_gc/inventory.cpp#L12C39-L12C58
        {
            CMsgSOIDOwner soid;
            soid.id().set(G::g_SteamUser->GetSteamID().GetAccountID());
            soid.owner_type().set(1);
            msg.owner_soid().add(soid);

            CMsgClientWelcome::SubscribedType st;
            for (int i = 0; i < V::othermedals.size(); i++) {
                CSOEconItem item2;
                item2.id().set(MEDAL_BASE_ID + V::othermedals[i]);
                item2.account_id().set(G::g_SteamUser->GetSteamID().GetAccountID());
                item2.def_index().set(V::othermedals[i]);
                item2.inventory().set(MEDAL_BASE_ID + V::othermedals[i]);
                item2.origin().set(9);
                item2.quantity().set(4);
                item2.level().set(0);
                item2.flags().set(0);
                item2.in_use().set(false);
                item2.rarity().set(6);
                item2.quality().set(4);

                st.object_data().add(item2.serialize());
            }

        }
        {

            CMsgClientWelcome::SubscribedType st;
            st.type_id().set(1);
            msg.objects().add(st);
        }
        {
            CSOPersonaDataPublic personaData;
            personaData.elevated_state().set(true);
            personaData.player_level().set(V::iLevel);


            CMsgClientWelcome::SubscribedType st;
            st.type_id().set(2);
            st.object_data().set(personaData.serialize());
        }
        return msg;
    }

    static inline EGCResults __fastcall hkSendMsg(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData) {

        uint32_t messageType = unMsgType & 0x7FFFFFFF;
        switch (messageType) {

            case 9172: {
                // TODO: I NEED TO GET ACC WITH LEVEL 40 SINCE IDK PROPER RESPONSE THAT DOESN'T CRASH GAME
                /*CMsgGCCStrike15_v2_Client2GCRequestPrestigeCoin response((void*)((DWORD)pubData + 8), cubData - 8);

                response.defindex().set(1376);
                response.hours().set(2);
                response.prestigetime().set(1755626035);
                response.upgradeid().set(1);

                auto packet = response.serialize();

                CNetworking::QueueMessage(9172, packet, 1500);*/

                int MEDAL_ID = 1375;

                switch (G::gameVer) {
                    case 2016: MEDAL_ID = 1338; break;
                    case 2017: MEDAL_ID = 1356; break;
                    case 2018: MEDAL_ID = 1366; break;
                    case 2019: MEDAL_ID = 1375; break;
                    case 2020: MEDAL_ID = 4673; break;
                    case 2021: MEDAL_ID = 4736; break;
                    case 2022: MEDAL_ID = 4818; break;
                    case 2023: MEDAL_ID = 4872; break;
                    default: MEDAL_ID = 1331; break; // 2015 medal for unsupported builds
                }

                if (G::gameVer > 2015 && G::gameVer < 2024)
                {
					bool bHasMaxMedal = false;
					for (int i = 0; i < V::othermedals.size(); i++) {
						if (V::othermedals[i] == MEDAL_ID + 6) {
							bHasMaxMedal = true;
							break;
						}
					}
                    int iMedalLevel = 0;
                    for (int i = 0; i < V::othermedals.size(); i++) {
						if (V::othermedals[i] == MEDAL_ID + i) {
                            iMedalLevel = i;
							break;
						}
                    }
                    if (iMedalLevel > 0)
                    {
                        // replace old medal
						for (int i = 0; i < V::othermedals.size(); i++) {
							if (V::othermedals[i] == MEDAL_ID + iMedalLevel) {
								V::othermedals[i] = MEDAL_ID + iMedalLevel + 1;
								break;
							}
						}
					}
					else {
						V::othermedals.push_back(MEDAL_ID + 1);
					}
                }
                V::iLevel = 1;
                V::iXP = 0;
                V::iServiceMedalLevel++;

                SendClientHello();

                V::SaveConfig();
                {

                }
                break;
            }
            case 4006: {
                CMsgClientWelcome msg;

                msg.version().set(0);
                msg.outofdate_subscribed_caches().set(BuildSoCache());
                CMsgClientWelcome::Location loc;
                loc.latitude().set(0.f);
                loc.longitude().set(0.f);
                loc.country().set("PL");
                msg.location().set(loc);

                MatchmakingGC2ClientHello msg2;

                msg2.vac_banned().set(false);


                msg2.player_cur_xp().set(V::iXP);
                msg2.player_level().set(V::iLevel);


                msg.game_data2().set(msg2.serialize());
                msg.rtime32_gc_welcome_timestamp().set(static_cast<uint32_t>(time(nullptr)));
                msg.currency().set(2);
                msg.txn_country_code().set("PL");

                auto packet = msg.serialize();
            }
        }
        console::log(std::format("Sent packet {}", messageType).c_str());
        return oSendMsg(ecx, edx, unMsgType, pubData, cubData);
    }

    static inline EGCResults __fastcall hkRetrieveMessage(
        void* ecx, void* edx,
        uint32_t* punMsgType,
        void* pubDest, uint32_t cubDest,
        uint32_t* pcubMsgSize)
    {
        std::lock_guard<std::recursive_mutex> lock(queueMutex);

        if (!msgQueue.empty()) {

            for (size_t i = 0; i < msgQueue.size(); i++) {
                auto& msg = msgQueue[i];

                if (msg.data.size() == *pcubMsgSize) {

                    uint32_t totalSize = msg.data.size() + 8;
                    if (cubDest < totalSize)
                        return k_EGCResultBufferTooSmall;

                    ((uint32_t*)pubDest)[0] = msg.packetId | (1 << 31);
                    ((uint32_t*)pubDest)[1] = 0;
                    memcpy((uint8_t*)pubDest + 8, msg.data.data(), msg.data.size());

                    *punMsgType = msg.packetId | (1 << 31);
                    *pcubMsgSize = totalSize;

                    msgQueue.erase(msgQueue.begin() + i);


                    console::log(std::format("Dropped {}", msg.packetId).c_str());

                    return k_EGCResultOK;
                }
            }
        }
        else {
            auto res = oRetrieveMessage(ecx, edx, punMsgType, pubDest, cubDest, pcubMsgSize);;
            uint32_t uMsgType = *punMsgType & 0x7FFFFFFF;

            console::log(std::format("Received packet {}", uMsgType).c_str());
            switch (uMsgType) {
                case 4004: {
                    {
                        CMsgClientWelcome msg((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);
                        if (!msg.outofdate_subscribed_caches().has())
                            break;

                        auto cache = msg.outofdate_subscribed_caches().get();




                        auto objects = cache.objects().get_all();
                        for (size_t i = 0; i < objects.size(); i++) {
                            auto object = objects[i];
                            if (!object.type_id().has() || (object.type_id().get() != 1 && object.type_id().get() != 2))
                                continue;
                            
                            object.object_data().clear();
                            if (object.type_id().get() == 1) {


                                for (int i = 0; i < V::othermedals.size(); i++) {
                                    CSOEconItem item2;
                                    item2.id().set(MEDAL_BASE_ID + V::othermedals[i]);
                                    item2.account_id().set(G::g_SteamUser->GetSteamID().GetAccountID());
                                    item2.def_index().set(V::othermedals[i]);
                                    item2.inventory().set(MEDAL_BASE_ID + V::othermedals[i]);
                                    item2.origin().set(9);
                                    item2.quantity().set(4);
                                    item2.level().set(0);
                                    item2.flags().set(0);
                                    item2.in_use().set(false);
                                    item2.rarity().set(6);
                                    item2.quality().set(4);

                                    object.object_data().add(item2.serialize());
                                }

                            }
                            else {
                                {
                                    CSOPersonaDataPublic personaData;
                                    personaData.elevated_state().set(true);
                                    personaData.player_level().set(V::iLevel);


                                    CMsgClientWelcome::SubscribedType st;
                                    object.object_data().set(personaData.serialize());
                                }
                            }
                            cache.objects().set(object, i);
                        }

                        msg.outofdate_subscribed_caches().set(cache);
                        auto packet = msg.serialize();


                        CNetworking::QueueMessage(4004, packet, 250);

                    }

                    {
                        MatchmakingGC2ClientHello msg;

                        msg.vac_banned().set(false);


                        msg.player_cur_xp().set(V::iXP);
                        msg.player_level().set(V::iLevel);

                        CNetworking::QueueMessage(9110, msg.serialize(), 0);
                    }

                    break;
                }
            }

            return res;
        }

        return oRetrieveMessage(ecx, edx, punMsgType, pubDest, cubDest, pcubMsgSize);
    }

    static inline bool __fastcall hkIsMessageAvailable(void* ecx, void* edx, uint32* pcubMsgSize) {
        std::lock_guard<std::recursive_mutex> lock(queueMutex);

        for (auto& msg : msgQueue) {
            int delta = msg.delay - G::g_GlobalVars->currentTime;
            if (delta >= -0.2 && delta <= 0.2) {
                *pcubMsgSize = msg.data.size();
                return true;
            }
        }

        return oIsMessageAvailable(ecx, edx, pcubMsgSize);
    }


    static inline void QueueMessage(int packetId, const std::string& data, int delay) {
        std::lock_guard<std::recursive_mutex> lock(queueMutex);
        msgQueue.push_back({ data, packetId, (G::g_GlobalVars->currentTime + (delay / 1000.f))});

        console::log(std::format("Added {} To Queue", packetId).c_str());

    }

};