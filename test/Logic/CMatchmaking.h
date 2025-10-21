#include "vars.h"
#include "../SDK/proto/Messages.h"
#include "memory.h"
#include "console/console.h"
namespace CMatchmaking {
    float flUpdateTime = 0;
    bool bHasMessagePending = false;
    int _steamid = 0;
    std::string msgt;
    int _wonRounds = -1;
    int _lostRounds = -1;
    bool bShouldDrop = false;
	int handleEndGame(int kills, int assists, int wonrounds, int lostRounds, int steamid, float currTime) {
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
        if(oldXp + addXp >= 5000) {
            bShouldDrop = true;
        }
        CCSUsrMsg_XpUpdate msg1;

        msg1.data().set(msg);

        auto res = msg1.serialize();

        flUpdateTime = currTime + 3.f;
        msgt = res;
        bHasMessagePending = true;
        _wonRounds = wonrounds;
        _lostRounds = lostRounds;
        
        return addXp;
	}
    void UpdateRank(int gameMode) {
        switch (gameMode) {
            case 2: {
                int& ogRank = V::Ranks::Wingman::iCurrentRank;
                int& winStreak = V::Ranks::Wingman::iWinStreak;
                int& lossStreak = V::Ranks::Wingman::iLossStreak;
                int& elo = V::Ranks::Wingman::iElo;

                int wins = _wonRounds;
                int losses = _lostRounds;

                if (wins > losses) {
                    winStreak++;
                    lossStreak = 0;
                    int nElo = winStreak * 25;
                    if (nElo > 200)
                        nElo = 200;
                    elo += nElo;
                    V::Ranks::Wingman::iWins++;
                }
                else {
                    lossStreak++;
                    winStreak = 0;
                    int nElo = lossStreak * 25;
                    if (nElo < -200)
                        nElo = -200;
                    elo -= nElo;
                    V::Ranks::Wingman::iLosses++;
                }

                if (V::Ranks::Wingman::iWins < 10) break;
                if (elo < 400)
                    ogRank = 1; // Silver I
                else if (elo < 700)
                    ogRank = 2; // Silver II
                else if (elo < 1000)
                    ogRank = 3; // Silver III
                else if (elo < 1300)
                    ogRank = 4; // Silver IV
                else if (elo < 1500)
                    ogRank = 5; // Silver Elite
                else if (elo < 1700)
                    ogRank = 6; // Silver Elite Master
                else if (elo < 1900)
                    ogRank = 7; // Gold Nova I
                else if (elo < 2100)
                    ogRank = 8; // Gold Nova II
                else if (elo < 2300)
                    ogRank = 9; // Gold Nova III
                else if (elo < 2500)
                    ogRank = 10; // Gold Nova Master
                else if (elo < 2700)
                    ogRank = 11; // Master Guardian I
                else if (elo < 2900)
                    ogRank = 12; // Master Guardian II
                else if (elo < 3100)
                    ogRank = 13; // MGE
                else if (elo < 3300)
                    ogRank = 14; // DMG
                else if (elo < 3500)
                    ogRank = 15; // Legendary Eagle
                else if (elo < 3700)
                    ogRank = 16; // LEM
                else if (elo < 3900)
                    ogRank = 17; // Supreme
                else
                    ogRank = 18; // Global Elite

                break;
            }
            case 1: {

                int& ogRank = V::Ranks::Competetive::iCurrentRank;
                int& winStreak = V::Ranks::Competetive::iWinStreak;
                int& lossStreak = V::Ranks::Competetive::iLossStreak;
                int& elo = V::Ranks::Competetive::iElo;

                int wins = _wonRounds;
                int losses = _lostRounds;

                if (wins > losses) {
                    winStreak++;
                    lossStreak = 0;
                    int nElo = winStreak * 25;
                    if (nElo > 200)
                        nElo = 200;
                    elo += nElo;
                    V::Ranks::Competetive::iWins++;
                }
                else {
                    lossStreak++;
                    winStreak = 0;
                    int nElo = lossStreak * 25;
                    if (nElo < -200)
                        nElo = -200;
                    elo -= nElo;
                    V::Ranks::Competetive::iLosses++;
                }
                if (V::Ranks::Competetive::iWins < 10) break;
                if (elo < 400)
                    ogRank = 1; // Silver I
                else if (elo < 700)
                    ogRank = 2; // Silver II
                else if (elo < 1000)
                    ogRank = 3; // Silver III
                else if (elo < 1300)
                    ogRank = 4; // Silver IV
                else if (elo < 1500)
                    ogRank = 5; // Silver Elite
                else if (elo < 1700)
                    ogRank = 6; // Silver Elite Master
                else if (elo < 1900)
                    ogRank = 7; // Gold Nova I
                else if (elo < 2100)
                    ogRank = 8; // Gold Nova II
                else if (elo < 2300)
                    ogRank = 9; // Gold Nova III
                else if (elo < 2500)
                    ogRank = 10; // Gold Nova Master
                else if (elo < 2700)
                    ogRank = 11; // Master Guardian I
                else if (elo < 2900)
                    ogRank = 12; // Master Guardian II
                else if (elo < 3100)
                    ogRank = 13; // MGE
                else if (elo < 3300)
                    ogRank = 14; // DMG
                else if (elo < 3500)
                    ogRank = 15; // Legendary Eagle
                else if (elo < 3700)
                    ogRank = 16; // LEM
                else if (elo < 3900)
                    ogRank = 17; // Supreme
                else
                    ogRank = 18; // Global Elite
                break;

            }
        }
    }
    void Refresh(float currTime, bool bPanoramaDll, void* g_VClient, int GameVer) {
		if (!bHasMessagePending)
			return;
		if (currTime < flUpdateTime)
			return;

        using DispatchUserMessageFn = bool(__thiscall*)(void*, int msg_type, int32_t nPassthroughFlags, int size, const void* msg);
        DispatchUserMessageFn DispatchUserMessage = reinterpret_cast<DispatchUserMessageFn>(reinterpret_cast<uintptr_t**>((*(void***)g_VClient)[(GameVer > 2018 || (GameVer == 2018 && bPanoramaDll)) ? 38 : 37]));

        DispatchUserMessage(g_VClient, 65, 0, msgt.size(), msgt.c_str());
        if (bShouldDrop) {
            CCSUsrMsg_SendPlayerItemDrops drops;
            for (int i = 0; i < 5; i++) {
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
            bShouldDrop = false;
            auto s = drops.serialize();
            DispatchUserMessage(g_VClient, 61, 0, s.size(), s.c_str());
        }
        
        // i could instead just implement convars, but why when you can just do it 10x more unsafe but faster!!!
        static auto GetGameMode = M::PatternScan(bPanoramaDll ? "client_panorama.dll" : "client.dll", "8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? C3 8B 01 FF 60 ? CC CC E8");
        if (GetGameMode) {

            CCSUsrMsg_ServerRankUpdate rank;
            CCSUsrMsg_ServerRankUpdate::RankUpdate rankupd;

            // for now, the pattern is only for panorama, i need to get it for scaleform too
            switch (reinterpret_cast<int(__thiscall*)(void*)>(GetGameMode)(GetGameMode)){ // don't touch works :tf:
                case 1: {
                    rankupd.account_id().set(V::STEAM_ID);
                    rankupd.rank_old().set(V::Ranks::Competetive::iCurrentRank);
                    UpdateRank(1);
                    rankupd.rank_new().set(V::Ranks::Competetive::iCurrentRank);
                    rankupd.num_wins().set(V::Ranks::Competetive::iWins);
                    rankupd.rank_type_id().set(6);
                    break;
                }
                case 2: {

                    rankupd.account_id().set(V::STEAM_ID);
                    rankupd.rank_old().set(V::Ranks::Wingman::iCurrentRank);
                    UpdateRank(2);
                    rankupd.rank_new().set(V::Ranks::Wingman::iCurrentRank);
                    rankupd.num_wins().set(V::Ranks::Wingman::iWins);
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