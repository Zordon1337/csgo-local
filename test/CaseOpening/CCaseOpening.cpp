#pragma once
#include "CCaseOpening.h"
#include "../vars.h"
#include <iostream>
#include <string>
#include <random>

void CCaseOpening::CreateCrates()
{
	CCaseOpening::vCrates.push_back(CCrate{ "CS:GO Weapon Case", 4001, 1203, 1, {CItem{33, 3, 11}, CItem{8, 3, 73}, CItem{39, 3, 98}, CItem{4, 4, 48}, CItem{61, 4, 60}, CItem{60, 4, 60}, CItem{7, 5, 44}, CItem{1, 5, 61}, CItem{9, 6, 51}, CItem{500, 7, -1}, CItem{500, 7, 38}, CItem{500, 7, 59}, CItem{500, 7, 44}, CItem{500, 7, 12}, CItem{500, 7, 42}, CItem{500, 7, 40}, CItem{500, 7, 43}, CItem{500, 7, 72}, CItem{500, 7, 175}, CItem{500, 7, 143}, CItem{500, 7, 5}, CItem{500, 7, 77}, CItem{505, 7, -1}, CItem{505, 7, 38}, CItem{505, 7, 12}, CItem{505, 7, 59}, CItem{505, 7, 44}, CItem{505, 7, 40}, CItem{505, 7, 143}, CItem{505, 7, 42}, CItem{505, 7, 43}, CItem{505, 7, 5}, CItem{505, 7, 72}, CItem{505, 7, 175}, CItem{505, 7, 77}, CItem{506, 7, -1}, CItem{506, 7, 38}, CItem{506, 7, 12}, CItem{506, 7, 44}, CItem{506, 7, 59}, CItem{506, 7, 43}, CItem{506, 7, 42}, CItem{506, 7, 5}, CItem{506, 7, 143}, CItem{506, 7, 40}, CItem{506, 7, 72}, CItem{506, 7, 175}, CItem{506, 7, 77}, CItem{507, 7, -1}, CItem{507, 7, 38}, CItem{507, 7, 59}, CItem{507, 7, 12}, CItem{507, 7, 44}, CItem{507, 7, 42}, CItem{507, 7, 40}, CItem{507, 7, 77}, CItem{507, 7, 43}, CItem{507, 7, 72}, CItem{507, 7, 143}, CItem{507, 7, 5}, CItem{507, 7, 175}, CItem{508, 7, -1}, CItem{508, 7, 38}, CItem{508, 7, 59}, CItem{508, 7, 12}, CItem{508, 7, 44}, CItem{508, 7, 42}, CItem{508, 7, 40}, CItem{508, 7, 43}, CItem{508, 7, 175}, CItem{508, 7, 143}, CItem{508, 7, 5}, CItem{508, 7, 77}, CItem{508, 7, 72}, } });
	CCaseOpening::vCrates.push_back(CCrate{ "eSports 2013 Case", 4002, 1204, 1, {CItem{16, 3, 176}, CItem{27, 3, 177}, CItem{10, 3, 178}, CItem{13, 4, 83}, CItem{29, 4, 83}, CItem{36, 4, 162}, CItem{7, 5, 14}, CItem{9, 5, 174}, CItem{19, 6, 156}, CItem{500, 7, -1}, CItem{500, 7, 38}, CItem{500, 7, 59}, CItem{500, 7, 44}, CItem{500, 7, 12}, CItem{500, 7, 42}, CItem{500, 7, 40}, CItem{500, 7, 43}, CItem{500, 7, 72}, CItem{500, 7, 175}, CItem{500, 7, 143}, CItem{500, 7, 5}, CItem{500, 7, 77}, CItem{505, 7, -1}, CItem{505, 7, 38}, CItem{505, 7, 12}, CItem{505, 7, 59}, CItem{505, 7, 44}, CItem{505, 7, 40}, CItem{505, 7, 143}, CItem{505, 7, 42}, CItem{505, 7, 43}, CItem{505, 7, 5}, CItem{505, 7, 72}, CItem{505, 7, 175}, CItem{505, 7, 77}, CItem{506, 7, -1}, CItem{506, 7, 38}, CItem{506, 7, 12}, CItem{506, 7, 44}, CItem{506, 7, 59}, CItem{506, 7, 43}, CItem{506, 7, 42}, CItem{506, 7, 5}, CItem{506, 7, 143}, CItem{506, 7, 40}, CItem{506, 7, 72}, CItem{506, 7, 175}, CItem{506, 7, 77}, CItem{507, 7, -1}, CItem{507, 7, 38}, CItem{507, 7, 59}, CItem{507, 7, 12}, CItem{507, 7, 44}, CItem{507, 7, 42}, CItem{507, 7, 40}, CItem{507, 7, 77}, CItem{507, 7, 43}, CItem{507, 7, 72}, CItem{507, 7, 143}, CItem{507, 7, 5}, CItem{507, 7, 175}, CItem{508, 7, -1}, CItem{508, 7, 38}, CItem{508, 7, 59}, CItem{508, 7, 12}, CItem{508, 7, 44}, CItem{508, 7, 42}, CItem{508, 7, 40}, CItem{508, 7, 43}, CItem{508, 7, 175}, CItem{508, 7, 143}, CItem{508, 7, 5}, CItem{508, 7, 77}, CItem{508, 7, 72}, } });
	CCaseOpening::vCrates.push_back(CCrate{ "CS:GO Weapon Case 2", 4004, 1203, 1, {CItem{30, 3, 216}, CItem{60, 3, 217}, CItem{10, 3, 218}, CItem{36, 3, 219}, CItem{38, 3, 232}, CItem{3, 4, 44}, CItem{34, 4, 61}, CItem{35, 4, 214}, CItem{2, 4, 220}, CItem{19, 5, 67}, CItem{61, 5, 221}, CItem{40, 6, 222}, CItem{500, 7, -1}, CItem{500, 7, 38}, CItem{500, 7, 59}, CItem{500, 7, 44}, CItem{500, 7, 12}, CItem{500, 7, 42}, CItem{500, 7, 40}, CItem{500, 7, 43}, CItem{500, 7, 72}, CItem{500, 7, 175}, CItem{500, 7, 143}, CItem{500, 7, 5}, CItem{500, 7, 77}, CItem{505, 7, -1}, CItem{505, 7, 38}, CItem{505, 7, 12}, CItem{505, 7, 59}, CItem{505, 7, 44}, CItem{505, 7, 40}, CItem{505, 7, 143}, CItem{505, 7, 42}, CItem{505, 7, 43}, CItem{505, 7, 5}, CItem{505, 7, 72}, CItem{505, 7, 175}, CItem{505, 7, 77}, CItem{506, 7, -1}, CItem{506, 7, 38}, CItem{506, 7, 12}, CItem{506, 7, 44}, CItem{506, 7, 59}, CItem{506, 7, 43}, CItem{506, 7, 42}, CItem{506, 7, 5}, CItem{506, 7, 143}, CItem{506, 7, 40}, CItem{506, 7, 72}, CItem{506, 7, 175}, CItem{506, 7, 77}, CItem{507, 7, -1}, CItem{507, 7, 38}, CItem{507, 7, 59}, CItem{507, 7, 12}, CItem{507, 7, 44}, CItem{507, 7, 42}, CItem{507, 7, 40}, CItem{507, 7, 77}, CItem{507, 7, 43}, CItem{507, 7, 72}, CItem{507, 7, 143}, CItem{507, 7, 5}, CItem{507, 7, 175}, CItem{508, 7, -1}, CItem{508, 7, 38}, CItem{508, 7, 59}, CItem{508, 7, 12}, CItem{508, 7, 44}, CItem{508, 7, 42}, CItem{508, 7, 40}, CItem{508, 7, 43}, CItem{508, 7, 175}, CItem{508, 7, 143}, CItem{508, 7, 5}, CItem{508, 7, 77}, CItem{508, 7, 72}, } });
	CCaseOpening::vCrates.push_back(CCrate{ "Operation Bravo Case", 4003, 1203, 1, {CItem{39, 3, 186}, CItem{2, 3, 190}, CItem{35, 3, 191}, CItem{13, 3, 192}, CItem{24, 3, 193}, CItem{11, 3, 195}, CItem{61, 4, 183}, CItem{16, 4, 187}, CItem{17, 4, 188}, CItem{60, 4, 189}, CItem{19, 5, 182}, CItem{32, 5, 211}, CItem{9, 5, 212}, CItem{7, 6, 180}, CItem{1, 6, 185}, CItem{500, 7, -1}, CItem{500, 7, 38}, CItem{500, 7, 59}, CItem{500, 7, 44}, CItem{500, 7, 12}, CItem{500, 7, 42}, CItem{500, 7, 40}, CItem{500, 7, 43}, CItem{500, 7, 72}, CItem{500, 7, 175}, CItem{500, 7, 143}, CItem{500, 7, 5}, CItem{500, 7, 77}, CItem{505, 7, -1}, CItem{505, 7, 38}, CItem{505, 7, 12}, CItem{505, 7, 59}, CItem{505, 7, 44}, CItem{505, 7, 40}, CItem{505, 7, 143}, CItem{505, 7, 42}, CItem{505, 7, 43}, CItem{505, 7, 5}, CItem{505, 7, 72}, CItem{505, 7, 175}, CItem{505, 7, 77}, CItem{506, 7, -1}, CItem{506, 7, 38}, CItem{506, 7, 12}, CItem{506, 7, 44}, CItem{506, 7, 59}, CItem{506, 7, 43}, CItem{506, 7, 42}, CItem{506, 7, 5}, CItem{506, 7, 143}, CItem{506, 7, 40}, CItem{506, 7, 72}, CItem{506, 7, 175}, CItem{506, 7, 77}, CItem{507, 7, -1}, CItem{507, 7, 38}, CItem{507, 7, 59}, CItem{507, 7, 12}, CItem{507, 7, 44}, CItem{507, 7, 42}, CItem{507, 7, 40}, CItem{507, 7, 77}, CItem{507, 7, 43}, CItem{507, 7, 72}, CItem{507, 7, 143}, CItem{507, 7, 5}, CItem{507, 7, 175}, CItem{508, 7, -1}, CItem{508, 7, 38}, CItem{508, 7, 59}, CItem{508, 7, 12}, CItem{508, 7, 44}, CItem{508, 7, 42}, CItem{508, 7, 40}, CItem{508, 7, 43}, CItem{508, 7, 175}, CItem{508, 7, 143}, CItem{508, 7, 5}, CItem{508, 7, 77}, CItem{508, 7, 72}, } });
	CCaseOpening::vCrates.push_back(CCrate{ "eSports 2013 Winter Case", 4005, 1204, 1, {CItem{13, 3, 216}, CItem{3, 3, 223}, CItem{26, 3, 224}, CItem{35, 3, 225}, CItem{11, 3, 229}, CItem{36, 3, 230}, CItem{7, 4, 226}, CItem{19, 4, 228}, CItem{10, 5, 154}, CItem{9, 5, 227}, CItem{1, 5, 231}, CItem{16, 6, 215}, CItem{500, 7, -1}, CItem{500, 7, 38}, CItem{500, 7, 59}, CItem{500, 7, 44}, CItem{500, 7, 12}, CItem{500, 7, 42}, CItem{500, 7, 40}, CItem{500, 7, 43}, CItem{500, 7, 72}, CItem{500, 7, 175}, CItem{500, 7, 143}, CItem{500, 7, 5}, CItem{500, 7, 77}, CItem{505, 7, -1}, CItem{505, 7, 38}, CItem{505, 7, 12}, CItem{505, 7, 59}, CItem{505, 7, 44}, CItem{505, 7, 40}, CItem{505, 7, 143}, CItem{505, 7, 42}, CItem{505, 7, 43}, CItem{505, 7, 5}, CItem{505, 7, 72}, CItem{505, 7, 175}, CItem{505, 7, 77}, CItem{506, 7, -1}, CItem{506, 7, 38}, CItem{506, 7, 12}, CItem{506, 7, 44}, CItem{506, 7, 59}, CItem{506, 7, 43}, CItem{506, 7, 42}, CItem{506, 7, 5}, CItem{506, 7, 143}, CItem{506, 7, 40}, CItem{506, 7, 72}, CItem{506, 7, 175}, CItem{506, 7, 77}, CItem{507, 7, -1}, CItem{507, 7, 38}, CItem{507, 7, 59}, CItem{507, 7, 12}, CItem{507, 7, 44}, CItem{507, 7, 42}, CItem{507, 7, 40}, CItem{507, 7, 77}, CItem{507, 7, 43}, CItem{507, 7, 72}, CItem{507, 7, 143}, CItem{507, 7, 5}, CItem{507, 7, 175}, CItem{508, 7, -1}, CItem{508, 7, 38}, CItem{508, 7, 59}, CItem{508, 7, 12}, CItem{508, 7, 44}, CItem{508, 7, 42}, CItem{508, 7, 40}, CItem{508, 7, 43}, CItem{508, 7, 175}, CItem{508, 7, 143}, CItem{508, 7, 5}, CItem{508, 7, 77}, CItem{508, 7, 72}, } });
}

