#pragma once
#include "../../../WinAPI_Project/Code/Macro.h"
#include "../Defines.h"

class IOCP
{
	SINGLE_TONE(IOCP)

	bool Initialize();
	void Run();

private:
	void ErrorDisplay(const char*, int);

	void ThreadPool();
	void ReleaseWorkerThread();

	void Accept();
	void ReleaseAcceptTrhead();

	void Recv(int);
	void Send(int, char*);

	void ProcessPacket(int, char*);

	void Disconnect(int);

private:
	void 	Send_Login_Ok_Packet(int);
	void Send_Login_Fail_Packet(int);
	void 	Send_Add_Object_Packet(int to, int obj);
	void 	Send_Position_Packet(int, int);
	void Send_Remove_Object_Packet(int to, int id);

private:
	HANDLE m_IOCPHandle;
	vector<thread> m_WorkerThread;
	vector<class Character*> m_Characters;
	bool m_IsRelease;
	SOCKET m_ListenSocket;
};