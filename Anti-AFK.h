#pragma once
#include <windows.h>
#include <iostream>

//HWND curWin = NULL;
extern HWND HAntiAFKWindow;
extern bool HExit, HActive, HAutoMouse, HCheckAFK, HConfigure, HAutoPress, HAutoWindow;
extern int HCurButton;
extern std::string HCommand;