#include "INI.h"
#include <Windows.h>
#include "winError.h"
#include <iostream>
#include <fstream>

INI::INI() {
	// Create path:
	std::wstring INIPath;
	INIPath.resize(MAX_PATH);
	GetModuleFileNameW(NULL, &INIPath[0], 531);
	INIPath.erase(INIPath.rfind('\\'), INIPath.size());
	INIPath += L"\\Config";

	// Create directory based on the path:
	BOOL newDir = CreateDirectoryW(&INIPath[0], NULL);
	if (newDir == 0) {
		winError(L"Error while trying to create the directory for AntiAFK.ini");
	}

	// Create the INI file:
	std::ofstream file(INIPath + L"\\AntiAFK.ini");
	file.write("TEST", 100);
	if (!file.is_open()) {
		std::cerr << "Error trying to open Config/AntiAfk.ini in the program's directory!" << std::endl;
		return;
	}
}