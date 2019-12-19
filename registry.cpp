#include "registry.h"
#include "winError.h"

registry::registry() {
	// Create default program keys:
	createKey(L"Software\\AntiAFK");
	createKey(L"Software\\AntiAFK\\Buttons");
	createKey(L"Software\\AntiAFK\\ButtonTimes");
	createKey(L"Software\\AntiAFK\\MouseCoords");
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

void registry::writeSubkeyString(const wchar_t* KeyName, const wchar_t* subKeyName, const wchar_t* Value) {
	// Open the program's registry key:
	HKEY regKey = openKey(KeyName);

	// Create subkey:
	LSTATUS writeKey = RegSetValueExW(regKey, subKeyName, NULL, REG_SZ, (LPBYTE)Value, 100);
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
		}
		else {
			winError err(L"Error trying to read a registry key");
		}

	}

	RegCloseKey(regKey);
	return keyValue;
}

std::wstring registry::getKeyString(const wchar_t* KeyName, const wchar_t* subKeyName) {
	DWORD keyValueSize = 200;
	wchar_t keyValue[200];
	HKEY regKey = openKey(KeyName);
	LSTATUS readKey = RegQueryValueExW(regKey, subKeyName, NULL, NULL, (LPBYTE)&keyValue, &keyValueSize);

	if (readKey != ERROR_SUCCESS) {
		if (ERROR_FILE_NOT_FOUND) { // If the file isn't found then create with default value
			writeSubkeyString(L"Software\\AntiAFK\\MouseCoords", subKeyName, L"0,0");
		} else {
			winError err(L"Error trying to read a registry key");
		}

	}

	RegCloseKey(regKey);
	return keyValue;
}

void registry::renameSubKey(const wchar_t* valName, const wchar_t* newName, bool button) {
	DWORD keyVal = NULL;
	LSTATUS delKey = NULL;

	// Replace subkey with new subkey:
	HKEY regKey = openKey(L"Software\\AntiAFK");
	if (button == true) {
		keyVal = getKey(L"Software\\AntiAFK\\Buttons", valName);
		delKey = RegDeleteKeyValueW(regKey, L"Buttons", valName);
		writeSubkey(L"Software\\AntiAFK\\Buttons", newName, keyVal);
	} else {
		keyVal = getKey(L"Software\\AntiAFK\\ButtonTimes", valName);
		delKey = RegDeleteKeyValueW(regKey, L"ButtonTimes", valName);
		writeSubkey(L"Software\\AntiAFK\\ButtonTimes", newName, keyVal);
	}

	if (delKey != ERROR_SUCCESS) {
		winError err(L"Error while trying to delete a registry subkey");
	}

	RegCloseKey(regKey);
}

void registry::renameSubKeyString(const wchar_t* valName, const wchar_t* newName) {
	std::wstring keyVal = getKeyString(L"Software\\AntiAFK\\MouseCoords", valName);
	std::wcout << "THE VALUE: " << keyVal << std::endl;
	HKEY regKey = openKey(L"Software\\AntiAFK");
	LSTATUS delKey = RegDeleteKeyValueW(regKey, L"MouseCoords", valName);
	writeSubkeyString(L"Software\\AntiAFK\\MouseCoords", newName, &keyVal[0]);

	if (delKey != ERROR_SUCCESS) {
		winError err(L"Error while trying to delete a registry subkey");
	}

	RegCloseKey(regKey);
}

std::vector<DWORD> registry::getAllSubkeys(const wchar_t* KeyName) {
	std::vector<DWORD> AllValues;
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
	
		if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)subKeyValues, &subKeyValueSize) == ERROR_SUCCESS) {
			AllValues.push_back(subKeyValues[0]); // It is unknown why the values are only written to index 0
		} else if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)subKeyValues, &subKeyValueSize) != ERROR_NO_MORE_ITEMS) {
			winError err(L"Error with enumerating registry subkeys");
		}

		if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)subKeyValues, &subKeyValueSize) == ERROR_NO_MORE_ITEMS) {
			break; // Save resources by stopping the loop when there are no more subkeys
		}
	}

	RegCloseKey(regKey);
	//std::cout << AllValues[0] << std::endl;
	return AllValues;
}

std::vector<std::wstring> registry::getAllSubkeysString(const wchar_t* KeyName) {
	std::vector<std::wstring> AllValues;
	HKEY regKey = openKey(KeyName);

	// Get the name of every subkey:
	wchar_t subKeyValues[1048];
	DWORD subKeyValueSize = 1048;
	DWORD regType = REG_SZ;
	DWORD index = 0;
	wchar_t subKeyNames[1048] = { 0 };

	// Loop through subkeys until none are found:
	for (int i = 0; i < 1048; i++) {
		subKeyValueSize = 1048; // Resize every time or else memory errors will occur

		if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)&subKeyValues, &subKeyValueSize) == ERROR_SUCCESS) {
			AllValues.push_back(subKeyValues); // It is unknown why the values are only written to index 0
		}
		else if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)&subKeyValues, &subKeyValueSize) != ERROR_NO_MORE_ITEMS) {
			winError err(L"Error with enumerating registry subkeys");
		}

		if (RegEnumValueW(regKey, i, (LPWSTR)&subKeyNames, &subKeyValueSize, NULL, &regType, (LPBYTE)&subKeyValues, &subKeyValueSize) == ERROR_NO_MORE_ITEMS) {
			break; // Save resources by stopping the loop when there are no more subkeys
		}
	}

	RegCloseKey(regKey);
	return AllValues;
}

void registry::removeButton(const wchar_t* ButtonName, bool button) {
	LSTATUS delKey = NULL;
	HKEY regKey = openKey(L"Software\\AntiAFK");

	if (button == true) {
		delKey = RegDeleteKeyValueW(regKey, L"Buttons", ButtonName);
	} else {
		delKey = RegDeleteKeyValueW(regKey, L"ButtonTimes", ButtonName);
	}
	
	if (delKey != ERROR_SUCCESS) {
		winError err(L"Error while trying to delete a registry subkey");
	}

	RegCloseKey(regKey);
}

void registry::removeCoords(const wchar_t* CoordName) {
	HKEY regKey = openKey(L"Software\\AntiAFK");
	LSTATUS delKey = RegDeleteKeyValueW(regKey, L"MouseCoords", CoordName);

	if (delKey != ERROR_SUCCESS) {
		winError err(L"Error while trying to delete a registry subkey");
	}

	RegCloseKey(regKey);
}