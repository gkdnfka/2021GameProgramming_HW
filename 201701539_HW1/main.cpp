#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "stu.h"

std::vector<stu> stu_vec;
std::string nameArr[10] = { "이현민", "김철수", "최길동", "박현우", "홍현민", "엄준식", "이지환", "조한민", "정지우", "도민수" };
HWND MainWindowHandle = 0;

bool InitWindowsApp(HINSTANCE instanceHandle, int show);
int  Run();
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT OnPaint(HWND hWnd);

int WINAPI WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR pCmdLine, int nShowCmd)
{
	srand(time(NULL));
	if (!InitWindowsApp(hInstance, nShowCmd))
	{
		::MessageBox(0, "Init - Failed", "Error", MB_OK);
		return 0;
	}
	return Run();
}

bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instanceHandle;
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName = 0;
	wc.lpszClassName = "Hello";
	if (!::RegisterClass(&wc)) {
		::MessageBox(0, "RegisterClass - Failed", 0, 0);
		return false;
	}
	MainWindowHandle = ::CreateWindow(
		"Hello", "Hello", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, instanceHandle, 0);

	SetWindowPos(MainWindowHandle, 0, 500, 20, 1024, 768, SWP_NOZORDER);
	if (MainWindowHandle == 0) {
		::MessageBox(0, "CreateWindow - Failed", 0, 0);
		return false;
	}
	::ShowWindow(MainWindowHandle, show);
	::UpdateWindow(MainWindowHandle);
	return true;
}

int Run()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return msg.wParam;
}

bool is_top(POINT p) {
	int lastidx = stu_vec.size()-1;
	
	if (p.y >= 110 + (50 * (7 - lastidx)) && p.y <= 500 - (50 * lastidx)) {
		if (p.x >= 340 && p.x <= 684) {
			return true;
		}
	}
	
	return false;
}
LRESULT CALLBACK WndProc(HWND   windowHandle, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(MainWindowHandle);
		return 0;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		OnPaint(windowHandle);
		break;

	case WM_LBUTTONUP:
		
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(windowHandle, &p);

		if (is_top(p) && !stu_vec.empty()) {
			stu_vec.pop_back();
		}

		else if (stu_vec.empty() && is_top(p)) {
			MessageBox(windowHandle, "학생정보가 존재하지 않습니다.", "정보없음", 0);
		}

		else if (stu_vec.size() >= 7) {
			MessageBox(windowHandle, "학생정보를 더이상 추가할 수 없습니다.", "추가불가", 0);
		}
		else {
			stu_vec.push_back(stu(nameArr[rand()%10], rand() % 60 + 30));
		}
		std::sort(stu_vec.begin(), stu_vec.end(), [](stu a, stu b) { return a < b; });
		InvalidateRect(windowHandle, NULL, true);
		break;

	default:
		return ::DefWindowProc(windowHandle, msg, wParam, lParam);
	}

	return 0;

}

LRESULT OnPaint(HWND hWnd) {
	PAINTSTRUCT ps;
	BeginPaint(hWnd, &ps);

	RECT rc;
	GetClientRect(hWnd, &rc);
	FillRect(ps.hdc, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));

	for (auto iter = stu_vec.begin(); iter != stu_vec.end(); iter++) {
		int idx = std::distance(stu_vec.begin(), iter);
		HPEN hMyRedPen = CreatePen(PS_SOLID, 3, RGB(255 - (idx * 40), idx * 30, idx * 15));
		HGDIOBJ HoldPen = SelectObject(ps.hdc, hMyRedPen);

		Rectangle(ps.hdc, 340, 110 + (50 * (7 - idx)), 684, 500 - (50 * idx));

		std::string text = "이름 : " + (iter)->name + ", 점수 : " + std::to_string((iter)->score);
		TextOut(ps.hdc, 420, 110 + (50 * (7 - idx)) + 10, text.c_str(), text.size());

		SelectObject(ps.hdc, HoldPen);
		DeleteObject(hMyRedPen);
	}
	EndPaint(hWnd, &ps);
	return 0;
}