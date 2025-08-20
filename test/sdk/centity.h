#pragma once
class CEntity {
	inline int getTeam() {
		return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(this) + 0x0B68);
	}
};