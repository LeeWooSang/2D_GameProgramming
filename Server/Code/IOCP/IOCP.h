#pragma once
#include "../../../WinAPI_Project/Code/Macro.h"
#include "../Defines.h"
#include "../Protocol.h"

constexpr int MAX_WORKER_THREAD = 4;
enum EVENT_TYPE { EV_RECV, EV_SEND, EV_MOVE, EV_DIE, EV_RESPAWN, EV_ATTACK, EV_HP_HEAL };
struct OverEx
{
	WSAOVERLAPPED	overlapped;
	WSABUF					dataBuffer;
	char							messageBuffer[BUF_SIZE];
	EVENT_TYPE				eventType;
	int								eventTargetID;
};

class IOCP
{
	SINGLE_TONE(IOCP)

	bool Initialize();
	void Run();

private:
	void ThreadPool();
	void ErrorDisplay(const char*, int);
	void Recv(int);
	void Send(int, char*);
	void ProcessPacket(int, char*);
	void Disconnect(int);

	HANDLE m_IOCPHandle;
	vector<thread> m_WorkerThread;
};