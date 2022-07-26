#pragma once

#include <Windows.h>
#include <stdlib.h>
#include <util/dstr.h>

#define PROJECTOR_WINDOW "Projector Window"
#define ZOOM_EXE "Zoom.exe"
#define ZOOM_ENTRY_NAME "[Zoom]: " PROJECTOR_WINDOW
#define ZOOM_ENTRY_VAL PROJECTOR_WINDOW ":ZPContentViewWndClass:" ZOOM_EXE

#define JW_EXE "JWLibrary.exe"
#define JW_ENTRY_NAME "[JW Library]: " PROJECTOR_WINDOW
#define JW_ENTRY_VAL PROJECTOR_WINDOW ":Windows.UI.Core.CoreWindow:" JW_EXE

#ifdef __cplusplus
extern "C" {
#endif

struct WINDOW_DATA {
	HWND hwnd;
	DWORD pid;
	DWORD tid;
	char caption_name[256];
	char class_name[256];
	uint64_t monitor_id;
	bool primary_monitor;
	bool visible_on_screen;
};

EXPORT struct WINDOW_DATA getWindowDescription(HWND hwnd);

EXPORT bool matchZoomProjector(struct WINDOW_DATA descriptor);
EXPORT HWND getZoomProjector();

EXPORT bool matchJWProjector(struct WINDOW_DATA descriptor);
EXPORT HWND getJWProjector();

#ifdef __cplusplus
}
#endif
