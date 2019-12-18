#include "config.h"
#include <string>
#include <thread>
#include "registry.h"
#include "button.h"
#include "start.h"
#include "Anti-AFK.h"

registry Reg;
button Button;

config::config() {
	AFKTime = Reg.getKey(L"Software\\AntiAFK", L"AFKTime");
	SelectWindowKey = Reg.getKey(L"Software\\AntiAFK", L"SelectWindowKey");
	BtnFreq = Reg.getKey(L"Software\\AntiAFK", L"ButtonFrequency");
	UpdateButtons();
	UpdateCoords();
}

int config::Configure() {
	HConfigure = true;

	char input[100];
	std::cin >> input;
	while (atoi(input) < 1 || atoi(input) > 6) 
	{
		std::cin >> input;
	}

	return atoi(input);
}

void config::UpdateButtons() {
	PressButtons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
	buttonCount = 0;
	
	// Get button amount (Max allowed is 100):
	for (int i = 0; i < 100; i++) 
	{
		if (i < PressButtons.size()) 
		{
			buttonCount++;
		}
	}
}

void config::UpdateCoords() {
	MouseCoords = Reg.getAllSubkeysString(L"Software\\AntiAFK\\MouseCoords");
	coordCount = 0;

	// Save coordinate amount:
	for (int i = 0; i < 100; i++) {
		if (i < MouseCoords.size()) {
			if (MouseCoords[i].length() > 0) {	
				coordCount++;
			}
		}
	}

	// Translate coordinates from strings to proper integers:
	for (int i = 0; i < coordCount; i++) {
		std::wstring Y = MouseCoords[i].substr(MouseCoords[i].rfind(L",") + 1, MouseCoords[i].length());
		std::wstring X = MouseCoords[i].substr(0, MouseCoords[i].rfind(L","));
		mouseX[i] = stoi(X);
		mouseY[i] = stoi(Y);
	}
}

int config::SetWindowKey() {
	HCurButton = 0;
	while (HCurButton == 0) 
	{
		Sleep(50); // Delay or else button is never able to get set
	}

	Reg.writeSubkey(L"Software\\AntiAFK", L"SelectWindowKey", HCurButton);
	SetWindowKey(Reg.getKey(L"Software\\AntiAFK", L"SelectWindowKey"));
	return HCurButton;
}

int config::SetAFKTime() {
	char secAmount[100] = "";

	std::cin >> secAmount;

	while (atoi(secAmount) <= 0) {
		std::cin >> secAmount;
	}

	Reg.writeSubkey(L"Software\\AntiAFK", L"AFKTime", atoi(secAmount));
	AFKTime = Reg.getKey(L"Software\\AntiAFK", L"AFKTime");

	return atoi(secAmount);
}

int config::SetButtonFrequency() 
{
	char secAmount[100] = "";

	while (atoi(secAmount) <= 0) 
	{
		std::cin >> secAmount;
	}

	while (atoi(secAmount) > GetAFKTime() && atoi(secAmount) > 0) 
	{
		std::cout << "The input frequency cannot be higher than the AFKTime (" << GetAFKTime() << ")" << std::endl;
		std::cin >> secAmount;
	}

	Reg.writeSubkey(L"Software\\AntiAFK", L"ButtonFrequency", atoi(secAmount));
	BtnFreq = Reg.getKey(L"Software\\AntiAFK", L"ButtonFrequency");

	return atoi(secAmount);
}

int config::GetButtonOption() 
{
	char option[100] = "";
	while (atoi(option) < 1 || atoi(option) > 4)
	{
		std::cin >> option;
	}
	return atoi(option);
}

const wchar_t* config::AddButton()
{
	HCurButton = 0;
	while (HCurButton == 0) 
	{
		Sleep(50);
	}

	int btnVal = HCurButton;
	buttonCount += 1;
	wchar_t buttonFormat[400] = L"";
	wchar_t buttonTimeFormat[400] = L"";
	swprintf_s(buttonFormat, sizeof(buttonFormat) / sizeof(wchar_t), L"Button%i", buttonCount);
	swprintf_s(buttonTimeFormat, sizeof(buttonTimeFormat) / sizeof(wchar_t), L"Button%iTime", buttonCount);
	Reg.writeSubkey(L"Software\\AntiAFK\\Buttons", buttonFormat, btnVal);
	Reg.writeSubkey(L"Software\\AntiAFK\\ButtonTimes", buttonTimeFormat, 0); // Save as 0 seconds incase the next part never finishes
	return buttonTimeFormat;
}

