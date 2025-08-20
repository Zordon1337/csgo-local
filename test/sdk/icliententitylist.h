#pragma once
#include "CEntity.h"
#include <cstdint>
class IClientEntityList
{
public:
	constexpr CEntity* GetEntityFromIndex(const std::int32_t index) noexcept
	{
		using Function = CEntity * (__thiscall*)(void*, int32_t);
		return (*reinterpret_cast<Function**>(this))[3](this, index);
	}
};