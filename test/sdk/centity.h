#pragma once
#include "../vars.h"
class CEntity {
public:
	inline int getTeam() {
		return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + 0x0B68);
	}
	inline BYTE getLifeState() {
		return *reinterpret_cast<BYTE*>(reinterpret_cast<uintptr_t>(this) + 0x25F);
	}

	inline UINT* getWeapons() {
		return reinterpret_cast<UINT*>(reinterpret_cast<uintptr_t>(this) + 0x2DF8);
	}
	NETVAR(m_hMyWeapons, "CBaseCombatCharacter->m_hMyWeapons", std::array<unsigned long, 48>);
	NETVAR(m_lifeState, "CCSPlayer->m_lifeState", byte);
	NETVAR(m_iHealth, "CCSPlayer->m_iHealth", int);
	NETVAR(m_iTeamNum, "CCSPlayer->m_iTeamNum", int);

};
class CBaseAttributableItem : CEntity {
public:

	NETVAR(m_iItemDefinitionIndex, "CBaseAttributableItem->m_iItemDefinitionIndex", uint16_t);
	NETVAR(m_iEntityQuality, "CBaseAttributableItem->m_iEntityQuality", int);
	NETVAR(m_iItemIDHigh, "CBaseAttributableItem->m_iItemIDHigh", int);
	NETVAR(m_nFallbackPaintKit, "CBaseAttributableItem->m_nFallbackPaintKit", int);
	NETVAR(m_flFallbackWear, "CBaseAttributableItem->m_flFallbackWear", float);
	NETVAR(m_nFallbackSeed, "CBaseAttributableItem->m_nFallbackSeed", int);
	NETVAR(m_nModelIndex, "CBaseAttributableItem->m_nModelIndex", int);
	
};