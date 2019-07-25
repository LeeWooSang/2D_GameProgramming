#include "Framework.h"
#include "../D2DManager/D2DManager.h"

INIT_INSTACNE(Framework)

Framework::Framework()
{
}

Framework::~Framework()
{
	GET_INSTANCE(D2DManager)->Release();

	cout << "Framework - ¼Ò¸êÀÚ" << endl;
}

bool Framework::Initialize(HWND hwnd)
{
	if (GET_INSTANCE(D2DManager)->Initialize(hwnd) == false)
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
}

void Framework::Render()
{

}