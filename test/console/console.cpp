#pragma once
#include "console.h"
using DevMsgFn = void(__fastcall*)(const char*, ...);
DevMsgFn oDevMsg = (DevMsgFn)GetProcAddress(GetModuleHandleA("tier0.dll"), "DevMsg");
void console::log(const char* msg) {
	std::cout << "[*] [" << std::chrono::system_clock::now() << "] " << msg << std::endl;
	oDevMsg(msg);
}
void console::init() {
	AllocConsole();
	freopen_s(&fl, "CONOUT$", "w", stdout);
}