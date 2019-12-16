#include <Windows.h>
#include <iostream>
#include <vector>

#pragma once

class registry
{
	public: 
		registry();
		void writeSubkey(const wchar_t* KeyName, const wchar_t* subKeyName, DWORD Value);
		void writeSubkeyString(const wchar_t* KeyName, const char* subKeyName, const char* Value);
		void renameSubKey(const wchar_t* valName, const wchar_t* newName, bool button);
		void removeButton(const wchar_t* ButtonName, bool button);
		DWORD getKey(const wchar_t* KeyName, const wchar_t* subKeyName);
		DWORD* getAllSubkeys(const wchar_t*);
		std::vector<std::string> getAllSubkeysString(const wchar_t* KeyName);
	private:
		HKEY openKey(const wchar_t* KeyName);
		void createKey(const wchar_t* KeyName);
};

