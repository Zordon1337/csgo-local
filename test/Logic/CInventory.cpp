#include "CInventory.h"
#include "../sdk/networking.h"
int CInventory::GetSlotID(int definition_index)
{
	switch (definition_index)
	{
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_CANIS:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFE_WIDOWMAKER:
		return 0;
	case WEAPON_C4:
		return 1;
	case WEAPON_USP_SILENCER:
	case WEAPON_HKP2000:
	case WEAPON_GLOCK:
		return 2;
	case WEAPON_ELITE:
		return 3;
	case WEAPON_P250:
		return 4;
	case WEAPON_TEC9:
	case WEAPON_CZ75A:
	case WEAPON_FIVESEVEN:
		return 5;
	case WEAPON_DEAGLE:
	case WEAPON_REVOLVER:
		return 6;
	case WEAPON_MP9:
	case WEAPON_MAC10:
		return 8;
	case WEAPON_MP7:
		return 9;
	case WEAPON_UMP45:
		return 10;
	case WEAPON_P90:
		return 11;
	case WEAPON_BIZON:
		return 12;
	case WEAPON_FAMAS:
	case WEAPON_GALILAR:
		return 14;
	case WEAPON_M4A1_SILENCER:
	case WEAPON_M4A1:
	case WEAPON_AK47:
		return 15;
	case WEAPON_SSG08:
		return 16;
	case WEAPON_SG553:
	case WEAPON_AUG:
		return 17;
	case WEAPON_AWP:
		return 18;
	case WEAPON_G3SG1:
	case WEAPON_SCAR20:
		return 19;
	case WEAPON_NOVA:
		return 20;
	case WEAPON_XM1014:
		return 21;
	case WEAPON_SAWEDOFF:
	case WEAPON_MAG7:
		return 22;
	case WEAPON_M249:
		return 23;
	case WEAPON_NEGEV:
		return 24;
	case GLOVE_STUDDED_BLOODHOUND:
	case GLOVE_SPORTY:
	case GLOVE_SLICK:
	case GLOVE_LEATHER_WRAP:
	case GLOVE_MOTORCYCLE:
	case GLOVE_SPECIALIST:
		return 41;
	default:
		return -1;
	}
}
bool CInventory::EquipSlot(long itemId, int teamId, int newSlot)
{
	CInventory::Equips[newSlot][teamId] = itemId;

	return true;
}

bool CInventory::isEquipped(long itemId, int teamId, int newSlot) {
	return CInventory::Equips[newSlot][teamId] == itemId;
}

CItem CInventory::GetItem(int teamId, int slotId, int idx)
{
	auto itemId = CInventory::Equips[slotId][teamId];
	for (const auto item : V::items) {
		if (itemId == item.iItemId && item.iDefIdx == idx) return item;
	}
	return {};
}

CItem& CInventory::GetItemByIdPtr(long itemId)
{
	for (CItem& item : V::items) {
		if (itemId == item.iItemId) return item;
	}
	CItem item = {};
	return item;
}
CItem& CInventory::GetItemPtr(int teamId, int slotId, int idx)
{
	auto itemId = CInventory::Equips[slotId][teamId];
	for (CItem& item : V::items) {
		if (itemId == item.iItemId && item.iDefIdx == idx) return item;
	}
	CItem item = {};
	return item; 
}


int CInventory::GetCurrentMusicKit() {
	auto itemId = CInventory::Equips[54][0];
	for (const auto item : V::items) {
		if (itemId == item.iItemId) return item.iDefIdx;
	}
	return 0;
}

ItemDefinitionIndex CInventory::GetKnifeEquipped(int teamId) {
	auto itemId = CInventory::Equips[0][teamId];
	for (const auto item : V::items) {
		if (itemId == item.iItemId) return (ItemDefinitionIndex)item.iDefIdx;
	}
	return teamId == 2 ? ItemDefinitionIndex::WEAPON_KNIFE_T : ItemDefinitionIndex::WEAPON_KNIFE;
}
