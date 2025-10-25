#pragma once
#include "memory.h"
#include <cstdint>
class IVModelInfoClient
{
public:
    constexpr std::int32_t GetModelIndex(const char* path) noexcept
    {
        if (!this) return 0;
        using Function = std::int32_t(__thiscall*)(void*, const char*);
        return (*reinterpret_cast<Function**>(this))[2](this, path);
    }
};