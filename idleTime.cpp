#include "idleTime.h"
#include <iostream>
#include <Windows.h>

idleTime::idleTime(int inputSec) {
	//int seconds = inputSec * 1000;

	//LASTINPUTINFO inputInfo;
	//inputInfo.cbSize = sizeof(inputInfo);
	//BOOL lastInput = GetLastInputInfo(&inputInfo);

	//// If we're able to get the last input time:
	//if (lastInput != 0) {
	//	// Calculate number of seconds:
	//	int idlems = GetTickCount64() - inputInfo.dwTime;

	//	// Wait for user specified amount of seconds to pass:
	//	while (true) {	
	//		std::cout << GetTickCount64() - inputInfo.dwTime << std::endl;
	//		idlems = GetTickCount64() - inputInfo.dwTime;
	//		
	//		// When the time has passed:
	//		if (idlems > seconds) {
	//			std::cout << "You've gone inactive! Pressing hotkeys.." << std::endl;
	//		}

	//		Sleep(1000);
	//	}
	//}
	//// If we're not able to get the last input time for some reason:
	//else {
	//	char errBuffer[200];
	//	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), GetUserDefaultLangID(), errBuffer, 200, NULL);
	//	std::cerr << "Failed to get last input time! " << errBuffer << std::endl;
	//}
}