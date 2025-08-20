#pragma once
#include "console.h"

void console::log(const char* msg) {
	std::cout << "[*] [" << std::chrono::system_clock::now() << "] " << msg << std::endl;
}
void console::init() {
	AllocConsole();
	freopen_s(&fl, "CONOUT$", "w", stdout);
}