#include "button.h"
#include <iostream>
#include <Windows.h>
#include "winError.h"
#include "registry.h"

button::button() {
}

wchar_t* button::GetName(int virtKey) {
	wchar_t keyName[1024];
	UINT mappedKey = MapVirtualKeyW(virtKey, NULL);
	LONG lParam = (mappedKey << 16);
	int getKey = GetKeyNameTextW(lParam, keyName, 1024);
	std::wcout << "";
	return keyName;
}