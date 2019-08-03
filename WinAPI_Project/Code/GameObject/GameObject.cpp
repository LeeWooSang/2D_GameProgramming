#include "GameObject.h"

GameObject::GameObject()
	: m_Pos{ 0 }, m_SizeX(0), m_SizeY(0)
{
}

GameObject::~GameObject()
{
	cout << "GameObject - ¼Ò¸êÀÚ" << endl;
}
