#pragma once
#include <vector>
#include "CCaseOpening.h"
#include <unordered_map>

enum ItemDefinitionIndex : int {
	WEAPON_NONE = 0,
	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SCAR20 = 38,
	WEAPON_SG553,
	WEAPON_SSG08,
	WEAPON_KNIFEGG,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_KNIFE_BAYONET = 500,
	WEAPON_KNIFE_CSS = 503,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT = 506,
	WEAPON_KNIFE_KARAMBIT = 507,
	WEAPON_KNIFE_M9_BAYONET = 508,
	WEAPON_KNIFE_TACTICAL = 509,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY = 515,
	WEAPON_KNIFE_PUSH = 516,
	WEAPON_KNIFE_CORD = 517,
	WEAPON_KNIFE_CANIS = 518,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE = 520,
	WEAPON_KNIFE_OUTDOOR = 521,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER = 523,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
};
namespace CInventory {
	long Equips[255][255];
	// https://github.com/0TheSpy/Seaside/blob/12dd1b6d82a4c1035cde6ebcd3ec5f4f8fe73e1e/SpyCustom/inventory_changer.h#L316C1-L397C2
	int GetSlotID(int definition_index);
	bool EquipSlot(long itemId, int teamId, int newSlot);
	bool isEquipped(long itemId, int teamId, int newSlot);
	CItem GetItem(int teamId, int slotId, int idx);
	int GetCurrentMusicKit();
	ItemDefinitionIndex GetKnifeEquipped(int teamId);

	const std::unordered_map<ItemDefinitionIndex, const char*> knifeModels = {
		{WEAPON_KNIFE_CSS,"models/weapons/v_knife_css.mdl"},
		{ WEAPON_KNIFE_CORD, "models/weapons/v_knife_cord.mdl" },
		{WEAPON_KNIFE_WIDOWMAKER ,"models/weapons/v_knife_widowmaker.mdl"},
		{ WEAPON_KNIFE_GYPSY_JACKKNIFE,"models/weapons/v_knife_gypsy_jackknife.mdl" },
		{ WEAPON_KNIFE_URSUS, "models/weapons/v_knife_ursus.mdl" },
		{ WEAPON_KNIFE_PUSH ,"models/weapons/v_knife_push.mdl" },
		{ WEAPON_KNIFE_SURVIVAL_BOWIE,"models/weapons/v_knife_survival_bowie.mdl" },
		{ WEAPON_KNIFE_BUTTERFLY ,"models/weapons/v_knife_butterfly.mdl" },
		{ WEAPON_KNIFE_FALCHION,"models/weapons/v_knife_falchion_advanced.mdl" },
		{ WEAPON_KNIFE_TACTICAL,"models/weapons/v_knife_tactical.mdl" },
		{ WEAPON_KNIFE_M9_BAYONET,"models/weapons/v_knife_m9_bay.mdl" },
		{ WEAPON_KNIFE_KARAMBIT,"models/weapons/v_knife_karam.mdl" },
		{ WEAPON_KNIFE_GUT,"models/weapons/v_knife_gut.mdl" },
		{ WEAPON_KNIFE_FLIP, "models/weapons/v_knife_flip.mdl" },
		{ WEAPON_KNIFE_BAYONET, "models/weapons/v_knife_bayonet.mdl" },
		{ WEAPON_KNIFE_CANIS ,"models/weapons/v_knife_canis.mdl" },
		{ (ItemDefinitionIndex)0 ,"models/weapons/v_knife_skeleton.mdl" }
	};
	const char* FindKnifeModel(ItemDefinitionIndex idx) {
		if (ItemDefinitionIndex::WEAPON_KNIFE == idx || ItemDefinitionIndex::WEAPON_KNIFE_T == idx) {
			return idx == ItemDefinitionIndex::WEAPON_KNIFE ? "models/weapons/v_knife_default_ct.mdl" : "models/weapons/v_knife_default_t.mdl";
		}
		if (knifeModels.find(idx) != knifeModels.end()) {
			return knifeModels.find(idx)->second;
		}
		else {
			return "";
		}
	}
}