#include "config.h"
#include <iostream>
#include <string>
#include "registry.h"
#include "button.h"
#include <thread>

registry Reg;
button Button;

config::config() {
	AFKTime = Reg.getKey(L"Software\\AntiAFK", L"AFKTime");
	SelectWindowKey = Reg.getKey(L"Software\\AntiAFK", L"SelectWindowKey");
	UpdateButtons();
}

void config::Configure() {
	std::cout << "Press 1 to set the Select Window button \n";
	std::cout << "Press 2 to modify the buttons that Anti-AFK presses when you're AFK \n";

	int input = 0;
	while (input == 0) {
		std::cin >> input;
	}

	while (std::cin.fail()) { // If the user enters something like a letter instead
		input = 0;
		std::cin.clear();
		std::cin.ignore();
		std::cout << "Enter a valid number:" << std::endl;
	}

	switch (input) { 
		case 1: 
			SetWindowKey();
			return;
		case 2:
			SetAntiAFKButtons();
	}
}

void config::UpdateButtons() {
	PressButtons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
	buttonCount = 0;
	
	// Get button amount (Max allowed is 100):
	for (int i = 0; i < 100; i++) {
		if (PressButtons[i] > 0) {
			buttonCount++;
			std::cout << i << std::endl;
		}
	}
}

void config::SetAFKTime() {
	int secAmount;
	std::cout << "Amount of seconds to be AFK before automatic activity is applied" << std::endl;
	std::cin >> secAmount;
	Reg.writeSubkey(L"Software\\AntiAFK", L"AFKTime", secAmount);
	AFKTime = Reg.getKey(L"Software\\AntiAFK", L"AFKTime");
}

void config::SetWindowKey() {
	std::cout << "What button should be used to select the current window for Anti-AFK to use?" << std::endl;
	int key = Button.GetButton();
	Reg.writeSubkey(L"Software\\AntiAFK", L"SelectWindowKey", key);
	SelectWindowKey = Reg.getKey(L"Software\\AntiAFK", L"SelectWindowKey");
}

void config::SetAntiAFKButtons() {
	int curButton = buttonCount; // Give lambda functions access to private header variable
	DWORD* buttons = PressButtons; // Also give them access to buttons pointer

	// Add the currently existing buttons before adding the new ones:
	DWORD newButtons[100] {0};
	for (int i = 0; i < buttonCount; i++) {
		newButtons[i] = PressButtons[i];
	}

	auto RefreshScr = [](std::wstring newMsg) {
		system("cls");
		std::cout << "Press 1 to add another button \n";
		std::cout << "Press 2 to remove a button \n";
		std::cout << "Press 3 to list buttons added \n";
		std::cout << "Press 4 to finish \n";

		if (newMsg.length() > 0) {
			std::wcout << newMsg << std::endl;
		}
	};


	RefreshScr(L"");

	auto ListButtons = [buttons, &curButton, RefreshScr]() {
		if (curButton == 0) {
			RefreshScr(L"You have not added any buttons yet.");
		}

		for (int i = 0; i < curButton; i++) {
			std::cout << i << ": " << Button.GetName(buttons[i]) << "\n"; // Print the name of every button the user added previously
		}
	};

	auto RemoveButton = [buttons, &curButton, &newButtons, RefreshScr, ListButtons]() {
		int remove = 0;
		if (curButton == 0) {
			RefreshScr(L"There are no keys to remove.");
		} else {
			ListButtons();

			std::cout << "Enter the number of the button you want to remove" << std::endl;
			std::cin >> remove;

			while (remove < 0 || remove > curButton) {
				std::cin >> remove;
			}

			std::wstring subKeyName = L"Button" + remove;
			curButton -= 1;
			newButtons[remove] = 0;
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

	auto AddButton = [this, curButton, RefreshScr, &newButtons](int button) {
		std::wstring conText = L"Adding ";
		conText += Button.GetName(button);
		conText += L" to Anti-AFK. Total buttons added: ";
		conText += this->GetButtonCount();
		RefreshScr(conText);
		wchar_t buttonFormat[400];
		this->SetButtonCount(this->GetButtonCount() + 1);
		swprintf_s(buttonFormat, sizeof(buttonFormat) / sizeof(wchar_t), L"Button %i", this->GetButtonCount());
		newButtons[this->GetButtonCount()] = button; // Insert new button into newButtons array.
		Reg.writeSubkey(L"Software\\AntiAFK\\Buttons", buttonFormat, button);
	};

	auto buttonLoop = []() {
		int key = 0;
		while (key == 0) {
			for (int i = 0; i <= 256; i++) {
				if (GetAsyncKeyState(i) && i > 4) { // 1-4 is for mouse buttons
					key = i;
				}
			}
		}

		return key;
	};

	auto function = [AddButton, RemoveButton, GetInput, buttonLoop, curButton, RefreshScr, ListButtons]() {
		while (true) {
			int input = 0;	
			if (input == 0) { // Get input when we need it
				input = GetInput();
			}
			
			if (input == 1) { // The user wants to add a key to Anti-AFK
				std::cout << "Press a key" << std::endl;
				Sleep(200); // Wait 200ms to stop GetAsyncKey from getting the key the user just pressed to add keys
				input = 0; // Reset the input so GetInput() is called again
				if (buttonLoop() > 1 && buttonLoop() != 13) { // 13 is the enter key
					AddButton(buttonLoop());
				}
			}

			if (input == 2) { // The user wants to remove one of the keys they added
				input = 0;
				RemoveButton();
			}

			if (input == 3) { // User wants a list of buttons added previously
				ListButtons();
			}

			if (input == 4) { // User wants to exit
				if (curButton == 0) {
					RefreshScr(L"You must add at least 1 key for Anti-AFK");
				} else {
					break; 
				}
			}
		}	
	};

	std::thread t1(function);
	t1.join();
}