#pragma once
#include "../Defines.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual bool Initialize() = 0;
	virtual void Update(float) = 0;
	virtual void Render() = 0;
	virtual void Release() = 0;

	void SetX(float x) { m_Pos.x = x; }
	void SetY(float y) { m_Pos.y = y; }

	int GetID()		const { return m_ID; }
	void SetID(int id) { m_ID = id; }

protected:
	POINT m_Pos;
	int m_SizeX;
	int m_SizeY;

	int m_ID;
};
