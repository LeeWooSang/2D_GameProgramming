#include "D2DManager.h"
#include "../Camera/Camera.h"

INIT_INSTACNE(D2DManager)

D2DManager::D2DManager()
	: m_pFactory(nullptr), m_pRenderTarget(nullptr), m_pWriteFactory(nullptr), m_pFontCollection(nullptr), m_pWICImagingFactory(nullptr)
{
}

D2DManager::~D2DManager()
{
	//갖고 있는 비트맵이미지를 모두 지움
	for (auto iter = m_ImageInfoMap.begin(); iter != m_ImageInfoMap.end(); )
	{
		(*iter).second.m_Bitmap->Release();
		iter = m_ImageInfoMap.erase(iter);
	}
	m_ImageInfoMap.clear();

	// 폰트 컬러를 모두 지움
	for (auto iter = m_FontColorMap.begin(); iter != m_FontColorMap.end(); )
	{
		(*iter).second->Release();
		iter = m_FontColorMap.erase(iter);
	}
	m_FontColorMap.clear();

	for (auto iter = m_FontInfoMap.begin(); iter != m_FontInfoMap.end(); )
	{
		(*iter).second.m_pFont->Release();
		(*iter).second.m_pTextLayout->Release();
		iter = m_FontInfoMap.erase(iter);
	}
	m_FontInfoMap.clear();

	if (m_pFontCollection)
		m_pFontCollection->Release();
	if (m_pWICImagingFactory)
		m_pWICImagingFactory->Release();
	if (m_pWriteFactory)
		m_pWriteFactory->Release();
	if (m_pRenderTarget)
		m_pRenderTarget->Release();
	if (m_pFactory)
		m_pFactory->Release();

	cout << "D2DManager - 소멸자" << endl;
}

bool D2DManager::Initialize(HWND hWnd)
{
	// D2DManager Factory 생성
	HRESULT result = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory);
	if (result != S_OK)
		return false;

	// 콤객체 초기화
	result = CoInitialize(nullptr);
	if (result != S_OK)
		return false;

	// 렌더타겟 생성
	D2D1_SIZE_U size = D2D1::SizeU(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	D2D1_RENDER_TARGET_PROPERTIES d2dRTProps = D2D1::RenderTargetProperties();
	D2D1_HWND_RENDER_TARGET_PROPERTIES d2dHwndRTProps = D2D1::HwndRenderTargetProperties(hWnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
	result = m_pFactory->CreateHwndRenderTarget(d2dRTProps, d2dHwndRTProps, &m_pRenderTarget);
	if (result != S_OK)
		return false;

	// 텍스트를 위한 Factory 생성
	result = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&m_pWriteFactory);
	if (result != S_OK)
		return false;

	// 이미지를 위한 Factory 생성
	result = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&m_pWICImagingFactory);
	if (result != S_OK)
		return false;

	CreateGameFont();
	CreateGameFontColor();

	return true;
}

bool D2DManager::CreateTexture(const string& key, ImageInfo info)
{
	// [ Bitmap 이미지 초기화 방법 ] 
	// 1. Com객체 초기화
	// 2. IWICImagingFactory 생성
	// 3. Decoder 생성
	// 4. 이미지의 프레임 얻어오기
	// 5. Converter 생성
	// 6. Bitmap 생성

	IWICBitmapDecoder* pBitmapDecoder;
	HRESULT result = m_pWICImagingFactory->CreateDecoderFromFilename(info.m_FilePath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &pBitmapDecoder);
	if (result != S_OK)
		return false;

	IWICBitmapFrameDecode* pFrameDecode;
	result = pBitmapDecoder->GetFrame(0, &pFrameDecode);
	if (result != S_OK)
		return false;
	
	IWICFormatConverter* pFormatConverter;
	result = m_pWICImagingFactory->CreateFormatConverter(&pFormatConverter);
	if (result != S_OK)
		return false;

	result = pFormatConverter->Initialize(pFrameDecode, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.0f, WICBitmapPaletteTypeCustom);
	if (result != S_OK)
		return false;

	result = m_pRenderTarget->CreateBitmapFromWicBitmap(pFormatConverter, &info.m_Bitmap);
	if (result != S_OK)
		return false;

	if (pBitmapDecoder)
		pBitmapDecoder->Release();

	if (pFrameDecode)
		pFrameDecode->Release();

	if (pFormatConverter)
		pFormatConverter->Release();

	m_ImageInfoMap.emplace(key, info);
	return true;
}

