#pragma once
#include "../globals.h"
#include "../console/console.h"

class EventListener;
namespace E {

	inline EventListener* g_EventListener = nullptr;
}
class EventListener : public IGameEventListener2 {
public:
	EventListener() {

	};

	~EventListener() {

	};

	virtual void FireGameEvent(IGameEvent* pEvent) {

		// TODO: FIX XP CALCULATION/OTHER WEIRD BUGS HERE
		static int kills = 0;
		static int assists = 0;
		static int wonRounds = 0;

		const char* txt = pEvent->GetName();
		console::log(txt);
		if (strcmp(txt, "cs_win_panel_match") == 0) {
			int newxp = CMatchmaking::handleEndGame(kills, assists, wonRounds, V::STEAM_ID, G::g_GlobalVars->currentTime);
			V::iXP += newxp;
			while (V::iXP >= 5000) {
				V::iXP -= 5000;
				V::iLevel++;
			}

			V::SaveConfig();
			kills = 0;
			assists = 0;
			wonRounds = 0;
		}
		else if (strcmp(txt, "cs_win_panel_round") == 0) {
			wonRounds++;
		}
		else if (strcmp(txt, "cs_game_disconnected") == 0) {
			kills = 0;
			assists = 0;
			wonRounds = 0;
			console::log("reset kills");
		}
		else {
			auto userid = G::g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
			auto idx = G::g_EngineClient->GetLocalPlayerIndex();
			auto attacker = pEvent->GetInt("attacker");
			auto assister = pEvent->GetInt("assister");
			if (userid != attacker && userid != idx && idx == G::g_EngineClient->GetPlayerForUserID(attacker)) {
				kills++;
				console::log(std::format("added kill from {}", attacker).c_str());
			}
			else if (assister != userid && userid != idx && idx == G::g_EngineClient->GetPlayerForUserID(assister)) {
				assists++;
				console::log(std::format("added assist from {}", assister).c_str());
			}
		}
	};

	int GetEventDebugID() override {
		return 42; // 42
	};
};