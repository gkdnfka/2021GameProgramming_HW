#include "stdafx.h"
#include "App.h"

static int tick = 0;

App::App() :
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pRenderTarget(NULL),
	m_pMeteorPath(NULL),
	m_Animation() {

	m_pPlayer = new Player(wstring(L".\\Images\\player_ship.png"), POINT{ 0, 768 / 2 });
	m_pEnemy = new Enemy(wstring(L".\\Images\\enemy_ship.png"), POINT{ 1024, 768 / 2 });
}

App::~App() {
	SafeRelease(&m_pDirect2dFactory);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pMeteorPath);
	SafeRelease(&m_pMeteorPathBrush);
}

void App::RunMessageLoop() {
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT App::Initialize(HINSTANCE hInstance)
{
	HRESULT hr;

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{
		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = App::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"HW3";

		RegisterClassEx(&wcex);


		// Create the window.
		m_hwnd = CreateWindow(
			L"HW3",
			L"HW3",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			1040,
			768,
			NULL,
			NULL,
			hInstance,
			this
		);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			float length;
			hr = m_pMeteorPath->ComputeLength(NULL, &length);
			if (SUCCEEDED(hr)) {
				m_Animation.SetStart(0);
				m_Animation.SetEnd(length);
				m_Animation.SetDuration(5.0f);

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}

	}

	QueryPerformanceFrequency(&m_nFrequency);
	QueryPerformanceCounter(&m_nPrevTime);

	return hr;
}

HRESULT App::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}

	if (SUCCEEDED(hr)) {

		hr = m_pDirect2dFactory->CreatePathGeometry(&m_pMeteorPath);
		if (SUCCEEDED(hr)) {
			ID2D1GeometrySink* pSink = NULL;
			hr = m_pMeteorPath->Open(&pSink);
			if (SUCCEEDED(hr)) {

				pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
				pSink->BeginFigure(D2D1::Point2F(150, 170), D2D1_FIGURE_BEGIN_FILLED);
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(400, 170), D2D1::SizeF(85, 85), 0.0f,
					D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
				pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(525, 170), D2D1::SizeF(85, 85), 0.0f,
					D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
				pSink->EndFigure(D2D1_FIGURE_END_CLOSED);

				pSink->Close();
				SafeRelease(&pSink);
			}
		}
	}

	return hr;
}

HRESULT App::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(
			rc.right - rc.left,
			rc.bottom - rc.top
		);

		// Create a Direct2D render target.
		hr = m_pDirect2dFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&m_pRenderTarget
		);


		if (SUCCEEDED(hr))
		{
			// Create a gray brush.
			hr = m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(D2D1::ColorF::Yellow),
				&m_pMeteorPathBrush
			);
		}


		if (SUCCEEDED(hr))
		{
			//backgorund bitmap load
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Images\\Background.jpg", 0, 0, &m_pBackground);
		}

		if (SUCCEEDED(hr)) {
			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, L".\\Images\\meteor.png", 0, 0, &m_pMeteor);
		}
		if (SUCCEEDED(hr)) {

			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, m_pPlayer->getPath().c_str(), 0, 0, m_pPlayer->getBitmapRef());
		}

		if (SUCCEEDED(hr)) {

			hr = LoadBitmapFromFile(m_pRenderTarget, m_pWICFactory, m_pEnemy->getPath().c_str(), 0, 0, m_pEnemy->getBitmapRef());
		}


	}
	return hr;
}

void App::DiscardDeviceResources()
{
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_pMeteorPathBrush);
}

LRESULT CALLBACK App::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		App* pApp = (App*)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(pApp)
		);

		result = 1;
	}
	else
	{
		App* pApp = reinterpret_cast<App*>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
				hwnd,
				GWLP_USERDATA
			)));

		bool wasHandled = false;

		if (pApp)
		{
			if (GetAsyncKeyState(VK_UP) & 0x8000) {
				pApp->m_pPlayer->moveUp();
				pApp->m_pEnemy->moveUp();
			}
			if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
				pApp->m_pPlayer->moveDown();
				pApp->m_pEnemy->moveDown();
			}

			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pApp->OnResize(width, height);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_PAINT:
			{
				pApp->OnRender();
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;

			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}


HRESULT App::OnRender()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();
	if (SUCCEEDED(hr)) {
		m_pRenderTarget->BeginDraw();

		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		D2D1_SIZE_F size = m_pBackground->GetSize();
		m_pRenderTarget->DrawBitmap(m_pBackground, D2D1::RectF(0, 0, 1024, 768));

		m_pRenderTarget->DrawGeometry(m_pMeteorPath, m_pMeteorPathBrush);

		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;
		static float ani_time = 0.0f;
		float length = m_Animation.GetValue(ani_time);
		m_pMeteorPath->ComputePointAtLength(length, NULL, &point, &tangent);
		D2D1_MATRIX_3X2_F meteorMatrix = D2D1::Matrix3x2F(
			tangent.x, tangent.y,
			-tangent.y, tangent.x,
			point.x, point.y);

		m_pRenderTarget->DrawBitmap(m_pMeteor, D2D1::RectF(point.x - (m_pMeteor->GetSize().width/2), point.y - m_pMeteor->GetSize().height / 2
			, point.x + m_pMeteor->GetSize().width/2, point.y + m_pMeteor->GetSize().height/2));
		size = m_pPlayer->getBitmap()->GetSize();
		m_pRenderTarget->DrawBitmap(m_pPlayer->getBitmap(), m_pPlayer->getDrawArea());

		m_pRenderTarget->DrawBitmap(m_pEnemy->getBitmap(), m_pEnemy->getDrawArea());
		hr = m_pRenderTarget->EndDraw();

		if (ani_time >= m_Animation.GetDuration())
		{
			ani_time = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;

			ani_time += elapsedTime;
		}
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}

void App::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here, because the error will be returned again
		// the next time EndDraw is called.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

HRESULT App::LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, ID2D1Bitmap** ppBitmap)
{
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;
	IWICBitmapScaler* pScaler = NULL;

	HRESULT hr = pIWICFactory->CreateDecoderFromFilename(uri, NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &pDecoder);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = pSource->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationHeight) / static_cast<FLOAT>(originalHeight);
					destinationWidth = static_cast<UINT>(scalar * static_cast<FLOAT>(originalWidth));
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = static_cast<FLOAT>(destinationWidth) / static_cast<FLOAT>(originalWidth);
					destinationHeight = static_cast<UINT>(scalar * static_cast<FLOAT>(originalHeight));
				}

				hr = pIWICFactory->CreateBitmapScaler(&pScaler);
				if (SUCCEEDED(hr))
				{
					hr = pScaler->Initialize(pSource, destinationWidth, destinationHeight, WICBitmapInterpolationModeCubic);
				}
				if (SUCCEEDED(hr))
				{
					hr = pConverter->Initialize(pScaler, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
		}
	}
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);
	SAFE_RELEASE(pScaler);

	return hr;
}
