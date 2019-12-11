#include <iostream>
#include <windows.h>
#include "winError.h"
#include "config.h"
#include "start.h"

int main()
{
	config Config;
	
	// Registry settings:
	std::cout << Config.GetButtonCount() << std::endl;
	
	if (Config.GetAFKTime() == 0) {
		Config.SetAFKTime();
	}

	if (Config.GetButtonCount() == 0) {
		Config.SetAntiAFKButtons();
	}
	
	if (Config.GetWindowKey() == 0) {
		Config.SetWindowKey();
	}

	start Start;
}