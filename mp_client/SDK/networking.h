#pragma once
#include "proto/ProtoParse.h"
#include "proto/Messages.h"
#include <queue>
#include <mutex>
#include <string>
#include "../globals.h"
#include "../Logic/CInventory.h"
#include "../console/console.h"
#include "http.h"
#include "../memory.h"

class CNetworking {
private:
    static inline void* _ecx;
    static inline int _a2;

    struct GCMessage {
        std::string data;
        int packetId;
        float delay;
    };
    struct Message {
        int packet;
        uint32_t msgSize;
        std::string msg;
    };
    static inline std::deque<GCMessage> msgQueue;
    static inline std::recursive_mutex queueMutex;


public:

    static bool SendMatchmakingClient2GCHello()
    {
        ProtoWriter msg(0);
        auto packet = msg.serialize();
        void* ptr = malloc(packet.size() + 8);

        if (!ptr)
            return false;

        ((uint32_t*)ptr)[0] = 9109 | ((DWORD)1 << 31);
        ((uint32_t*)ptr)[1] = 0;

        memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
        bool result = G::g_GameCoordinator->SendMsg(9109 | ((DWORD)1 << 31), ptr, packet.size() + 8) == EGCResults::k_EGCResultOK;
        free(ptr);

        return result;
    }
    static constexpr uint32_t MakeAddress(uint32_t v1, uint32_t v2, uint32_t v3, uint32_t v4)
    {
        return v4 | (v3 << 8) | (v2 << 16) | (v1 << 24);
    }
    static bool SendClientHello()
    {
        if (G::gameVer < 2016) {
            // pre 2015 doesnt send
            {
                CMsgCStrike15Welcome c;
                c.store_item_hash().set(136617352);
                c.timeplayedconsecutively().set(0);
                c.time_first_played().set(1329845773);
                c.last_time_played().set(1680260376);
                c.last_ip_address().set(MakeAddress(127, 0, 0, 1));

                MatchmakingGC2ClientHello2  wlcm;
                wlcm.account_id().set(V::STEAM_ID);
                wlcm.penalty_reason().set(11);
                wlcm.penalty_seconds().set(9200);

                PlayerCommendationInfo plr;
                plr.cmd_friendly().set(999);
                plr.cmd_leader().set(999);
                plr.cmd_teaching().set(999);
                wlcm.commendation().set(plr);
                wlcm.vac_banned().set(0);

                MatchmakingGC2ClientHello2::PlayerRankingInfo2 plr2;
                plr2.account_id().set(V::STEAM_ID);
                plr2.rank_id().set(V::Ranks::Competetive::iCurrentRank);
                plr2.wins().set(V::Ranks::Competetive::iWins);
                plr2.rank_change().set(V::Ranks::Competetive::iCurrentRank);
                wlcm.ranking().set(plr2);

                CMsgClientWelcome2 msg;
                msg.version().set(0);
                msg.game_data().set(c.serialize());
                msg.game_data2().set(wlcm.serialize());
                msg.currency().set(2);
                msg.rtime32_gc_welcome_timestamp().set(static_cast<uint32_t>(time(nullptr)));
                
                
                CMsgClientWelcome::CMsgSOCacheSubscribed cache = CMsgClientWelcome::CMsgSOCacheSubscribed();

                CMsgClientWelcome::SubscribedType object = CMsgClientWelcome::SubscribedType();
                object.type_id().set(1);
               
                CMsgSOIDOwner typ;
                typ.id().set((0x1100001ULL << 32) | (V::STEAM_ID & 0xffffffff));
                typ.owner_type().set(1);

                cache.owner_soid().add(typ);



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
                        item2.attribute().add(make_econ_item_attribute(81, 0));
                        item2.attribute().add(make_econ_item_attribute(80, (int)item.flStattrack));
                        item2.quality().set(2);
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
                cache.objects().add(object);
                msg.outofdate_subscribed_caches().set(cache);
                auto packet = msg.serialize();


                CNetworking::QueueMessage(4004, msg.serialize(), 100);

            }



            if (V::iCaseResult) {
                CMsgGCItemCustomizationNotification msg2;
                msg2.item_id().set(V::iCaseResult);
                msg2.request().set(1007);
                CNetworking::QueueMessage(1090, msg2.serialize(), 150);
                V::iCaseResult = 0;
            }
            return true;
        }
       
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

            V::SaveConfig();
            break;
        }
            case 9172: {
                // TODO: I NEED TO GET ACC WITH LEVEL 40 SINCE IDK PROPER RESPONSE THAT DOESN'T CRASH GAME
                

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

                CMsgGCCStrike15_v2_Client2GCRequestPrestigeCoin response((void*)((DWORD)pubData + 8), cubData - 8);

                response.defindex().set(MEDAL_ID);
                response.hours().set(2);
                response.prestigetime().set(1755626035);
                response.upgradeid().set(1);
                

                auto packet = response.serialize();

                CNetworking::QueueMessage(9172, packet, 2000);

                if (G::gameVer > 2015 && G::gameVer < 2024)
                {
					bool bHasMaxMedal = false;
					for (int i = 0; i < V::items.size(); i++) {
						if (V::items[i].iDefIdx == MEDAL_ID + 6) {
							bHasMaxMedal = true;
							break;
						}
					}
                    int iMedalLevel = 0;
                    for (int i = 0; i < V::items.size(); i++) {
						if (V::items[i].iDefIdx == MEDAL_ID + i) {
                            iMedalLevel = i;
							break;
						}
                    }
                    if (iMedalLevel > 0)
                    {
                        // replace old medal
						for (int i = 0; i < V::items.size(); i++) {
							if (V::items[i].iDefIdx == MEDAL_ID + iMedalLevel) {
                                V::items[i].iDefIdx = MEDAL_ID + iMedalLevel + 1; // should change id too tho
								break;
							}
						}
					}
					else {
                        CItem item;
                        item.iDefIdx = MEDAL_ID + 1;
                        item.bHasStattrack = false;
                        item.flPaintKit = 0;
                        item.flStattrack = 0;
                        item.flWear = 0;
                        item.iItemId = rand() % 10000;
                        item.iPattern = 0;
                        item.iFlag = 0;
                        item.iQuality = 4;
                        item.iRarity = 6;
                        V::items.push_back(item);
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

    inline static std::vector<Message> outgoing = {};
    static inline EGCResults __fastcall hkRetrieveMessage(
        void* ecx, void* edx,
        uint32_t* punMsgType,
        void* pubDest, uint32_t cubDest,
        uint32_t* pcubMsgSize)
    {
        std::lock_guard<std::recursive_mutex> lock(queueMutex);

        if (!outgoing.empty()) {

            for (size_t i = 0; i < outgoing.size(); i++) {
                auto& msg = outgoing[i];

                if (msg.msgSize == *pcubMsgSize) {

                    uint32_t totalSize = msg.msg.size() + 8;
                    if (cubDest < totalSize)
                        return k_EGCResultBufferTooSmall;

                    ((uint32_t*)pubDest)[0] = msg.packet | (1 << 31);
                    ((uint32_t*)pubDest)[1] = 0;
                    memcpy((uint8_t*)pubDest + 8, msg.msg.data(), msg.msg.size());

                    *punMsgType = msg.packet | (1 << 31);
                    *pcubMsgSize = totalSize;

                    outgoing.erase(outgoing.begin() + i);


                    console::log(std::format("Dropped {}", msg.packet).c_str());

                    return k_EGCResultOK;
                }
            }
        }
        else {
            auto res = oRetrieveMessage(ecx, edx, punMsgType, pubDest, cubDest, pcubMsgSize);;
            uint32_t uMsgType = *punMsgType & 0x7FFFFFFF;

            console::log(std::format("Received packet {}", uMsgType).c_str());
            switch (uMsgType) {
                case 9104: {

                    *pcubMsgSize = 0;
                    break;
                }
                case 9110: {




                    MatchmakingGC2ClientHello msg;
                    msg.player_cur_xp().set(V::iXP);
                    msg.player_level().set(V::iLevel);
                    CNetworking::QueueMessage(9110, msg.serialize(), 800);
					break;
                }
                case 9194: {
                    CMsgGCCStrike15_v2_ClientGCRankUpdate rank;
                    MatchmakingGC2ClientHello::PlayerRankingInfo rankinfo;
                    rankinfo.account_id().set(V::STEAM_ID);
                    rankinfo.rank_type_id().set(7);
                    rankinfo.rank_id().set(V::Ranks::Wingman::iCurrentRank);
                    rankinfo.wins().set(V::Ranks::Wingman::iWins);

                    rank.ranking().add(rankinfo);
                    rankinfo.account_id().set(V::STEAM_ID);
                    rankinfo.rank_type_id().set(6);
                    rankinfo.rank_id().set(V::Ranks::Competetive::iCurrentRank);
                    rankinfo.wins().set(V::Ranks::Competetive::iWins);

                    rank.ranking().add(rankinfo);

                    auto packet = rank.serialize();
                    memcpy((void*)((DWORD)pubDest + 8), (void*)packet.data(), packet.size());
                    *pcubMsgSize = packet.size() + 8;
                    break;
                }
                case 9128: {
					CMsgGCCStrike15_v2_PlayersProfile request((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);
                    int id = request.account_profiles().get(0).account_id().safeget();
                    auto response = http::Get(std::wstring(V::remoteAddr.begin(), V::remoteAddr.end()).c_str(), L"/get_user_profile?userId=" + std::to_wstring(id));
                    console::log(response.c_str());
                    try {
                        auto resJson = nlohmann::json::parse(response);

                        if (resJson.is_array() && !resJson.empty())
                            resJson = resJson[0];
                        CMsgGCCStrike15_v2_PlayersProfile responseMsg;
                        MatchmakingGC2ClientHello responseProfile;
                        responseProfile.account_id().set(id);
                        responseProfile.player_level().set(resJson.value("lvl", 1));
                        for (auto& inv : CInventory::remoteInventories) {
							if (inv.steamID == id) {
                                inv.lvl = resJson.value("lvl", 1);
                                inv.xp = resJson.value("xp", 0);
                                inv.mmrank = resJson.value("mmrank", 0);
                                inv.mmwins = resJson.value("mmwins", 0);
                                inv.wmrank = resJson.value("wmrank", 0);
                                inv.wmwins = resJson.value("wmwins", 0);
								break;
							}
                        }
                        responseProfile.player_cur_xp().set(resJson.value("xp", 0));
                        auto medals = resJson.value("medals", std::vector<int>{});
                        PlayerMedalsInfo medalsInfo;
                        for (auto medal : medals) {
                            medalsInfo.display_items_defidx().add(medal);
                        }
                        responseProfile.medals().set(medalsInfo);
                        PlayerCommendationInfo commendation;
                        commendation.cmd_friendly().set(999);
                        commendation.cmd_leader().set(999);
                        commendation.cmd_teaching().set(999);
                        responseProfile.commendation().set(commendation);
                        MatchmakingGC2ClientHello::PlayerRankingInfo ranking;
                        ranking.account_id().set(id);
                        ranking.rank_id().set(resJson.value("wmrank", 0));
                        ranking.wins().set(resJson.value("wmwins", 0));
                        ranking.rank_type_id().set(7);
                        responseProfile.ranking().add(ranking);
                        MatchmakingGC2ClientHello::PlayerRankingInfo ranking2;
                        ranking2.account_id().set(id);
                        ranking2.rank_id().set(resJson.value("mmrank", 0));
                        ranking2.wins().set(resJson.value("mmwins", 0));
                        ranking2.rank_type_id().set(6);
                        responseProfile.ranking().add(ranking2);
                        responseMsg.account_profiles().add(responseProfile);
                        auto packet = responseMsg.serialize();


                        memcpy((void*)((DWORD)pubDest + 8), (void*)packet.data(), packet.size());
                        *pcubMsgSize = packet.size() + 8;
                    }
                    catch (...) {
                        CMsgGCCStrike15_v2_PlayersProfile responseMsg;
                        MatchmakingGC2ClientHello responseProfile;
                        responseProfile.account_id().set(id);
                        responseProfile.player_level().set(1);
                        responseProfile.player_cur_xp().set(0);
                        PlayerMedalsInfo medalsInfo;
                        responseProfile.medals().set(medalsInfo);
                        PlayerCommendationInfo commendation;
                        commendation.cmd_friendly().set(999);
                        commendation.cmd_leader().set(999);
                        commendation.cmd_teaching().set(999);
                        responseProfile.commendation().set(commendation);
                        responseMsg.account_profiles().add(responseProfile);
                        auto packet = responseMsg.serialize();


                        memcpy((void*)((DWORD)pubDest + 8), (void*)packet.data(), packet.size());
                        *pcubMsgSize = packet.size() + 8;
                    }
                    break;
                }
                case 4004: {


                    {

                        CMsgClientWelcome msg((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8);
                        if (!msg.outofdate_subscribed_caches().has())
                            break;

                        auto cache = msg.outofdate_subscribed_caches().get();




                        MatchmakingGC2ClientHello  wlcm;
                        wlcm.account_id().set(V::STEAM_ID);
                        wlcm.penalty_reason().set(2);
                        wlcm.penalty_seconds().set(92000);

                        PlayerCommendationInfo plr;
                        plr.cmd_friendly().set(999);
                        plr.cmd_leader().set(999);
                        plr.cmd_teaching().set(999);
                        wlcm.commendation().set(plr);
                        wlcm.vac_banned().set(0);

                        MatchmakingGC2ClientHello::PlayerRankingInfo rankinfo;
                        rankinfo.account_id().set(V::STEAM_ID);
                        rankinfo.rank_type_id().set(7);
                        rankinfo.rank_id().set(V::Ranks::Wingman::iCurrentRank);
                        rankinfo.wins().set(V::Ranks::Wingman::iWins);

                        wlcm.ranking().add(rankinfo);
                        rankinfo.account_id().set(V::STEAM_ID);
                        rankinfo.rank_type_id().set(6);
                        rankinfo.rank_id().set(V::Ranks::Competetive::iCurrentRank);
                        rankinfo.wins().set(V::Ranks::Competetive::iWins);

                        wlcm.ranking().add(rankinfo);

                        wlcm.player_level().set(V::iLevel);
                        wlcm.player_cur_xp().set(V::iXP);

                        msg.game_data2().set(wlcm.serialize());
                        

                        auto objects = cache.objects().get_all();
                        for (size_t i = 0; i < objects.size(); i++) {
                            auto object = objects[i];
                            if (!object.type_id().has())
                                continue;
                            int id = object.type_id().get();
                            if (id == 2) {

                                CSOPersonaDataPublic data;
                                data.elevated_state().set(true);
                                data.player_level().set(V::iLevel);
                                data.commendation().set(plr);
                                object.object_data().clear();
                                object.object_data().set(data.serialize());
                                cache.objects().set(object, i);
                            }
                            if (id == 1) {

                                object.object_data().clear();
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
                                        item2.attribute().add(make_econ_item_attribute(81, 0));
                                        item2.attribute().add(make_econ_item_attribute(80, (int)item.flStattrack));
                                        item2.quality().set(2);
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
                                cache.objects().set(object, i);
                            }

                        }
                        msg.outofdate_subscribed_caches().set(cache);
                        CNetworking::QueueMessage(4004, msg.serialize(), 100);

                        

                    }


                    if (V::iCaseResult) {
                        CMsgGCItemCustomizationNotification msg2;
                        msg2.item_id().set(V::iCaseResult);
                        msg2.request().set(1007);
                        CNetworking::QueueMessage(1090, msg2.serialize(), 150);
                        V::iCaseResult = 0;
                    }


                    break;
                }
            }

            return res;
        }

        return oRetrieveMessage(ecx, edx, punMsgType, pubDest, cubDest, pcubMsgSize);
    }
    static bool HasOutgoingMessages(uint32_t& messageSize) {
        if (outgoing.size() > 0) {
            messageSize = outgoing.back().msgSize;
            return true;
        }
        else {
            return false;
        }
    }
    static inline bool __fastcall hkIsMessageAvailable(void* ecx, void* edx, uint32* pcubMsgSize) {
        std::lock_guard<std::recursive_mutex> lock(queueMutex);
        auto og = oIsMessageAvailable(ecx, edx, pcubMsgSize);
		if (og)
			return og;
        
        uint32_t msgSize;
        if (HasOutgoingMessages(msgSize)) {
            *pcubMsgSize = msgSize;
            return true;
        }

        return og;
    }


    static inline void QueueMessage(int packetId, const std::string& data, int delay) {

        std::lock_guard<std::recursive_mutex> lock(queueMutex);
        msgQueue.push_back({ data, packetId, (float)((GetTickCount() + delay)/1000.f)});
        
        outgoing.push_back({ packetId, static_cast<uint32_t>(data.size() + 8), data});
        console::log(std::format("Added {} To Queue", packetId).c_str());

        uint8_t* match = M::PatternScan("client.dll", "A1 ? ? ? ? 8B 74 24 ? 83 C0 ? 89 44 24 ? 81 E6 ? ? ? ? 8B 44 24 ? C7 44 24");
        if (!match) return;
        auto s_pCGCGameSpecificClientSystem = **reinterpret_cast<void***>(match + 1);
        if (!s_pCGCGameSpecificClientSystem) return;
        void* clientgc = reinterpret_cast<char*>(s_pCGCGameSpecificClientSystem) + (G::bIsPanoramaDll ? 0x70 : 0x20); // 0x70 for panorama, 0x20 for scaleform, most likely as i checked only 2020, 2023, late 2018 and 2016, 2015, 2014

        if (!clientgc) return;

        auto OnGCMessageAvailable = M::PatternScan("client.dll", "55 8B EC 83 E4 ? 83 EC ? 53 8B D9 56 57 8B 4B ? 85 C9");


        if (!OnGCMessageAvailable) return;
        using sendHelloFn = void(__thiscall*)(void*, int); // 2nd param is GCMessageAvailable_t but its unused?
        sendHelloFn func = (sendHelloFn)(OnGCMessageAvailable);

        func(clientgc, 0);
    }

};