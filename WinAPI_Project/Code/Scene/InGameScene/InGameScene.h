#pragma once
#include "../Scene.h"
#include "../../Defines.h"

class InGameScene : public Scene
{
public:
	InGameScene();
	virtual ~InGameScene();

	virtual bool Initialize() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void Release() override;

private:
	list<class GameObject*> m_ObjectList;
	class Player* m_pPlayer;
};