#include "button.h"
#include <iostream>
#include <Windows.h>
#include "winError.h"
#include "registry.h"

button::button() {
}

int button::GetButton() {
	int key = 0;
	while (key == 0) {
		for (int i = 0; i <= 256; i++) {
			if (GetAsyncKeyState(i) && i > 4) { // 1-4 is for mouse buttons
				key = i;
			}
		} 

		Sleep(100);
	}

	return key;
}

wchar_t* button::GetName(int virtKey) {
	wchar_t keyName[1024];
	UINT mappedKey = MapVirtualKeyW(virtKey, NULL);
	LONG lParam = (mappedKey << 16);
	int getKey = GetKeyNameTextW(lParam, keyName, 1024);

	if (getKey == 0) {
		registry Registry;
		Registry.writeSubkey(L"SelectWindowKey", 0); // Reset bind since there's something wrong with it
		winError err(L"Error converting the chosen key bind into text");
		std::abort(); // Stop the program to avoid further errors with this invalid bind
	}

	std::wcout << "";
	return keyName;
}