#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>

enum class PROJ_OPERATION {
	MAXIMIZE,
	MINIMIZE,
	NOTHING
};

/// <summary>
/// Iterate through all windows found by FindWindowEx.
/// </summary>
/// <param name="iterateWindows"></param>
void ToggleJWLibraryProjector(PROJ_OPERATION operation);
