#include "config.h"
#include <iostream>
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
}

void config::Configure() {
	HConfigure = true;
	std::cout << "Press 1 to set the Select Window button\n";
	std::cout << "Press 2 to modify the buttons that Anti-AFK presses when you're AFK\n";
	std::cout << "Press 3 to set the amount of seconds to be AFK\n";
	std::cout << "Press 4 to set the frequency (in seconds) that Anti-AFK should press buttons when you're AFK.\n";
	std::cout << "Press 5 to cancel\n";

	int input = 0;
	Sleep(200); // Delay required or else the first letter of the input is not visible.
	std::cin >> input;
	while (input < 1 || input > 5) {
		std::cin >> input;

		if (std::cin.fail()) { // If the user enters something like a letter instead
			input = 0;
			std::cin.clear();
			std::cin.ignore();
		}
	}

	switch (input) { 
		case 1: 
			SetWindowKey();
			return;
		case 2:
			SetAntiAFKButtons();
			return;
		case 3:
			SetAFKTime();
			return;
		case 4:
			SetButtonFrequency();
			return;
		case 5:
			start Start;
			Start.startStuff();
	}
}

int config::inputButton() {
	int key = 0;
	while (key == 0) {
		for (int i = 0; i <= 256; i++) {
			if (GetAsyncKeyState(i) && i > 4 && i != 13) { // 1-4 is for mouse buttons, 13 is for Enter
				key = i;
			}
		}
	}

	return key;
}

void config::UpdateButtons() {
	PressButtons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
	buttonCount = 0;
	
	// Get button amount (Max allowed is 100):
	for (int i = 0; i < 100; i++) {
		if (PressButtons[i] > 0) {
			buttonCount++;
		}
	}
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

void config::SetWindowKey() {
	std::cout << "What button should be used to select the current window for Anti-AFK to use?" << std::endl;

	auto function = [this]() {
		bool exit = false;
		HCurButton = 0;
		while (HCurButton == 0) {}
		system("cls");
		std::wcout << "Set select window key to " << Button.GetName(HCurButton) << std::endl;
		Reg.writeSubkey(L"Software\\AntiAFK", L"SelectWindowKey", HCurButton);
		SetWindowKey(Reg.getKey(L"Software\\AntiAFK", L"SelectWindowKey"));
		Sleep(500);
	};

	std::thread t2(function);
	t2.join();
	config Config;
	Config.Configure();
}

void config::SetAntiAFKButtons() {
	auto RefreshScr = [](std::wstring newMsg) {
		system("cls");
		std::cout << "Press 1 to add a button \n";
		std::cout << "Press 2 to remove a button \n";
		std::cout << "Press 3 to list buttons added \n";
		std::cout << "Press 4 to finish \n";

		if (newMsg.length() > 0) {
			std::wcout << newMsg << std::endl;
		}
	};

	RefreshScr(L"");

	auto ListButtons = [this, RefreshScr]() {
		if (GetButtonCount() == 0) {
			RefreshScr(L"You have not added any buttons yet.");
		}

		DWORD* Buttons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");

		// Print the name of every button the user added:
		for (int i = 0; i < GetButtonCount(); i++) {
			std::cout << i;
			std::wcout << ": " << Button.GetName(Buttons[i]) << " (" << Buttons[i] << ")" << std::endl; 
		}
	};

	auto RemoveButton = [this, RefreshScr, ListButtons]() {
		char remove[100];
		int removeInt = -1;
		if (GetButtonCount() == 0) {
			RefreshScr(L"There are no keys to remove.");
		} else {
			ListButtons();

			std::cout << "Enter the number of the button you want to remove" << std::endl;
			std::cin >> remove;
			removeInt = atoi(remove);

			while (removeInt == 0 && strcmp(remove, "back") != 0 && strcmp(remove, "0") != 0 || removeInt < 0 || removeInt > GetButtonCount()) {
				std::cout << strlen(remove) << std::endl;
				ListButtons();
				std::cin >> remove;
				removeInt = atoi(remove);
			}

			while (strcmp(remove, "back") == 0) {
				RefreshScr(L"");
				return;
			}

			wchar_t subKeyName[200];
			swprintf_s(subKeyName, 200, L"Button%i", removeInt + 1);
			this->SetButtonCount(this->GetButtonCount() - 1);

			for (int i = 0; i <= this->GetButtonCount(); i++) {
				wchar_t valName[200], newName[200], valName2[200],newName2[200];
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

			std::cout << "Removed button from Anti-AFK" << std::endl;
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

	auto AddButton = [this, RefreshScr](int button) {
		SetButtonCount(GetButtonCount() + 1);
		system("cls");
		std::cout << "Press 1 to add a button \n";
		std::cout << "Press 2 to remove a button \n";
		std::cout << "Press 3 to list buttons added \n";
		std::cout << "Press 4 to finish \n";
		wchar_t buttonFormat[400];
		wchar_t buttonTimeFormat[400];
		swprintf_s(buttonFormat, sizeof(buttonFormat) / sizeof(wchar_t), L"Button%i", GetButtonCount());
		swprintf_s(buttonTimeFormat, sizeof(buttonTimeFormat) / sizeof(wchar_t), L"Button%iTime", GetButtonCount());
		Reg.writeSubkey(L"Software\\AntiAFK\\Buttons", buttonFormat, button);
		Reg.writeSubkey(L"Software\\AntiAFK\\ButtonTimes", buttonTimeFormat, 0); // Save as 0 seconds incase the next part never finishes

		DWORD holdTime = 0;
		std::wcout << "How many seconds should " << Button.GetName(button) << " be held for? (Max is: " << GetAFKTime() << ")" << std::endl;
	
		while (true) {
			std::cin >> holdTime;
			if (std::cin.fail()) {
				std::cin.clear();
				std::cin.ignore();
				std::cout << "Enter a valid amount of seconds:" << std::endl;		
			} else {
				if (holdTime > GetAFKTime()) {
					std::cout << "The button cannot be held longer than the AFK time (" << GetAFKTime() << ")" << std::endl;
				} else {
					break;
				}
			}
		}

		Reg.writeSubkey(L"Software\\AntiAFK\\ButtonTimes", buttonTimeFormat, holdTime);
		std::wcout << "Adding " << Button.GetName(button) << " to Anti-AFK. Total buttons added: " << GetButtonCount() << std::endl;
	};

	auto function = [this, AddButton, RemoveButton, GetInput, RefreshScr, ListButtons]() {
		while (true) {
			int input = 0;	
			if (input == 0) { // Get input when we need it
				input = GetInput();
			}
			
			if (input == 1) { // The user wants to add a key to Anti-AFK
				std::cout << "Press a key" << std::endl;
				Sleep(200); // Wait 200ms to stop GetAsyncKey from getting the key the user just pressed to add keys
				input = 0; // Reset the input so GetInput() is called again
				HCurButton = 0;
				while (HCurButton == 0) {}
				AddButton(HCurButton);
			}

			if (input == 2) { 
				input = 0;
				RemoveButton();
			}

			if (input == 3) { 
				ListButtons();
			}

			if (input == 4) { // User wants to exit
				if (this->GetButtonCount() == 0) {
					RefreshScr(L"You must add at least 1 key for Anti-AFK");
				} else {
					break; 
				}
			}
		}	
	};

	std::thread t1(function);
	t1.join();
	config Config;
	Config.Configure();
}