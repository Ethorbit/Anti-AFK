#include <Windows.h>
#include <iostream>
#include <vector>

#pragma once

class registry
{
	public: 
		registry();
		void writeSubkey(const wchar_t* KeyName, const wchar_t* subKeyName, DWORD Value);
		void writeSubkeyString(const wchar_t* KeyName, const wchar_t* subKeyName, const wchar_t* Value);
		void renameSubKey(const wchar_t* valName, const wchar_t* newName, bool button);
		void renameSubKeyString(const wchar_t* valName, const wchar_t* newName);
		void removeButton(const wchar_t* ButtonName, bool button);
		void removeCoords(const wchar_t* CoordName);
		DWORD getKey(const wchar_t* KeyName, const wchar_t* subKeyName);
		std::wstring getKeyString(const wchar_t* KeyName, const wchar_t* subKeyName);
		std::vector<DWORD> getAllSubkeys(const wchar_t*);
		std::vector<std::wstring> getAllSubkeysString(const wchar_t* KeyName);
	private:
		HKEY openKey(const wchar_t* KeyName);
		void createKey(const wchar_t* KeyName);
};

