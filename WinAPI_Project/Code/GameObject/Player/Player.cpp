#include "Player.h"
#include"../../D2DManager/D2DManager.h"
#include "../../Network/Network.h"
#include "../../Camera/Camera.h"

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
	if (GET_INSTANCE(D2DManager)->CreateTexture("Player", ImageInfo(L"../Resource/Textures/Character/Pinkbin_IDLE.png", 720, 120, 6, 1, 0, 0, 120, 120)) == false)
		return false;

	m_Speed = 0.5f;

	GET_INSTANCE(Camera)->SetTarget(this);

	return true;
}

void Player::Update(float elapsedTime)
{
	if (m_Frame <= 6)
		m_Frame += elapsedTime * 5;
	else
		m_Frame = 0.f;

	float moveSize = m_Speed * elapsedTime;

	char dir = DIRECTION::IDLE;
	float gap = 0.9f;
	
	XMFLOAT2 pos = m_WorldPosition;

	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		dir |= DIRECTION::UP;
		m_WorldPosition.y += moveSize;
		if (m_WorldPosition.y >= gap)
			m_WorldPosition.y = gap;

		//GET_INSTANCE(Camera)->SetWorldPosition(XMFLOAT2(cameraPos.x, cameraPos.y - moveSize));
	}

	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		dir |= DIRECTION::DOWN;
		m_WorldPosition.y -= moveSize;
		if (m_WorldPosition.y <= -gap)
			m_WorldPosition.y = -gap;

		//GET_INSTANCE(Camera)->SetWorldPosition(XMFLOAT2(cameraPos.x, cameraPos.y + moveSize));
	}

	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		dir |= DIRECTION::RIGHT;
		m_WorldPosition.x += moveSize;
		if (m_WorldPosition.x >= gap)
			m_WorldPosition.x = gap;

		//GET_INSTANCE(Camera)->SetWorldPosition(XMFLOAT2(cameraPos.x - moveSize, cameraPos.y));
	}

	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		dir |= DIRECTION::LEFT;
		m_WorldPosition.x -= moveSize;
		if (m_WorldPosition.x <= -gap)
			m_WorldPosition.x = -gap;

		//GET_INSTANCE(Camera)->SetWorldPosition(XMFLOAT2(cameraPos.x + moveSize, cameraPos.y));
	}
	
	GET_INSTANCE(Camera)->Update(dir, moveSize);
	if (dir != DIRECTION::IDLE)
	{
		//GET_INSTANCE(Network)->Send_Move_Packet(dir);
	}
}

void Player::Render()
{
	GET_INSTANCE(D2DManager)->Render("Player", m_WorldPosition, static_cast<int>(m_Frame), 0);
}

void Player::Release()
{
}
