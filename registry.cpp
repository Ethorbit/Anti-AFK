#include "registry.h"
#include "winError.h"

registry::registry() {
	// Create default program key:
	HKEY regKey;
	LSTATUS setKey = RegCreateKeyEx(HKEY_CURRENT_USER, 
	L"Software\\AntiAFK", NULL, NULL, REG_OPTION_NON_VOLATILE, 
	KEY_ALL_ACCESS, NULL, &regKey, NULL);

	if (setKey != ERROR_SUCCESS) {
		winError(L"Error while trying to create a registry key for the program");
	}

	RegCloseKey(regKey);
}

HKEY registry::openKey() {
	// Open program's default registry key:
	HKEY regKey;
	LSTATUS openResult = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\AntiAFK", NULL, KEY_ALL_ACCESS, &regKey);
	if (openResult != ERROR_SUCCESS) {
		winError err(L"Error while trying to open the program's registry key");
	}

	return regKey;
}

void registry::writeSubkey(std::wstring subKeyName, DWORD Value) {
	// Open the program's registry key:
	HKEY regKey = openKey();

	// Create subkey:
	LSTATUS writeKey = RegSetValueExW(regKey, &subKeyName[0], NULL, REG_DWORD, (LPBYTE)&Value, sizeof(Value));
	if (writeKey != ERROR_SUCCESS) {
		winError err(L"Error while trying to write a value to the program's registry key");
	}

	RegCloseKey(regKey);
}

DWORD registry::getKey(std::wstring subKeyName) {
	DWORD keyValueSize = 400;
	DWORD keyValue = NULL;
	HKEY regKey = openKey();
	LSTATUS readKey = RegQueryValueExW(regKey, &subKeyName[0], NULL, NULL, (LPBYTE)&keyValue, &keyValueSize);

	if (readKey != ERROR_SUCCESS) {
		if (ERROR_FILE_NOT_FOUND) { // If the file then create with default value
			writeSubkey(subKeyName, 0);
		} else {
			winError err(L"Error trying to read a registry key");
		}
		
	}
	
	return keyValue;
}