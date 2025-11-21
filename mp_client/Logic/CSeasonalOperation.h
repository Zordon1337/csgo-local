#pragma once
#include <vector>
#include <string>
// just to note here, im using DD/MM/YYYY for date
struct CSeason {
	int iId;
	int beginDate[3];
	int endDate[3];
};
struct CItem;
namespace CSeasonalOperation {
	inline std::vector<CSeason> vSeasons = {};
	inline int iCurrSeason = -1;
	inline CSeason currSeason; // very confusing name but no idea for another
	void Init(int ver[3]);
	void UpdateMedals();
	std::string GetOperationName();
	CItem& GetCurrentCoin();
	void OnRoundEnd();
	void OnKill();
	void OnDeath();
	void OnWinPanel(bool bWon);
	void OnMvp();
}