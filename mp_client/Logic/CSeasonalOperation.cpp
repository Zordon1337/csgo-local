#include "CSeasonalOperation.h"
#include "CInventory.h"
#include "../vars.h"
#include <ctime>
#include <iostream>


void CSeasonalOperation::Init(int ver[3])
{
	vSeasons.push_back({ 0, {25, 4, 2013}, {31,8,2013} }); // Payback
	vSeasons.push_back({ 1, {19, 9, 2013}, {5,2,2014} }); // Bravo
	vSeasons.push_back({ 2, {20, 2, 2014}, {11,6,2014} }); // Phoenix
	vSeasons.push_back({ 3, {1, 7, 2014}, {2,10,2014} }); // Breakout
	vSeasons.push_back({ 4, {11, 11, 2014}, {8,2,2015} }); // Vanguard
	vSeasons.push_back({ 5, {26, 5, 2015}, {1,10,2015} }); // Bloodhound
	vSeasons.push_back({ 6, {17, 2, 2016}, {15,7,2016} }); // Wildfire
	vSeasons.push_back({ 7, {23, 5, 2017}, {13,11,2017} }); // Hydra
	// TODO: Shattered web, broken fang & riptide
	
	// generate timestamp
	std::tm currTime{};
	currTime.tm_year = ver[2] - 1900;
	currTime.tm_mon = ver[1] - 1; // why months begins from 0
	currTime.tm_mday = ver[0];
	std::time_t currTimestamp = std::mktime(&currTime);
	for (const auto& season : vSeasons) {
		std::tm startTime{};

		startTime.tm_year = season.beginDate[2] - 1900;
		startTime.tm_mon = season.beginDate[1] - 1; // why months begins from 0
		startTime.tm_mday = season.beginDate[0];
		std::time_t startTimestamp = std::mktime(&startTime);
		std::tm endTime{};
		endTime.tm_year = season.endDate[2] - 1900;
		endTime.tm_mon = season.endDate[1] - 1; // why months begins from 0
		endTime.tm_mday = season.endDate[0];
		std::time_t endTimestamp = std::mktime(&endTime);

		if (currTimestamp >= startTimestamp && currTimestamp <= endTimestamp) {
			iCurrSeason = season.iId;
			currSeason = season;
		}
	}
	UpdateMedals();


}

