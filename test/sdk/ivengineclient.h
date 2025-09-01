#pragma once
#include "memory.h"
class IVEngineClient
{
public:
    constexpr std::int32_t GetLocalPlayerIndex() noexcept
    {
        using Function = std::int32_t(__thiscall*)(void*);
        return (*reinterpret_cast<Function**>(this))[12](this);
    }
    constexpr std::int32_t GetPlayerForUserID(int userid) noexcept
    {
        using Function = std::int32_t(__thiscall*)(void*, int);
        return (*reinterpret_cast<Function**>(this))[9](this, userid);
    }
};