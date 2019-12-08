#include <iostream>
#include <windows.h>
#include <winuser.h>
#include "registry.h"
#include "CurrentWindow.h"
#include "winError.h"
#include "afkFind.h"

int main()
{
	std::cout << "Type config to change settings" << std::endl;

	// Registry settings:
	registry Reg;
	DWORD AFKTime = Reg.getKey(L"AFKTime");
	DWORD SelectWindowKey = Reg.getKey(L"SelectWindowKey");

	if (SelectWindowKey == 0) {
		std::cout << "What button should be used to select the current window for Anti-AFK to use?" << std::endl;
	}

	while (SelectWindowKey == 0) {
		for (int i = 0; i <= 256; i++) {
			if (GetAsyncKeyState(i)) {
				Reg.writeSubkey(L"SelectWindowKey", i);
				SelectWindowKey = Reg.getKey(L"SelectWindowKey");
			}
		}

		Sleep(100);
	} 
	
	while (AFKTime == 0) {
		int secAmount;
		std::cout << "Amount of seconds to be AFK before automatic activity is applied" << std::endl;
		std::cin >> secAmount;
		Reg.writeSubkey(L"AFKTime", secAmount);
		AFKTime = Reg.getKey(L"AFKTime");
	}


	WCHAR keyName[1024];
	UINT mappedKey = MapVirtualKeyW(SelectWindowKey, NULL);
	LONG lParam = (mappedKey << 16);
	int getKey = GetKeyNameTextW(lParam, keyName, 1024);
	std::wcout << "Press " << keyName << " to activate Anti-AFK for the current window" << std::endl;
	
	if (getKey == 0) {
		winError err(L"Error converting virtual key into text");
	}


	// Get the current window when the user saved key bind is pressed:
	bool windowSelected = false;
	while (true) {	
		Sleep(100);
		if (GetAsyncKeyState(SelectWindowKey)) {
			CurrentWindow curWin;
			afkFind afk(curWin.GetCurrentWindow(), AFKTime);
		}
	}
}