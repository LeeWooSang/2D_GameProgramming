#include "Network.h"
#include "../Scene/SceneManager.h"
#include "../Scene/InGameScene/InGameScene.h"
#include "../GameObject/Player/Player.h"

INIT_INSTACNE(Network)

Network::Network()
	: m_Socket(0), m_InPacketSize(0), m_SavedPacketSize(0)
{
	ZeroMemory(m_SendBuf, sizeof(char) * BUF_SIZE);
	ZeroMemory(m_RecvBuf, sizeof(char) * BUF_SIZE);

	m_SendWsaBuf.buf = m_SendBuf;
	m_SendWsaBuf.len = BUF_SIZE;
	m_RecvWsaBuf.buf = m_RecvBuf;
	m_RecvWsaBuf.len = BUF_SIZE;

	ZeroMemory(m_PacketBuf, sizeof(char) * BUF_SIZE);
}

Network::~Network()
{
	if(m_Socket != 0)
		closesocket(m_Socket);

	WSACleanup();

	cout << "Network - 소멸자" << endl;
}

bool Network::Initialize()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		WSACleanup();
		return false;
	}

	// socket()
	m_Socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	if (m_Socket == INVALID_SOCKET)
	{
		closesocket(m_Socket);
		WSACleanup();
		return false;
	}

	return true;
}

void Network::Connect(HWND hWnd)
{	
	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(SOCKADDR_IN));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	serveraddr.sin_port = htons(PORT);

	int result = WSAConnect(m_Socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr), nullptr, nullptr, nullptr, nullptr);
	if (result == SOCKET_ERROR)
	{
		result = WSAGetLastError();
		cout << "Connect Error : " << result << endl;
		//m_ConnectState = CONNECT_STATE::FAIL;
		return;	
	}

	WSAAsyncSelect(m_Socket, hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	cout << "서버 연결 성공!!" << endl;
	//m_ConnectState = CONNECT_STATE::OK;
}

void Network::Recv()
{
	unsigned long iobyte, ioflag = 0;

	int result = WSARecv(m_Socket, &m_RecvWsaBuf, 1, &iobyte, &ioflag, nullptr, nullptr);
	if (result)
	{
		result = WSAGetLastError();
		cout << "Recv Error - " << result << endl;
		return;
	}

	unsigned char* ptr = reinterpret_cast<unsigned char*>(m_RecvBuf);

	while (iobyte != 0)
	{
		if (m_InPacketSize == 0)
			m_InPacketSize = ptr[0];

		if (iobyte + m_SavedPacketSize >= m_InPacketSize)
		{
			memcpy(m_PacketBuf + m_SavedPacketSize, ptr, m_InPacketSize - m_SavedPacketSize);

			// 패킷 처리
			ProcessPacket(m_PacketBuf);

			ptr += m_InPacketSize - m_SavedPacketSize;
			iobyte -= m_InPacketSize - m_SavedPacketSize;
			m_InPacketSize = 0;
			m_SavedPacketSize = 0;
		}

		else
		{
			memcpy(m_PacketBuf + m_SavedPacketSize, ptr, iobyte);
			m_SavedPacketSize += iobyte;
			iobyte = 0;
		}
	}
}

void Network::Send()
{
	unsigned long iobyte = 0;
	WSASend(m_Socket, &m_SendWsaBuf, 1, &iobyte, 0, nullptr, nullptr);
}

void Network::ProcessPacket(char* buf)
{
	switch (buf[1])
	{
	case SC_LOGIN_OK:
		{
			SC_Packet_Login_OK* packet = reinterpret_cast<SC_Packet_Login_OK*>(buf);
			m_MyID = packet->id;
			break;
		}

	case SC_LOGIN_FAIL:
		{
			SC_Packet_Login_Fail* packet = reinterpret_cast<SC_Packet_Login_Fail*>(buf);
			break;
		}

	case SC_ADD_OBJECT:
		{
			SC_Packet_Add_Object* packet = reinterpret_cast<SC_Packet_Add_Object*>(buf);
			int id = packet->id;

			GameObject* object = GET_INSTANCE(SceneManager)->GetInGameScene()->FindObject(id);
			if (object != nullptr)
			{
				object->SetX(packet->x);
				object->SetY(packet->y);
			}

			break;
		}

	case SC_POSITION:
		{
			SC_Packet_Position* packet = reinterpret_cast<SC_Packet_Position*>(buf);
			int id = packet->id;

			GameObject* object = GET_INSTANCE(SceneManager)->GetInGameScene()->FindObject(id);
			if (object != nullptr)
			{
				object->SetX(packet->x);
				object->SetY(packet->y);
			}

			break;
		}

	case SC_REMOVE_OBJECT:
		break;

	default:
		break;
	}
}

void Network::ProcessWindowMessage(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))
	{
		::PostQuitMessage(0);
		return;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
		Recv();
		break;

	case FD_CLOSE:
		::PostQuitMessage(0);
		break;
	}
}

void Network::Send_Move_Packet(char dir)
{
	CS_Packet_Move* packet = reinterpret_cast<CS_Packet_Move*>(m_SendBuf);
	packet->size = sizeof(CS_Packet_Move);
	packet->type = CS_MOVE;
	packet->direction = dir;

	m_SendWsaBuf.len = sizeof(CS_Packet_Move);

	Send();
}

