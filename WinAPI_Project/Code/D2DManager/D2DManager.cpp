#include "D2DManager.h"

INIT_INSTACNE(D2DManager)

D2DManager::D2DManager()
	: m_pFactory(nullptr), m_pRenderTarget(nullptr), m_pWriteFactory(nullptr), m_pFontCollection(nullptr), m_pWICImagingFactory(nullptr)
{
}

D2DManager::~D2DManager()
{
	//���� �ִ� ��Ʈ���̹����� ��� ����
	for (auto iter = m_ImageInfoMap.begin(); iter != m_ImageInfoMap.end(); )
	{
		(*iter).second.m_Bitmap->Release();
		iter = m_ImageInfoMap.erase(iter);
	}
	m_ImageInfoMap.clear();

	// ��Ʈ �÷��� ��� ����
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

	cout << "D2DManager - �Ҹ���" << endl;
}

bool D2DManager::Initialize(HWND hwnd)
{
	// D2DManager Factory ����
	HRESULT result = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory);
	if (result != S_OK)
		return false;

	// �ް�ü �ʱ�ȭ
	result = CoInitialize(nullptr);
	if (result != S_OK)
		return false;

	// ����Ÿ�� ����
	D2D1_SIZE_U size = D2D1::SizeU(FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	D2D1_RENDER_TARGET_PROPERTIES d2dRTProps = D2D1::RenderTargetProperties();
	D2D1_HWND_RENDER_TARGET_PROPERTIES d2dHwndRTProps = D2D1::HwndRenderTargetProperties(hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY);
	result = m_pFactory->CreateHwndRenderTarget(d2dRTProps, d2dHwndRTProps, &m_pRenderTarget);
	if (result != S_OK)
		return false;

	// �ؽ�Ʈ�� ���� Factory ����
	result = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory5), (IUnknown**)&m_pWriteFactory);
	if (result != S_OK)
		return false;

	// �̹����� ���� Factory ����
	result = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (void**)&m_pWICImagingFactory);
	if (result != S_OK)
		return false;

	return true;
}

bool D2DManager::CreateBitmapImage(string key, ImageInfo info)
{
	// [ Bitmap �̹��� �ʱ�ȭ ��� ] 
	// 1. Com��ü �ʱ�ȭ
	// 2. IWICImagingFactory ����
	// 3. Decoder ����
	// 4. �̹����� ������ ������
	// 5. Converter ����
	// 6. Bitmap ����

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
	// ��Ʈ ���
	wstring fontPath[] = { L"../Resource/Font/a�ǿ��ǵ���.ttf", L"../Resource/Font/�����ý��丮.ttf" };

	// ��Ʈ�� ���� ��ġ�Ҷ� ���
	//AddFontResourceEx(fontPath[0].c_str(), FR_PRIVATE, 0);
	//AddFontResourceEx(fontPath[1].c_str(), FR_PRIVATE, 0);

	// ��Ʈ�� ��ġ���� �ʰ�, �޸𸮿� �÷��� ����� ���
	// ���� ����
	IDWriteFontSetBuilder1* pFontSetBuilder;
	HRESULT result = m_pWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);

	IDWriteFontFile* pFontFile[MAX_FONT_COUNT];
	IDWriteFontSet* pFontSet[MAX_FONT_COUNT];

	wstring FontName[MAX_FONT_COUNT];
	unsigned int max_length = 64;

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		// �ش��ϴ� ��ο��� ��Ʈ ������ �ε��Ѵ�.
		result = m_pWriteFactory->CreateFontFileReference(fontPath[i].c_str(), nullptr, &pFontFile[i]);
		// ������ ��Ʈ �߰�
		result = pFontSetBuilder->AddFontFile(pFontFile[i]);
		result = pFontSetBuilder->CreateFontSet(&pFontSet[i]);
		// ��Ʈ Collection�� ��Ʈ �߰� => ��Ʈ Collection���� ���� ����� ��Ʈ�� ����Ǿ� ����
		result = m_pWriteFactory->CreateFontCollectionFromFontSet(pFontSet[i], &m_pFontCollection);

		// ��Ʈ �̸��� ������ ���
		IDWriteFontFamily* pFontFamily;
		IDWriteLocalizedStrings* pLocalizedFontName;

		result = m_pFontCollection->GetFontFamily(i, &pFontFamily);
		result = pFontFamily->GetFamilyNames(&pLocalizedFontName);
		// ����Ǿ��ִ� ��Ʈ�� �̸��� ����
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

	float dx = ((float)FRAME_BUFFER_WIDTH / (float)1280);
	float dy = ((float)FRAME_BUFFER_HEIGHT / (float)720);

	float dTotal = dx * dy;
	float fontSize = (25.f * dTotal);

	// ��Ʈ ��ü
	IDWriteTextFormat* pFont[MAX_FONT_COUNT];
	// ��Ʈ ����
	IDWriteTextLayout*	pTextLayout[MAX_FONT_COUNT];
	wstring wstr = L"TextLayout Initialize";

	for (int i = 0; i < MAX_FONT_COUNT; ++i)
	{
		// ��Ʈ ��ü ����	
		result = m_pWriteFactory->CreateTextFormat(FontName[i].c_str(), m_pFontCollection, DWRITE_FONT_WEIGHT_DEMI_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"en-US", &pFont[i]);

		if (i == 0)
		{
			// ��Ʈ�� �߾ӿ� ���Ľ�Ű��
			result = pFont[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
			result = pFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			result = m_pWriteFactory->CreateTextLayout(wstr.c_str(), wstr.length(), pFont[i], 1024.0f, 1024.0f, &pTextLayout[i]);

			m_FontInfoMap.emplace("�ǿ��ǵ���", FontInfo(pFont[i], pTextLayout[i], fontSize));
		}
		else
		{
			result = pFont[i]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
			result = pFont[i]->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
			result = m_pWriteFactory->CreateTextLayout(wstr.c_str(), wstr.length(), pFont[i], 4096.0f, 4096.0f, &pTextLayout[i]);

			m_FontInfoMap.emplace("������", FontInfo(pFont[i], pTextLayout[i], fontSize));
		}
	}
}

