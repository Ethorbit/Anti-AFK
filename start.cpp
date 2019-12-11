#include "start.h"
#include <iostream>
#include <windows.h>
#include <thread>
#include <string>
#include "button.h"
#include "CurrentWindow.h"
#include "afkFind.h"
#include "config.h"

start::start() {
	bool Exit = false;
	config Config;
	button Button;
	std::cout << "Type config to configure the program" << std::endl;
	std::wcout << "Press " << Button.GetName(Config.GetWindowKey()) << " to activate Anti-AFK for the current window" << std::endl;

	auto WaitForSelection = [&Config, &Exit]() {
		// Get the current window when the user saved key bind is pressed:
		bool windowSelected = false;
		while (true) {
			if (Exit == true) { // Stop to prevent duplicate window loops
				break;
			} else {
				Sleep(100);
				if (GetAsyncKeyState(Config.GetWindowKey())) {
					Exit = true;
					CurrentWindow curWin;
					afkFind afk(curWin.GetCurrentWindow(), Config.GetAFKTime());
				}
			}
		}
	};

	std::thread t2(WaitForSelection);
	std::string command;

	while (command != "config") {
		if (Exit == true) { // Don't continue input loop if the current window was set and awaiting inactivity
			break;
		}
		std::getline(std::cin, command);
	}
	
	Exit = true; // Stop infinite loops before continuing
	t2.join();
	Config.Configure();
}
