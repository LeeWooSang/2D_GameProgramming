#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
using namespace std;

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <Windows.h>

enum DEFINE { START_X = 0, START_Y = 0 };

constexpr int MAX_WORKER_THREAD = 4;
constexpr int MAX_USER = 2000;

#include "Protocol.h"

enum EVENT_TYPE { EV_QUIT, EV_RECV, EV_SEND };
struct OverEx
{
	WSAOVERLAPPED	overlapped;
	WSABUF					dataBuffer;
	char							messageBuffer[BUF_SIZE];
	EVENT_TYPE				eventType;
};
