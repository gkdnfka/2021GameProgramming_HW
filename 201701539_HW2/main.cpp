#include "stdafx.h"

std::vector<stu> stu_vec;
std::wstring nameArr[10] = { L"������", L"��ö��", L"�ֱ浿", L"������", L"ȫ����", L"���ؽ�", L"����ȯ", L"���ѹ�", L"������", L"���μ�" };

POINT cursorPos;
float scale, rotate = 0.0f;

bool isDragNow = false;
bool isStuInit = false;
bool isStuDel = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	srand(time(NULL));
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;
			if (SUCCEEDED(app.Initialize(hInstance)))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}
	return 0;
}
DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pDirect2dFactory(NULL),
	m_pDWriteFactory(NULL),
	m_pRenderTarget(NULL),
	m_pLightSlateGrayBrush(NULL),
	m_pCornflowerBlueBrush(NULL),
	m_pPosTextBrush(NULL),
	m_pStuInforTextBrush(NULL)
{
}

DemoApp::~DemoApp()
{
	DiscardDeviceResources();
	SAFE_RELEASE(m_pDirect2dFactory);
}

HRESULT DemoApp::Initialize(HINSTANCE hInstance)
{

	cursorPos.x = 0.0f;
	cursorPos.y = 0.0f;
	HRESULT hr;


	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{

		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = DemoApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = hInstance;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = L"D2DDemoApp";
		RegisterClassEx(&wcex);


		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"HW2",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			1024, 768, NULL, NULL, hInstance, this
		);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);

		}
	}

	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	static const WCHAR msc_fontName[] = L"Consolas";
	static const FLOAT msc_fontSize = 13;
	HRESULT hr = S_OK;

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);
	}
	if (SUCCEEDED(hr))
	{
		// Create a DirectWrite text format object.
		hr = m_pDWriteFactory->CreateTextFormat(
			msc_fontName,
			NULL,
			DWRITE_FONT_WEIGHT_LIGHT,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			msc_fontSize,
			L"euc-kr",
			&m_pTextFormat
		);
	}
	if (SUCCEEDED(hr))
	{
		// Center the text horizontally and vertically.
		m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

		m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);
	return hr;
}


HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRenderTarget)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		hr = m_pDirect2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRenderTarget);

		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGray), &m_pLightSlateGrayBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), &m_pCornflowerBlueBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pPosTextBrush);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &m_pStuInforTextBrush);
		}
	}

	return hr;
}

// ��ġ ������ �ڿ����� �ݳ���. ��ġ�� �ҽǵǸ� �̵� �ڿ��� �ٽ� �����ؾ� ��.
void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRenderTarget);
	SAFE_RELEASE(m_pLightSlateGrayBrush);
	SAFE_RELEASE(m_pCornflowerBlueBrush);
}

// ���� ������ �޽��� ������ ������.
void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CalcScale(D2D1_SIZE_F rtSize) {
	float destY = (rtSize.height - 100.0f - (100.0f * (stu_vec.size()-1))) + 50.0f;
	scale = (1 / (50.0f - destY)) * (destY - cursorPos.y) + 2;

	if (scale < 1.0f)
		scale = 1.0f;
	if (scale > 2.0f)
		scale = 2.0f;
}

