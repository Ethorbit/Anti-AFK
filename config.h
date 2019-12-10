#pragma once
#include <Windows.h>

class config
{
	public:
		config();
		void SetAFKTime();
		void SetWindowKey();
		void SetAntiAFKButtons();

		DWORD GetWindowKey() {
			return SelectWindowKey;
		}

		DWORD GetAFKTime() {
			return AFKTime;
		}

		DWORD* GetButtons() {
			return PressButtons;
		}

		int GetButtonCount() {
			return buttonCount;
		}

		void SetButtonCount(int count) {
			buttonCount = count;
		}

		void Configure();
	private:
		void UpdateButtons();
		DWORD AFKTime = 0;
		DWORD SelectWindowKey = 0;
		DWORD* PressButtons = nullptr;
		int buttonCount = 0;
};

