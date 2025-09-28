#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "protos/Messages.h"
#include "CaseOpening/CCaseOpening.h"
#include "json/json.hpp"
namespace V {

    float flXpMultipler = 2.f;
    int iServiceMedalLevel = 0;
    int iLevel = 1;
    int iXP = 0;
    std::vector<int> othermedals{ };
    std::vector<CItem> items{ };
    std::vector<CCrate> cases{ };
    int STEAM_ID = 0;
    bool PENDING_UPDATE = false;
    int iCaseResult = 0; // temp solution , i need to find proper one ETA: ages

    void SaveConfig() {
		system("@echo off");
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
            j["items"].push_back({
                {"iDefIdx", item.iDefIdx},
                {"iRarity", item.iRarity},
                {"flPaintKit", item.flPaintKit},
                {"bHasStattrack", item.bHasStattrack},
                {"flStattrack", item.flStattrack},
                {"iItemId", item.iItemId},
                {"flWear", item.flWear},
                {"iPattern", item.iPattern},
                {"iQuality", item.iQuality}
                });
        }
		j["cases"] = nlohmann::json::array();
		for (const auto& crate : cases) {
			nlohmann::json jcrate;
			jcrate["szCaseName"] = crate.szCaseName;
			jcrate["iDefIdx"] = crate.iDefIdx;
			jcrate["iKeyIdx"] = crate.iKeyIdx;
			jcrate["iRarity"] = crate.iRarity;
			jcrate["vItems"] = nlohmann::json::array();
			for (const auto& item : crate.vItems) {
				jcrate["vItems"].push_back({
					{"iDefIdx", item.iDefIdx},
					{"iRarity", item.iRarity},
					{"flPaintKit", item.flPaintKit},
					{"bHasStattrack", item.bHasStattrack},
					{"flStattrack", item.flStattrack},
					{"iItemId", item.iItemId},
					{"flWear", item.flWear},
					{"iPattern", item.iPattern},
					{"iQuality", item.iQuality}
					});
			}
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

		nlohmann::json j;
		ifs >> j;
		iServiceMedalLevel = j.value("iServiceMedalLevel", 0);
		iLevel = j.value("iLevel", 1);
		iXP = j.value("iXP", 0);
		othermedals = j.value("othermedals", std::vector<int>{});
		items.clear();
		for (const auto& item : j["items"]) {
			items.push_back(CItem{
				item.value("iDefIdx", 0),
				item.value("iRarity", 0),
				item.value("flPaintKit", 0.f),
				item.value("bHasStattrack", false),
				item.value("flStattrack", 0.f),
				item.value("iItemId", 0),
				item.value("flWear", 0.f),
				item.value("iPattern", 0),
				item.value("iQuality", 0)
				});
		}
		cases.clear();
		for (const auto& crate : j["cases"]) {
			CCrate c;
			std::string tmp;
			tmp = crate.value("szCaseName", "");
			c.szCaseName = tmp.c_str();
			c.iDefIdx = crate.value("iDefIdx", 0);
			c.iKeyIdx = crate.value("iKeyIdx", 0);
			c.iRarity = crate.value("iRarity", 0);
			for (const auto& item : crate["vItems"]) {
				c.vItems.push_back(CItem{
					item.value("iDefIdx", 0),
					item.value("iRarity", 0),
					item.value("flPaintKit", 0.f),
					item.value("bHasStattrack", false),
					item.value("flStattrack", 0.f),
					item.value("iItemId", 0),
					item.value("flWear", 0.f),
					item.value("iPattern", 0),
					item.value("iQuality", 0)
					});
			}
			cases.push_back(c);
		}
		
    }
}
