#include "Anti-AFK.h"
#include <string>
#include <thread>
#include <future>
#include <conio.h>
#include "winError.h"
#include "config.h"
#include "start.h"
#include "afkFind.h"
#include "button.h"

// Hook keyboard and mouse for afkFind.cpp to find inactivity:
LRESULT CALLBACK KeyboardPress(int nCode, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		HCurButton = kbStruct->vkCode;
	 
		if (kbStruct->vkCode == VK_ESCAPE) {
			if (GetForegroundWindow() == GetConsoleWindow()) { // Don't exit if they do escape in another window
				HExit = true;
			}
		}

		if (GetForegroundWindow() == HAntiAFKWindow) {
			if (HAutoPress == false && HAutoWindow == false) { // Make sure the key or window was not automated from Anti-AFK
				HActive = true; // User pressed a button
			}
		}
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int nCode, WPARAM wParam, LPARAM lParam) {
	if (GetForegroundWindow() == HAntiAFKWindow) {
		if (HAutoWindow == false && HAutoMouse == false) {
			HActive = true;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HWND HAntiAFKWindow;
bool HExit, HActive, HAutoMouse, HCheckAFK, HConfigure, HAutoPress, HAutoWindow;
int HCurButton;
std::string HCommand;

int main()
{
	HAntiAFKWindow = NULL;
	HExit, HActive, HAutoMouse, HCheckAFK, HConfigure, HAutoPress, HAutoWindow = false;
	HCurButton = -1;
	HCommand = "";

	// Settings:
	config Config;
	if (Config.GetAFKTime() == 0) {
		Config.SetAFKTime();
	}

	//if (Config.GetButtonCount() == 0) {
	//	Config.SetAntiAFKButtons();
	//}
	
	if (Config.GetWindowKey() == 0) {
		Config.SetWindowKey();
	}

	// Handle ALL console outputting here (Doing std::cout on multiple threads causes unpredicted errors!)
	auto commandLoop = []() {	
		button Button;
		auto RefreshScr = [&Button](std::string text) {
			config Config;

			system("cls");
			std::cout << "Type config to configure the program" << std::endl;
			std::wcout << "Press " << Button.GetName(Config.GetWindowKey()) << " to activate Anti-AFK for the current window" << std::endl;

			if (text.length() > 0) {
				std::cout << text;
			}	
		};

		RefreshScr("");

		while (true) {
			std::cin >> HCommand;
			if (HCommand != "config")
			{
				std::cin >> HCommand;
			}
			else
			{
				std::cout << "Press 1 to set the Select Window button\n";
				std::cout << "Press 2 to modify the buttons that Anti-AFK presses when AFK\n";
				std::cout << "Press 3 to modify the mouse coordinates for Anti-AFK to move your mouse to when AFK\n";
				std::cout << "Press 4 to set the amount of seconds to be AFK\n";
				std::cout << "Press 5 to set the frequency (in seconds) that Anti-AFK should apply activity when AFK\n";
				std::cout << "Press 6 to cancel\n";

				std::future<int> future = std::async([] 
				{
					config Config;
					return Config.Configure();
				});

				int result = future.get();

				if (result == 1) 
				{
					std::cout << "What button should be used to select the current window for Anti-AFK to use?" << std::endl;

					std::future<int> button = std::async([]
					{
						config Config;
						return Config.SetWindowKey();
					});

					std::wcout << "Set select window key to " << Button.GetName(button.get()) << std::endl;
					Sleep(500);
					RefreshScr("");
					HCommand = "";
				}
				else if (result == 2) 
				{
					int futureOption = 0;
					while (futureOption != 4)
					{
						auto RefreshScr2 = [](std::wstring newMsg) {
							system("cls");
							std::cout << "Press 1 to add a button \n";
							std::cout << "Press 2 to remove a button \n";
							std::cout << "Press 3 to list buttons added \n";
							std::cout << "Press 4 to finish \n";

							if (newMsg.length() > 0) {
								std::wcout << newMsg << std::endl;
							}
						};

						RefreshScr2(L"");

						std::future<int> btnOption = std::async([]
						{
							config Config;
							return Config.GetButtonOption();
						});

						int option = btnOption.get();
						if (option == 1)
						{
							std::cout << "Press a key" << std::endl;
							const wchar_t* timeFormat = L"";
							std::future<const wchar_t*> btnTimeFormat = std::async([]
							{
								config Config;
								return Config.AddButton();
							});

							timeFormat = btnTimeFormat.get();
							config Config;
							button Button;
							std::wcout << "How many seconds should " << Button.GetName(Config.GetButtons()[Config.GetButtonCount() - 1]) << " be held for? (Max is: " << Config.GetAFKTime() << ")" << std::endl;
							Config.SetButtonTime(timeFormat);
							RefreshScr2(L"");
							std::wcout << "Adding " << Button.GetName(Config.GetButtons()[Config.GetButtonCount() - 1]) << " to Anti-AFK. Total buttons added: " << Config.GetButtonCount() << std::endl;
							Sleep(2000);
						}
						else if (option == 2)
						{
							std::cout << "2 was pressed" << std::endl;
							RefreshScr2(L"");
						}
						else if (option == 3)
						{
							std::cout << "3 was pressed" << std::endl;
							RefreshScr2(L"");
						}
						else if (option == 4)
						{
							futureOption = 4;
						}
					}

					RefreshScr("");
					HCommand = "";	
				}
				else if (result == 3) 
				{
					config Config;
					Config.SetAntiAFKMouseCoords();
				}
				else if (result == 4)
				{
					config Config;
					Config.SetAFKTime();
				}
				else if (result == 5)
				{
					config Config;
					Config.SetButtonFrequency();
				}
				else if (result == 6)
				{
					RefreshScr("");
					HCommand = "";
				}
			}
		}
	};


	// Get user input commands:
	//auto commandLoop = [&Config]() {
	//	button Button;

	//	while (true) {
	//		auto RefreshScr = [&Button, &Config](std::string text) {
	//			if (HCheckAFK == false && HConfigure == false) {
	//				system("cls");
	//				std::cout << "Type config to configure the program" << std::endl;
	//				std::wcout << "Press " << Button.GetName(Config.GetWindowKey()) << " to activate Anti-AFK for the current window" << std::endl;

	//				if (text.length() > 0) {
	//					std::cout << text;
	//				}
	//			}
	//		};

	//		if (HCheckAFK == false && HConfigure == false) { // Only get commands when Anti-AFK is not checking for inactivity
	//			char inputChar = _getch();
	//			if (inputChar != VK_RETURN && inputChar != VK_BACK) {
	//				if (std::isalpha(inputChar)) { // Don't add stuff like > to the string
	//					HCommand += inputChar;
	//					RefreshScr(HCommand);
	//				}
	//			}

	//			// Allow backspaces like normal:
	//			if (inputChar == VK_BACK) {
	//				if (HCommand.length() > 0) {
	//					HCommand.pop_back();
	//					RefreshScr(HCommand);
	//				}
	//			}

	//			// Compare value after they submit:
	//			if (inputChar == VK_RETURN) {
	//				if (HCommand != "config") {
	//					HCommand.clear(); // Reset it and try again.
	//					RefreshScr("");
	//				} else {
	//					HCommand.clear();
	//				}
	//			}
	//		}
	//	}
	//};


	std::thread t1(commandLoop);

	// Run the rest of the program in another thread:
	//start Start;
	//std::thread t1(&start::startStuff, Start);

	//t1.join();
	//t2.join();
	
	SetWindowsHookEx(WH_MOUSE_LL, MouseMove, NULL, NULL);
	SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardPress, NULL, NULL);

	// Get messages for program window:
	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	t1.join();
}