void CalcRotate(D2D1_SIZE_F rtSize) {
	float destY = (rtSize.height - 100.0f - (100.0f * (stu_vec.size() - 1))) + 50.0f;
	rotate = (360 / (50.0f - destY)) * (destY - cursorPos.y);
}
// Ŭ���̾�Ʈ ������ ȭ���� �׸�.
// ����: �� �Լ��� ����Ǵ� ���ȿ� ��ġ�� �ҽǵǸ� ��ġ ������ �ڿ����� �ݳ���. �� ���� ȣ�� �ÿ� �� �ڿ����� �ٽ� ������.
HRESULT DemoApp::OnRender()
{
	HRESULT hr = S_OK;

	hr = CreateDeviceResources();

	if (SUCCEEDED(hr))
	{
		m_pRenderTarget->BeginDraw();
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
		m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

		D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

		D2D1_ROUNDED_RECT stu_init_rect = D2D1::RoundedRect(
			D2D1::Rect(rtSize.width / 2 - 100.0f, 0.0f,
				rtSize.width / 2 + 100.0f, 100.0f), 10.0f, 10.0f
		);

		m_pRenderTarget->FillRoundedRectangle(&stu_init_rect, m_pLightSlateGrayBrush);

		std::string informText = "Mouse X,Y : " + std::to_string(cursorPos.x) + ',' + std::to_string(cursorPos.y) + '\n';
		informText += "Scale : " + std::to_string(scale) + '\n';
		informText += "Rotate : " + std::to_string(rotate);

		std::wstring wide = std::wstring(informText.begin(), informText.end());

		m_pRenderTarget->DrawText(wide.c_str(),
			wide.size(), m_pTextFormat,
			D2D1::RectF(10, 10, 100, 100),
			m_pPosTextBrush);


		//�л� ���� �߰� ������ �׸�.
		if (isDragNow && (isStuInit || isStuDel)) {
			
			if (isStuInit) {

				D2D1_ROUNDED_RECT stu_const_temp_rect = D2D1::RoundedRect(
					D2D1::Rect(rtSize.width / 2 - 200.0f, rtSize.height - 100.0f - (100.0f * (stu_vec.size() - 1)),
						rtSize.width / 2 + 200.0f, rtSize.height - (100.0f * (stu_vec.size() - 1))), 10.0f, 10.0f
				);
				m_pRenderTarget->DrawRoundedRectangle(&stu_const_temp_rect, m_pCornflowerBlueBrush);
			}

			CalcScale(rtSize);
			CalcRotate(rtSize);
			D2D1_MATRIX_3X2_F Scaling = D2D1::Matrix3x2F::Scale(D2D1::SizeF(scale, 1.0f), D2D1::Point2(cursorPos.x, cursorPos.y));
			D2D1_MATRIX_3X2_F Rotation = D2D1::Matrix3x2F::Rotation(rotate, D2D1::Point2(cursorPos.x, cursorPos.y));
			m_pRenderTarget->SetTransform(Scaling * Rotation);

			D2D1_ROUNDED_RECT stu_rect = D2D1::RoundedRect(
				D2D1::RectF(cursorPos.x - 100.0f , cursorPos.y - 50.0f, 
					cursorPos.x + 100.0f, cursorPos.y + 50.0f), 10.0f, 10.0f);
			
			m_pRenderTarget->FillRoundedRectangle(stu_rect, m_pLightSlateGrayBrush);
			
			auto initStu = stu_vec.back();

			std::wstring stuInform = L"�̸�: " + initStu.name + L" ���� : " + std::to_wstring(initStu.score);

			m_pRenderTarget->DrawText(stuInform.c_str(),
				stuInform.length(), m_pTextFormat,
				D2D1::RectF(cursorPos.x - 100.0f, cursorPos.y - 50.0f,
					cursorPos.x + 100.0f, cursorPos.y + 50.0f), m_pStuInforTextBrush
				);

			m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity()); //transform ����ġ.
			
		}

		if (isDragNow) { //drag ���� ������ �� ���� ��ġ�� �簢���� ������ �ʾƾ��Ѵ�.
			for (auto iter = stu_vec.begin(); iter != stu_vec.end()-1; iter++) {
				int idx = std::distance(stu_vec.begin(), iter);
				D2D1_ROUNDED_RECT stu_store_rect = D2D1::RoundedRect(
					D2D1::Rect(rtSize.width / 2 - 200.0f, rtSize.height - 100.0f - (100.0f * (idx)),
						rtSize.width / 2 + 200.0f, rtSize.height - (100.0f * (idx))), 10.0f, 10.0f
				);
				m_pRenderTarget->FillRoundedRectangle(&stu_store_rect, m_pCornflowerBlueBrush);

				std::wstring stuInform = L"�̸�: " + (iter)->name + L" ���� : " + std::to_wstring((iter)->score);
	
				m_pRenderTarget->DrawText(stuInform.c_str(),
					stuInform.length(), m_pTextFormat,
					D2D1::Rect(rtSize.width / 2 - 200.0f, rtSize.height - 100.0f - (100.0f * (idx)),
						rtSize.width / 2 + 200.0f, rtSize.height - (100.0f * (idx))), m_pStuInforTextBrush
				);
			}
		}
		else { //drag���� �ƴϸ� ��ü ���
			for (auto iter = stu_vec.begin(); iter != stu_vec.end(); iter++) {
				int idx = std::distance(stu_vec.begin(), iter);
				D2D1_ROUNDED_RECT stu_store_rect = D2D1::RoundedRect(
					D2D1::Rect(rtSize.width / 2 - 200.0f, rtSize.height - 100.0f - (100.0f * (idx)),
						rtSize.width / 2 + 200.0f, rtSize.height - (100.0f * (idx))), 10.0f, 10.0f
				);
				m_pRenderTarget->FillRoundedRectangle(&stu_store_rect, m_pCornflowerBlueBrush);

				std::wstring stuInform = L"�̸�: " + (iter)->name + L" ���� : " + std::to_wstring((iter)->score);
	
				m_pRenderTarget->DrawText(stuInform.c_str(),
					stuInform.length(), m_pTextFormat,
					D2D1::Rect(rtSize.width / 2 - 200.0f, rtSize.height - 100.0f - (100.0f * (idx)),
						rtSize.width / 2 + 200.0f, rtSize.height - (100.0f * (idx))), m_pStuInforTextBrush
				);
			}
		}
		hr = m_pRenderTarget->EndDraw();
	}

	if (hr == D2DERR_RECREATE_TARGET)
	{
		hr = S_OK;
		DiscardDeviceResources();
	}

	return hr;
}


