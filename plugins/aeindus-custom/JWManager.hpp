#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include "util.hpp"
#include "UI\obs-frontend-api\obs-frontend-api.h"

class JWManager {
private:
	PROJ_OPERATION desired_state = PROJ_OPERATION::NOTHING;
	std::mutex lock_projector_access;
	std::atomic<bool> shutdown_event = false;
	std::thread manager;
	config_t* global_config;

	static bool matchProjector(WINDOW_DATA descriptor);

	void runner();
	void internalToggle(PROJ_OPERATION operation);

public:
	JWManager();
	~JWManager();

	void setGlobalConfig(config_t* config){
		global_config = config;
	}

	void toggle(PROJ_OPERATION operation);
};

