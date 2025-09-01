#pragma once
#include <fstream>
#include <string>
#include <vector>

namespace V {

    float flXpMultipler = 2.f;
    int iServiceMedalLevel = 0;
    int iLevel = 1;
    int iXP = 0;
    std::vector<int> othermedals{ 970 };
    int STEAM_ID = 0;
    bool PENDING_UPDATE = false;

    void SaveConfig() {
		system("mkdir C:\\CSGO_LOCAL");
        std::ofstream ofs("C:\\CSGO_LOCAL\\config.txt");
        if (!ofs) return;

        ofs << iServiceMedalLevel << '\n';
        ofs << iLevel << '\n';
        ofs << iXP << '\n';
        ofs << othermedals.size() << '\n';
        for (size_t i = 0; i < othermedals.size(); ++i)
            ofs << othermedals[i] << (i < othermedals.size() - 1 ? ' ' : '\n');
    }

    void LoadConfig() {
        std::ifstream ifs("C:\\CSGO_LOCAL\\config.txt");
        if (!ifs) {
			SaveConfig();
            return;
        }

        size_t medalCount = 0;
        ifs >> iServiceMedalLevel;
        ifs >> iLevel;
        ifs >> iXP;
        ifs >> medalCount;
        othermedals.clear();
        for (size_t i = 0; i < medalCount; ++i) {
            int val;
            ifs >> val;
            othermedals.push_back(val);
        }
    }
}
