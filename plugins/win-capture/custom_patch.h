#pragma once

#include <Windows.h>
#include <stdlib.h>
#include <util/dstr.h>

struct WINDOW_DATA {
	HWND hwnd;
	DWORD pid;
	DWORD tid;
	char caption_name[256];
	char class_name[256];
	int monitor_id;
	bool primary_monitor;
	bool visible_on_screen;
};

HWND getZoomProjector();
