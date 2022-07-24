#include <Windows.h>
#include <dwmapi.h>
#include <stdlib.h>
#include <util/dstr.h>
#include "custom_patch.h"

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

struct WINDOW_DATA getWindowDescription(HWND hwnd) {
	struct WINDOW_DATA descriptor;
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


bool zoomMatchProjector(struct WINDOW_DATA descriptor) {
	bool matched_class_name = strcmp(descriptor.class_name, "ZPContentViewWndClass") == 0;
	// Don't know exactly how the window might be named. It might be Zoom Meeting or not
	bool matched_caption_name = strstr(descriptor.caption_name, "Zoom") != 0;

	// Iterate through child windows and see if "Meeting Tools" can be found
	// The second window absolutely does not have this control child window
	HWND content_left_panel = FindWindowExA(descriptor.hwnd, NULL, NULL, "ContentLeftPanel");
	if (content_left_panel == NULL) return false;
	HWND meeting_tools = FindWindowExA(content_left_panel, NULL, NULL, "Meeting Tools");

	return matched_class_name && matched_caption_name && (meeting_tools == NULL);
}

HWND getZoomProjector() {
	HWND prevWindow = 0;

	while (true) {
		HWND desktopWindow = GetDesktopWindow();
		if (!desktopWindow)
			break;

		HWND nextWindow = FindWindowEx(desktopWindow, prevWindow, NULL, NULL);
		if (!nextWindow) break;

		// Filter out windows that do not fit the rules
		struct WINDOW_DATA descriptor = getWindowDescription(nextWindow);
		if (zoomMatchProjector(descriptor) && descriptor.visible_on_screen) {
			return descriptor.hwnd;
		}

		prevWindow = nextWindow;
	}
	return NULL;
}
