#include "vars.h"
#include "protos/Messages.h"
#include "memory.h"
#include "console/console.h"
namespace CMatchmaking {
    float flUpdateTime = 0;
    bool bHasMessagePending = false;
    int _steamid = 0;
    std::string msgt;

	int handleEndGame(int kills, int assists, int wonrounds, int steamid, float currTime) {
        _steamid = steamid;
        if (wonrounds < 1) wonrounds = 1;
        // TODO: FIX RANDOM CRASH
        int addXp = kills * 6 + assists * 6 + 30 * wonrounds;
        console::log(std::format("Kills: {}, Assists: {}\n OldXp: {}, OldLvl {}\n newXp: {}",kills, assists, V::iXP, V::iLevel, addXp).c_str());
        int oldXp = V::iXP;
        int oldLvl = V::iLevel;

        CMsgGCCstrike15_v2_GC2ServerNotifyXPRewarded msg;
        msg.account_id().set(steamid);

        XpProgressData xp;
        xp.xp_category().set(1);
        xp.xp_points().set(addXp);

        msg.xp_progress_data().add(xp);

        XpProgressData xp2;
        if (oldLvl < 5)
            xp2.xp_points().set(addXp * static_cast<int>(3.f * V::flXpMultipler));
        else if (oldLvl < 10)
            xp2.xp_points().set(addXp * static_cast<int>(2.5f * V::flXpMultipler));
        else if (oldLvl < 15)
            xp2.xp_points().set(addXp * static_cast<int>(2.f * V::flXpMultipler));
		else if (oldLvl < 20)
            xp2.xp_points().set(addXp * static_cast<int>(1.75f * V::flXpMultipler));
		else if (oldLvl < 30)
			xp2.xp_points().set(addXp * static_cast<int>(1.25f * V::flXpMultipler));
		else
			xp2.xp_points().set(addXp * static_cast<int>(0.5f * V::flXpMultipler));

        xp2.xp_category().set(3); // 5 - OW bonus, 4 - OW Reward 
        msg.xp_progress_data().add(xp2);

        addXp += xp2.xp_points().get();


        msg.current_level().set(oldLvl);
        msg.current_xp().set(oldXp);
        CCSUsrMsg_XpUpdate msg1;

        msg1.data().set(msg);

        auto res = msg1.serialize();

        flUpdateTime = currTime + 3.f;
        msgt = res;
        bHasMessagePending = true;
        
        return addXp;
	}

    void Refresh(float currTime, bool bPanoramaDll, void* g_VClient, int GameVer) {
		if (!bHasMessagePending)
			return;
		if (currTime < flUpdateTime)
			return;

        using DispatchUserMessageFn = bool(__thiscall*)(void*, int msg_type, int32_t nPassthroughFlags, int size, const void* msg);
        //DispatchUserMessageFn DispatchUserMessage = reinterpret_cast<DispatchUserMessageFn>(M::PatternScan(bPanoramaDll ? "client_panorama.dll" : "client.dll", "55 8B EC A1 ? ? ? ? A8 ? 75 ? 83 C8 ? A3 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? B9"));
        DispatchUserMessageFn DispatchUserMessage = reinterpret_cast<DispatchUserMessageFn>(reinterpret_cast<uintptr_t**>((*(void***)g_VClient)[(GameVer > 2018 || (GameVer == 2018 && bPanoramaDll)) ? 38 : 37]));

        DispatchUserMessage(g_VClient, 65, 0, msgt.size(), msgt.c_str());
        CCSUsrMsg_SendPlayerItemDrops drops;
        for (int i = 0; i < 4; i++) {
            CCrateOwned newcase;
            auto basecase = CCaseOpening::vCrates[rand() % CCaseOpening::vCrates.size()];
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, CCaseOpening::vCrates.size());
                basecase = CCaseOpening::vCrates[dis(gen)];
            }
            newcase.iDefIdx = basecase.iDefIdx;
            newcase.iKeyIdx = basecase.iKeyIdx;
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(1, 9999);
            newcase.iOCaseIdx = dis(gen);
            newcase.iOKeyIdx = dis(gen);
            newcase.iRarity = basecase.iRarity;
            newcase.szCaseName = basecase.szCaseName;
            newcase.vItems = basecase.vItems;
            newcase.bIsMusicKitBox = basecase.bIsMusicKitBox;

            CEconItemPreviewDataBlock item;
            item.accountid().set(_steamid);
            item.defindex().set(newcase.iDefIdx);
            item.rarity().set(1); 
            item.itemid().set(newcase.iOCaseIdx);
            item.dropreason().set(0);
            item.customname().set("");
            item.entindex().set(newcase.iDefIdx);



            V::cases.push_back(newcase);
            drops.entity_updates().add(item); 
        }

        auto s = drops.serialize();
        DispatchUserMessage(g_VClient, 61, 0, s.size(), s.c_str());
        
        // i could instead just implement convars, but why when you can just do it 10x more unsafe but faster!!!
        static auto GetGameMode = M::PatternScan("client.dll", "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? C3 8B 01 FF 60 ? CC CC E8"); 
        if (GetGameMode) {

            CCSUsrMsg_ServerRankUpdate rank;
            CCSUsrMsg_ServerRankUpdate::RankUpdate rankupd;

            // for now, the pattern is only for panorama, i need to get it for scaleform too
            switch (reinterpret_cast<int(__thiscall*)(void*)>(GetGameMode)(GetGameMode)){ // don't touch works :tf:
                case 1: {
                    rankupd.account_id().set(V::STEAM_ID);
                    rankupd.num_wins().set(++V::Ranks::Competetive::iWins);
                    rankupd.rank_new().set(V::Ranks::Competetive::iCurrentRank);
                    rankupd.rank_old().set(V::Ranks::Competetive::iCurrentRank);
                    rankupd.rank_type_id().set(6);
                    break;
                }
                case 2: {

                    rankupd.account_id().set(V::STEAM_ID);
                    rankupd.num_wins().set(++V::Ranks::Wingman::iWins);
                    rankupd.rank_new().set(V::Ranks::Wingman::iCurrentRank);
                    rankupd.rank_old().set(V::Ranks::Wingman::iCurrentRank);
                    rankupd.rank_type_id().set(7);

                    break;
                }
            }
            rank.rank_update().set(rankupd);
            auto pk = rank.serialize();
            DispatchUserMessage(g_VClient, 52, 0, pk.size(), pk.c_str());


        }

		bHasMessagePending = false;
		msgt.clear();

        V::PENDING_UPDATE = true;
    }
}