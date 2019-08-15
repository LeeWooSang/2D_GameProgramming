#include "Character.h"

Character::Character()
{
	m_OverEx.dataBuffer.len = BUF_SIZE;
	m_OverEx.dataBuffer.buf = m_OverEx.messageBuffer;
	m_OverEx.eventType = EV_RECV;
	ZeroMemory(&m_OverEx.overlapped, sizeof(WSAOVERLAPPED));

	m_X = 0.f;
	m_Y = 0.f;

	m_ViewList.clear();

	m_Hp = 0;
	m_AttackPower = 0;
	m_Exp = 0;
	m_Level = 1;
}

Character::~Character()
{
}

void Character::Move(char dir, float& x, float& y) 
{
	switch (dir)
	{
	case DIRECTION::UP:
		--y;
		if (y < 0)
			y = 0;
		break;

	case DIRECTION::DOWN:
		++y;
		if (y >= WORLD::HEIGHT)
			y = WORLD::HEIGHT - 1;
		break;

	case DIRECTION::RIGHT:
		if (x < WORLD::WIDTH - 1)
			x++;
		break;

	case DIRECTION::LEFT:
		if (x > 0)
			x--;
		break;

	case DIRECTION::UP_RIGHT:
		--y;
		if (y < 0)
			y = 0;
		if (x < WORLD::WIDTH - 1)
			x++;
		break;

	case DIRECTION::UP_LEFT:
		--y;
		if (y < 0)
			y = 0;
		if (x > 0)
			x--;
		break;

	case DIRECTION::DOWN_RIGHT:
		++y;
		if (y >= WORLD::HEIGHT)
			y = WORLD::HEIGHT - 1;
		if (x < WORLD::WIDTH - 1)
			x++;
		break;

	case DIRECTION::DOWN_LEFT:
		++y;
		if (y >= WORLD::HEIGHT)
			y = WORLD::HEIGHT - 1;
		if (x > 0)
			x--;
		break;

	default:
		break;
	}

	m_X = x;
	m_Y = y;
}

short Character::GetMaxHP()
{
	int r = 2;
	short maxHp = 50 * static_cast<short>(pow(r, m_Level - 1));
	return maxHp;
}

Player::Player() : Character()
{
	ZeroMemory(&m_PacketBuf, sizeof(char));
	m_IsConnect = false;

	m_PrevSize = 0;
	m_SendBytes = 0;

	m_Level = 1;
	m_Exp = 0;
	m_AttackPower = 0;
}

Player::~Player()
{
	ClearPlayerInfo();
}

void Player::ClearPlayerInfo()
{
	closesocket(m_Socket);
	m_Socket = { 0 };

	m_OverEx.dataBuffer.len = BUF_SIZE;
	m_OverEx.dataBuffer.buf = m_OverEx.messageBuffer;
	m_OverEx.eventType = EV_RECV;

	ZeroMemory(&m_OverEx.overlapped, sizeof(WSAOVERLAPPED));

	m_ViewListMtx.lock();
	m_ViewList.clear();
	m_ViewListMtx.unlock();

	m_X = START_X;
	m_Y = START_Y;

	ZeroMemory(&m_PacketBuf, sizeof(char));

	m_IsConnect = false;
	m_PrevSize = 0;
	m_SendBytes = 0;
	m_Level = 1;
	m_Exp = 0;
	m_Hp = 50;
	m_AttackPower = 0;
}


