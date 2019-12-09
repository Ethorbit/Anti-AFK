#include "config.h"
#include <iostream>
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

void config::UpdateButtons() {
	PressButtons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
	buttonCount = 0;

	// Get button amount (Max allowed is 100):
	/*for (int i = 0; i < 100; i++) {
		if (PressButtons[i] > 0) {
			buttonCount++;
		}
	}*/
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
	DWORD newButtons[100];
	for (int i = 0; i < buttonCount; i++) {
		newButtons[i] = PressButtons[i];
	}

	std::cout << "What buttons should Anti-AFK send to the active window when you're AFK?" << std::endl;
	
	auto AddButton = [&newButtons, curButton]() {
		newButtons[curButton + 1] = Button.GetButton(); // Insert new button into newButtons array.
		std::wstring subKeyName = L"Button" + curButton + 1;
		Reg.writeSubkey(L"Software\\AntiAFK\\Buttons", &subKeyName[0], curButton + 1);
		std::cout << "Finished AddButton()" << std::endl;
	};

	auto RemoveButton = [buttons, &curButton, &newButtons]() {
		int remove = 0;
		for (int i = 0; i < (int)curButton; i++) {
			std::cout << i << ": " << Button.GetName(buttons[i]) << "\n"; // Print the name of every button the user added previously
		}

		std::cout << "Enter the number of the button you want to remove" << std::endl;
		std::cin >> remove;

		while (remove < 0 || remove > curButton) { 
			std::cin >> remove;
		}
		
		std::wstring subKeyName = L"Button" + remove;
		curButton -= 1;
		newButtons[remove] = 0;
	};

	auto GetInput = []() {
		int input = 0;
		std::cout << "Press 1 to add another button \n";
		std::cout << "Press 2 to remove a button \n";
		std::cout << "Press 3 to finish \n";

		std::cin >> input;
		while (input < 0 || input > 3) { 
			std::cin >> input;
		}
		
		return input;
	};

	std::thread* newThread = nullptr;

	while (true) {
		int inputNum = GetInput();
		if (inputNum == 1) {
			AddButton();
			//GetInput();
		}
		
		if (inputNum == 2) {
			GetInput();
			RemoveButton();
		}	
	} 

	newThread->join();
}