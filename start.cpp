#include "start.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <conio.h>
#include <thread>
#include "button.h"
#include "CurrentWindow.h"
#include "config.h"
#include "Anti-AFK.h"

start::start() {
	HCheckAFK = false;
	HConfigure = false;
}

void start::startStuff() {
	bool Exit = false;
	config Config;
	button Button;

	std::cout << "Type config to configure the program" << std::endl;
	std::wcout << "Press " << Button.GetName(Config.GetWindowKey()) << " to activate Anti-AFK for the current window" << std::endl;

	bool doWin = false;
	auto WaitForSelection = [&Config, &doWin, &Exit]() {
		// Get the current window when the user saved key bind is pressed:
		bool windowSelected = false;
		HCurButton = 0; // Reset in case the last button was the select window key
		while (!Exit) {
			Sleep(10);
			if (HCurButton == Config.GetWindowKey()) {
				Exit = true;
				doWin = true;
				break;
			}
		}
	};

	std::thread t1(WaitForSelection);
	
	while (true) {
		if (HCommand == "config" && HCurButton == VK_RETURN) {
			std::cout << std::endl;
			Exit = true; // Stop infinite loops before continuing
			Config.Configure();
			break;
		}

		if (doWin == true) {
			Exit = true; // Stop infinite loops before continuing
			system("cls");
			CurrentWindow curWin;
			HAntiAFKWindow = curWin.GetCurrentWindow(); // Let the keyboard hook know what the selected window is
			afkFind afk(Config.GetAFKTime());
			break;
		}
	}

	t1.join();
}