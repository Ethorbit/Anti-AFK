#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

class config
{
	public:
		config();
		void SetAFKTime();
		void SetWindowKey();
		void SetAntiAFKButtons();
		int inputButton();

		DWORD GetWindowKey() {
			return SelectWindowKey;
		}

		DWORD GetAFKTime() {
			return AFKTime;
		}

		DWORD* GetButtons() {
			return PressButtons;
		}

		std::vector<std::wstring> GetMouseCoords() {
			return MouseCoords;
		}

		int GetButtonCount() {
			return buttonCount;
		}

		int GetCoordCount() {
			return coordCount;
		}

		void SetButtonCount(int count) {
			buttonCount = count;
		}

		void Configure();

		void SetWindowKey(DWORD key) {
			SelectWindowKey = key;
		}

		int GetButtonFrequency() {
			return BtnFreq;
		}

		int* GetXCoords() {
			return mouseX;
		}

		int* GetYCoords() {
			return mouseY;
		}
	private:
		void UpdateButtons();
		void UpdateCoords();
		void SetButtonFrequency();
		void SetAntiAFKMouseCoords();
		DWORD AFKTime = 0;
		DWORD SelectWindowKey = 0;
		DWORD* PressButtons = nullptr;
		std::vector<std::wstring>MouseCoords;
		int buttonCount, coordCount, BtnFreq = 0;
		int mouseX[100], mouseY[100] {-1};
};

