#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include "CurrentWindow.h"
#include "winError.h"
#include "afkFind.h"
#include "button.h"
#include "config.h"

int main()
{
	config Config;
	std::cout << "Type config to configure the program" << std::endl;

	// Registry settings:
	std::cout << Config.GetButtonCount() << std::endl;
	
	if (Config.GetButtonCount() == 0) {
		Config.SetAntiAFKButtons();
	}
	
	if (Config.GetWindowKey() == 0) {
		Config.SetWindowKey();
	}

	button Button;
	std::wcout << "Press " << Button.GetName(Config.GetWindowKey()) << " to activate Anti-AFK for the current window" << std::endl;

	while (Config.GetAFKTime() == 0) {
		Config.SetAFKTime();
	}

	auto WaitForSelection = [&Config]() {
		// Get the current window when the user saved key bind is pressed:
		bool windowSelected = false;
		while (true) {
			Sleep(100);
			if (GetAsyncKeyState(Config.GetWindowKey())) {
				CurrentWindow curWin;
				afkFind afk(curWin.GetCurrentWindow(), Config.GetAFKTime());
			}
		}
	};

	std::thread t2(WaitForSelection);
	std::string command;
	
	while (command != "config") {
		std::getline(std::cin, command);
	}

	Config.Configure();
	t2.join();
}