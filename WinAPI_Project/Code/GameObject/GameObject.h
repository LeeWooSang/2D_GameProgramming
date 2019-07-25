#pragma once
#include "../Defines.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual bool Initialize() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void Release() = 0;

private:
	POINT m_Pos;
};
