#include "SceneManager.h"
#include "../GameObject/Player/Player.h"
#include "InGameScene/InGameScene.h"

INIT_INSTACNE(SceneManager)

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
	for (auto iter = m_SceneList.begin(); iter != m_SceneList.end(); )
	{
		SAFE_DELETE((*iter).second);
		iter = m_SceneList.erase(iter);
	}
	m_SceneList.clear();

	cout << "SceneManager - ¼Ò¸êÀÚ" << endl;
}

bool SceneManager::Initialize()
{
	InGameScene* pInGameScene = new InGameScene;
	if (pInGameScene->Initialize() == false)
		return false;
	m_SceneList.emplace("InGameScene", pInGameScene);

	return true;
}

void SceneManager::Update()
{
	for (auto iter = m_SceneList.begin(); iter != m_SceneList.end(); ++iter)
	{
		(*iter).second->Update();
	}
}

void SceneManager::Render()
{
	for (auto iter = m_SceneList.begin(); iter != m_SceneList.end(); ++iter)
	{
		(*iter).second->Render();
	}
}
