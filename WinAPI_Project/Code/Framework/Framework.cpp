#include "Framework.h"
#include "../D2DManager/D2DManager.h"
#include "../Scene/SceneManager.h"
#include "../GameTimer/GameTimer.h"

INIT_INSTACNE(Framework)

Framework::Framework()
	: m_pTimer(nullptr)
{
}

Framework::~Framework()
{
	GET_INSTANCE(SceneManager)->Release();
	GET_INSTANCE(D2DManager)->Release();
	if (m_pTimer != nullptr)
	{
		m_pTimer->Stop();
		SAFE_DELETE(m_pTimer)
	}
	
	cout << "Framework - ¼Ò¸êÀÚ" << endl;
}

bool Framework::Initialize(HWND hWnd)
{
	m_pTimer = new GameTimer;

	if (GET_INSTANCE(D2DManager)->Initialize(hWnd) == false)
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
		m_pTimer->Tick(60.0);
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
		
	GET_INSTANCE(D2DManager)->GetRenderTarget()->EndDraw();
}