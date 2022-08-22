#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <dwmapi.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <vector>
#include <obs-module.h>
#include "plugin-macros.generated.hpp"
#include "util.hpp"

using namespace std;

vector<tuple<string, uint64_t>> window_styles = {
	{"WS_BORDER",0x00800000L},
	{"WS_CAPTION",0x00C00000L},
	{"WS_CHILD",0x40000000L},
	{"WS_CHILDWINDOW",0x40000000L},
	{"WS_CLIPCHILDREN",0x02000000L},
	{"WS_CLIPSIBLINGS",0x04000000L},
	{"WS_DISABLED",0x08000000L},
	{"WS_DLGFRAME",0x00400000L},
	{"WS_GROUP",0x00020000L},
	{"WS_HSCROLL",0x00100000L},
	{"WS_ICONIC",0x20000000L},
	{"WS_MAXIMIZE",0x01000000L},
	{"WS_MAXIMIZEBOX",0x00010000L},
	{"WS_MINIMIZE",0x20000000L},
	{"WS_MINIMIZEBOX",0x00020000L},
	{"WS_POPUP",0x80000000L},
	{"WS_SIZEBOX",0x00040000L},
	{"WS_SYSMENU",0x00080000L},
	{"WS_TABSTOP",0x00010000L},
	{"WS_THICKFRAME",0x00040000L},
	{"WS_VISIBLE",0x10000000L},
	{"WS_VSCROLL",0x00200000L}
};
vector<tuple<string, uint64_t>> extended_window_styles = {
	{"WS_EX_APPWINDOW",0x00040000L},
	{"WS_EX_LAYERED",0x00080000L},
	{"WS_EX_MDICHILD",0x00000040L},
	{"WS_EX_NOACTIVATE",0x08000000L},
	{"WS_EX_NOINHERITLAYOUT",0x00100000L},
	{"WS_EX_NOPARENTNOTIFY",0x00000004L},
	{"WS_EX_NOREDIRECTIONBITMAP",0x00200000L},
	{"WS_EX_OVERLAPPEDWINDOW",0x00000300L},
	{"WS_EX_TOPMOST",0x00000008L},
	{"WS_EX_TRANSPARENT",0x00000020L},
	{"WS_EX_WINDOWEDGE",0x00000100L}
};
vector<tuple<string, uint64_t>> window_show_states = {
	{"SW_SHOWNORMAL"    ,   1},
	{"SW_SHOWMINIMIZED" ,   2},
	{"SW_SHOWMAXIMIZED" ,   3}
};

void WINDOW_DATA::refresh() {
	MONITORINFO monitor_info;
	HMONITOR monitor;

	monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
	monitor_info.cbSize = sizeof(monitor_info);

	GetWindowTextA(hwnd, caption_name, sizeof(caption_name));
	GetMonitorInfo(monitor, &monitor_info);

	monitor_id = (uint64_t)monitor;
	visible_on_screen = isWindowVisibleOnScreen(hwnd);
	primary_monitor = (monitor_info.dwFlags == MONITORINFOF_PRIMARY);
}

// Thanks Raymond! https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
bool isWindowCloaked(HWND hwnd) {
	BOOL isCloaked = FALSE;
	return (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked, sizeof(isCloaked)))) && isCloaked;
}
bool isWindowVisibleOnScreen(HWND hwnd) {
	WINDOWPLACEMENT window_placement = { sizeof(window_placement) };
	GetWindowPlacement(hwnd, &window_placement);

	return IsWindowVisible(hwnd) &&
		!isWindowCloaked(hwnd) && !(window_placement.showCmd == SW_SHOWMINIMIZED);
}

string convertStylesToString(WINDOW_DATA descriptor) {
	ostringstream result;
	WINDOWPLACEMENT window_placement = { sizeof(window_placement) };
	uint64_t styles_code = GetWindowLongPtrA(descriptor.hwnd, GWL_STYLE);
	uint64_t extended_styles_code = GetWindowLongPtrA(descriptor.hwnd, GWL_EXSTYLE);

	for (const tuple<string, uint64_t>& el : window_styles) {
		if (styles_code & get<1>(el)) {
			result << get<0>(el) << "|";
		}
	}

	result << endl;
	for (const tuple<string, uint64_t>& el : extended_window_styles) {
		if (extended_styles_code & get<1>(el)) {
			result << get<0>(el) << "|";
		}
	}

	result << endl;
	if (!GetWindowPlacement(descriptor.hwnd, &window_placement)) {
		result << "GetWindowPlacement failed";
	} else {
		for (const tuple<string, uint64_t>& el : window_show_states) {
			if (window_placement.showCmd == get<1>(el)) {
				result << get<0>(el) << "|";
			}
		}
	}

	return result.str();
}

