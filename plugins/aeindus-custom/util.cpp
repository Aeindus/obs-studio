#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>
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

struct WINDOW_DATA {
	HWND hwnd;
	DWORD pid;
	DWORD tid;
	char caption_name[256];
	char class_name[256];
};

WINDOW_DATA GetWindowDescription(HWND hwnd) {
	WINDOW_DATA descriptor;
	bool has_class_name = GetClassNameA(hwnd, descriptor.class_name, sizeof(descriptor.class_name));
	bool has_caption_name = GetWindowTextA(hwnd, descriptor.caption_name, sizeof(descriptor.caption_name));
	DWORD process_pid = 0;
	DWORD thread_id = 0;

	descriptor.hwnd = hwnd;
	descriptor.tid = GetWindowThreadProcessId(hwnd, &descriptor.pid);
	return descriptor;
}
string DescriptionToString(WINDOW_DATA descriptor) {
	std::ostringstream ss;
	ss << "PID/TID:" << descriptor.pid << "/" << descriptor.tid << " "
		<< "hwnd:" << std::hex << (uint64_t)descriptor.hwnd << ", "
		<< "class: " << descriptor.class_name << ", "
		<< "caption:" << descriptor.caption_name;

	return ss.str();
}


bool CompareWindowData(WINDOW_DATA descriptor) {
	bool matched_class_name = strcmp(descriptor.class_name, "ApplicationFrameWindow") == 0;
	bool matched_caption_name = strcmp(descriptor.caption_name, "JW Library") == 0;
	bool matched_extended_style = GetWindowLongPtrA(descriptor.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;

	return matched_class_name && matched_caption_name && matched_extended_style;
}

void ToggleJWLibraryProjector(PROJ_OPERATION operation) {
	// Now need to run a loop to get the correct window for process.
	bool bFound = false;
	HWND prevWindow = 0;

	while (!bFound) {
		HWND desktopWindow = GetDesktopWindow();
		if (!desktopWindow)
			break;

		HWND nextWindow = FindWindowEx(desktopWindow, prevWindow, NULL, NULL);
		if (!nextWindow)
			break;

		// Filter out windows that do not fit the rules
		WINDOW_DATA descriptor = GetWindowDescription(nextWindow);

		if (CompareWindowData(descriptor)) {
			blog(LOG_INFO,"Restored app %s", DescriptionToString(descriptor).c_str());

			if (operation == PROJ_OPERATION::MAXIMIZE) {
				WINDOWPLACEMENT window;
				window.length = sizeof(WINDOWPLACEMENT);
				GetWindowPlacement(descriptor.hwnd, &window);

				// Only maximize if not already maximized
				if (window.showCmd != SW_NORMAL) {
					ShowWindow(descriptor.hwnd, SW_NORMAL);
				}

				// The window seems to be invisible even though it has been maximized and
				// GetWindowPlacement says it is maximized. Fix by bringing to top.
				BringWindowToTop(descriptor.hwnd);
			} else if (operation == PROJ_OPERATION::MINIMIZE) {
				ShowWindow(descriptor.hwnd, SW_MINIMIZE);
			}
		}

		prevWindow = nextWindow;
	}
}
