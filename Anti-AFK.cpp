#include <iostream>
#include <windows.h>
#include <winuser.h>
#include "registry.h"
#include "CurrentWindow.h"
#include "winError.h"
#include "afkFind.h"
#include "button.h"

int main()
{
	std::cout << "Type config to configure the program" << std::endl;

	// Registry settings:
	registry Reg;
	DWORD AFKTime = Reg.getKey(L"Software\\AntiAFK", L"AFKTime");
	DWORD SelectWindowKey = Reg.getKey(L"Software\\AntiAFK", L"SelectWindowKey");
	DWORD* PressButtons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");

	button Button;
	if (SelectWindowKey == 0) {
		std::cout << "What button should be used to select the current window for Anti-AFK to use?" << std::endl;
		int key = Button.GetButton();
		Reg.writeSubkey(L"SelectWindowKey", key);
		SelectWindowKey = Reg.getKey(L"Software\\AntiAFK", L"SelectWindowKey");
	}

	std::wcout << "Press " << Button.GetName(SelectWindowKey) << " to activate Anti-AFK for the current window" << std::endl;

	while (AFKTime == 0) {
		int secAmount;
		std::cout << "Amount of seconds to be AFK before automatic activity is applied" << std::endl;
		std::cin >> secAmount;
		Reg.writeSubkey(L"AFKTime", secAmount);
		AFKTime = Reg.getKey(L"Software\\AntiAFK", L"AFKTime");
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