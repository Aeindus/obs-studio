#include "ZoomManager.hpp"
#include <obs-module.h>
#include <thread>
#include <atomic>
#include <mutex>
#include "plugin-macros.generated.hpp"
#include "util.hpp"

bool ZoomManager::matchProjector(WINDOW_DATA descriptor) {
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

void ZoomManager::internalToggle(PROJ_OPERATION operation) {
	auto found_windows = getMatchingWindows(matchProjector);
	toggleWindowsAsProjector(operation, found_windows);
}
void ZoomManager::runner() {
	while (!shutdown_event) {
		Sleep(2000);

		// ----------- Guard access -----------
		std::lock_guard<std::mutex> guard(lock_projector_access);
		// ----------- Guard access -----------

		//TODO: if the window is minimized then I cannot find the true
		// projector. 
		// Fix: determine which window is on which screen. Pick the one
		// on the second monitor

		auto jw_projector_windows = getMatchingWindows(matchProjector);
		for (auto descriptor : jw_projector_windows) {
			descriptor.refresh();

			if (desired_state == PROJ_OPERATION::MAXIMIZE && (!descriptor.visible_on_screen || descriptor.primary_monitor)) {
				// Focus was lost. Fix it
				internalToggle(PROJ_OPERATION::MAXIMIZE);
			}
		}
	}
}


ZoomManager::ZoomManager() {
	manager = std::thread(&ZoomManager::runner, this);
}
ZoomManager::~ZoomManager() {
	shutdown_event = true;

	try {
		manager.join();
	} catch (...) {
		blog(LOG_WARNING, "Thread join returned exception");
	}
}

void ZoomManager::toggle(PROJ_OPERATION operation) {
	// ----------- Guard access -----------
	std::lock_guard<std::mutex> guard(lock_projector_access);
	// ----------- Guard access -----------

	desired_state = operation;
	internalToggle(operation);
}

