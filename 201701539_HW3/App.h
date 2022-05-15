#pragma once
#include "stdafx.h"
#include "Player.h"
#include "Enemy.h"
#include "Animation.h"
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

class App
{
public:
	App();
	~App();
	HRESULT Initialize(HINSTANCE hInstance);  // Register the window class and call methods for instantiating drawing resources
	void RunMessageLoop();  // Process and dispatch messages

private:
	HRESULT CreateDeviceIndependentResources();  // Initialize device-independent resources.
	HRESULT CreateDeviceResources();  // Initialize device-dependent resources.
	void DiscardDeviceResources();  // Release device-dependent resource.
	HRESULT OnRender();   // Draw content.
	void OnResize(UINT width, UINT height);  // Resize the render target.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);  // The windows procedure.

	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR uri, UINT destinationWidth, UINT destinationHeight, __deref_out ID2D1Bitmap** ppBitmap);
private:
	HWND m_hwnd;
	ID2D1Factory* m_pDirect2dFactory;
	IDWriteFactory* m_pDWriteFactory;
	IWICImagingFactory* m_pWICFactory;

	ID2D1HwndRenderTarget* m_pRenderTarget;

	ID2D1Bitmap* m_pBackground;

	ID2D1Bitmap* m_pMeteor;
	ID2D1PathGeometry* m_pMeteorPath;
	ID2D1SolidColorBrush* m_pMeteorPathBrush;
	
	MeteorAnimation<float> m_Animation;
	LARGE_INTEGER m_nPrevTime;
	LARGE_INTEGER m_nFrequency;
private:
	Player* m_pPlayer;
	Enemy* m_pEnemy;

};

template<class Interface>
inline void SafeRelease(
	Interface** ppInterfaceToRelease
)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif



#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif