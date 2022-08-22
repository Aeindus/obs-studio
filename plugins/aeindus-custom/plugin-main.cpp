// clang-format off

#include <Windows.h>
#include <sstream>
#include <string>
#include <cstring>
#include <thread>
#include <obs-module.h>
#include "UI\obs-frontend-api\obs-frontend-api.h"
#include "plugin-macros.generated.hpp"

#include "JWManager.hpp"
#include "ZoomManager.hpp"

#define STOP MessageBoxA(NULL, "Test", "Test", MB_OK)
#define EXTERNAL_MONITOR 1

OBS_DECLARE_MODULE()

bool first_run = true;
JWManager jw_manager;

void customCallback(obs_frontend_event event, void* private_data) {
	if (event == OBS_FRONTEND_EVENT_SCENE_CHANGED) {
		obs_source_t* scene = obs_frontend_get_current_scene();
		std::string current_scene_name = obs_source_get_name(scene);
		obs_source_release(scene);

		blog(LOG_INFO, "SCENE_CHANGED");
		blog(LOG_INFO, "Current scene name: %s", current_scene_name.c_str());

		size_t start_of_scene = current_scene_name.find("Scena");
		if (start_of_scene == std::string::npos) {
			start_of_scene = current_scene_name.find("Scene");
		}

		if (start_of_scene == std::string::npos) {
			blog(LOG_ERROR, "Scene identifier could not be found in scene name");
			return;
		}

		std::string flags = current_scene_name.substr(0, start_of_scene);

		if (flags.find("JW") != std::string::npos) {
			jw_manager.toggle(PROJ_OPERATION::MAXIMIZE);
		} else {
			// We will not minimize JW projector because we want it
			// to be shown by default and at all times except
			// when overriden
			jw_manager.toggle(PROJ_OPERATION::NOTHING);
		}

		if (flags.find("P") != std::string::npos) {
			// Close and restart projector
			// This is also what OBSBasic::OpenProjector() with CloseExistingProjectors
			// setting enabled do.
			obs_frontend_close_projectors(EXTERNAL_MONITOR);
			obs_frontend_open_projector("Scene", EXTERNAL_MONITOR, "", current_scene_name.c_str());
		} else {
			// Disable obs projector
			obs_frontend_close_projectors(EXTERNAL_MONITOR);
		}

		if (!first_run) {
			if (flags.find("V") != std::string::npos) {
				obs_frontend_stop_virtualcam();
			} else {
				if (!obs_frontend_virtualcam_active())
					obs_frontend_start_virtualcam();
			}
		}
		
		first_run = false;
	} else if (event == OBS_FRONTEND_EVENT_SCENE_LIST_CHANGED) {
		blog(LOG_INFO, "LIST_CHANGED");
	}
}

bool obs_module_load(void) {
	blog(LOG_INFO, "plugin loaded successfully");

	jw_manager.setGlobalConfig(obs_frontend_get_global_config());

	// Register events
	obs_frontend_add_event_callback(customCallback, nullptr);

	return true;
}

void obs_module_unload() {
	jw_manager.stop();
	blog(LOG_INFO, "plugin unloaded");
}

