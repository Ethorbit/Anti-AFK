#include <iostream>
#include <Windows.h>
#pragma once

class registry
{
	public: 
		registry();
		void writeSubkey(std::wstring subKeyName, DWORD Value);
		DWORD getKey(std::wstring subKeyName);
	private:
		HKEY openKey();
};

