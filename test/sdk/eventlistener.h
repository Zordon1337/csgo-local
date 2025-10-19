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

		static int kills = 0;
		static int assists = 0;
		static int wonRounds = 0;
		static int lostRounds = 0;

		const char* txt = pEvent->GetName();
		console::log(txt);
		if (strcmp(txt, "cs_win_panel_match") == 0) {
			int newxp = CMatchmaking::handleEndGame(kills, assists, wonRounds, lostRounds, V::STEAM_ID, G::g_GlobalVars->currentTime);
			V::iXP += newxp;
			while (V::iXP >= 5000) {
				V::iXP -= 5000;
				V::iLevel++;
			}

			V::SaveConfig();
			kills = 0;
			assists = 0;
			wonRounds = 0;
			lostRounds = 0;
		}
		else if (strcmp(txt, "cs_win_panel_round") == 0) {

		}
		else if (strcmp(txt, "cs_game_disconnected") == 0) {
			kills = 0;
			assists = 0;
			wonRounds = 0;
			lostRounds = 0;
			V::SaveConfig();
		}
		else if (strcmp(txt, "round_mvp") == 0) {
			auto& it = CInventory::GetItemPtr(0, 54, CInventory::GetCurrentMusicKit());
			auto userid = G::g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
			auto idx = G::g_EngineClient->GetLocalPlayerIndex();
			if (it.bHasStattrack && idx == userid) {
				it.flStattrack++;
				pEvent->SetInt("musickitmvps", it.flStattrack);
			}
		}
		else if (strcmp(txt, "round_end") == 0) {
			auto winner = pEvent->GetInt("winner");

			if (winner == 3 || winner == 2) {
				// we dont want to add loss when spectator wins lol=
				auto idx = G::g_EntityList->GetEntityFromIndex(G::g_EngineClient->GetLocalPlayerIndex());
				if (idx->m_iTeamNum() == winner) {
					wonRounds++;
				}
				else {

					lostRounds++;
				}
			}
		}
		else {
			auto userid = G::g_EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"));
			auto idx = G::g_EngineClient->GetLocalPlayerIndex();
			auto attacker = pEvent->GetInt("attacker");
			auto assister = pEvent->GetInt("assister");
			auto weapon_itemid = pEvent->GetInt("weapon_itemid");
			if (userid != attacker && userid != idx && idx == G::g_EngineClient->GetPlayerForUserID(attacker)) {
				kills++;
				CEntity* local = G::g_EntityList->GetEntityFromIndex(idx);
				if (!local || local->m_lifeState() != 0) {
					return;
				}

				auto weapon = (CBaseAttributableItem*)G::g_EntityList->GetClientEntityFromHandle(local->m_hActiveWeapon());
				if (!weapon)
					return;

				auto& wp = CInventory::GetItemPtr(
					local->m_iTeamNum(),
					CInventory::GetSlotID(weapon->m_iItemDefinitionIndex()),
					weapon->m_iItemDefinitionIndex()
				);

				if (wp.bHasStattrack) {
					wp.flStattrack++;
					weapon->OnDataChanged(5);
					weapon->OnDataChanged(0);
					weapon->PostDataUpdate(0);
				}
				//console::log(std::format("added kill from {}", attacker).c_str());
			}
			else if (assister != userid && userid != idx && idx == G::g_EngineClient->GetPlayerForUserID(assister)) {
				assists++;
				//console::log(std::format("added assist from {}", assister).c_str());
			}
		}
	};

	int GetEventDebugID() override {
		return 42; // 42
	};
};