void D2DManager::CreateGameFont()
{
	// 폰트 경로
	wstring fontPath[] = { L"../Resource/Fonts/a피오피동글.ttf", L"../Resource/Fonts/메이플스토리.ttf" };

	// 폰트를 직접 설치할때 사용
	//AddFontResourceEx(fontPath[0].c_str(), FR_PRIVATE, 0);
	//AddFontResourceEx(fontPath[1].c_str(), FR_PRIVATE, 0);

	// 폰트를 설치하지 않고, 메모리에 올려서 사용할 경우
	// 빌더 생성
	IDWriteFontSetBuilder1* pFontSetBuilder;
	HRESULT result = m_pWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);

	IDWriteFontFile* pFontFile[MAX_FONT_COUNT];
	IDWriteFontSet* pFontSet[MAX_FONT_COUNT];

	wstring FontName[MAX_FONT_COUNT];
	unsigned int max_length = 64;

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		// 해당하는 경로에서 폰트 파일을 로드한다.
		result = m_pWriteFactory->CreateFontFileReference(fontPath[i].c_str(), nullptr, &pFontFile[i]);
		// 빌더에 폰트 추가
		result = pFontSetBuilder->AddFontFile(pFontFile[i]);
		result = pFontSetBuilder->CreateFontSet(&pFontSet[i]);
		// 폰트 Collection에 폰트 추가 => 폰트 Collection에서 내가 사용할 폰트가 저장되어 있음
		result = m_pWriteFactory->CreateFontCollectionFromFontSet(pFontSet[i], &m_pFontCollection);

		// 폰트 이름을 얻어오는 방법
		IDWriteFontFamily* pFontFamily;
		IDWriteLocalizedStrings* pLocalizedFontName;

		result = m_pFontCollection->GetFontFamily(i, &pFontFamily);
		result = pFontFamily->GetFamilyNames(&pLocalizedFontName);
		// 저장되어있는 폰트의 이름을 얻어옴
		result = pLocalizedFontName->GetString(0, const_cast<wchar_t*>(FontName[i].c_str()), max_length);

		pFontFamily->Release();
		pLocalizedFontName->Release();
	}

	pFontSetBuilder->Release();

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		pFontFile[i]->Release();
		pFontSet[i]->Release();
	}

	float fontSize = 25.f;

	// 폰트 객체
	IDWriteTextFormat* pFont[MAX_FONT_COUNT];
	// 폰트 형식
	IDWriteTextLayout*	pTextLayout[MAX_FONT_COUNT];
	wstring wstr = L"TextLayout Initialize";

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		// 폰트 객체 생성	
		result = m_pWriteFactory->CreateTextFormat(FontName[i].c_str(), m_pFontCollection, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-US", &pFont[i]);

		if (i == 0)
		{
			// 폰트를 중앙에 정렬시키기
			result = pFont[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			result = pFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			result = m_pWriteFactory->CreateTextLayout(wstr.c_str(), wstr.length(), pFont[i], 1024.0f, 1024.0f, &pTextLayout[i]);

			m_FontInfoMap.emplace("피오피동글", FontInfo(pFont[i], pTextLayout[i], fontSize));
		}
		else
		{
			result = pFont[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			result = pFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			result = m_pWriteFactory->CreateTextLayout(wstr.c_str(), wstr.length(), pFont[i], 4096.0f, 4096.0f, &pTextLayout[i]);

			m_FontInfoMap.emplace("메이플", FontInfo(pFont[i], pTextLayout[i], fontSize));
		}
	}
}

void D2DManager::CreateGameFontColor()
{
	ID2D1SolidColorBrush* pColor[MAX_COLOR_COUNT];

	int index = 0;
	HRESULT result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DeepPink, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("분홍색", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Brown, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("갈색", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("흰색", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("검은색", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("하늘색", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LavenderBlush, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("연분홍색", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("빨간색", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("주황색", pColor[index++]);
}

void D2DManager::WorldToScreen(D2D1_RECT_F& screenPos, int sizeX, int sizeY, const XMFLOAT2& worldPos)
{
	D3D12_VIEWPORT viewport = GET_INSTANCE(Camera)->GetViewPort();

	float x = ((worldPos.x + 1) * viewport.Width) / 2.0f;
	float y = ((1 - worldPos.y) * viewport.Height) / 2.0f;
	float gapX = sizeX * 0.5f;
	float gapY = sizeY * 0.5f;

	screenPos.left = x - gapX;
	screenPos.top = y - gapY;
	screenPos.right = x + gapX;
	screenPos.bottom = y + gapY;
}

//void D2DManager::ScreenToWorld()
//{
//	D3D12_VIEWPORT viewport = GET_INSTANCE(Camera)->GetViewPort();
//
//	m_WorldPosition.x = (((2.0f * m_ScreenPosition.x) / viewport.Width) - 1) / 1;
//	m_WorldPosition.y = -(((2.0f * m_ScreenPosition.y) / viewport.Height) - 1) / 1;
//}

void D2DManager::Render(const XMFLOAT2& pos)
{
	for (auto iter = m_ImageInfoMap.begin(); iter != m_ImageInfoMap.end(); )
	{
		D2D1_RECT_F screenPos = { 0 };
		WorldToScreen(screenPos, (*iter).second.m_SizeX, (*iter).second.m_SizeY, pos);
		m_pRenderTarget->DrawBitmap((*iter).second.m_Bitmap, screenPos);
	}
}

void D2DManager::Render(const string& key, const XMFLOAT2& pos)
{
	auto iter = m_ImageInfoMap.find(key);
	if (iter != m_ImageInfoMap.end())
	{
		D2D1_RECT_F screenPos = { 0 };
		WorldToScreen(screenPos, (*iter).second.m_SizeX, (*iter).second.m_SizeY, pos);

		int width = (*iter).second.m_WidthPixel;
		int height = (*iter).second.m_HeightPixel;

		int totalX = (*iter).second.m_TotalFrameX;
		int totalY = (*iter).second.m_TotalFrameY;

		int frameX = (*iter).second.m_FrameXNum;
		int frameY = (*iter).second.m_FrameYNum;

		D2D1_RECT_F framePos;
		//< 전체크기를 전체X프레임으로 나눠 앞으로 뿌려줄 X프레임의 크기를 구한다.
		int nX = width / totalX;
		//< 전체크기를 전체Y프레임으로 나눠 앞으로 뿌려줄 Y프레임의 크기를 구한다.
		int nY = height / totalY;

		//< 최종적으로 뿌릴 위치를 담는다. 
		framePos.left = static_cast<float>(nX * frameX);
		framePos.top = static_cast<float>(nY * frameY);
		framePos.right = static_cast<float>(framePos.left + nX);
		framePos.bottom = static_cast<float>(framePos.top + nY);

		m_pRenderTarget->DrawBitmap((*iter).second.m_Bitmap, screenPos, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, framePos);
	}
}

void D2DManager::Render(const string& key, const XMFLOAT2& pos, int fx, int fy)
{
	auto iter = m_ImageInfoMap.find(key);
	if (iter != m_ImageInfoMap.end())
	{
		D2D1_RECT_F screenPos = { 0 };
		WorldToScreen(screenPos, (*iter).second.m_SizeX, (*iter).second.m_SizeY, pos);

		int width = (*iter).second.m_WidthPixel;
		int height = (*iter).second.m_HeightPixel;

		int totalX = (*iter).second.m_TotalFrameX;
		int totalY = (*iter).second.m_TotalFrameY;

		D2D1_RECT_F framePos;
		//< 전체크기를 전체X프레임으로 나눠 앞으로 뿌려줄 X프레임의 크기를 구한다.
		int nX = width / totalX;
		//< 전체크기를 전체Y프레임으로 나눠 앞으로 뿌려줄 Y프레임의 크기를 구한다.
		int nY = height / totalY;

		//< 최종적으로 뿌릴 위치를 담는다. 
		framePos.left = static_cast<float>(nX * fx);
		framePos.top = static_cast<float>(nY * fy);
		framePos.right = static_cast<float>(framePos.left + nX);
		framePos.bottom = static_cast<float>(framePos.top + nY);

		m_pRenderTarget->DrawBitmap((*iter).second.m_Bitmap, screenPos, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, framePos);
	}
}

void D2DManager::Render(const wstring& text, const string& font, const string& color, D2D1_RECT_F& pos)
{
	m_pRenderTarget->DrawTextW(text.c_str(), text.length(), m_FontInfoMap[font].m_pFont, &pos, m_FontColorMap[color]);
}

