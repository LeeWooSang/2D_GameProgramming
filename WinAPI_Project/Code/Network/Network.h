#pragma once
#include "../Macro.h"
#include "../Defines.h"
#include "../../../Server/Code/Protocol.h"

class Network
{
	SINGLE_TONE(Network)

	bool Initialize();
	void Connect(HWND);
	void Recv();
	void Send();
	void ProcessPacket(char*);
	void ProcessWindowMessage(WPARAM, LPARAM);

	void SetMyID(int id) { m_MyID = id; }
	int GetMyID()	const { return m_MyID; }

private:
	SOCKET m_Socket;

	WSABUF	m_SendWsaBuf;
	char 	m_SendBuf[BUF_SIZE];
	WSABUF	m_RecvWsaBuf;
	char	m_RecvBuf[BUF_SIZE];

	char	m_PacketBuf[BUF_SIZE];
	unsigned long	m_InPacketSize;
	unsigned long	m_SavedPacketSize;

	int m_MyID;
};