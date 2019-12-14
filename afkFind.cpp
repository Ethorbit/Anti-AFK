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
	auto afkWait = [afkSeconds, &afkTime, &Reg, &Config]() {
		DWORD* Buttons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
		DWORD* ButtonTimes = Reg.getAllSubkeys(L"Software\\AntiAFK\\ButtonTimes");

		// Randomize value from saved buttons array:
		int RandKey = std::rand() % Config.GetButtonCount();

		// Randomize seed for randomized array value:
		auto randomizeKey = [&RandKey, &Buttons, &Config]() {
			SYSTEMTIME time;
			GetLocalTime(&time);
			srand(time.wSecond);
			RandKey = std::rand() % Config.GetButtonCount();
		};

		while (true) {
			if (HExit == true) {
				break;
			} else {
				if (std::chrono::system_clock::now() > afkTime) {
					afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
					std::cout << "Inactivity detected, sending inputs to selected window..." << std::endl;

					//randomizeKey();
					//INPUT input;
					//input.type = INPUT_KEYBOARD;
					//input.ki.dwFlags = KEYEVENTF_UNICODE;
					//input.ki.wScan = MapVirtualKeyW(Buttons[RandKey], MAPVK_VK_TO_VSC); // Scan code required or DirectX games will not process keys
					//input.ki.wVk = Buttons[RandKey];
					//input.ki.time = 10000;
					//auto holdTime = std::chrono::system_clock::now() + std::chrono::seconds(ButtonTimes[RandKey]);
					//while (std::chrono::system_clock::now() <= holdTime) { 
					//	SetForegroundWindow(HAntiAFKWindow); // Set window as active so keys can be sent to it
					//	SendInput(1, &input, sizeof(input));
					//	Sleep(10);
					//}
				}
			}
			Sleep(100);
		}
	};

	auto infiniteLoop = [&afkTime, afkSeconds]() {
		while (true) {
			if (HExit == true) {
				break;
			} else {
				if (HActive == true) {
					afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
					HActive = false;
				}
			}

			Sleep(100);
		}
	};

	// Handle threads for multiple loops at once:
	std::thread t1(infiniteLoop);
	std::thread t2(afkWait);
	t1.join();
	t2.join();
	system("cls");
	std::cout << "Can't believe this actually worked!" << std::endl;
	start Start;
	Start.startStuff();
}