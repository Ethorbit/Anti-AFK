#include "start.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <string>
#include "button.h"
#include "CurrentWindow.h"
#include "config.h"
#include "Anti-AFK.h"

start::start() {
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
		while (true) {
			if (Exit == true) {
				break;
			}

			Sleep(100);
			if (GetAsyncKeyState(Config.GetWindowKey())) {
				Exit = true;
				doWin = true;
				break;
			}
		}
	};

	std::thread t1(WaitForSelection);
	std::string command;

	bool configure = false;
	auto function = [&command, &Exit, &Config, &configure]() {
		while (true) {
			if (command != "config") {
				if (Exit == true) { // Don't continue input loop if the current window was set and awaiting inactivity
					break;
				}

				std::getline(std::cin, command);
			}
			else {
				configure = true;
				Exit = true;
				break;
			}
		}
	};

	std::thread t2(function);

	while (true) {
		if (configure == true) {
			Config.Configure();
			break;
		}

		if (doWin == true) {
			system("cls");
			CurrentWindow curWin;
			HAntiAFKWindow = curWin.GetCurrentWindow(); // Let the keyboard hook know what the selected window is
			afkFind afk(Config.GetAFKTime());
			break;
		}
	}

	Exit = true; // Stop infinite loops before continuing
	t1.join();
	t2.join();
}