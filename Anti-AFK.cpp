#include "Anti-AFK.h"
#include <iostream>
#include "winError.h"
#include "config.h"
#include "start.h"
#include <thread>
#include "afkFind.h"

// Hook keyboard and mouse for afkFind.cpp
LRESULT CALLBACK KeyboardPress(int nCode, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		if (kbStruct->vkCode == VK_ESCAPE) {
			HExit = true;
		}

		if (GetForegroundWindow() == HAntiAFKWindow) {
			HActive = true; // User pressed a button
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int nCode, WPARAM wParam, LPARAM lParam) {
	if (GetForegroundWindow() == HAntiAFKWindow) {
		HActive = true;
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HWND HAntiAFKWindow;
bool HExit;
bool HActive;
int HCurButton;

int main()
{
	HAntiAFKWindow = NULL;
	HExit = false;
	HActive = false;
	HCurButton = -1;

	config Config;
	
	// Registry settings:
	std::cout << Config.GetButtonCount() << std::endl;
	
	if (Config.GetAFKTime() == 0) {
		Config.SetAFKTime();
	}

	if (Config.GetButtonCount() == 0) {
		Config.SetAntiAFKButtons();
	}
	
	if (Config.GetWindowKey() == 0) {
		Config.SetWindowKey();
	}

	SetWindowsHookEx(WH_MOUSE_LL, MouseMove, NULL, NULL);
	SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardPress, NULL, NULL);
	
	// Run the rest of the program in another thread:
	start Start;
	std::thread t1(&start::startStuff, Start);

	// Get messages for program window:
	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	t1.join();
}