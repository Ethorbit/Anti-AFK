#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>

class config
{
	public:
		config();
		void SetAFKTime();
		int SetWindowKey();
		//void SetAntiAFKButtons();
		int GetButtonOption();
		const wchar_t* AddButton();
		void SetButtonTime(const wchar_t* buttonTimeFormat);

		DWORD GetWindowKey() {
			return SelectWindowKey;
		}

		DWORD GetAFKTime() {
			return AFKTime;
		}

		std::vector<DWORD> GetButtons() {
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

		int Configure();

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

		void SetAntiAFKMouseCoords();
		void SetButtonFrequency();

	private:
		void UpdateButtons();
		void UpdateCoords();	
		DWORD AFKTime = 0;
		DWORD SelectWindowKey = 0;
		std::vector<DWORD> PressButtons;
		std::vector<std::wstring>MouseCoords;
		int buttonCount, coordCount, BtnFreq = 0;
		int mouseX[100], mouseY[100] {-1};
};

