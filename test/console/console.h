#pragma once
#include <iostream>
#include "Windows.h"
#include <chrono>
namespace console
{
	FILE* fl;
	void init();
	void log(const char* msg);
}