#include "InGameScene.h"
#include "../../Macro.h"
#include "../../GameObject/Player/Player.h"

InGameScene::InGameScene()
	: m_pPlayer(nullptr)
{
}

InGameScene::~InGameScene()
{
	Release();
	cout << "InGameScene ¼Ò¸êÀÚ" << endl;
}

bool InGameScene::Initialize()
{
	m_pPlayer = new Player;
	if (m_pPlayer->Initialize() == false)
		return false;

	return true;
}

void InGameScene::Update()
{
	if (m_pPlayer != nullptr)
		m_pPlayer->Update();
}

void InGameScene::Render()
{
	if (m_pPlayer != nullptr)
		m_pPlayer->Render();
}

void InGameScene::Release()
{
	SAFE_DELETE(m_pPlayer);
}
