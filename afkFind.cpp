#include "afkFind.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "start.h"
#include "config.h"
#include "registry.h"
#include "Anti-AFK.h"

afkFind::afkFind(int afkSeconds) {
	registry Reg;
	config Config;

	HExit = false;
	HCheckAFK = true;
	std::cout << "Waiting for inactivity..." << std::endl;

	auto afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
	
	auto afkWait = [this, afkSeconds, &afkTime, &Reg]() {
		while (true) {
			if (HExit == true) {
				break;
			} else {
				if (std::chrono::system_clock::now() > afkTime) {
					afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
					
					if (!isAFK) { // Only show the message once when AFK
						std::cout << "Inactivity detected, sending inputs to selected window..." << std::endl;
					}

					isAFK = true;
				}
			}
			Sleep(100);
		}
	};

	auto buttonPresser = [this, &Config, &Reg]() {
		int oldCursorX, oldCursorY = 0;
		INPUT input;
		POINT point;
		HWND PrevWnd = NULL;
		DWORD SavedButtons[100] {0};
		DWORD SavedButtonTimes[100] {0};
		DWORD* Buttons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
		for (int i = 0; i < Config.GetButtonCount(); i++) { // The memory address for the Buttons array gets replaced, so copy the array
			SavedButtons[i] = Buttons[i];
		}
		DWORD* ButtonTimes = Reg.getAllSubkeys(L"Software\\AntiAFK\\ButtonTimes");

		// Randomize values from saved buttons/mousecoord arrays:
		int RandKey = std::rand() % Config.GetButtonCount();
		int RandCoord = std::rand() % Config.GetCoordCount();

		// Randomize seed for randomized arrays:
		auto randomizeKey = [&RandKey, &Config]() {
			SYSTEMTIME time;
			GetLocalTime(&time);
			srand(time.wSecond);
		};

		auto holdButton = [&SavedButtons, &ButtonTimes, &RandKey, &RandCoord, &randomizeKey, &Config, &PrevWnd, &input, &oldCursorX, &oldCursorY]() {
			// Move the mouse:
			if (Config.GetCoordCount() > 0) {
				randomizeKey();
				RandCoord = std::rand() % Config.GetCoordCount();
				std::cout << "Random mouse index value: " << RandCoord << std::endl;
				input.type = INPUT_MOUSE;
				input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
				input.mi.mouseData = 0;
				input.mi.dx = Config.GetXCoords()[RandCoord];
				input.mi.dy = Config.GetYCoords()[RandCoord];
				HAutoMouse = true; // Don't become active if the mouse is auto moved
				SendInput(1, &input, sizeof(input));
				HAutoMouse = false;
			}
			
			// Hold button time:
			auto holdTime = std::chrono::system_clock::now() + std::chrono::seconds(ButtonTimes[RandKey]);

			// Make sure the keys are in random order every time:
			randomizeKey();
			RandKey = std::rand() % Config.GetButtonCount();

			auto parseButton = [&randomizeKey, &RandKey, &RandCoord, &Config, &SavedButtons, &PrevWnd, &input, &oldCursorX, &oldCursorY]() {
				// Send random buttons to target window:
				input.type = INPUT_KEYBOARD;
				input.ki.dwFlags = 0; //KEYEVENTF_UNICODE
				input.ki.wScan = MapVirtualKeyW(SavedButtons[RandKey], MAPVK_VK_TO_VSC); // Scan code required or DirectX games will not process keys
				input.ki.wVk = SavedButtons[RandKey];
				
				if (PrevWnd != NULL) {
					HAutoWindow = true;
				}

				SetForegroundWindow(HAntiAFKWindow); // Set the window to the selected one real quick for SendInput to work
				HAutoPress = true; // Don't let the keyboard hook think this is a key the user pressed manually
				SendInput(1, &input, sizeof(input));
				HAutoPress = false;
			};

			if (ButtonTimes[RandKey] == 0) {
				parseButton();
			}

			while (std::chrono::system_clock::now() <= holdTime) {
				Sleep(100); // Input keys at a natural speed
				parseButton();			
			}

			// Let go of the key:
			input.ki.dwFlags = KEYEVENTF_KEYUP;
			HAutoPress = true;
			SendInput(1, &input, sizeof(input));
			HAutoPress = false;

			if (PrevWnd != NULL) { // Set the window and cursor back to what they were before
				SetForegroundWindow(PrevWnd);
				SetCursorPos(oldCursorX, oldCursorY);
				HAutoWindow = false;
			} 
		};

		while (true) {
			if (HExit == true) {
				break;
			} else {
				if (isAFK == true) {
					if (GetForegroundWindow() != HAntiAFKWindow) {
						PrevWnd = GetForegroundWindow();
						GetCursorPos(&point);
						oldCursorX = point.x;
						oldCursorY = point.y;
					}
						
					// Bypass waiting for button frequency by running in other threads:
					std::thread t1(holdButton); 

					// Button frequency:
					auto endTime = std::chrono::system_clock::now() + std::chrono::seconds(Config.GetButtonFrequency());
					while (std::chrono::system_clock::now() < endTime) {}
					endTime = std::chrono::system_clock::now() + std::chrono::seconds(Config.GetButtonFrequency());
					t1.join();
				}
			}

			Sleep(100);
		}
	};

	auto infiniteLoop = [this, &afkTime, afkSeconds]() {
		while (true) {
			if (HExit == true) {
				break;
			} else {
				if (HActive == true) {
					afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
					HActive = false;

					if (isAFK == true) {
						std::cout << "You are no longer inactive from the window." << std::endl;
					}

					isAFK = false;
				}
			}

			Sleep(100);
		}
	};

	// Handle threads for multiple loops at once:
	std::thread t1(infiniteLoop);
	std::thread t2(afkWait);
	std::thread t3(buttonPresser);
	t1.join();
	t2.join();
	t3.join();
	system("cls");
	start Start;
	Start.startStuff();
}