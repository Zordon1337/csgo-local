#pragma once
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib> 
namespace M {

    inline std::uint8_t* PatternScan(const char* moduleName, const char* pattern) noexcept
    {
        static auto patternToByte = [](const char* pattern) noexcept -> const std::vector<std::int32_t>
            {
                std::vector<std::int32_t> bytes = std::vector<std::int32_t>{ };
                char* start = const_cast<char*>(pattern);
                const char* end = const_cast<char*>(pattern) + std::strlen(pattern);

                for (auto current = start; current < end; ++current)
                {
                    if (*current == '?')
                    {
                        ++current;

                        if (*current == '?')
                            ++current;

                        bytes.push_back(-1);
                    }
                    else
                        bytes.push_back(std::strtoul(current, &current, 16));
                }

                return bytes;
            };

        const HINSTANCE handle = GetModuleHandleA(moduleName);

        if (!handle)
            return nullptr;

        const PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
        const PIMAGE_NT_HEADERS ntHeaders =
            reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + dosHeader->e_lfanew);

        const std::uintptr_t size = ntHeaders->OptionalHeader.SizeOfImage;
        const std::vector<std::int32_t> bytes = patternToByte(pattern);
        std::uint8_t* scanBytes = reinterpret_cast<std::uint8_t*>(handle);

        const std::size_t s = bytes.size();
        const std::int32_t* d = bytes.data();

        for (std::size_t i = 0ul; i < size - s; ++i)
        {
            bool found = true;

            for (std::size_t j = 0ul; j < s; ++j)
            {
                if (scanBytes[i + j] != d[j] && d[j] != -1)
                {
                    found = false;
                    break;
                }
            }

            if (found)
                return &scanBytes[i];
        }

        return nullptr;
    }
}