void CSeasonalOperation::UpdateMedals()
{
	if (iCurrSeason == -1) return;
	enum EOperationMedals {
		PaybackBronze = 1001,
		PaybackSilver,
		PaybackGold,
		BravoBronze = 1013,
		BravoSilver,
		BravoGold,
		PhoenixBronze = 1024,
		PhoenixSilver,
		PhoenixGold,
		BreakoutBronze = 1028,
		BreakoutSilver,
		BreakoutGold,
		VanguardBronze = 1316,
		VanguardSilver,
		VanguardGold,
		BloodhoundBronze = 1327,
		BloodhoundSilver,
		BloodhoundGold,
		WildfireBronze = 1336,
		WildfireSilver,
		WildfireGold,
		HydraBronze = 4353,
		HydraSilver,
		HydraGold,
		HydraDiamond,
	};

	EOperationMedals operation = EOperationMedals::PaybackBronze;
	auto operations = new EOperationMedals[]{ EOperationMedals::PaybackBronze, EOperationMedals::BravoBronze, EOperationMedals::PhoenixBronze, EOperationMedals::BreakoutBronze, EOperationMedals::VanguardBronze, EOperationMedals::BloodhoundBronze, EOperationMedals::WildfireBronze, EOperationMedals::HydraBronze, };
	operation = operations[iCurrSeason];

	if (operation > 1000) {
		int maxCoinLevel = 3;
		if (iCurrSeason > 6) // hydra was first operation with diamond coin
			maxCoinLevel = 4;
		int ItemId = -1;
		for (const auto& item : V::items) {
			for (int i = 0; i < maxCoinLevel; i++) {
				if (item.iDefIdx == operation + i) {
					ItemId = item.iItemId;
				}
			}
		}
		if (ItemId != -1) {
			auto& medal = CInventory::GetItemByIdPtr(ItemId);
			auto competetiveWins = 0;
			auto competetiveKills = 0;

			for (const auto& attr : medal.vAttributes) {
				if (attr.iId == 97) competetiveKills = attr.iValue;
				if (attr.iId == 103) competetiveWins = attr.iValue;
			}

			auto medalStage = medal.iDefIdx - operation;

			if (medalStage == 0 && competetiveWins >= 10 && competetiveKills >= 100) {
				medal.iDefIdx++; // Silver
			}
			else if (medalStage == 1 && competetiveWins >= 25 && competetiveKills >= 200) {
				medal.iDefIdx++; // Gold
			}
			else if ((medalStage == 2 && maxCoinLevel == 4) && competetiveWins >= 40 && competetiveKills >= 300) {
				medal.iDefIdx++; // Diamond
			}
		}
		else {
			CItem medal;
			medal.iDefIdx = operation;
			medal.iFlag = 0;
			medal.iItemId = CInventory::GetNextItemId();
			medal.vAttributes.push_back({ 171, 0 }); // Missions done, unfortunately i am unable to implement the missions so this is just placeholder
			std::tm currTime{};currTime.tm_year = currSeason.beginDate[2] - 1900;currTime.tm_mon = currSeason.beginDate[1] - 1;currTime.tm_mday = currSeason.beginDate[0];std::time_t currTimestamp = std::mktime(&currTime);
			medal.vAttributes.push_back({ 180,  (uint32_t)currTimestamp }); // Deployment date (in timestamp)
			medal.vAttributes.push_back({ 172, 0 }); // Kills on maps from operation
			medal.vAttributes.push_back({ 97, 0 }); // Competetive Kills
			medal.vAttributes.push_back({ 98, 0 }); // Competetive 3k
			medal.vAttributes.push_back({ 99, 0 }); // Competetive 4k
			medal.vAttributes.push_back({ 101, 0 }); // Competetive 5k/Ace
			medal.vAttributes.push_back({ 103, 0 }); // Competetive Wins
			medal.vAttributes.push_back({ 104, 0 }); // Competetive Mvps
			medal.bHasStattrack = false;
			medal.flStattrack = false;
			medal.iRarity = 6;
			medal.iQuality = 4;
			medal.flPaintKit = 0;
			V::items.push_back(medal);
		}
	}
}

std::string CSeasonalOperation::GetOperationName()
{
	if (iCurrSeason == -1) return "None";
	std::string operationNames[] = { "Operation Payback", "Operation Bravo", "Operation Phoenix", "Operation Breakout", "Operation Vanguard", "Operation Bloodhound", "Operation Wildfire", "Operation Hydra", "Operation Shattered Web", "Operation Broken Fang", "Operation Riptide" };
	return operationNames[iCurrSeason];
}

