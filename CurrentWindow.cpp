#include "CurrentWindow.h"
#include <iostream>
#include "winError.h"

CurrentWindow::CurrentWindow() {
	HWND curWin = GetForegroundWindow();
	wchar_t curWinTitle[800];

	std::cout << "Press ESC to cancel" << std::endl;
	Sleep(1000);
	system("cls");

	if (curWin != NULL) {
		GetWindowTextW(curWin, curWinTitle, sizeof(curWinTitle) / sizeof(wchar_t));
		std::wcout << "New window selected: " << curWinTitle << std::endl;
		CurWin = curWin;
	} else {
		winError err(L"Error getting the current window");
	}
}