#include "Map.h"
#include "../../D2DManager/D2DManager.h"

Map::Map()
	: GameObject()
{
}

Map::~Map()
{
	cout << "Map - ¼Ò¸êÀÚ" << endl;
}

bool Map::Initialize()
{
	D2D1_RECT_F pos = { 0, 0, 800, 600 };
	if (GET_INSTANCE(D2DManager)->CreateTexture("Map", ImageInfo(L"../Resource/Textures/Map/Login.png", pos, 795, 560, 1, 1, 0, 0)) == false)
		return false;


	return true;
}

void Map::Update(float elapsedTime)
{
}

void Map::Render()
{
	D2D1_RECT_F pos = GET_INSTANCE(D2DManager)->GetTexture("Map").m_Pos;

	GET_INSTANCE(D2DManager)->Render("Map");
}

void Map::Release()
{
}
