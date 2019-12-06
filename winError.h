#pragma once
#include <iostream>
#include <Windows.h>

class winError
{
	public:
		winError(std::wstring extraInfo);

		wchar_t* GetErrorMsg() {
			return errMsg;
		}
	private:
		wchar_t errMsg[500];
		wchar_t errBuffer[400];
};

