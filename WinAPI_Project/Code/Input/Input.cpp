#include "Input.h"
#include <imm.h>
#pragma comment(lib,"imm32.lib")

#include "../D2DManager/D2DManager.h"

INIT_INSTACNE(Input)

Input::Input()
{
	strcpy(m_Text, "");
	strcpy(m_CombinationText, "");
	m_Chatting = L"";

	m_Mode = false;
}

Input::~Input()
{
}

bool Input::Initialize()
{
	return true;
}

LRESULT Input::ProcessWindowMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_IME_COMPOSITION:
	case WM_IME_NOTIFY:			// 한자입력...
	case WM_CHAR:				// 1byte 문자 (ex : 영어)
	case WM_KEYDOWN:
	case WM_KEYUP:
		//Alt키 눌렀을 때, 멈추는 현상을 해결하기위해
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		ProcessKeyboardMessage(hWnd, message, wParam, lParam);
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		ProcessMouseMessage(hWnd, message, wParam, lParam);
		break;

	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}

	return 0;
}

LRESULT Input::ProcessKeyboardMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int len = 0;
	// IME 핸들
	HIMC m_hIMC = nullptr;   

	//D2D1_RECT_F pos;
	//float x = 200;
	//float y = 200;
	//int sizeX = 100;
	//int sizeY = 100;
	//pos = { x, y, x + sizeX, y + sizeY };

	//GET_INSTANCE(D2DManager)->Render(L"ㅎㅇ", "메이플", "검은색", pos);

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_HANGEUL:
			m_Mode = !m_Mode;
			ChangeIMEMode(hWnd, m_Mode);
			break;
		}
		break;
	}

 	switch (message)
	{
	case WM_CHAR:				// 1byte 문자 (ex : 영어)
		break;
	case WM_IME_COMPOSITION:
		m_hIMC = ImmGetContext(hWnd);	// ime핸들을 얻는것
		if (lParam & GCS_RESULTSTR)
		{
			if ((len = ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, NULL, 0)) > 0)
			{
				// 완성된 글자가 있다.
				ImmGetCompositionString(m_hIMC, GCS_RESULTSTR, m_CombinationText, len);
				m_CombinationText[len] = 0;
				strcpy(m_Text + strlen(m_Text), m_CombinationText);
				memset(m_CombinationText, 0, 10);
				{
					//ConvertString();
					char szTemp[256] = "";
					sprintf(szTemp, "완성된 글자 : %s\r\n", m_Text);
					wcout << szTemp << endl;
				}
			}

		}
		else if (lParam & GCS_COMPSTR)
		{
			// 현재 글자를 조합 중이다.

			// 조합중인 길이를 얻는다.
			// str에  조합중인 문자를 얻는다.
			len = ImmGetCompositionString(m_hIMC, GCS_COMPSTR, NULL, 0);
			ImmGetCompositionString(m_hIMC, GCS_COMPSTR, m_CombinationText, len);
			m_CombinationText[len] = 0;
			{
				//ConvertString();
				char szTemp[256] = "";
				sprintf(szTemp, "조합중인 글자 : %s\r\n", m_CombinationText);
				wcout << szTemp << endl;

				//OutputDebugString(szTemp));
			}
		}

		ImmReleaseContext(hWnd, m_hIMC);	// IME 핸들 반환!!
		return 0;

	case WM_IME_NOTIFY:			// 한영키
		//
		break;
	}
	return 1;
}


void Input::ProcessMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
}

void Input::ConvertString()
{
	size_t size = 0;
	wchar_t wstr[MAX_LEN] = { 0, };

	mbstowcs_s(&size, wstr, MAX_LEN, m_Text, MAX_LEN);

	m_Chatting = wstr;

	//wcout << m_Chatting << endl;
}

void Input::ChangeIMEMode(HWND hWnd, bool korean)
{
	HIMC hIMC = ImmGetContext(hWnd);
	DWORD dwConv, dwSent;
	DWORD dwTemp;

	ImmGetConversionStatus(hIMC, &dwConv, &dwSent);

	dwTemp = dwConv & ~IME_CMODE_LANGUAGE;

	// 상태를 바꿉니다. 
	if (korean)
		dwTemp |= IME_CMODE_NATIVE;      // 한글 
	else
		dwTemp |= IME_CMODE_ALPHANUMERIC;  // 영문

	dwConv = dwTemp;

	ImmSetConversionStatus(hIMC, dwConv, dwSent);
	ImmReleaseContext(hWnd, hIMC);
}