void config::SetButtonTime(const wchar_t* buttonTimeFormat)
{
	DWORD holdTime = 0;

	while (true) 
	{
		std::cin >> holdTime;
		if (std::cin.fail()) 
		{
			std::cin.clear();
			std::cin.ignore();	
		} else {
			if (holdTime > GetAFKTime()) 
			{
				std::cout << "The button cannot be held longer than the AFK time (" << GetAFKTime() << ")" << std::endl;
			} else 
			{
				break;
			}
		}
	}

	Reg.writeSubkey(L"Software\\AntiAFK\\ButtonTimes", buttonTimeFormat, holdTime);
}

bool config::RemoveButton()
{
	char remove[100] = "";
	int removeInt = -1;

	std::cin >> remove;
	removeInt = atoi(remove);	
	while (removeInt == 0 && strcmp(remove, "back") != 0 && strcmp(remove, "0") != 0 || removeInt < 0 || removeInt > GetButtonCount()) 
	{
		std::cin >> remove;
		removeInt = atoi(remove);
	}
	
	while (strcmp(remove, "back") == 0) 
	{
		return true;
	}
	
	wchar_t subKeyName[200] = L"";
	swprintf_s(subKeyName, 200, L"Button%i", removeInt + 1);
	SetButtonCount(GetButtonCount() - 1);
	
	for (int i = 0; i <= GetButtonCount(); i++) {
		wchar_t valName[200] = L"", newName[200] = L"", valName2[200] = L"",newName2[200] = L"";
		if (i > removeInt) {
			swprintf_s(valName, 200, L"Button%i", i + 1);
			swprintf_s(valName2, 200, L"Button%iTime", i + 1);
			if (i + 1 != 1) { // Don't replace anything if it's the first button
				swprintf_s(newName, 200, L"Button%i", i);
				swprintf_s(newName2, 200, L"Button%iTime", i);
				Reg.renameSubKey(valName, newName, true);
				Reg.renameSubKey(valName2, newName2, false);
			} 
		} else if (i == removeInt) {
			swprintf_s(valName, 200, L"Button%i", i + 1);
			swprintf_s(valName2, 200, L"Button%iTime", i + 1);
			Reg.removeButton(valName, true); 
			Reg.removeButton(valName2, false);
		}
	}

	return false;
}

int config::GetXCoordinate()
{
	int x;
	int minX = GetSystemMetrics(SM_CXSCREEN) * -1, maxX = GetSystemMetrics(SM_CXSCREEN);

	std::cin >> x;
	while (true) {
		if (x <= maxX && x >= minX) {
			break;
		}
	
		if (x > maxX || x < minX) {
			std::cout << "ERROR: X value out of range, max: " << maxX << std::endl;
			std::cin >> x;
		}
	
		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore();
			std::cin >> x;
		}
	}

	return x;
}

int config::GetYCoordinate()
{
	int y;
	int minY = GetSystemMetrics(SM_CYSCREEN) * -1, maxY = GetSystemMetrics(SM_CYSCREEN);

	std::cin >> y;
	while (true) {
		if (y <= maxY && y >= minY) {
			break;
		}

		if (y > maxY || y < minY) {
			std::cout << "ERROR: Y value out of range, max: " << maxY << std::endl;
			std::cin >> y;
		}

		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore();
			std::cin >> y;
		}
	}

	return y;
}

void config::AddMouseCoordinates(int x, int y)
{
	coordCount += 1;
	wchar_t format[200] = L"", coords[200] = L"";
	swprintf_s(format, 200, L"Coords%i", GetCoordCount());
	swprintf_s(coords, 100, L"%i,%i", x, y);
	Reg.writeSubkeyString(L"Software\\AntiAFK\\MouseCoords", format, coords);
}

bool config::RemoveCoordinates()
{
	char remove[100] = "";
	int removeInt = -1;

	std::cin >> remove;
	removeInt = atoi(remove);

	while (removeInt == 0 && strcmp(remove, "back") != 0 && strcmp(remove, "0") != 0 || removeInt < 0 || removeInt > coordCount) {
		std::cin >> remove;
		removeInt = atoi(remove);
	}

	while (strcmp(remove, "back") == 0) {
		return true;
	}

	wchar_t subKeyName[200] = L"";
	swprintf_s(subKeyName, 200, L"Coords%i", removeInt + 1);
	coordCount -= 1;
	for (int i = 0; i <= coordCount; i++) {
		wchar_t valName[200] = L"", newName[200] = L"";
		if (i > removeInt) {
			swprintf_s(valName, 200, L"Coords%i", i + 1);
			if (i + 1 != 1) { // Don't replace anything if it's the first coords
				swprintf_s(newName, 200, L"Coords%i", i);
				Reg.renameSubKeyString(valName, newName);
			}
		} else if (i == removeInt) {
			swprintf_s(valName, 200, L"Coords%i", i + 1);
			Reg.removeCoords(valName);
		}
	}

	return false;
}