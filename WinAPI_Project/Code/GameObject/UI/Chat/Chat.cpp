#include "Chat.h"
#include"../../../D2DManager/D2DManager.h"
#include "../../../Input/Input.h"

Chat::Chat()
	: UI() 
{
	m_Pos.x = 0;
	m_Pos.y = 300;
	m_SizeX = 400;
	m_SizeY = 200;
}

Chat::~Chat()
{
	cout << "Chat - ¼Ò¸êÀÚ" << endl;
}

bool Chat::Initialize()
{
	D2D1_RECT_F pos = { m_Pos.x, m_Pos.y, m_Pos.x + m_SizeX, m_Pos.y+ m_SizeY };

	if (GET_INSTANCE(D2DManager)->CreateTexture("ChatView", ImageInfo(L"../Resource/Textures/UI/ChatView.png", pos, 640, 528, 1, 1, 0, 0)) == false)
		return false;

	return true;
}

void Chat::Update(float elapsedTime)
{
	UI::Update(elapsedTime);
}

void Chat::Render()
{
	UI::Render();

	float x = 10;
	float y = 300;
	int sizeX = 400;
	int sizeY = 100;
	D2D1_RECT_F textPos = { x, y, x + sizeX, y + sizeY };

	size_t len = GET_INSTANCE(Input)->GetText().length();
	float fontSize = 20.f;
	D2D1_RECT_F combPos = { x + fontSize * len, y, x + fontSize * len, y + sizeY };

	GET_INSTANCE(D2DManager)->Render(GET_INSTANCE(Input)->GetComb(), "¸ÞÀÌÇÃ", "Èò»ö", combPos);
	GET_INSTANCE(D2DManager)->Render(GET_INSTANCE(Input)->GetText(), "¸ÞÀÌÇÃ", "Èò»ö", textPos);
}

void Chat::Release()
{
	UI::Release();
}
