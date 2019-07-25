#include "Player.h"
#include"../../D2DManager/D2DManager.h"

Player::Player() : GameObject()
{
}

Player::~Player()
{
	cout << "Player - ¼Ò¸êÀÚ" << endl;
}

bool Player::Initialize()
{
	D2D1_RECT_F pos = { 0, 0, 100, 100 };
	if (GET_INSTANCE(D2DManager)->CreateBitmapImage("Player", ImageInfo(L"../Resource/Textures/Player.png", pos, 133, 144, 1, 1, 0, 0)) == false)
		return false;

	return true;
}

void Player::Update()
{
	D2D1_RECT_F pos = GET_INSTANCE(D2DManager)->GetImageInfo("Player").m_Pos;

	if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
	{
		pos.left += 10;
		pos.right += 10;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x0001)
	{
		pos.left -= 10;
		pos.right -= 10;
	}

	if (GetAsyncKeyState(VK_UP) & 0x0001)
	{
		pos.top -= 10;
		pos.bottom -= 10;
	}

	if (GetAsyncKeyState(VK_DOWN) & 0x0001)
	{
		pos.top += 10;
		pos.bottom += 10;
	}

	GET_INSTANCE(D2DManager)->GetImageInfo("Player").m_Pos = pos;
}

void Player::Render()
{
	GET_INSTANCE(D2DManager)->Render("Player");
}

void Player::Release()
{
}