CItem CCaseOpening::GetRandomItem(int iCaseIdx)
{
	CCrate cCase;
	for (auto i : CCaseOpening::vCrates) {
		if (i.iDefIdx == iCaseIdx) {
			cCase = i;
			break;
		}
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 10000);
	std::uniform_int_distribution<> distId(0, INT_MAX);
	std::uniform_int_distribution<> dist3(0, 10);
	std::uniform_real_distribution<float> distWear(0.f, 1.f);
	std::uniform_int_distribution<> distPattern(0, 1000);

	int iRarity = dist(gen);

	if (iRarity < 7992)               // 79.92%
		iRarity = ITEM_RARITY_RARE;       // Mil-Spec (blue)
	else if (iRarity < 9590)          // +15.98%
		iRarity = ITEM_RARITY_MYTHICAL;   // Restricted (purple)
	else if (iRarity < 9910)          // +3.20%
		iRarity = ITEM_RARITY_LEGENDARY;  // Classified (pink)
	else if (iRarity < 9974)          // +0.64%
		iRarity = ITEM_RARITY_ANCIENT;    // Covert (red)
	else                           // ~0.26%
		iRarity = ITEM_RARITY_IMMORTAL;   // Exceedingly Rare (gold)

	while (iRarity >= ITEM_RARITY_RARE &&
		!CCaseOpening::DoesCaseHaveThisRarity(iCaseIdx, iRarity)) {
		iRarity--;
	}
	std::vector<CItem> vPossibleItems;
	for (auto item : cCase.vItems) {
		if (item.iRarity == iRarity) vPossibleItems.push_back(item);
	}

	if (vPossibleItems.size() <= 0) return CItem{ 0,0,0,false,0 };
	std::uniform_int_distribution<> dist2(0, vPossibleItems.size() - 1);
	int iItemIdx = dist2(gen);
	bool isStatTrak = dist3(gen) == 1; 
	if (isStatTrak) {
		vPossibleItems[iItemIdx].bHasStattrack = true;
		vPossibleItems[iItemIdx].flStattrack = 0;
	}
	else {
		vPossibleItems[iItemIdx].bHasStattrack = false;
		vPossibleItems[iItemIdx].flStattrack = 0;
	}
	vPossibleItems[iItemIdx].iItemId = distId(gen);
	vPossibleItems[iItemIdx].flWear = distWear(gen);
	vPossibleItems[iItemIdx].iPattern = distPattern(gen);

	if (iRarity == 7) {
		vPossibleItems[iItemIdx].iQuality = (ITEM_QUALITY_UNUSUAL);
		vPossibleItems[iItemIdx].iRarity = (ITEM_RARITY_ANCIENT);
	}
	else {
		vPossibleItems[iItemIdx].iQuality = 0;
	}
	return vPossibleItems[iItemIdx];
}

bool CCaseOpening::DoesCaseHaveThisRarity(int iCaseIdx, int iRarity)
{
	for (auto& cCase : CCaseOpening::vCrates) {
		if (cCase.iDefIdx == iCaseIdx) {
			for (auto& item : cCase.vItems) {
				if (item.iRarity == iRarity) return true;
			}
		}
	}
	return false;
}

int CCaseOpening::GetAmountOfCases(int iCaseIdx)
{
	int i = 0;
	for (auto& cCase : V::cases) {
		if (cCase.iDefIdx == iCaseIdx) {
			i++;
		}
	}
	return i;
}
