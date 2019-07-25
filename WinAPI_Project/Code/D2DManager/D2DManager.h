#pragma once
#include "../Macro.h"
#include "../Defines.h"

constexpr int MAX_FONT_COUNT = 2;
constexpr int MAX_COLOR_COUNT = 8;

struct ImageInfo
{
	ImageInfo(wstring path, D2D1_RECT_F& pos, int width, int height, int totalX, int totalY, int frameX, int frameY)
		: m_FilePath(path), m_Bitmap(nullptr), m_Pos(pos), m_WidthPixel(width), m_HeightPixel(height), m_TotalFrameX(totalX), m_TotalFrameY(totalY), m_FrameXNum(frameX), m_FrameYNum(frameY) {}

	wstring m_FilePath;
	ID2D1Bitmap* m_Bitmap;
	D2D1_RECT_F m_Pos;

	int m_WidthPixel = 0;
	int m_HeightPixel = 0;

	// 총 가로 몇프레임
	int m_TotalFrameX = 0;
	// 총 세로 몇프레임
	int m_TotalFrameY = 0;

	// 현재 프레임
	int m_FrameXNum = 0;
	int m_FrameYNum = 0;
};

struct FontInfo
{
	FontInfo()
		: m_pFont(nullptr), m_pTextLayout(nullptr), m_FontSize(0.f) {}
	FontInfo(IDWriteTextFormat* pFont, IDWriteTextLayout* layout, float size)
		: m_pFont(pFont), m_pTextLayout(layout), m_FontSize(size) {}

	// 폰트 객체
	IDWriteTextFormat*			m_pFont{ nullptr };
	// 폰트 형식
	IDWriteTextLayout*			m_pTextLayout{ nullptr };

	float m_FontSize;
};

class D2DManager
{
	SINGLE_TONE(D2DManager)

	bool Initialize(HWND);

	void Render();
	void Render(string key);
	void Render(string key, D2D1_RECT_F pos, int fx, int fy);
	void Render(string key, string color, wstring text, D2D1_RECT_F pos);

private:
	void CreateBitmapImage(ImageInfo info, string key);
	void CreateGameFont();
	void CreateGameFontColor();

	ID2D1Factory3*						m_pFactory;
	ID2D1Device2*						m_pDevice;
	// D2DManager로 텍스트를 그리기 위한 팩토리
	IDWriteFactory5*					m_pWriteFactory;
	IDWriteFontCollection1*		m_pFontCollection;
	// D2DManager로 이미지를 그리기 위한 팩토리
	IWICImagingFactory*			m_pWICImagingFactory;
	// 렌더타겟
	ID2D1HwndRenderTarget*	m_pRenderTarget;

	// 이미지를 저장함
	unordered_map<string, ImageInfo>						m_ImageInfoMap;
	// 폰트를 저장함
	unordered_map<string, FontInfo>						m_FontInfoMap;
	// 폰트 색상을 저장함
	unordered_map<string, ID2D1SolidColorBrush*> m_FontColorMap;
};