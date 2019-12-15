#pragma once
#include <windows.h>
#include <iostream>

//HWND curWin = NULL;
extern HWND HAntiAFKWindow;
extern bool HExit, HActive, HCheckAFK, HConfigure, HAutoPress;
extern int HCurButton;
extern std::string HCommand;