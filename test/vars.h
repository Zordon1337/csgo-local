#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "protos/Messages.h"
#include "CaseOpening/CCaseOpening.h"
#include "CaseOpening/CInventory.h"
#include "json/json.hpp"
#include "sdk/hash.h"
#include <random>
#include "sdk/recv.h"
namespace V {
	inline std::unordered_map<uint32_t, std::uint32_t> netvars = { };
#define NETVAR(name, var, ...) \
	inline std::add_lvalue_reference_t<__VA_ARGS__> name() noexcept \
	{ \
		static const std::uint32_t offset = V::netvars[hash::CompileTime(var)]; \
		return *reinterpret_cast<std::add_pointer_t<__VA_ARGS__>>(std::uint32_t(this) + offset); \
	} 
    float flXpMultipler = 2.f;
    int iServiceMedalLevel = 0;
    int iLevel = 1;
    int iXP = 0;
    std::vector<int> othermedals{ };
    std::vector<CItem> items{ };
    std::vector<CCrateOwned> cases{ };
    int STEAM_ID = 0;
    bool PENDING_UPDATE = false;
    int iCaseResult = 0; // temp solution , i need to find proper one ETA: ages
	namespace Ranks::Wingman {
		int iCurrentRank = 18;
		int iWins = 344;
		int iLosses = 1;
		int iWinStreak = 0;
		int iLossStreak = 0;
		int iElo = 0; // iWinStreak/iLossStreak * 25
	}
	namespace Ranks::Competetive {
		int iCurrentRank = 17;
		int iWins = 166;
		int iLosses = 1;
		int WinStreak = 2;
		int iLossStreak = 0;
		int iElo = 0;

	}
	RecvVarProxyFn oViewModelProxy;

    void SaveConfig() {
		system("mkdir C:\\CSGO_LOCAL");
        std::ofstream ofs("C:\\CSGO_LOCAL\\config.txt");
        if (!ofs) return;

		nlohmann::json j;
		j["iServiceMedalLevel"] = iServiceMedalLevel;
		j["iLevel"] = iLevel;
		j["iXP"] = iXP;
		j["othermedals"] = othermedals;
		j["items"] = nlohmann::json::array();
        for (const auto& item : items) {

			bool isCtEquipped = CInventory::isEquipped(item.iItemId, 3, CInventory::GetSlotID(item.iDefIdx));
			bool isTEquipped = CInventory::isEquipped(item.iItemId, 2, CInventory::GetSlotID(item.iDefIdx));
			bool is0Equipped = CInventory::isEquipped(item.iItemId, 0, item.iFlag == 4 ? 55 : 54);
            j["items"].push_back({
                {"iDefIdx", item.iDefIdx},
                {"iRarity", item.iRarity},
                {"flPaintKit", item.flPaintKit},
                {"bHasStattrack", item.bHasStattrack},
                {"flStattrack", item.flStattrack},
                {"iItemId", item.iItemId},
                {"flWear", item.flWear},
                {"iPattern", item.iPattern},
                {"iQuality", item.iQuality},
                {"iFlag", item.iFlag},
				{"bIs0Equipped", is0Equipped},
				{"bIsTEquipped", isTEquipped},
				{"bIsCtEquipped", isCtEquipped}
                });
        }
		j["cases"] = nlohmann::json::array();
		for (const auto& crate : cases) {
			nlohmann::json jcrate;
			jcrate["iDefIdx"] = crate.iDefIdx;
			jcrate["iKeyIdx"] = crate.iKeyIdx;
			jcrate["iRarity"] = crate.iRarity;
			jcrate["iOCaseIdx"] = crate.iOCaseIdx;
			jcrate["iOKeyIdx"] = crate.iOKeyIdx;
			jcrate["bIsMusicKitBox"] = crate.bIsMusicKitBox;
			j["cases"].push_back(jcrate);
		}
		
		ofs << j.dump(4);

		ofs.close();
    }

    void LoadConfig() {
        std::ifstream ifs("C:\\CSGO_LOCAL\\config.txt");
        if (!ifs) {
			SaveConfig();
            return;
        }
        try {

			nlohmann::json j;
			ifs >> j;
			iServiceMedalLevel = j.value("iServiceMedalLevel", 0);
			iLevel = j.value("iLevel", 1);
			iXP = j.value("iXP", 0);
			othermedals = j.value("othermedals", std::vector<int>{});
			items.clear();
			for (const auto& item : j["items"]) {
				auto item2 = CItem{
					item.value("iDefIdx", 0),
					item.value("iRarity", 0),
					item.value("flPaintKit", 0.f),
					item.value("bHasStattrack", false),
					item.value("flStattrack", 0.f),
					item.value("iItemId", 0),
					item.value("flWear", 0.f),
					item.value("iPattern", 0),
					item.value("iQuality", 0),
					item.value("iFlag", 0)
				};

				bool isCtEquipped = item.value("bIsCtEquipped", false);
				bool isTEquipped = item.value("bIsTEquipped", false);
				bool is0Equipped = item.value("bIs0Equipped", false);
				if (isCtEquipped) CInventory::EquipSlot(item2.iItemId, 3, CInventory::GetSlotID(item2.iDefIdx));
				if (isTEquipped) CInventory::EquipSlot(item2.iItemId, 2, CInventory::GetSlotID(item2.iDefIdx));
				if (is0Equipped) {
					if (item2.iFlag == 4) {
						CInventory::EquipSlot(item2.iItemId, 0, 55);
					} else if (item2.iFlag == 5) {
						CInventory::EquipSlot(item2.iItemId, 0, 54);
					}
				}
				items.push_back(item2);
			}
			cases.clear();
			for (const auto& crate : j["cases"]) {
				CCrateOwned c;;
				c.iDefIdx = crate.value("iDefIdx", 0);
				c.iKeyIdx = crate.value("iKeyIdx", 0);
				c.iRarity = crate.value("iRarity", 0);
				c.iOCaseIdx = crate.value("iOCaseIdx", 0);
				c.iOKeyIdx = crate.value("iOKeyIdx", 0);
				c.bIsMusicKitBox = crate.value("bIsMusicKitBox", 0);
				for (auto& ca : CCaseOpening::vCrates) {
					if (ca.iDefIdx == c.iDefIdx)
					{
						c.vItems = ca.vItems;

						c.szCaseName = ca.szCaseName;
					}
				}
				cases.push_back(c);
			}
		}
		catch(const char* ex) {
			SaveConfig();
			
		}
		
    }
}