CItem& CSeasonalOperation::GetCurrentCoin()
{

	auto it = CItem{ 0 };
	if (CSeasonalOperation::iCurrSeason == -1) return it;
	enum EOperationMedals {
		PaybackBronze = 1001,
		PaybackSilver,
		PaybackGold,
		BravoBronze = 1013,
		BravoSilver,
		BravoGold,
		PhoenixBronze = 1024,
		PhoenixSilver,
		PhoenixGold,
		BreakoutBronze = 1028,
		BreakoutSilver,
		BreakoutGold,
		VanguardBronze = 1316,
		VanguardSilver,
		VanguardGold,
		BloodhoundBronze = 1327,
		BloodhoundSilver,
		BloodhoundGold,
		WildfireBronze = 1336,
		WildfireSilver,
		WildfireGold,
		HydraBronze = 4353,
		HydraSilver,
		HydraGold,
		HydraDiamond,
	};
	EOperationMedals operation = EOperationMedals::PaybackBronze;
	auto operations = new EOperationMedals[]{ EOperationMedals::PaybackBronze, EOperationMedals::BravoBronze, EOperationMedals::PhoenixBronze, EOperationMedals::BreakoutBronze, EOperationMedals::VanguardBronze, EOperationMedals::BloodhoundBronze, EOperationMedals::WildfireBronze, EOperationMedals::HydraBronze, };
	operation = operations[CSeasonalOperation::iCurrSeason];
	if (operation) {
		int maxCoinLevel = 3;
		if (CSeasonalOperation::iCurrSeason > 6) // hydra was first operation with diamond coin
			maxCoinLevel = 4;
		int ItemId = -1;
		for (const auto& item : V::items) {
			for (int i = 0; i < maxCoinLevel; i++) {
				if (item.iDefIdx == operation + i) {
					ItemId = item.iItemId;

					return CInventory::GetItemByIdPtr(ItemId); // ngl shit workaround
				}
			}
		}
		if (ItemId == -1) {
			CItem medal{};
			medal.iDefIdx = operation;
			medal.iItemId = CInventory::GetNextItemId(); // i highly doubt that someone will actually reach this high id normally
			medal.vAttributes.push_back({ 171, 0 }); // Missions done, unfortunately i am unable to implement the missions so this is just placeholder
			std::tm currTime{}; currTime.tm_year = CSeasonalOperation::currSeason.beginDate[2] - 1900; currTime.tm_mon = CSeasonalOperation::currSeason.beginDate[1] - 1; currTime.tm_mday = CSeasonalOperation::currSeason.beginDate[0]; std::time_t currTimestamp = std::mktime(&currTime);
			medal.vAttributes.push_back({ 180,  (uint32_t)currTimestamp }); // Deployment date (in timestamp)
			medal.vAttributes.push_back({ 172, 0 }); // Kills on maps from operation
			medal.vAttributes.push_back({ 97, 0 }); // Competetive Kills
			medal.vAttributes.push_back({ 98, 0 }); // Competetive 3k
			medal.vAttributes.push_back({ 99, 0 }); // Competetive 4k
			medal.vAttributes.push_back({ 101, 0 }); // Competetive 5k/Ace
			medal.vAttributes.push_back({ 103, 0 }); // Competetive Wins
			medal.vAttributes.push_back({ 104, 0 }); // Competetive Mvps
			medal.bHasStattrack = false;
			medal.flStattrack = false;
			medal.iRarity = 6;
			medal.iQuality = 4;
			V::items.push_back(medal);
			return V::items.back();
		}
	}
	return it;
}

int _RoundKills = 0;
int _Kills = 0;
int mvps = 0;

int triplekills = 0;
int fourkills = 0;
int fivekills = 0;

void CSeasonalOperation::OnRoundEnd()
{
	if (_RoundKills == 5) {
		fivekills++;
	}
	else if (_RoundKills >= 4) {
		fourkills++;
	}
	else if (_RoundKills == 3) {
		triplekills++;
	}
	_RoundKills = 0;
}

void CSeasonalOperation::OnKill()
{
	_RoundKills++;
	_Kills++;
}

void CSeasonalOperation::OnDeath()
{
}

void CSeasonalOperation::OnWinPanel(bool bWon)
{
	auto& medal = GetCurrentCoin();
	if (medal.iItemId == 0) return; // not in season currently

	for (auto& attr : medal.vAttributes) {
		auto val = attr.iValue;
		switch (attr.iId) {
			case 172: {
				// TODO: Check if map is operation map, if yes increment by kills made
				break;
			}
			case 97: {
				attr.iValue += _Kills;
				break;
			}
			case 98: {
				attr.iValue += triplekills;
				break;
			}
			case 99: {
				attr.iValue += fourkills;
				break;
			}
			case 101: {
				attr.iValue += fivekills;
				break;
			}
			case 106: {
				attr.iValue += bWon; // bWon is technically 0 when not won, so i guess it will work?
				break;
			}
			case 177: {
				attr.iValue += mvps;
				break;
			}
		}
	}
	mvps = 0;
	fourkills = 0;
	fivekills = 0;
	_Kills = 0;
	triplekills = 0;
	_RoundKills = 0;
	UpdateMedals();
}

void CSeasonalOperation::OnMvp()
{
	mvps++;
}
