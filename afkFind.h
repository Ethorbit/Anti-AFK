#pragma once
#include <Windows.h>

class afkFind
{
	public:
		afkFind(int afkSeconds);

	private:
		bool isAFK = false;
};

