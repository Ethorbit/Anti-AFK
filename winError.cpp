#include "winError.h"
#include <iostream>

winError::winError(std::wstring extraInfo) {
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), GetUserDefaultLangID(), errBuffer, 400, NULL);
	swprintf_s(errMsg, sizeof(errMsg) / sizeof(wchar_t), L"%s - %s", extraInfo.c_str(), errBuffer);
	std::wcout << errMsg;
}