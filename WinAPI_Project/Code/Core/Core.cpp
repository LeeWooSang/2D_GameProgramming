#include "Core.h"
#include "../Framework/Framework.h"

INIT_INSTACNE(Core)
bool Core::m_isUpdate = true;

Core::Core()
	: m_hInstance(nullptr), m_hWnd(nullptr), m_ClassName(L"MapleStory")
{
}

Core::~Core()
{
	GET_INSTANCE(Framework)->Release();

	cout << "Core - 소멸자" << endl;
}

bool Core::Initialize(HINSTANCE hInst)
{
	m_hInstance = hInst;

	if (m_hInstance == nullptr)
		return false;

	MyRegisterClass();

	RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;

	// 윈도우 생성
	m_hWnd = CreateWindowW(m_ClassName.c_str(), m_ClassName.c_str(), dwStyle, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_hInstance, nullptr);
	
	if (m_hWnd == nullptr)
		return false;

	AdjustWindowRect(&rc, dwStyle, false);

	// 윈도우 크기 및 위치 설정
	int posX = 100;
	int posY = 100;
	SetWindowPos(m_hWnd, HWND_TOPMOST, posX, posY, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);

	if (GET_INSTANCE(Framework)->Initialize(m_hWnd) == false)
		return false;

	::ShowWindow(m_hWnd, SW_SHOW);
	::UpdateWindow(m_hWnd);

	return true;
}

ATOM Core::MyRegisterClass()
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Core::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hInstance;
	wcex.hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = m_ClassName.c_str();
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return ::RegisterClassEx(&wcex);
}

int Core::Run()
{
	MSG msg;

	while (m_isUpdate)
	{
		if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else
			GET_INSTANCE(Framework)->Run();
	}

	GET_INSTANCE(Core)->Release();

	return (int)msg.wParam;
}

LRESULT Core::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
		case WM_PAINT:
			hdc = ::BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			break;

		case WM_DESTROY:
			m_isUpdate = false;
			::PostQuitMessage(0);
			break;

		default:
			return(::DefWindowProc(hWnd, message, wParam, lParam));
	}

	return 0;
}