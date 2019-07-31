#pragma once
#include "../Macro.h"
#include "../Defines.h"

class SceneManager
{
	SINGLE_TONE(SceneManager)

	bool Initialize();
	void Update(float);
	void Render();

private:
	unordered_map<string, class Scene*> m_SceneList;
};