#include "Anti-AFK.h"
#include <string>
#include <thread>
#include <future>
#include <conio.h>
#include "winError.h"
#include "config.h"
#include "registry.h"
#include "afkFind.h"
#include "button.h"
#include "CurrentWindow.h"

// Hook keyboard and mouse for afkFind.cpp to find inactivity:
LRESULT CALLBACK KeyboardPress(int nCode, WPARAM wParam, LPARAM lParam) {
	KBDLLHOOKSTRUCT* kbStruct = (KBDLLHOOKSTRUCT*)lParam;

	if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
		HCurButton = kbStruct->vkCode;
	 
		if (kbStruct->vkCode == VK_ESCAPE) {
			if (GetForegroundWindow() == GetConsoleWindow()) { // Don't exit if they do escape in another window
				HExit = true;
				if (HCheckAFK == true)
				{
					HPause = (HPause == true) ? false : true;
				}	
			}
		}

		if (GetForegroundWindow() == HAntiAFKWindow) {
			if (HAutoPress == false && HAutoWindow == false) { // Make sure the key or window was not automated from Anti-AFK
				HActive = true; // User pressed a button
			}
		}
	}
	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseMove(int nCode, WPARAM wParam, LPARAM lParam) {
	if (GetForegroundWindow() == HAntiAFKWindow) {
		if (HAutoWindow == false && HAutoMouse == false) {
			HActive = true;
		}
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

HWND HAntiAFKWindow;
bool HExit, HActive, HAutoMouse, HCheckAFK, HConfigure, HAutoPress, HAutoWindow, HPause;
int HCurButton;
std::string HCommand;

int main()
{
	HAntiAFKWindow = NULL;
	HExit, HActive, HAutoMouse, HCheckAFK, HConfigure, HAutoPress, HAutoWindow, HPause = false;
	HCurButton = -1;
	HCommand = "";

	// Handle ALL console outputting here (Doing std::cout on multiple threads causes unpredicted errors!)
	auto commandLoop = []() {	
		button Button;
		auto RefreshScr = [&Button](std::string text) {
			config Config;

			system("cls");
			std::cout << "Type config to configure the program" << std::endl;
			if (Config.GetWindowKey() == 0)
			{
				std::cout << "Press (UNCONFIGURED KEY) to activate Anti-AFK for the current window" << std::endl;
			}
			else
			{
				std::wcout << "Press " << Button.GetName(Config.GetWindowKey()) << " to activate Anti-AFK for the current window" << std::endl;
			}
			
			if (text.length() > 0) {
				std::cout << text;
			}	
		};

		RefreshScr("");

		while (true) 
		{
			if (HCommand != "config")
			{
				std::cin >> HCommand;
			}
			else
			{
				HConfigure = true;
				std::cout << "Press 1 to set the Select Window button\n";
				std::cout << "Press 2 to modify the buttons that Anti-AFK presses when AFK\n";
				std::cout << "Press 3 to modify the mouse coordinates for Anti-AFK to move your mouse to when AFK\n";
				std::cout << "Press 4 to set the amount of seconds to be AFK\n";
				std::cout << "Press 5 to set the frequency (in seconds) that Anti-AFK should apply activity when AFK\n";
				std::cout << "Press 6 to cancel\n";

				std::future<int> future = std::async([] 
				{
					config Config;
					return Config.Configure();
				});

				int result = future.get();

				if (result == 1) 
				{
					std::cout << "What button should be used to select the current window for Anti-AFK to use?" << std::endl;

					std::future<int> button = std::async([]
					{
						config Config;
						return Config.SetWindowKey();
					});

					std::wcout << "Set select window key to " << Button.GetName(button.get()) << std::endl;
					Sleep(500);
					RefreshScr("");
					HCommand = "";
				}
				else if (result == 2) 
				{
					int futureOption = 0;
					auto RefreshScr2 = [](std::wstring newMsg)
					{
						system("cls");
						std::cout << "Press 1 to add a button \n";
						std::cout << "Press 2 to remove a button \n";
						std::cout << "Press 3 to list buttons added \n";
						std::cout << "Press 4 to finish \n";

						if (newMsg.length() > 0)
						{
							std::wcout << newMsg << std::endl;
						}
					};

					RefreshScr2(L"");
					while (futureOption != 4)
					{
						std::future<int> btnOption = std::async([]
						{
							config Config;
							return Config.GetButtonOption();
						});

						auto ListButtons = [&RefreshScr2]() 
						{
							config Config;
							registry Reg;
							button Button;

							if (Config.GetButtonCount() == 0) 
							{
								RefreshScr2(L"You have not added any buttons yet.");
							}
								
							std::vector<DWORD> Buttons = Reg.getAllSubkeys(L"Software\\AntiAFK\\Buttons");
								
							// Print the name of every button the user added:
							for (int i = 0; i < Config.GetButtonCount(); i++) 
							{
								std::cout << i;
								std::wcout << ": " << Button.GetName(Buttons[i]) << " (" << Buttons[i] << ")" << std::endl; 
							}
						};

						int option = btnOption.get();
						if (option == 1)
						{
							std::cout << "Press a key" << std::endl;
							const wchar_t* timeFormat = L"";
							std::future<const wchar_t*> btnTimeFormat = std::async([]
							{
								config Config;
								return Config.AddButton();
							});

							timeFormat = btnTimeFormat.get();
							config Config;
							button Button;
							std::wcout << "How many seconds should " << Button.GetName(Config.GetButtons()[Config.GetButtonCount() - 1]) << " be held for? (Max is: " << Config.GetAFKTime() << ")" << std::endl;
							Config.SetButtonTime(timeFormat);
							RefreshScr2(L"");
							std::wcout << "Added " << Button.GetName(Config.GetButtons()[Config.GetButtonCount() - 1]) << " to Anti-AFK. Total buttons added: " << Config.GetButtonCount() << std::endl;
						}
						else if (option == 2)
						{
							config Config;
							if (Config.GetButtonCount() == 0)
							{
								std::cout << "There are no buttons to remove." << std::endl;
							}
							else
							{
								std::cout << "Type back to cancel" << std::endl;
								ListButtons();
								std::cout << "Enter the number of the button you want to remove:" << std::endl;
								std::future<bool> lastMsg = std::async([&Config]
								{
									return Config.RemoveButton();
								});
								
								if (lastMsg.get() == false)
								{
									RefreshScr2(L"");
									std::cout << "Removed button from Anti-AFK." << std::endl;
								}
								else
								{
									RefreshScr2(L"");
								}
							}		
						}
						else if (option == 3)
						{
							ListButtons();
						}
						else if (option == 4)
						{
							futureOption = 4;
						}
					}

					RefreshScr("");
					HCommand = "";
				}
				else if (result == 3) 
				{
					int futureOption = 0;
					auto RefreshScr2 = [](std::wstring newMsg)
					{
						system("cls");
						std::cout << "Press 1 to add mouse coordinates \n";
						std::cout << "Press 2 to remove mouse coordinates \n";
						std::cout << "Press 3 to list mouse coordinates \n";
						std::cout << "Press 4 to finish \n";

						if (newMsg.length() > 0)
						{
							std::wcout << newMsg << std::endl;
						}
					};

					auto ListMouseCoords = [RefreshScr]() {
						config Config;
						registry Reg;
						if (Config.GetCoordCount() == 0) {
							RefreshScr("You have not added any mouse coordinates yet.");
						}
					
						std::vector<std::wstring> Coords = Reg.getAllSubkeysString(L"Software\\AntiAFK\\MouseCoords");
					
						// Print the name of every mouse coord the user added:
						for (int i = 0; i < Config.GetCoordCount(); i++) {
							std::cout << i;
							std::wcout << ": " << Coords[i] << std::endl;
						}
					};

					RefreshScr2(L"");
					while (futureOption != 4)
					{
						std::future<int> mouseOption = std::async([]
						{
							config Config;
							return Config.GetButtonOption();
						});

						int option = mouseOption.get();
						if (option == 1)
						{
							config Config;
							int x, y;
							std::cout << "Enter the X value:" << std::endl;
							std::future<int> xVal = std::async([&Config]
							{
								return Config.GetXCoordinate();
							});

							x = xVal.get();

							std::cout << "Enter the Y value:" << std::endl;
							std::future<int> yVal = std::async([&Config]
							{
								return Config.GetYCoordinate();
							});
							
							y = yVal.get();

							Config.AddMouseCoordinates(x, y);
							RefreshScr2(L"Added mouse coordinates for Anti-AFK");
						}
						else if (option == 2)
						{
							config Config;
							if (Config.GetCoordCount() == 0)
							{
								RefreshScr2(L"There are no coordinates to remove.");
							}
							else
							{
								std::cout << "Type back to cancel" << std::endl;
								ListMouseCoords();
								std::cout << "Enter the number of the mouse coords you want to remove:" << std::endl;
								std::future<bool> lastMsg = std::async([]
								{
									config Config;
									return Config.RemoveCoordinates();
								});
								if (lastMsg.get() == false) 
								{ 
									RefreshScr2(L"");
									std::cout << "Removed mouse coordinates from Anti-AFK" << std::endl;
								}
								else
								{
									RefreshScr2(L"");
								}
							}
						}
						else if (option == 3)
						{
							ListMouseCoords();
						}
						else if (option == 4)
						{
							futureOption = 4;
						}
					}
				}
				else if (result == 4)
				{
					std::cout << "Amount of seconds to be AFK before automatic activity is applied:" << std::endl;
					std::future<int> AFKTime = std::async([]
					{
						config Config;
						return Config.SetAFKTime();
					});

					int secAmount = AFKTime.get();
					if (secAmount > 1) 
					{
						std::cout << "Set AFK time to " << secAmount << " seconds." << std::endl;
					}
					else 
					{
						std::cout << "Set AFK time to 1 second." << std::endl;
					}

					Sleep(1000);
				}
				else if (result == 5)
				{
					std::cout << "Every time this amount of seconds pass by have Anti-AFK apply activity when AFK:" << std::endl;
					config Config;
					std::future<int> inputFreq = std::async([&Config]
					{
						return Config.SetButtonFrequency();
					});

					int secAmount = inputFreq.get();

					if (secAmount > 1) {
						std::cout << "Set Anti-AFK's button frequency to " << secAmount << " seconds." << std::endl;
					}
					else {
						std::cout << "Set Anti-AFK's button frequency to 1 second." << std::endl;
					}

					Sleep(1000);
				}
				else if (result == 6)
				{
					RefreshScr("");
					HCommand = "";
				}
			}

			HCurButton = 0;
			HConfigure = false;
			RefreshScr("");
		}
	};

	std::thread t1(commandLoop);

	auto buttonLoop = []()
	{
		// Get the current window when the user saved key bind is pressed:
		bool windowSelected = false;
		HCurButton = -1; // Reset in case the last button was the select window key
		config Config;
		while (true) 
		{
			Sleep(10);
			if (Config.GetWindowKey() != 0 && HCurButton == Config.GetWindowKey() && HConfigure == false)
			{
				//HCurButton = 0;
				//if (Config.GetAFKTime() == 0)
				//{
				//	std::cout << "ERROR: AFKTime not set! Go to config to set." << std::endl;
				//}
				//if (Config.GetButtonFrequency() == 0)
				//{
				//	std::cout << "ERROR: Input frequency not set! Go to config to set." << std::endl;
				//}
				//if (Config.GetButtonCount() == 0 && Config.GetCoordCount() == 0)
				//{
				//	std::cout << "ERROR: No buttons or mouse coordinates set! Go to config to add one." << std::endl;
				//}

				//if (Config.GetAFKTime() != 0 && Config.GetButtonFrequency() != 0)
				//{
				//	if (Config.GetButtonCount() != 0 || Config.GetCoordCount() != 0)
				//	{
						HPause = (HPause == true) ? false : true;
				
						if (HPause == true)
						{
							CurrentWindow curWin;
							HAntiAFKWindow = curWin.GetCurrentWindow(); // Let the keyboard hook know what the selected window is
							afkFind afk(Config.GetAFKTime());
							std::cout << "TEST" << std::endl;
						}
					}
			//	}
			//}
		}
	};

	std::thread t2(buttonLoop);

	SetWindowsHookEx(WH_MOUSE_LL, MouseMove, NULL, NULL);
	SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardPress, NULL, NULL);

	// Get messages for program window:
	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	t1.join();
	t2.join();
}