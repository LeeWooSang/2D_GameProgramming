#include "Player.h"
#include"../../D2DManager/D2DManager.h"
#include "../../Network/Network.h"

Player::Player() 
	: GameObject(), m_Frame(0.f)
{
}

Player::~Player()
{
	cout << "Player - ¼Ò¸êÀÚ" << endl;
}

bool Player::Initialize()
{
	//D2D1_RECT_F pos = { 0, 0, 100, 100 };
	//if (GET_INSTANCE(D2DManager)->CreateBitmapImage("Player", ImageInfo(L"../Resource/Textures/Player.png", pos, 133, 144, 1, 1, 0, 0)) == false)
	//	return false;

	D2D1_RECT_F pos = { 0, 0, 100, 100 };
	if (GET_INSTANCE(D2DManager)->CreateTexture("Player", ImageInfo(L"../Resource/Textures/Character/PinkBin2.png", pos, 828, 126, 6, 1, 0, 0)) == false)
		return false;


	return true;
}

void Player::Update(float elapsedTime)
{
	if (m_Frame < 6)
		m_Frame += elapsedTime * 5;
	else
		m_Frame = 0.f;

	D2D1_RECT_F pos = GET_INSTANCE(D2DManager)->GetTexture("Player").m_Pos;
	float moveSize = 2.f;

	char dir = DIRECTION::IDLE;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		dir |= DIRECTION::UP;

		pos.top -= moveSize;
		pos.bottom -= moveSize;
	}

	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		dir |= DIRECTION::DOWN;

		pos.top += moveSize;
		pos.bottom += moveSize;
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		dir |= DIRECTION::RIGHT;

		pos.left += moveSize;
		pos.right += moveSize;
	}

	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		dir |= DIRECTION::LEFT;

		pos.left -= moveSize;
		pos.right -= moveSize;
	}

	if(dir != DIRECTION::IDLE)
		GET_INSTANCE(Network)->Send_Move_Packet(dir);

	GET_INSTANCE(D2DManager)->GetTexture("Player").m_Pos = pos;
}

void Player::Render()
{
	D2D1_RECT_F pos = GET_INSTANCE(D2DManager)->GetTexture("Player").m_Pos;

	GET_INSTANCE(D2DManager)->Render("Player", pos, static_cast<int>(m_Frame), 0);
}

void Player::Release()
{
}
