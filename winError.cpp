#include "winError.h"
#include <iostream>

winError::winError(std::wstring extraInfo) {
	if (GetLastError() == 0) {
		swprintf_s(errMsg, sizeof(errMsg) / sizeof(wchar_t), L"%s - An unexpected error occured. \n", extraInfo.c_str());
		std::wcout << errMsg;
	} else {
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), GetUserDefaultLangID(), errBuffer, 400, NULL);
		swprintf_s(errMsg, sizeof(errMsg) / sizeof(wchar_t), L"%s - %s \n", extraInfo.c_str(), errBuffer);
		std::wcout << errMsg;
	}
}