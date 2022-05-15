#pragma once
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite")
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files:
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <time.h>
#include <math.h>

// DX Header Files:
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#include <string>
#include <list>
#include <algorithm>
using namespace std;

// 자원 안전 반환 매크로.
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }