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

struct WINDOW_DATA {
	HWND hwnd;
	DWORD pid;
	DWORD tid;
	char caption_name[256];
	char class_name[256];
	int monitor_id;
	bool visible_on_screen;
	bool primary_monitor;

	void refresh();
};

typedef bool (*COMPARE_WINDOW_CALLBACK)(WINDOW_DATA);

/// <summary>
/// Returns wether the window is actually visible.
/// Does not use WM_VISIBLE as cloaking will keep this flag set
/// but hide the window.
/// </summary>
/// <param name="hwnd"></param>
/// <returns></returns>
bool isWindowVisibleOnScreen(HWND hwnd);

std::vector<WINDOW_DATA> getMatchingWindows(COMPARE_WINDOW_CALLBACK callback);
/// <summary>
/// Gets the second monitor handle. This monitor must be on the right
/// of the primary screen.
/// </summary>
/// <returns></returns>
HMONITOR getSecondMonitor();

/// <summary>
/// Toggles a window that acts as a projector for the second monitor.
/// By default these projects run fullscreen and are expected to reside
/// only within the second monitor.
/// If they are supposed to be moved between monitors then this is not a projector.
/// </summary>
void toggleProjectors(PROJ_OPERATION operation, std::vector<WINDOW_DATA>& windows);


/// <summary>
/// Toggles a window and makes it a projector.
/// This will move the window on the second screen and maximize it.
/// </summary>
void toggleWindowsAsProjector(PROJ_OPERATION operation, std::vector<WINDOW_DATA>& windows);