WINDOW_DATA getWindowDescription(HWND hwnd) {
	WINDOW_DATA descriptor;
	MONITORINFO monitor_info;
	HMONITOR monitor;
	DWORD process_pid = 0;
	DWORD thread_id = 0;

	monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONULL);
	monitor_info.cbSize = sizeof(monitor_info);

	GetClassNameA(hwnd, descriptor.class_name, sizeof(descriptor.class_name));
	GetWindowTextA(hwnd, descriptor.caption_name, sizeof(descriptor.caption_name));
	GetMonitorInfo(monitor, &monitor_info);

	descriptor.hwnd = hwnd;
	descriptor.tid = GetWindowThreadProcessId(hwnd, &descriptor.pid);
	descriptor.monitor_id = (uint64_t)monitor;
	descriptor.visible_on_screen = isWindowVisibleOnScreen(hwnd);
	descriptor.primary_monitor = (monitor_info.dwFlags == MONITORINFOF_PRIMARY);

	return descriptor;
}
string descriptionToString(WINDOW_DATA descriptor) {
	std::ostringstream ss;

	descriptor.refresh();

	ss << "PID/TID:" << descriptor.pid << "/" << descriptor.tid << " "
		<< "hwnd:" << std::hex << (uint64_t)descriptor.hwnd << std::dec << endl
		<< "class: " << descriptor.class_name << ", " << "caption:" << descriptor.caption_name << endl
		<< convertStylesToString(descriptor) << endl
		<< "IsWindowVisibleOnScreen:" << descriptor.visible_on_screen << endl
		<< "Monitor:" << descriptor.monitor_id << ", is on primary:" << (descriptor.primary_monitor ? "true" : "false")
		<< endl;

	return ss.str();
}


std::vector<WINDOW_DATA> getMatchingWindows(COMPARE_WINDOW_CALLBACK callback) {
	std::vector<WINDOW_DATA> result;
	HWND prevWindow = 0;

	while (true) {
		HWND desktopWindow = GetDesktopWindow();
		if (!desktopWindow)
			break;

		HWND nextWindow = FindWindowEx(desktopWindow, prevWindow, NULL, NULL);
		if (!nextWindow) break;

		// Filter out windows that do not fit the rules
		WINDOW_DATA descriptor = getWindowDescription(nextWindow);
		if (callback(descriptor)) {
			result.push_back(descriptor);
		}

		prevWindow = nextWindow;
	}
	return result;
}
HMONITOR getSecondMonitor() {
	int primary_monitor_width = GetSystemMetrics(SM_CXSCREEN);

	// Get point on the right monitor
	POINT second_screen_point;
	second_screen_point.x = primary_monitor_width + 1;
	second_screen_point.y = 0;

	return MonitorFromPoint(second_screen_point, MONITOR_DEFAULTTONULL);
}

void goFullscreen(HWND hwnd, MONITORINFO mi) {
	//https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
	WINDOWPLACEMENT g_wpPrev = { sizeof(g_wpPrev) };
	DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

	if (GetWindowPlacement(hwnd, &g_wpPrev)) {
		// Remove overlapped attribute from window in order to hide title bar
		SetWindowLong(hwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

		SetWindowPos(hwnd, HWND_TOP,
			mi.rcMonitor.left, mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
		ShowWindow(hwnd, SW_MAXIMIZE);
	}
}
void toggleProjectors(PROJ_OPERATION operation, std::vector<WINDOW_DATA>& windows) {
	for (auto descriptor : windows) {
		if (operation == PROJ_OPERATION::MAXIMIZE) {
			blog(LOG_INFO, "Maximized app %s", descriptionToString(descriptor).c_str());

			// Show window but use NORMAL flag as it seems to be
			// safer than MAXIMIZE (in my tests JW projector dislikes
			// to be maximized)
			ShowWindow(descriptor.hwnd, SW_MINIMIZE);
			ShowWindow(descriptor.hwnd, SW_NORMAL);

			// The window seems to be invisible even though it has been maximized and
			// GetWindowPlacement says it is maximized. Fix by bringing to top.
			BringWindowToTop(descriptor.hwnd);
		} else if (operation == PROJ_OPERATION::MINIMIZE) {
			blog(LOG_INFO, "Minimized app %s", descriptionToString(descriptor).c_str());

			ShowWindow(descriptor.hwnd, SW_MINIMIZE);
		}
	}
}
void toggleWindowsAsProjector(PROJ_OPERATION operation, std::vector<WINDOW_DATA>& windows) {
	for (auto descriptor : windows) {
		if (operation == PROJ_OPERATION::MAXIMIZE) {
			HMONITOR second_monitor = getSecondMonitor();
			MONITORINFO info = { sizeof(MONITORINFO) };

			blog(LOG_INFO, "Maximized app %s", descriptionToString(descriptor).c_str());

			if (second_monitor == NULL) return;
			if (!GetMonitorInfo(second_monitor, &info)) return;
			goFullscreen(descriptor.hwnd, info);

		} else if (operation == PROJ_OPERATION::MINIMIZE) {
			blog(LOG_INFO, "Minimized app %s", descriptionToString(descriptor).c_str());

			ShowWindow(descriptor.hwnd, SW_MINIMIZE);
		}
	}
}