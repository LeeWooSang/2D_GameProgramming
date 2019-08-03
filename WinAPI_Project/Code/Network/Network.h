#pragma once
#include "../Macro.h"
#include "../Defines.h"

enum NETWORK { PORT = 9000, BUF_SIZE = 1024 };
#define SERVER_IP "127.0.0.1"

class Network
{
	SINGLE_TONE(Network)

	bool Initialize();
	void Connect(HWND);
	void Recv();
	void Send();
	void ProcessPacket(char*);
	void ProcessWindowMessage(WPARAM, LPARAM);

private:
	SOCKET m_Socket;

	WSABUF	m_SendWsaBuf;
	char 	m_SendBuf[BUF_SIZE];
	WSABUF	m_RecvWsaBuf;
	char	m_RecvBuf[BUF_SIZE];

	char	m_PacketBuf[BUF_SIZE];
	unsigned long	m_InPacketSize;
	unsigned long	m_SavedPacketSize;
};