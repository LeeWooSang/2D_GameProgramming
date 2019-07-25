#include "Framework.h"
#include "../D2DManager/D2DManager.h"
#include "../Scene/SceneManager.h"

INIT_INSTACNE(Framework)

Framework::Framework()
{
}

Framework::~Framework()
{
	GET_INSTANCE(D2DManager)->Release();
	GET_INSTANCE(SceneManager)->Release();

	cout << "Framework - ¼Ò¸êÀÚ" << endl;
}

bool Framework::Initialize(HWND hwnd)
{
	if (GET_INSTANCE(D2DManager)->Initialize(hwnd) == false)
		return false;

	if (GET_INSTANCE(SceneManager)->Initialize() == false)
		return false;

	return true;
}

void Framework::Run()
{
	Update();
	Render();
}

void Framework::Update()
{
	GET_INSTANCE(SceneManager)->Update();
}

void Framework::Render()
{
	GET_INSTANCE(D2DManager)->GetRenderTarget()->BeginDraw();
	D2D1_COLOR_F clearcolor = D2D1_COLOR_F{ 1.f, 1.f, 1.f, 1.f };
	GET_INSTANCE(D2DManager)->GetRenderTarget()->Clear(&clearcolor);

	GET_INSTANCE(SceneManager)->Render();

	GET_INSTANCE(D2DManager)->GetRenderTarget()->EndDraw();
}