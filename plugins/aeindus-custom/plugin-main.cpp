/*
Plugin Name
Copyright (C) <Year> <Developer> <Email Address>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <Windows.h>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <obs-module.h>
#include "UI\obs-frontend-api\obs-frontend-api.h"
#include "util.hpp"
#include "plugin-macros.generated.hpp"

#define STOP MessageBoxA(NULL, "Test", "Test", MB_OK)

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

void custom_callback(obs_frontend_event event, void* private_data) {
	if (event == OBS_FRONTEND_EVENT_SCENE_CHANGED) {
		blog(LOG_INFO, "SCENE_CHANGED");

		std::string current_scene_name = obs_frontend_get_current_scene_name();

		OutputDebugStringA("Current scene name:");
		OutputDebugStringA(current_scene_name.c_str());

		if (current_scene_name.find("PScena") != std::string::npos) {
			// Pause the other app projector
			ToggleJWLibraryProjector(PROJ_OPERATION::MINIMIZE);
			
			// Enable projector here
			obs_frontend_open_projector("Scene", 1, "", current_scene_name.c_str());
		} else {
			// Disable obs projector
			obs_frontend_close_projectors(1);

			// Activate the other app projector
			if (current_scene_name.find("JWScena") != std::string::npos) {
				ToggleJWLibraryProjector(PROJ_OPERATION::MAXIMIZE);
			}
		}

	} else if (event == OBS_FRONTEND_EVENT_SCENE_LIST_CHANGED) {
		blog(LOG_INFO, "LIST_CHANGED");
	}
}


bool obs_module_load(void) {
	blog(LOG_INFO, "plugin loaded successfully");

	// Register events
	obs_frontend_add_event_callback(custom_callback, nullptr);

	return true;
}

void obs_module_unload() {
	blog(LOG_INFO, "plugin unloaded");
}

