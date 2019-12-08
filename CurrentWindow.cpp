#include "CurrentWindow.h"
#include <iostream>
#include "winError.h"

CurrentWindow::CurrentWindow() {
	HWND curWin = GetForegroundWindow();
	wchar_t curWinTitle[800];

	if (curWin != NULL) {
		GetWindowTextW(curWin, curWinTitle, sizeof(curWinTitle) / sizeof(wchar_t));
		std::wcout << "New window selected: " << curWinTitle << std::endl;
		CurWin = curWin;
	} else {
		winError err(L"Error getting the current window");
	}
}