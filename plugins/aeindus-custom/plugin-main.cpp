// clang-format off

#include <Windows.h>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <obs-module.h>
#include "plugin-macros.generated.hpp"
#include "UI\obs-frontend-api\obs-frontend-api.h"
#include "util.hpp"

#define STOP MessageBoxA(NULL, "Test", "Test", MB_OK)
#define EXTERNAL_MONITOR 1

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

void custom_callback(obs_frontend_event event, void* private_data) {
	if (event== OBS_FRONTEND_EVENT_SCENE_CHANGED) {
		obs_source_t *scene = obs_frontend_get_current_scene();
		std::string current_scene_name = obs_name_from_scene(scene);
		obs_source_release(scene);

		blog(LOG_INFO, "SCENE_CHANGED");
		blog(LOG_INFO, "Current scene name: %s",current_scene_name.c_str());

		// Activate the other app projector
		// The default behaviour is to leave this projector on
		// even when this scene is switched out.
		if (current_scene_name.find("JWScena") != std::string::npos) {
			ToggleJWLibraryProjector(PROJ_OPERATION::MAXIMIZE);
		}

		if (current_scene_name.find("PScena")!= std::string::npos) {
			// Pause the other app projector
			ToggleJWLibraryProjector(PROJ_OPERATION::MINIMIZE);
			
			// Close and restart projector
			// This is also what OBSBasic::OpenProjector() with CloseExistingProjectors
			// setting enabled do.
			obs_frontend_close_projectors(EXTERNAL_MONITOR);
			
			// Enable projector here only if it's not open yet
			obs_frontend_open_projector("Scene", EXTERNAL_MONITOR, "", current_scene_name.c_str());
		} else {
			// Disable obs projector
			obs_frontend_close_projectors(EXTERNAL_MONITOR);
		}

		if (current_scene_name.find("VScena") != std::string::npos) {
			obs_frontend_stop_virtualcam();
		} else {
			if(!obs_frontend_virtualcam_active())
				obs_frontend_start_virtualcam();
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

