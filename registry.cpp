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

void registry::writeSubkey(const wchar_t* KeyName, const wchar_t* subKeyName, DWORD Value) {
	// Open the program's registry key:
	HKEY regKey = openKey(KeyName);

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
			writeSubkey(L"Software\\AntiAFK", subKeyName, 0);
		} else {
			winError err(L"Error trying to read a registry key");
		}
		
	}
	
	RegCloseKey(regKey);
	return keyValue;
}

DWORD* registry::getAllSubkeys(const wchar_t* KeyName) {
	DWORD AllValues[1048] = {0};
	HKEY regKey = openKey(KeyName);

	// Get the name of every subkey:
	DWORD subKeyValues[1048] = {0};
	DWORD subKeyValueSize = 1048;
	DWORD regType = REG_DWORD;
	DWORD index = 0;
	wchar_t subKeyNames[1048] = {0};
	
	// Loop through subkeys until none are found:
	for (int i = 0; i < 1048; i++) {
		subKeyValueSize = 1048; // Resize every time or else memory errors will occur
	
		if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)&subKeyValues, &subKeyValueSize) == ERROR_SUCCESS) {
			AllValues[i] = subKeyValues[0]; // It is unknown why the values are only written to index 0
		} else if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)&subKeyValues, &subKeyValueSize) != ERROR_NO_MORE_ITEMS) {
			winError err(L"Error with enumerating registry subkeys");
		}

		if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)&subKeyValues, &subKeyValueSize) == ERROR_NO_MORE_ITEMS) {
			break; // Save resources by stopping the loop when there are no more subkeys
		}
	}

	RegCloseKey(regKey);
	return AllValues;
}