void D2DManager::CreateGameFontColor()
{
	ID2D1SolidColorBrush* pColor[MAX_COLOR_COUNT];

	int index = 0;
	HRESULT result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DeepPink, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("��ȫ��", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Brown, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("����", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("���", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("������", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("�ϴû�", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LavenderBlush, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("����ȫ��", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("������", pColor[index++]);

	result = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange, 1.0f), &pColor[index]);
	m_FontColorMap.emplace("��Ȳ��", pColor[index++]);
}

void D2DManager::Render()
{
	for (auto iter = m_ImageInfoMap.begin(); iter != m_ImageInfoMap.end(); )
		m_pRenderTarget->DrawBitmap((*iter).second.m_Bitmap, (*iter).second.m_Pos);
}

void D2DManager::Render(string key)
{
	auto iter = m_ImageInfoMap.find(key);
	if (iter != m_ImageInfoMap.end())
	{
		int width = (*iter).second.m_WidthPixel;
		int height = (*iter).second.m_HeightPixel;

		int totalX = (*iter).second.m_TotalFrameX;
		int totalY = (*iter).second.m_TotalFrameY;

		int frameX = (*iter).second.m_FrameXNum;
		int frameY = (*iter).second.m_FrameYNum;

		D2D1_RECT_F framePos;
		//< ��üũ�⸦ ��üX���������� ���� ������ �ѷ��� X�������� ũ�⸦ ���Ѵ�.
		int nX = width / totalX;
		//< ��üũ�⸦ ��üY���������� ���� ������ �ѷ��� Y�������� ũ�⸦ ���Ѵ�.
		int nY = height / totalY;

		//< ���������� �Ѹ� ��ġ�� ��´�. 
		framePos.left = static_cast<float>(nX * frameX);
		framePos.top = static_cast<float>(nY * frameY);
		framePos.right = static_cast<float>(framePos.left + nX);
		framePos.bottom = static_cast<float>(framePos.top + nY);

		m_pRenderTarget->DrawBitmap((*iter).second.m_Bitmap, (*iter).second.m_Pos, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, framePos);
	}
}

void D2DManager::Render(string key, D2D1_RECT_F pos, int fx, int fy)
{
	auto iter = m_ImageInfoMap.find(key);
	if (iter != m_ImageInfoMap.end())
	{
		int width = (*iter).second.m_WidthPixel;
		int height = (*iter).second.m_HeightPixel;

		int totalX = (*iter).second.m_TotalFrameX;
		int totalY = (*iter).second.m_TotalFrameY;

		D2D1_RECT_F framePos;
		//< ��üũ�⸦ ��üX���������� ���� ������ �ѷ��� X�������� ũ�⸦ ���Ѵ�.
		int nX = width / totalX;
		//< ��üũ�⸦ ��üY���������� ���� ������ �ѷ��� Y�������� ũ�⸦ ���Ѵ�.
		int nY = height / totalY;

		//< ���������� �Ѹ� ��ġ�� ��´�. 
		framePos.left = static_cast<float>(nX * fx);
		framePos.top = static_cast<float>(nY * fy);
		framePos.right = static_cast<float>(framePos.left + nX);
		framePos.bottom = static_cast<float>(framePos.top + nY);

		m_pRenderTarget->DrawBitmap((*iter).second.m_Bitmap, pos, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, framePos);
	}
}

void D2DManager::Render(string font, string color, wstring text, D2D1_RECT_F pos)
{
	m_pRenderTarget->DrawTextW(text.c_str(), text.length(), m_FontInfoMap[font].m_pFont, &pos, m_FontColorMap[color]);
}
