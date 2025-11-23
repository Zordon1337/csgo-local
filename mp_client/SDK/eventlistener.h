#pragma once
#include "../globals.h"
#include "networking.h"
#include "../console/console.h"
#include "http.h"
#include "../Logic/CSeasonalOperation.h"

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
		if (strcmp(txt, "cs_win_panel_match") == 0) {
			int newxp = CMatchmaking::handleEndGame(kills, assists, wonRounds, lostRounds, V::STEAM_ID, G::g_GlobalVars->currentTime);
			V::iXP += newxp;
			while (V::iXP >= 5000) {
				V::iXP -= 5000;
				V::iLevel++;
			}

			CSeasonalOperation::OnWinPanel(wonRounds > lostRounds);
			kills = 0;
			assists = 0;
			wonRounds = 0;
			lostRounds = 0;

			V::SaveConfig();
			http::SendUserProfileToServer();
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
			std::cout << "mvp for userid: " << userid << " local idx: " << idx << "mvp status: " << it.bHasStattrack << std::endl;
			if (it.bHasStattrack && idx == userid) {
				it.flStattrack++;
				pEvent->SetInt("musickitmvps", it.flStattrack);
				return;
			}
			else {
				if (idx == userid)
				{

					CSeasonalOperation::OnMvp();
					return;
				}
			}
			PlayerInfo plr;
			if (G::g_EngineClient->GetPlayerInfo(userid, &plr)) {
				if (plr.fakeplayer) return;
				if (plr.iSteamID < 0) return;
				auto& remoteInv = CInventory::GetRemoteInventoryPtr(plr.iSteamID);
				if (remoteInv.equips.size() <= 0) return;
				auto& musicKit = remoteInv.getEquipPtr(54, 0);
				if (musicKit.bHasStattrack) {
					musicKit.flStattrack++;
					pEvent->SetInt("musickitmvps", musicKit.flStattrack);
				}
			}
		}
		else if (strcmp(txt, "round_end") == 0) {
			auto it = CInventory::GetItemPtr(0, 54, CInventory::GetCurrentMusicKit());
			http::SendEquipToServer(it.iItemId, 0, 54, it);
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
			std::vector<int> pendingUpdate = {};
			for (int i = 0; i < CInventory::remoteInventories.size(); i++) {
				pendingUpdate.push_back(CInventory::remoteInventories[i].steamID);
			}
			CInventory::remoteInventories.clear();
			CInventory::CRemoteInventory inv;
			for (auto u : pendingUpdate) {
				inv = http::getRemoteInventory(u);
				if (inv.equips.size() <= 0) continue;
				for (int i = 0; i < inv.equips.size(); i++) {
					console::log(std::format("got remote equip: slotid {} teamid {} itemid {}", inv.equips[i].slotId, inv.equips[i].teamId, inv.equips[i].item.iItemId).c_str());
				}
				inv.steamID = u;

				CInventory::remoteInventories.push_back(inv);

				CMsgGCCStrike15_v2_ClientRequestPlayersProfile msg;
				msg.account_id().set(u);
				auto packet = msg.serialize();

				void* ptr = malloc(packet.size() + 8);

				if (!ptr)
					break;

				((uint32_t*)ptr)[0] = 9127 | ((DWORD)1 << 31);
				((uint32_t*)ptr)[1] = 0;

				memcpy((void*)((DWORD)ptr + 8), (void*)packet.data(), packet.size());
				bool result = G::g_GameCoordinator->SendMsg(9127 | ((DWORD)1 << 31), ptr, packet.size() + 8) == k_EGCResultOK;
				free(ptr);

				auto response = http::Get(std::wstring(V::remoteAddr.begin(), V::remoteAddr.end()).c_str(), L"/get_user_profile?userId=" + std::to_wstring(u));
				console::log(response.c_str());
				auto resJson = nlohmann::json::parse(response);

				if (resJson.is_array() && !resJson.empty())
					resJson = resJson[0];
				for (auto& inv : CInventory::remoteInventories) {
					if (inv.steamID == u) {
						inv.lvl = resJson.value("lvl", 1);
						inv.xp = resJson.value("xp", 0);
						inv.mmrank = resJson.value("mmrank", 0);
						inv.mmwins = resJson.value("mmwins", 0);
						inv.wmrank = resJson.value("wmrank", 0);
						inv.wmwins = resJson.value("wmwins", 0);
						break;
					}
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
				CEntity* enemy = G::g_EntityList->GetEntityFromIndex(userid);
				if (!local || local->m_lifeState() != 0 || enemy->m_iTeamNum() == local->m_iTeamNum()) {
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
					weapon->m_nFallbackStatTrak() = wp.flStattrack;
					weapon->OnDataChanged(5);
					weapon->PostDataUpdate(5);
					weapon->OnDataChanged(0);
					weapon->PostDataUpdate(0);
				}
				CSeasonalOperation::OnKill();
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