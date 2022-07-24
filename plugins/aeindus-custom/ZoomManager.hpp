#pragma once

#include <thread>
#include <atomic>
#include <mutex>
#include "util.hpp"

class ZoomManager {
private:
	PROJ_OPERATION desired_state = PROJ_OPERATION::NOTHING;
	std::mutex lock_projector_access;
	std::atomic<bool> shutdown_event = false;
	std::thread manager;

	static bool matchProjector(WINDOW_DATA descriptor);

	void internalToggle(PROJ_OPERATION operation);
	void runner();

public:
	ZoomManager();
	~ZoomManager();

	void toggle(PROJ_OPERATION operation);
};

