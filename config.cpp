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

void config::SetAFKTime() {
	int secAmount;
	std::cout << "Amount of seconds to be AFK before automatic activity is applied" << std::endl;
	std::cin >> secAmount;

	while (secAmount <= 0) {
		std::cin >> secAmount;
	}

	while (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore();
		std::cin >> secAmount;
	}

	Reg.writeSubkey(L"Software\\AntiAFK", L"AFKTime", secAmount);
	AFKTime = Reg.getKey(L"Software\\AntiAFK", L"AFKTime");
	system("cls");
	if (secAmount > 1) {
		std::cout << "Set AFK time to " << secAmount << " seconds." << std::endl;
	} else {
		std::cout << "Set AFK time to " << secAmount << " second." << std::endl;
	}
	config Config;
	Config.Configure();
}

void config::SetButtonFrequency() {
	int secAmount;
	std::cout << "Every time this amount of seconds pass by have Anti-AFK press a button when AFK:" << std::endl;
	std::cin >> secAmount;

	while (secAmount <= 0) {
		std::cin >> secAmount;
	}

	while (std::cin.fail()) {
		std::cin.clear();
		std::cin.ignore();
		std::cin >> secAmount;
	}

	Reg.writeSubkey(L"Software\\AntiAFK", L"ButtonFrequency", secAmount);
	BtnFreq = Reg.getKey(L"Software\\AntiAFK", L"ButtonFrequency");
	system("cls");
	if (secAmount > 1) {
		std::cout << "Set Anti-AFK's button frequency to " << secAmount << " seconds." << std::endl;
	} else {
		std::cout << "Set Anti-AFK's button frequency to " << secAmount << " second." << std::endl;
	}
	config Config;
	Config.Configure();
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

//void config::SetAntiAFKButtons() {
//	//auto RefreshScr = [](std::wstring newMsg) {
//	//	//system("cls");
//	//	//std::cout << "Press 1 to add a button \n";
//	//	//std::cout << "Press 2 to remove a button \n";
//	//	//std::cout << "Press 3 to list buttons added \n";
//	//	//std::cout << "Press 4 to finish \n";
//
//	//	if (newMsg.length() > 0) {
//	//		std::wcout << newMsg << std::endl;
//	//	}
//	//};
//
//	//RefreshScr(L"");
//
//	auto ListButtons = [this, RefreshScr]() {
//		if (GetButtonCount() == 0) {
//			RefreshScr(L"You have not added any buttons yet.");
//		}
//
//		DWORD* Buttons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
//
//		// Print the name of every button the user added:
//		for (int i = 0; i < GetButtonCount(); i++) {
//			std::cout << i;
//			std::wcout << ": " << Button.GetName(Buttons[i]) << " (" << Buttons[i] << ")" << std::endl; 
//		}
//	};
//
//	auto RemoveButton = [this, RefreshScr, ListButtons]() {
//		char remove[100] = "";
//		int removeInt = -1;
//		if (GetButtonCount() == 0) {
//			RefreshScr(L"There are no keys to remove.");
//		} else {
//			std::cout << "Type back to cancel" << std::endl;
//			ListButtons();
//			std::cout << "Enter the number of the button you want to remove" << std::endl;
//			std::cin >> remove;
//			removeInt = atoi(remove);
//
//			while (removeInt == 0 && strcmp(remove, "back") != 0 && strcmp(remove, "0") != 0 || removeInt < 0 || removeInt > GetButtonCount()) {
//				std::cout << strlen(remove) << std::endl;
//				ListButtons();
//				std::cin >> remove;
//				removeInt = atoi(remove);
//			}
//
//			while (strcmp(remove, "back") == 0) {
//				RefreshScr(L"");
//				return;
//			}
//
//			wchar_t subKeyName[200] = L"";
//			swprintf_s(subKeyName, 200, L"Button%i", removeInt + 1);
//			this->SetButtonCount(this->GetButtonCount() - 1);
//
//			for (int i = 0; i <= this->GetButtonCount(); i++) {
//				wchar_t valName[200] = L"", newName[200] = L"", valName2[200] = L"",newName2[200] = L"";
//				if (i > removeInt) {
//					swprintf_s(valName, 200, L"Button%i", i + 1);
//					swprintf_s(valName2, 200, L"Button%iTime", i + 1);
//					if (i + 1 != 1) { // Don't replace anything if it's the first button
//						swprintf_s(newName, 200, L"Button%i", i);
//						swprintf_s(newName2, 200, L"Button%iTime", i);
//						Reg.renameSubKey(valName, newName, true);
//						Reg.renameSubKey(valName2, newName2, false);
//					} 
//				} else if (i == removeInt) {
//					swprintf_s(valName, 200, L"Button%i", i + 1);
//					swprintf_s(valName2, 200, L"Button%iTime", i + 1);
//					Reg.removeButton(valName, true); 
//					Reg.removeButton(valName2, false);
//				}
//			}
//
//			std::cout << "Removed button from Anti-AFK" << std::endl;
//		}
//	};
//
//	auto GetInput = [RefreshScr]() {
//		int input = 0;
//		std::cin >> input;
//		while (input < 0 || input > 4) {
//			input = 0;
//			RefreshScr(L"");
//		}
//
//		while (input < 0 || input > 4) { 
//			std::cin >> input;
//		}
//
//		if (std::cin.fail()) {
//			std::cin.clear();
//			std::cin.ignore();
//			RefreshScr(L"Enter a valid number");
//			input = 0;
//		}
//
//		return input;
//	};
//
//	auto AddButton = [this, RefreshScr](int button) {
//		SetButtonCount(GetButtonCount() + 1);
//		system("cls");
//		std::cout << "Press 1 to add a button \n";
//		std::cout << "Press 2 to remove a button \n";
//		std::cout << "Press 3 to list buttons added \n";
//		std::cout << "Press 4 to finish \n";
//		wchar_t buttonFormat[400] = L"";
//		wchar_t buttonTimeFormat[400] = L"";
//		swprintf_s(buttonFormat, sizeof(buttonFormat) / sizeof(wchar_t), L"Button%i", GetButtonCount());
//		swprintf_s(buttonTimeFormat, sizeof(buttonTimeFormat) / sizeof(wchar_t), L"Button%iTime", GetButtonCount());
//		Reg.writeSubkey(L"Software\\AntiAFK\\Buttons", buttonFormat, button);
//		Reg.writeSubkey(L"Software\\AntiAFK\\ButtonTimes", buttonTimeFormat, 0); // Save as 0 seconds incase the next part never finishes
//
//		DWORD holdTime = 0;
//		std::wcout << "How many seconds should " << Button.GetName(button) << " be held for? (Max is: " << GetAFKTime() << ")" << std::endl;
//	
//		while (true) {
//			std::cin >> holdTime;
//			if (std::cin.fail()) {
//				std::cin.clear();
//				std::cin.ignore();
//				std::cout << "Enter a valid amount of seconds:" << std::endl;		
//			} else {
//				if (holdTime > GetAFKTime()) {
//					std::cout << "The button cannot be held longer than the AFK time (" << GetAFKTime() << ")" << std::endl;
//				} else {
//					break;
//				}
//			}
//		}
//
//		Reg.writeSubkey(L"Software\\AntiAFK\\ButtonTimes", buttonTimeFormat, holdTime);
//		std::wcout << "Adding " << Button.GetName(button) << " to Anti-AFK. Total buttons added: " << GetButtonCount() << std::endl;
//	};
//
//	auto function = [this, AddButton, RemoveButton, GetInput, RefreshScr, ListButtons]() {
//		while (true) {
//			int input = 0;	
//			if (input == 0) { // Get input when we need it
//				input = GetInput();
//			}
//			
//			if (input == 1) { // The user wants to add a key to Anti-AFK
//				std::cout << "Press a key" << std::endl;
//				Sleep(200); // Wait 200ms to stop GetAsyncKey from getting the key the user just pressed to add keys
//				input = 0; // Reset the input so GetInput() is called again
//				HCurButton = 0;
//				while (HCurButton == 0) {}
//				AddButton(HCurButton);
//			}
//
//			if (input == 2) { 
//				input = 0;
//				RemoveButton();
//			}
//
//			if (input == 3) { 
//				ListButtons();
//			}
//
//			if (input == 4) { // User wants to exit
//				if (GetButtonCount() == 0) {
//					RefreshScr(L"You must add at least 1 key for Anti-AFK");
//				} else {
//					break; 
//				}
//			}
//		}	
//	};
//
//	std::thread t1(function);
//	t1.join();
//	config Config;
//	Config.Configure();
//}

void config::SetAntiAFKMouseCoords() {
	auto RefreshScr = [](std::wstring newMsg) {
		system("cls");
		std::cout << "Press 1 to add mouse coordinates \n";
		std::cout << "Press 2 to remove mouse coordinates \n";
		std::cout << "Press 3 to list mouse coordinates \n";
		std::cout << "Press 4 to finish \n";

		if (newMsg.length() > 0) {
			std::wcout << newMsg << std::endl;
		}
	};

	auto GetInput = [RefreshScr]() {
		int input = 0;
		std::cin >> input;
		while (input < 0 || input > 4) {
			input = 0;
			RefreshScr(L"");
		}

		while (input < 0 || input > 4) {
			std::cin >> input;
		}

		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore();
			RefreshScr(L"Enter a valid number");
			input = 0;
		}

		return input;
	};

	auto ListMouseCoords = [this, RefreshScr]() {
		if (coordCount == 0) {
			RefreshScr(L"You have not added any mouse coordinates yet.");
		}

		std::vector<std::wstring> Coords = Reg.getAllSubkeysString(L"Software\\AntiAFK\\MouseCoords");

		// Print the name of every mouse coord the user added:
		for (int i = 0; i < coordCount; i++) {
			std::cout << i;
			std::wcout << ": " << Coords[i] << std::endl;
		}
	};

	auto RemoveMouseCoords = [this, RefreshScr, ListMouseCoords]() {
		char remove[100] = "";
		int removeInt = -1;
		if (coordCount == 0) {
			RefreshScr(L"There are no coordinates to remove.");
		} else {
			std::cout << "Type back to cancel" << std::endl;
			ListMouseCoords();
			std::cout << "Enter the number of the mouse coords you want to remove" << std::endl;
			std::cin >> remove;
			removeInt = atoi(remove);

			while (removeInt == 0 && strcmp(remove, "back") != 0 && strcmp(remove, "0") != 0 || removeInt < 0 || removeInt > coordCount) {
				ListMouseCoords();
				std::cin >> remove;
				removeInt = atoi(remove);
			}

			while (strcmp(remove, "back") == 0) {
				RefreshScr(L"");
				return;
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

			RefreshScr(L"Removed mouse coordinates from Anti-AFK");
		}
	};

	auto AddMouseCoords = [this, RefreshScr]() {
		int x, y;
		int minX = GetSystemMetrics(SM_CXSCREEN) * -1, maxX = GetSystemMetrics(SM_CXSCREEN);
		int minY = GetSystemMetrics(SM_CYSCREEN) * -1, maxY = GetSystemMetrics(SM_CYSCREEN);
	
		std::cout << "Enter the X value" << std::endl;
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

		std::cout << "Enter the Y value" << std::endl;
		std::cin >> y;
		while (true) {
			if (y <= maxY && y >= minY) {
				break;
			}

			if (y > maxY || y < minY) {
				std::cout << "ERROR: Y value out of range, max: " << maxY << std::endl;
				std::cin >> y;
			};

			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore();
				std::cin >> y;
			}
		}

		coordCount += 1;
		wchar_t format[200] = L"", coords[200] = L"";
		swprintf_s(format, 200, L"Coords%i", GetCoordCount());
		swprintf_s(coords, 100, L"%i,%i", x, y);
		Reg.writeSubkeyString(L"Software\\AntiAFK\\MouseCoords", format, coords);
		RefreshScr(L"Added mouse coordinates for Anti-AFK");
	};

	RefreshScr(L"");

	auto function = [this, AddMouseCoords, RemoveMouseCoords, ListMouseCoords, GetInput, RefreshScr]() {
		while (true) {
			int input = 0;
			if (input == 0) { // Get input when we need it
				input = GetInput();
			}

			if (input == 1) { // The user wants to add a key to Anti-AFK
				AddMouseCoords();
				input = 0; // Reset the input so GetInput() is called again
			}

			if (input == 2) {
				input = 0;
				RemoveMouseCoords();
			}

			if (input == 3) {
				ListMouseCoords();
			}

			if (input == 4) { // User wants to exit
				system("cls");
				break;
			}
		}
	};

	std::thread t1(function);
	t1.join();
	config Config;
	Config.Configure();
}