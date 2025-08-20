#pragma once
#include <fstream>
#include <string>
#include <vector>

namespace V {
    int iServiceMedalLevel = 1;
    int iLevel = 40;
    int iXP = 4755;
    std::vector<int> othermedals{ 970 }; // Now dynamic
    int STEAM_ID = 0;
    bool PENDING_UPDATE = false;

    void SaveConfig() {
        std::ofstream ofs("config.txt");
        if (!ofs) return;

        ofs << iServiceMedalLevel << '\n';
        ofs << iLevel << '\n';
        ofs << iXP << '\n';
        ofs << othermedals.size() << '\n';
        for (size_t i = 0; i < othermedals.size(); ++i)
            ofs << othermedals[i] << (i < othermedals.size() - 1 ? ' ' : '\n');
    }

    void LoadConfig() {
        std::ifstream ifs("config.txt");
        if (!ifs) return;

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
