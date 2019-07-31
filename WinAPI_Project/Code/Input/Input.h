#pragma once
#include "../Macro.h"
#include "../Defines.h"

class Input
{
	SINGLE_TONE(Input)

	bool Initialize();

	LRESULT CALLBACK ProcessWindowMessage(HWND, UINT, WPARAM, LPARAM);

	void ProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);
	void ProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);
};