#include "registry.h"
#include "winError.h"

registry::registry() {
	// Create default program keys:
	createKey(L"Software\\AntiAFK");
	createKey(L"Software\\AntiAFK\\Buttons");
}

void registry::createKey(const wchar_t* KeyName) {
	// Create default program key:
	HKEY regKey;
	LSTATUS setKey = RegCreateKeyEx(HKEY_CURRENT_USER,
		KeyName, NULL, NULL, REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS, NULL, &regKey, NULL);

	if (setKey != ERROR_SUCCESS) {
		winError(L"Error while trying to create a registry key for the program");
	}

	RegCloseKey(regKey);
}

HKEY registry::openKey(const wchar_t* KeyName) {
	// Open program's default registry key:
	HKEY regKey;
	LSTATUS openResult = RegOpenKeyEx(HKEY_CURRENT_USER, KeyName, NULL, KEY_ALL_ACCESS, &regKey);
	if (openResult != ERROR_SUCCESS) {
		winError err(L"Error while trying to open the program's registry key");
	}

	return regKey;
}

void registry::writeSubkey(const wchar_t* subKeyName, DWORD Value) {
	// Open the program's registry key:
	HKEY regKey = openKey(L"Software\\AntiAFK");

	// Create subkey:
	LSTATUS writeKey = RegSetValueExW(regKey, &subKeyName[0], NULL, REG_DWORD, (LPBYTE)&Value, sizeof(Value));
	if (writeKey != ERROR_SUCCESS) {
		winError err(L"Error while trying to write a value to the program's registry key");
	}

	RegCloseKey(regKey);
}

DWORD registry::getKey(const wchar_t* KeyName, const wchar_t* subKeyName) {
	DWORD keyValueSize = 400;
	DWORD keyValue = NULL;
	HKEY regKey = openKey(KeyName);
	LSTATUS readKey = RegQueryValueExW(regKey, subKeyName, NULL, NULL, (LPBYTE)&keyValue, &keyValueSize);

	if (readKey != ERROR_SUCCESS) {
		if (ERROR_FILE_NOT_FOUND) { // If the file isn't found then create with default value
			writeSubkey(subKeyName, 0);
		} else {
			winError err(L"Error trying to read a registry key");
		}
		
	}
	
	RegCloseKey(regKey);
	return keyValue;
}

DWORD* registry::getAllSubkeys(const wchar_t* KeyName) {
	DWORD allKeys[100];

	HKEY regKey = openKey(KeyName);

	// Get the name of every subkey:
	int iterator = 0;
	wchar_t subKeyNames[100];
	int subKeyNamesSize = 100;
	//LSTATUS enumKeys = RegEnumKeyExW(regKey, iterator, (LPWSTR)&subKeyNames, (LPDWORD)subKeyNamesSize, NULL, NULL, NULL, NULL);
	//if (enumKeys == ERROR_SUCCESS) { 
	//	iterator += 1;
	//}

	//// Get the value of every subkey:
	//for (int i = 0; i < iterator; i++) {
	//	std::cout << i << std::endl;
	//}

	RegCloseKey(regKey);
	return allKeys;
}