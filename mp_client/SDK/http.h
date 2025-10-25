#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#pragma once

#include <string>
#include <vector>

namespace http {
    std::string Get(const std::wstring& url, const std::wstring& path) {
        std::string result;
        HINTERNET hSession = WinHttpOpen(L"EquipClient/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) return result;

        HINTERNET hConnect = WinHttpConnect(hSession, url.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", path.c_str(), NULL,
            WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);

        if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
            if (WinHttpReceiveResponse(hRequest, NULL)) {
                DWORD dwSize = 0;
                do {
                    DWORD dwDownloaded = 0;
                    WinHttpQueryDataAvailable(hRequest, &dwSize);
                    std::vector<char> buffer(dwSize + 1);
                    ZeroMemory(buffer.data(), buffer.size());
                    WinHttpReadData(hRequest, buffer.data(), dwSize, &dwDownloaded);
                    result.append(buffer.data(), dwDownloaded);
                } while (dwSize > 0);
            }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return result;
    }
    bool SendEquipToServer(long itemId, int teamId, int slotId, const CItem& item) {
        std::ostringstream oss;
        oss << "{"
            << "\"userId\":" << V::STEAM_ID << ","
            << "\"defIdx\":" << item.iDefIdx << ","
            << "\"slotId\":" << slotId << ","
            << "\"teamId\":" << teamId << ","
            << "\"paintKit\":" << item.flPaintKit << ","
            << "\"hasStattack\":" << (item.bHasStattrack ? "true" : "false") << ","
            << "\"stattrack\":" << item.flStattrack << ","
            << "\"wear\":" << item.flWear << ","
            << "\"pattern\":" << item.iPattern << ","
            << "\"quality\":" << item.iQuality
            << "}";

        std::string jsonBody = oss.str();

        HINTERNET hSession = WinHttpOpen(L"EquipClient/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS,
            0);

        if (!hSession) return false;

        HINTERNET hConnect = WinHttpConnect(hSession, L"127.0.0.1", 3000, 0);
        if (!hConnect) {
            WinHttpCloseHandle(hSession);
            return false;
        }

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/set_user_equip",
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            0);

        if (!hRequest) {
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return false;
        }

        BOOL bResults = WinHttpAddRequestHeaders(hRequest,
            L"Content-Type: application/json\r\n",
            -1L, WINHTTP_ADDREQ_FLAG_ADD);


        bResults = WinHttpSendRequest(
            hRequest,
            WINHTTP_NO_ADDITIONAL_HEADERS,
            0,
            (LPVOID)jsonBody.c_str(),
            (DWORD)jsonBody.size(),
            (DWORD)jsonBody.size(),
            0
        );



        if (bResults)
            bResults = WinHttpReceiveResponse(hRequest, NULL);

        if (bResults) {
            DWORD dwSize = 0;
            do {
                WinHttpQueryDataAvailable(hRequest, &dwSize);
                if (!dwSize) break;

                std::string buffer(dwSize, 0);
                DWORD dwDownloaded = 0;
                WinHttpReadData(hRequest, &buffer[0], dwSize, &dwDownloaded);
                std::cout << "[equip-server] " << buffer << "\n";

            } while (dwSize > 0);
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return bResults;
    }
    /*
    * let userEquips = [
    { userId: 0, defIdx: 0, slotId: 0, teamId: 0, paintKit: 0, hasStattack: false, stattrack: -1, wear: 0, pattern: 0, quality: 0 },
    ]

    app.get("/get_user_equips", (req, res) => {
        let userId = parseInt(req.query.userId);
        let equips = userEquips.filter(e => e.userId === userId);
        res.json(equips);
    });

    */
    CInventory::CRemoteInventory getRemoteInventory(int SteamId) {
		console::log(std::format("[equip-client] Fetching remote inventory for {}...", SteamId).c_str());
        auto res = http::Get(L"http://127.0.0.1", L"/get_user_equips?userId=" + std::to_wstring(SteamId));
		CInventory::CRemoteInventory inventory;
		try {
			auto json = nlohmann::json::parse(res);
			for (auto& item : json) {
				CInventory::CRemoteEquip remoteItem;
				remoteItem.item.iDefIdx = item["defIdx"].get<int>();
				remoteItem.slotId = item["slotId"].get<int>();
				remoteItem.teamId = item["teamId"].get<int>();
				remoteItem.item.flPaintKit = item["paintKit"].get<float>();
				remoteItem.item.bHasStattrack = item["hasStattack"].get<bool>();
				remoteItem.item.flStattrack = item["stattrack"].get<float>();
				remoteItem.item.flWear = item["wear"].get<float>();
				remoteItem.item.iPattern = item["pattern"].get<int>();
				remoteItem.item.iQuality = item["quality"].get<int>();
				inventory.equips.push_back(remoteItem);
			}
		}
		catch (...) {
			console::log("[equip-client] Failed to parse inventory JSON response");
		}
		return inventory;
    }
}