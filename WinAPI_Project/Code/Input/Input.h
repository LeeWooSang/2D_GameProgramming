#pragma once
#include "../Macro.h"
#include "../Defines.h"

constexpr int MAX_LEN = 256;

class Input
{
	SINGLE_TONE(Input)

	bool Initialize();

	LRESULT CALLBACK ProcessWindowMessage(HWND, UINT, WPARAM, LPARAM);

	LRESULT ProcessKeyboardMessage(HWND, UINT, WPARAM, LPARAM);
	void ProcessMouseMessage(HWND, UINT, WPARAM, LPARAM);

	void SetText(const wstring& text) { m_Chatting += text; }
	const wstring& GetText() const { return m_Chatting; }

	void ConvertString();
	void ChangeIMEMode(HWND, bool);

private:
	char m_Text[MAX_LEN];
	char m_CombinationText[10];

	wstring m_Chatting;

	bool m_Mode;
};