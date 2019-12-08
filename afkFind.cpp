#include "afkFind.h"
#include <iostream>
#include <thread>
#include <chrono>

static bool Active;
static HWND AntiAFKWindow;

// Hook keyboard and mouse to find inactivity:
LRESULT CALLBACK KeyboardPress(int nCode, WPARAM wParam, LPARAM lParam) {
	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		if (GetForegroundWindow() == AntiAFKWindow) {
			Active = true; // User pressed a button
		}	
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int nCode, WPARAM wParam, LPARAM lParam) {
	if (GetForegroundWindow() == AntiAFKWindow) {
		Active = true; // User moved their mouse
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

afkFind::afkFind(HWND AFKWindow, int afkSeconds) {
	std::cout << "Waiting for inactivity..." << std::endl;
	AntiAFKWindow = AFKWindow;

	auto afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
	auto afkWait = [afkSeconds, &afkTime]() {
		while (true) {
			if (std::chrono::system_clock::now() > afkTime) {
				afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
				std::cout << "YOU ARE AFK" << std::endl;
			}
			Sleep(100);
		}
	};

	auto infiniteLoop = [&afkTime, afkSeconds]() {
		while (true) {
			if (Active == true) {
				afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
				Active = false;
				std::cout << "No longer AFK" << std::endl;
			}

			Sleep(100);
		}
	};

	// Call hooks for AFK detection:
	SetWindowsHookEx(WH_MOUSE_LL, MouseMove, NULL, NULL);
	SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardPress, NULL, NULL);

	// Handle threads for multiple loops at once:
	std::thread t1(infiniteLoop);
	std::thread t2(afkWait);

	// Get messages for program window:
	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	t1.join();
	t2.join();
}