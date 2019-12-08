#pragma once
#include <Windows.h>

class CurrentWindow
{
	public:
		CurrentWindow();

		HWND GetCurrentWindow() {
			return CurWin;
		}
	private:
		HWND CurWin;
};

