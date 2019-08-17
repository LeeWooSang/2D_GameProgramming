#include "Player.h"
#include"../../D2DManager/D2DManager.h"
#include "../../Network/Network.h"
#include "../../Camera/Camera.h"

Player::Player() 
	: GameObject(), m_PrevAnimation(0), m_AnimationName(""), m_Frame(0.f)
{
	m_AnimationMap.clear();
}

Player::~Player()
{
	m_AnimationMap.clear();

	cout << "Player - ¼Ò¸êÀÚ" << endl;
}

bool Player::Initialize()
{
	if (GET_INSTANCE(D2DManager)->CreateTexture("Idle", ImageInfo(L"../Resource/Textures/Character/Pinkbin_Idle.png", 720, 120, ANIMATION_FRAME::IDLE_FRAME, 1, 0, 0, 150, 150)) == false)

		return false;
	m_AnimationMap.emplace(ANIMATION::IDLE, ANIMATION_FRAME::IDLE_FRAME);

	if (GET_INSTANCE(D2DManager)->CreateTexture("Break", ImageInfo(L"../Resource/Textures/Character/Pinkbin_Break.png", 3770, 130, ANIMATION_FRAME::BREAK_FRAME, 1, 0, 0, 150, 150)) == false)
		return false;

	m_AnimationMap.emplace(ANIMATION::BREAK, ANIMATION_FRAME::BREAK_FRAME);

	if (GET_INSTANCE(D2DManager)->CreateTexture("Die", ImageInfo(L"../Resource/Textures/Character/Pinkbin_Die.png", 9570, 105, ANIMATION_FRAME::DIE_FRAME, 1, 0, 0, 150, 150)) == false)
		return false;

	m_AnimationMap.emplace(ANIMATION::DIE, ANIMATION_FRAME::DIE_FRAME);

	if (GET_INSTANCE(D2DManager)->CreateTexture("RagingBlow", ImageInfo(L"../Resource/Textures/Skill/Effect/RagingBlow.png", 5850, 380, ANIMATION_FRAME::RAGINGBLOW_FRAME, 1, 0, 0, 380, 380)) == false)
		return false;

	m_AnimationMap.emplace(ANIMATION::RAGINGBLOW, ANIMATION_FRAME::RAGINGBLOW_FRAME);

	//m_Frame = ANIMATION_FRAME::IDLE_FRAME;
	//m_PrevAnimation = ANIMATION::IDLE;
	//m_AnimationName = "Idle";

	m_PrevAnimation = ANIMATION::RAGINGBLOW;
	m_Frame = ANIMATION_FRAME::RAGINGBLOW_FRAME;
	m_AnimationName = "RagingBlow";
	m_Speed = 0.5f;

	GET_INSTANCE(Camera)->SetTarget(this);

	return true;
}

void Player::Update(float elapsedTime)
{
	float moveSize = m_Speed * elapsedTime;

	char dir = DIRECTION::IDLE;
	//char animation = ANIMATION::IDLE;
	char animation = ANIMATION::RAGINGBLOW;

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
		animation |= ANIMATION::BREAK;

		m_WorldPosition.x += moveSize;
		if (m_WorldPosition.x >= gap)
			m_WorldPosition.x = gap;

		//GET_INSTANCE(Camera)->SetWorldPosition(XMFLOAT2(cameraPos.x - moveSize, cameraPos.y));
	}

	else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		dir |= DIRECTION::LEFT;
		animation |= ANIMATION::BREAK;

		m_WorldPosition.x -= moveSize;
		if (m_WorldPosition.x <= -gap)
			m_WorldPosition.x = -gap;

		//GET_INSTANCE(Camera)->SetWorldPosition(XMFLOAT2(cameraPos.x + moveSize, cameraPos.y));
	}
	
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
	{
		animation |= ANIMATION::IDLE;
	}

	GET_INSTANCE(Camera)->Update(dir, moveSize);

	if (dir != DIRECTION::IDLE)
	{
		//GET_INSTANCE(Network)->Send_Move_Packet(dir);
	}

	ProcessAnimation(animation, elapsedTime);
}

void Player::Render()
{
	GET_INSTANCE(D2DManager)->Render(m_AnimationName, m_WorldPosition, static_cast<int>(m_Frame), 0);
}

void Player::Release()
{
}

void Player::ProcessAnimation(char animation, float elapsedTime)
{
	auto iter = m_AnimationMap.find(static_cast<ANIMATION>(animation));
	if (iter == m_AnimationMap.end())
		return;
	
	if (m_PrevAnimation != animation)
	{
		m_PrevAnimation = animation;

		switch (m_PrevAnimation)
		{
		case ANIMATION::IDLE:					m_AnimationName = "Idle";		break;
		case ANIMATION::BREAK:					m_AnimationName = "Break";	break;
		case ANIMATION::RAGINGBLOW:	m_AnimationName = "RagingBlow";	break;
		default:	
			break;
		}

		m_Frame = 0.f;
	}

	m_Frame += elapsedTime * 20;

	if (m_Frame > (*iter).second)
		m_Frame = 0.f;
}
