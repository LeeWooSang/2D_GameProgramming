#include "InGameScene.h"
#include "../../Macro.h"
#include "../../GameObject/Player/Player.h"
#include "../../GameObject/UI/Chat/Chat.h"

InGameScene::InGameScene()
{
}

InGameScene::~InGameScene()
{
	Release();

	cout << "InGameScene ¼Ò¸êÀÚ" << endl;
}

bool InGameScene::Initialize()
{
	Player* pPlayer = new Player;
	if (pPlayer->Initialize() == false)
		return false;
	m_ObjectList.emplace_back(pPlayer);

	Chat* pChatUI = new Chat;
	if (pChatUI->Initialize() == false)
		return false;
	m_ObjectList.emplace_back(pChatUI);

	return true;
}

void InGameScene::Update(float elapsedTime)
{
	for (auto iter = m_ObjectList.begin(); iter != m_ObjectList.end(); ++iter)
		(*iter)->Update(elapsedTime);
}

void InGameScene::Render()
{
	for (auto iter = m_ObjectList.begin(); iter != m_ObjectList.end(); ++iter)
		(*iter)->Render();
}

void InGameScene::Release()
{
	SAFE_DELETE_LIST(m_ObjectList);
}
