#include "Anti-AFK.h"
#include <string>
#include <thread>
#include <conio.h>
#include "winError.h"
#include "config.h"
#include "start.h"
#include "afkFind.h"
#include "button.h"

// Hook keyboard and mouse for afkFind.cpp to find inactivity:
LRESULT CALLBACK KeyboardPress(int nCode, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

	if (kbStruct->vkCode == CTRL_C_EVENT) {
		std::cout << "IT WORKED!" << std::endl;
	}

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		HCurButton = kbStruct->vkCode;

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
bool HExit, HActive, HCheckAFK, HConfigure;
int HCurButton;
std::string HCommand;

int main()
{
	HAntiAFKWindow = NULL;
	HExit, HActive, HCheckAFK, HConfigure = false;
	HCurButton = -1;
	HCommand = "";

	// Settings:
	config Config;
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
	
	// Get user input commands:
	auto commandLoop = [&Config]() {
		button Button;

		while (true) {
			auto RefreshScr = [&Button, &Config](std::string text) {
				system("cls");
				std::cout << "Type config to configure the program" << std::endl;
				std::wcout << "Press " << Button.GetName(Config.GetWindowKey()) << " to activate Anti-AFK for the current window" << std::endl;

				if (text.length() > 0) {
					std::cout << text;
				}
			};

			if (!HCheckAFK && !HConfigure) { // Only get commands when Anti-AFK is not checking for inactivity
				char inputChar = _getch();
				if (inputChar != VK_RETURN && inputChar != VK_BACK) {
					if (std::isalpha(inputChar)) { // Don't add stuff like > to the string
						HCommand += inputChar;
						RefreshScr(HCommand);
					}
				}

				// Allow backspaces like normal:
				if (inputChar == VK_BACK) {
					if (HCommand.length() > 0) {
						HCommand.pop_back();
						RefreshScr(HCommand);
					}
				}

				// Compare value after they submit:
				if (inputChar == VK_RETURN) {
					if (HCommand != "config") {
						HCommand.clear(); // Reset it and try again.
						RefreshScr("");
					} else {
						HCommand.clear();
					}
				}
			}
		}
	};

	std::thread t2(commandLoop);

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
	t2.join();
}