#pragma once
#include "../protos/ProtoParse.h"
#include "../protos/Messages.h"
#include <queue>
#include <mutex>
#include <string>
#include "globals.h"
#include "../CaseOpening/CInventory.h"

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
        
    }

    using SendMsgGC = EGCResults(__fastcall*)(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData);
    static inline SendMsgGC oSendMsg;

    using RetrieveMessageFn = EGCResults(__fastcall*)(void* ecx, void* edx, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize);
    static inline RetrieveMessageFn oRetrieveMessage;

    using IsMessageAvailableFn = bool(__fastcall*)(void* ecx, void* edx, uint32* pcubMsgSize);
    static inline IsMessageAvailableFn oIsMessageAvailable;

    template<typename T>
    inline static std::string get_4bytes(T value)
    {
        return std::string{ reinterpret_cast<const char*>(reinterpret_cast<void*>(&value)), 4 };
    }

    template<typename T>
    inline static CSOEconItemAttribute make_econ_item_attribute(int def_index, T value)
    {
        CSOEconItemAttribute attribute;
        attribute.def_index().set(def_index);
        attribute.value_bytes().set(get_4bytes(value));
        return attribute;
    }
    static inline EGCResults __fastcall hkSendMsg(void* ecx, void* edx, uint32_t unMsgType, const void* pubData, uint32_t cubData) {

        uint32_t messageType = unMsgType & 0x7FFFFFFF;
        switch (messageType) {

        case 1059: {
            CMsgAdjustItemEquippedState equip((void*)((DWORD)pubData + 8), cubData - 8);

            std::cout << "Equipped: " << equip.item_id().get() << " On Team: " << equip.new_class().get() << " On Slot: " << equip.new_slot().get() << "\n";
            CInventory::EquipSlot(equip.item_id().get(), equip.new_class().get(), equip.new_slot().get());

            break;
        }
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
            case 1007: {
                const uint8_t* buf = (uint8_t*)((DWORD)pubData);

                uint64_t first = 0, second = 0;

                std::memcpy(&first, buf + 18, 8);
                std::memcpy(&second, buf + 26, 8);

                printf("Key: %llu\n", first);
                printf("Case: %llu\n", second);

                auto item = CCaseOpening::GetRandomItem(second);

                V::items.push_back(item);
				
				V::iCaseResult = item.iItemId;
				V::SaveConfig();
                for (int i = 0; i < V::cases.size(); i++) {
                    if (V::cases[i].iOCaseIdx == second) {
                        V::cases.erase(V::cases.begin() + i);
                        break;
                    }
                }

                

				SendClientHello();


				break;
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
                            
                                for (auto item : V::items) {

                                    CSOEconItem item2;
                                    item2.id().set(item.iItemId);
                                    item2.account_id().set(G::g_SteamUser->GetSteamID().GetAccountID());
                                    if (item.iFlag == 5) {

                                        item2.def_index().set(1314);
                                    }
                                    else {
                                        item2.def_index().set(item.iDefIdx);
                                    }
                                    item2.inventory().set(item.iItemId);
                                    item2.origin().set(24);
                                    item2.level().set(1);
                                    item2.flags().set(0);
                                    item2.in_use().set(false);
                                    item2.rarity().set(item.iRarity);
                                    item2.quality().set(item.iQuality);
                                    if (item.bHasStattrack) {
                                        item2.attribute().add(make_econ_item_attribute(81, float(0)));
                                        item2.attribute().add(make_econ_item_attribute(80, item.flStattrack));
                                    }
                                    item2.attribute().add(make_econ_item_attribute(6, item.flPaintKit));
                                    item2.attribute().add(make_econ_item_attribute(7, float(item.iPattern)));
                                    item2.attribute().add(make_econ_item_attribute(8, item.flWear));
                                    if (item.iFlag == 5) {
                                        item2.attribute().add(make_econ_item_attribute(166, item.iDefIdx));
                                        
                                        if (CInventory::isEquipped(item.iItemId, 0, 54)) {
                                            CSOEconItemEquipped equip;
                                            equip.new_slot().set(54);
                                            equip.new_class().set(0);
                                            item2.equipped_state().add(equip);
                                        }

                                    }
                                    else if (item.iFlag == 4) {
                                        if (CInventory::isEquipped(item.iItemId, 0, 55)) {
                                            CSOEconItemEquipped equip;
                                            equip.new_slot().set(55);
                                            equip.new_class().set(0);
                                            item2.equipped_state().add(equip);
                                        }
                                    }
                                    else {
                                        int slotId = CInventory::GetSlotID(item.iDefIdx);
                                        if (CInventory::isEquipped(item.iItemId, 2, slotId)) {
                                            CSOEconItemEquipped equip;
                                            equip.new_slot().set(slotId);
                                            equip.new_class().set(2);
                                            item2.equipped_state().add(equip);
                                        }
                                        if (CInventory::isEquipped(item.iItemId, 3, slotId)) {
                                            CSOEconItemEquipped equip;
                                            equip.new_slot().set(slotId);
                                            equip.new_class().set(3);
                                            item2.equipped_state().add(equip);
                                        }
                                    }

                                    object.object_data().add(item2.serialize());
                                }
                                for (auto item : V::cases) {

                                    CSOEconItem item2;
                                    item2.id().set(item.iOCaseIdx);
                                    item2.account_id().set(G::g_SteamUser->GetSteamID().GetAccountID());
                                    item2.def_index().set(item.iDefIdx);
                                    item2.inventory().set(item.iOCaseIdx);
                                    item2.origin().set(8);
                                    item2.level().set(1);
                                    item2.flags().set(0);
                                    item2.in_use().set(false);
                                    item2.rarity().set(0);
                                    item2.quality().set(4);

                                    object.object_data().add(item2.serialize());
                                }
                                for (auto item : V::cases) {
									if (item.iOCaseIdx == 0 || item.iKeyIdx == 0) // ignore cases without keys
										continue;
                                    CSOEconItem item2;
                                    item2.id().set(item.iOKeyIdx);
                                    item2.account_id().set(G::g_SteamUser->GetSteamID().GetAccountID());
                                    item2.def_index().set(item.iKeyIdx);
                                    item2.inventory().set(item.iOKeyIdx);
                                    item2.origin().set(8);
                                    item2.level().set(1);
                                    item2.flags().set(0);
                                    item2.in_use().set(false);
                                    item2.rarity().set(0);
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


                        CNetworking::QueueMessage(4004, packet, 100);

                    }

                    {
                        MatchmakingGC2ClientHello msg;

                        msg.vac_banned().set(false);


                        msg.player_cur_xp().set(V::iXP);
                        msg.player_level().set(V::iLevel);

                        CNetworking::QueueMessage(9110, msg.serialize(), 250);
                    }

                    if (V::iCaseResult) {
                        CMsgGCItemCustomizationNotification msg;
                        msg.item_id().set(V::iCaseResult);
                        msg.request().set(1007);
                        CNetworking::QueueMessage(1090, msg.serialize(), 500);
                        V::iCaseResult = 0;
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
        auto og = oIsMessageAvailable(ecx, edx, pcubMsgSize);
		if (og)
			return og;
        for (auto& msg : msgQueue) {
            int delta = msg.delay - (float)(GetTickCount()/1000.f);
            if (delta >= -0.2 && delta <= 0.2) {
                *pcubMsgSize = msg.data.size();
                return true;
            }
        }

        return og;
    }


    static inline void QueueMessage(int packetId, const std::string& data, int delay) {

        std::lock_guard<std::recursive_mutex> lock(queueMutex);
        msgQueue.push_back({ data, packetId, (float)((GetTickCount() + delay)/1000.f)});
        
        console::log(std::format("Added {} To Queue", packetId).c_str());

    }

};