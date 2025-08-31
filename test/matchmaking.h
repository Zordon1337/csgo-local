#pragma once
#include "vars.h"
#include "protos/Messages.h"
#include "memory.h"

namespace CMatchmaking {
	int handleEndGame(int kills, int assists, int wonrounds, int steamid) {

        if (wonrounds < 1) wonrounds = 1;
        // TODO: FIX CRASH
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
        if (oldLvl < 21) { // multipler until level 21

            XpProgressData xp2;
            if(oldLvl < 5)
				xp2.xp_points().set(addXp * (3 * V::flXpMultipler));
			else if (oldLvl < 10)
				xp2.xp_points().set(addXp * (2 * V::flXpMultipler));
			else if (oldLvl < 15)
				xp2.xp_points().set(addXp * (1.5 * V::flXpMultipler));
			else
				xp2.xp_points().set(addXp * (1.25 * V::flXpMultipler));

			xp2.xp_category().set(3); // 5 - OW bonus, 4 - OW Reward 
            msg.xp_progress_data().add(xp2);

			addXp += xp2.xp_points().get();

        }


        msg.current_level().set(oldLvl);
        msg.current_xp().set(oldXp);
        using DispatchUserMessageFn = void(__cdecl*)(int msg_type, int32_t nPassthroughFlags, int size, const void* msg);
        DispatchUserMessageFn DispatchUserMessage = reinterpret_cast<DispatchUserMessageFn>(M::PatternScan("client_panorama.dll", "55 8B EC A1 ? ? ? ? A8 ? 75 ? 83 C8 ? A3 ? ? ? ? E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? B9"));
        CCSUsrMsg_XpUpdate msg1;

        msg1.data().set(msg);

        auto res = msg1.serialize();
        DispatchUserMessage(65, 0, res.size(), res.c_str());


        
        V::PENDING_UPDATE = true;



        return addXp;
	}
}