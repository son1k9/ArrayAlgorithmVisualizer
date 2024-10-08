#pragma once
#include "raylib.h"

namespace Rayutils {

	inline void toggleFullScreen() {
		static int width{1280};
		static int height{720};
		if (!IsWindowFullscreen()) {
			width = GetScreenWidth();
			height = GetScreenHeight();

			int monitor = GetCurrentMonitor();
			int mWidth = GetMonitorWidth(monitor);
			int mHeight = GetMonitorHeight(monitor);

			SetWindowSize(mWidth, mHeight);
			ToggleFullscreen();
		}
		else {
			ToggleFullscreen();
			SetWindowSize(width, height);
		}
	}

	inline int GetDisplayWidth() {
		if (IsWindowFullscreen()) {
			return GetMonitorWidth(GetCurrentMonitor());
		}
		return GetScreenWidth();
	}

	inline int GetDisplayHeight() {
		if (IsWindowFullscreen()) {
			return GetMonitorHeight(GetCurrentMonitor());
		}
		return GetScreenHeight();
	}
}