// ���� ���α׷����� WM_SIZE �޽����� �߻��Ǹ� �� �Լ��� ����Ÿ���� ũ�⸦ �ٽ� ������.
void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRenderTarget)
	{
		// ����: �Ʒ��� �Լ��� ���� �ÿ� ������ ���� ������, ���⿡�� ���� ������ üũ���� �ʰ� �Ѿ�� ��. ������ EndDraw �Լ��� ȣ��� ���� ������ Ȯ�ε� ���̹Ƿ� �׶� ó���ϸ� ��.
		m_pRenderTarget->Resize(D2D1::SizeU(width, height));
	}
}

bool isInInitBox(POINT p) {

	if (p.x >= 1024.0f / 2 - 100.0f && p.x <= 1024.0f / 2 + 100.0f) {
		if (p.y >= 0.0f && p.y <= 100.0f) {
			return true;
		}
	}

	return false;
}

bool isInTop(POINT p) {
	if (stu_vec.size() > 0) {
		if (p.x >= 1024.0f / 2 - 200.0f && p.x <= 1024.0f / 2 + 200.0f) {
			if (p.y >= 768.0f - 100.0f - (100.0f * (stu_vec.size()-1)) 
				&& p.y <= 768.0f - (100.0f * (stu_vec.size()-1))) {
				return true;
			}
		}
	}
	return false;
}

// ������ �޽����� ó��.
LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}
	else
	{
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DISPLAYCHANGE:
			{
				//InvalidateRect(hwnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_PAINT:
			{
				pDemoApp->OnRender();
				//ValidateRect(hwnd, NULL);
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

			case WM_MOUSEMOVE:
				cursorPos.x = LOWORD(lParam);
				cursorPos.y = HIWORD(lParam);

				result = 1;
				wasHandled = true;
				//InvalidateRect(hwnd, NULL, true);
				break;

			case WM_LBUTTONDOWN:
				cursorPos.x = LOWORD(lParam);
				cursorPos.y = HIWORD(lParam);
				if (isInInitBox(cursorPos)) {
					//MessageBox(hwnd, L"is Init!", L"check", MB_OK);
					if (stu_vec.size() >= 6) {
						MessageBox(hwnd, L"6���� �ִ��Դϴ�!", L"check", MB_OK);
					}
					else {
						isDragNow = true;
						isStuInit = true;

						stu_vec.push_back(stu(nameArr[rand() % 10], rand() % 60 + 30));
					}
				}

				if (isInTop(cursorPos)) {
					isDragNow = true;
					isStuDel = true;
				}
				
				result = 1;
				wasHandled = true;
				//InvalidateRect(hwnd, NULL, true);
				break;

			case WM_LBUTTONUP:
				cursorPos.x = LOWORD(lParam);
				cursorPos.y = HIWORD(lParam);

				if (isDragNow) {
					isDragNow = false;
					if (isInTop(cursorPos)) {
						isStuInit = false;
						std::sort(stu_vec.begin(), stu_vec.end(), [](stu a, stu b) { return a > b; });
					}

					if (isStuInit) {
						isStuInit = false;
						stu_vec.pop_back();
					}

					if (isStuDel) {
						isStuDel = false;
						if (isInInitBox(cursorPos)) {
							stu_vec.pop_back();
						}
					}

					scale = 0.0f;
					rotate = 0.0f;
				}

				result = 1;
				wasHandled = true;
				//InvalidateRect(hwnd, NULL, true);
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
