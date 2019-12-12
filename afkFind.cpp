#include "afkFind.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "start.h"
#include "Anti-AFK.h"

afkFind::afkFind(int afkSeconds) {
	HExit = false;
	std::cout << "Waiting for inactivity..." << std::endl;

	auto afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
	auto afkWait = [afkSeconds, &afkTime]() {
		while (true) {
			if (HExit == true) {
				break;
			} else {
				if (std::chrono::system_clock::now() > afkTime) {
					afkTime = std::chrono::system_clock::now() + std::chrono::seconds(afkSeconds);
					std::cout << "YOU ARE AFK" << std::endl;
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
					std::cout << "No longer AFK" << std::endl;
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