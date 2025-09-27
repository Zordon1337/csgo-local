#pragma once
#include <vector>

struct CItem {
	int iDefIdx;
	int iRarity;
	float flPaintKit;
	bool bHasStattrack;
	float flStattrack;
};
struct CCrate {
	const char* szCaseName;
	int iDefIdx;
	int iKeyIdx;
	int iRarity;
	std::vector<CItem> vItems;
};
enum eItemQuality {
	ITEM_QUALITY_DEFAULT,
	ITEM_QUALITY_GENUINE,
	ITEM_QUALITY_VINTAGE,
	ITEM_QUALITY_UNUSUAL,
	ITEM_QUALITY_SKIN,
	ITEM_QUALITY_COMMUNITY,
	ITEM_QUALITY_DEVELOPER,
	ITEM_QUALITY_SELFMADE,
	ITEM_QUALITY_CUSTOMIZED,
	ITEM_QUALITY_STRANGE,
	ITEM_QUALITY_COMPLETED,
	ITEM_QUALITY_SKIN_STRANGE,
	ITEM_QUALITY_TOURNAMENT
};

enum eItemRarity {
	ITEM_RARITY_DEFAULT,
	ITEM_RARITY_COMMON,
	ITEM_RARITY_UNCOMMON,
	ITEM_RARITY_RARE,
	ITEM_RARITY_MYTHICAL,
	ITEM_RARITY_LEGENDARY,
	ITEM_RARITY_ANCIENT,
	ITEM_RARITY_IMMORTAL
};


namespace CCaseOpening {
	std::vector<CCrate> vCrates;
	void CreateCrates();
	CItem GetRandomItem(int iCaseIdx);
	bool DoesCaseHaveThisRarity(int iCaseIdx, int iRarity);
}