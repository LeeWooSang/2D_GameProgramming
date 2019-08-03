#include "UI.h"
#include "../../D2DManager/D2DManager.h"

UI::UI()
	: GameObject()
{
}

UI::~UI()
{
	cout << "UI - ¼Ò¸êÀÚ" << endl;
}

void UI::Update(float)
{
}

void UI::Render()
{
	GET_INSTANCE(D2DManager)->Render("ChatView");
}

void UI::Release()
{
}
