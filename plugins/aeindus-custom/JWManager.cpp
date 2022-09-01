#include "JWManager.hpp"
#include <thread>
#include <atomic>
#include <mutex>
#include <obs-module.h>
#include <util/config-file.h>
#include "plugin-macros.generated.hpp"
#include "util.hpp"

bool JWManager::matchProjector(WINDOW_DATA descriptor) {
	bool matched_class_name = strcmp(descriptor.class_name, "ApplicationFrameWindow") == 0;
	bool matched_caption_name = strcmp(descriptor.caption_name, "JW Library") == 0;
	bool matched_extended_style = GetWindowLongPtrA(descriptor.hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST;

	return matched_class_name && matched_caption_name && matched_extended_style;
}

void JWManager::runner() {
	while (true) {
		SPIN_LOCK(shutdown_event, 2000, 100);
		if(shutdown_event) return;

		// ----------- Guard access -----------
		std::lock_guard<std::mutex> guard(lock_projector_access);
		// ----------- Guard access -----------

		if(!config_get_bool(global_config, "BasicWindow", "ForceJWProjector")) continue;

		//TODO: if the window is minimized then I cannot find the true
		// projector. 
		// Fix: determine which window is on which screen. Pick the one
		// on the second monitor

		auto jw_projector_windows = getMatchingWindows(matchProjector);
		for (auto descriptor : jw_projector_windows) {
			descriptor.refresh();

			if (desired_state == PROJ_OPERATION::MAXIMIZE && !descriptor.visible_on_screen) {
				// Focus was lost. Fix it
				internalToggle(PROJ_OPERATION::MAXIMIZE);
			}
		}
	}
}
void JWManager::internalToggle(PROJ_OPERATION operation) {
	auto found_windows = getMatchingWindows(matchProjector);
	toggleProjectors(operation, found_windows);
}

JWManager::JWManager() {
	manager = std::thread(&JWManager::runner, this);
}
JWManager::~JWManager() {
	shutdown_event = true;

	try {
		if(manager.joinable()) manager.join();
	} catch (...) {
		blog(LOG_WARNING, "Thread join returned exception");
	}
}

void JWManager::toggle(PROJ_OPERATION operation) {
	// ----------- Guard access -----------
	std::lock_guard<std::mutex> guard(lock_projector_access);
	// ----------- Guard access -----------

	// We do this in order to prevent unnecesarily flasing of the projector
	// if it already is in the requested state
	if(desired_state == operation) return;

	desired_state = operation;
	internalToggle(operation);
}

void JWManager::stop() {
	shutdown_event = true;

	try {
		if(manager.joinable()) manager.join();
	} catch (...) {
		blog(LOG_WARNING, "Thread join returned exception");
	}
}