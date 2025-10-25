#pragma once
#include "memory.h"

struct PlayerInfo
{
    __int64         unknown;            //0x0000 
    union {
        __int64       steamID64;        //0x0008 - SteamID64
        struct {
            int xuidlow;
            int xuidhigh;
        };
    };
    char            szName[128];        //0x0010 - Player Name
    int             userId;             //0x0090 - Unique Server Identifier
    char            szSteamID[20];      //0x0094 - STEAM_X:Y:Z
    char            pad_0x00A8[0x10];   //0x00A8
    unsigned long   iSteamID;           //0x00B8 - SteamID 
    char            szFriendsName[128];
    bool            fakeplayer;			// Is BOT
    bool            ishltv;
    unsigned int    customFiles[4];
    unsigned char   filesDownloaded;
};
class IVEngineClient
{
public:
    constexpr std::int32_t GetLocalPlayerIndex() noexcept
    {
        if (!this) return 0;
        using Function = std::int32_t(__thiscall*)(void*);
        return (*reinterpret_cast<Function**>(this))[12](this);
    }
    bool GetPlayerInfo(int index, PlayerInfo* pinfo) noexcept
    {
        if (!this) return false; 
        using Function = bool(__thiscall*)(void*, int, PlayerInfo*);
        return (*reinterpret_cast<Function**>(this))[8](this, index, pinfo);
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