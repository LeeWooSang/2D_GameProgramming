#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WM_SOCKET WM_USER + 1

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <random>
#include <chrono>
#include <thread>
using namespace std;

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <Windows.h>

#include <math.h>
#include "Default/Resource.h"

//다이렉트 2D 헤더

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <d2d1_3.h>
#include <dwrite_3.h>
#include <wincodec.h>

#pragma comment( lib, "d2d1.lib" )
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dwrite.lib")

constexpr int FRAME_BUFFER_WIDTH = 800;
constexpr int FRAME_BUFFER_HEIGHT = 600;