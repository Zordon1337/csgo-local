#pragma once
#include "memory.h"
class IVEngineClient
{
public:
    constexpr std::int32_t GetLocalPlayerIndex() noexcept
    {
        if (!this) return 0;
        using Function = std::int32_t(__thiscall*)(void*);
        return (*reinterpret_cast<Function**>(this))[12](this);
    }
    constexpr std::int32_t GetPlayerForUserID(int userid) noexcept
    {
        if (!this) return 0;
        using Function = std::int32_t(__thiscall*)(void*, int);
        return (*reinterpret_cast<Function**>(this))[9](this, userid);
    }
    constexpr void ClientCmd(const char* szCmdString)
    {
        using Function = void(__thiscall*)(void*, const char*);
        return (*reinterpret_cast<Function**>(this))[7](this, szCmdString);
    }
};