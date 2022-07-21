#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include "util.hpp"

class JWManager {
private:
	PROJ_OPERATION desired_state;
	std::mutex lock_projector_access;
	std::atomic<bool> shutdown_event = false;
	std::thread manager;

	static bool matchProjector(WINDOW_DATA descriptor);

	void runner();
	void internalToggle(PROJ_OPERATION operation);

public:
	JWManager();
	~JWManager();

	void toggle(PROJ_OPERATION operation);
};

