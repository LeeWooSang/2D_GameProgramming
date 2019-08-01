#include "Framework.h"
#include "../D2DManager/D2DManager.h"
#include "../Scene/SceneManager.h"
#include "../GameTimer/GameTimer.h"
#include "../Input/Input.h"

INIT_INSTACNE(Framework)

Framework::Framework()
	: m_pTimer(nullptr)
{
}

Framework::~Framework()
{
	if (m_pTimer != nullptr)
	{
		m_pTimer->Stop();
		SAFE_DELETE(m_pTimer)
	}

	GET_INSTANCE(D2DManager)->Release();
	GET_INSTANCE(SceneManager)->Release();

	cout << "Framework - 소멸자" << endl;
}

bool Framework::Initialize(HWND hwnd)
{
	m_pTimer = new GameTimer;

	if (GET_INSTANCE(D2DManager)->Initialize(hwnd) == false)
		return false;

	if (GET_INSTANCE(SceneManager)->Initialize() == false)
		return false;

	m_pTimer->Reset();

	return true;
}

void Framework::Run()
{
	float elapsedTime = 0.f;

	if (m_pTimer)
	{
		m_pTimer->Tick(60);
		elapsedTime = m_pTimer->GetElapsedTime();

		Update(elapsedTime);
		Render();
	}
}

void Framework::Update(float elapsedTime)
{
	GET_INSTANCE(SceneManager)->Update(elapsedTime);
}

void Framework::Render()
{
	GET_INSTANCE(D2DManager)->GetRenderTarget()->BeginDraw();

	D2D1_COLOR_F clearcolor = D2D1_COLOR_F{ 1.f, 1.f, 1.f, 1.f };
	GET_INSTANCE(D2DManager)->GetRenderTarget()->Clear(&clearcolor);

	GET_INSTANCE(SceneManager)->Render();

	D2D1_RECT_F pos;
	float x = 200;
	float y = 200;
	int sizeX = 100;
	int sizeY = 100;
	pos = { x, y, x + sizeX, y + sizeY };
	//wstring wstr = L"시발롬아";

	if(GET_INSTANCE(Input)->GetText().size() > 0)
		GET_INSTANCE(D2DManager)->Render(GET_INSTANCE(Input)->GetText(), "메이플", "검은색", pos);

	GET_INSTANCE(D2DManager)->GetRenderTarget()->EndDraw();
}