#include "IOCP.h"
#include "../Character/Character.h"
#include <crtdbg.h>

INIT_INSTACNE(IOCP)

IOCP::IOCP()
	: m_IOCPHandle(nullptr)
{
	// �����߻��� �ѱ۷� ��µǵ��� ���
	_wsetlocale(LC_ALL, L"korean");
	wcout.imbue(locale("korean"));

	m_WorkerThread.reserve(MAX_WORKER_THREAD);

	m_IsRelease = false;
	m_ListenSocket = 0;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// ��µ� ��ȣ�� �־��ָ� �� �������� �ٷ� �̵������ش�.
	// [ ���� ]
	// Detected memory leaks!
	//	Dumping objects ->
	// {233} normal block at 0x000001469D91A680, 24 bytes long.
	// 233 �̶�� �������� ���� ����	
	//_CrtSetBreakAlloc(265);
}

IOCP::~IOCP()
{
	m_WorkerThread.clear();

	CloseHandle(m_IOCPHandle);
	m_IOCPHandle = nullptr;

	SAFE_DELETE_LIST(m_Characters);

	cout << "IOCP Server ����" << endl;
}

void IOCP::ErrorDisplay(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		nullptr
	);

	cout << msg;
	wcout << L"���� : " << lpMsgBuf << endl;

	// �����߻��� ���ѷ����� ���߰���
	while (true);
	LocalFree(lpMsgBuf);
}

bool IOCP::Initialize()
{
	// IOCP ��ü ����
	m_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (m_IOCPHandle == nullptr)
		return false;

	for (int i = 0; i < MAX_USER; ++i)
		m_Characters.emplace_back(new Player);

	cout << "IOCP Server Initialize OK!!" << endl;

	return true;
}

void IOCP::Run()
{
	// ��Ŀ ������ ����
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
		m_WorkerThread.emplace_back(thread{ &IOCP::ThreadPool, this });

	thread acceptThread { &IOCP::Accept, this };

	while (true)
	{
		if (KEY_DOWN(VK_ESCAPE) && m_IsRelease == false)
			m_IsRelease = true;

		if (KEY_UP(VK_ESCAPE) && m_IsRelease == true)
		{	
			for (auto& worker : m_WorkerThread)
				ReleaseWorkerThread();

			// ��Ŀ ������ ���� ���
			for (auto& worker : m_WorkerThread)
				worker.join();

			ReleaseAcceptTrhead();
			// accept ������ ���� ���
			acceptThread.join();
			break;
		}	
	}
}

void IOCP::ThreadPool()
{
	while (true)
	{
		unsigned long io_byte;
		unsigned long long id;
		OverEx* pOverEx;
		// lpover�� recv���� send���� ������ �־�� ��.
		int is_error = GetQueuedCompletionStatus(m_IOCPHandle, &io_byte, &id, reinterpret_cast<LPWSAOVERLAPPED*>(&pOverEx), INFINITE);

		//  GetQueuedCompletionStatus( )�� �������� �ƴ��� Ȯ���Ѵ�
		if (is_error == false)
		{
			int error_num = WSAGetLastError();
			if (error_num != 64)
				ErrorDisplay(" GetQueuedCompletionStatus()", error_num);
			else
			{
				cout << id << "�� Ŭ���̾�Ʈ ����" << endl;
				Disconnect((int)id);
				continue;
			}
		}

		// Ŭ��� �������ٸ� (Ŭ�� ������ ��)
		if (io_byte == 0)
		{
			cout << id << "�� Ŭ���̾�Ʈ ����" << endl;
			Disconnect((int)id);
			continue;
		}

		switch (pOverEx->eventType)
		{
		case EV_QUIT:
			{
				delete pOverEx;
				return;
			}
	
		case EV_RECV:
			{
				Player* player = reinterpret_cast<Player*>(m_Characters[id]);
				// ���� ��Ŷ ũ��(���� ��Ŷ ũ��)
				int rest_size = io_byte;
				char* p = pOverEx->messageBuffer;
				char packet_size = 0;

				if (player->GetPrevSize() > 0)
					packet_size = player->GetPacketBuf()[0];

				while (rest_size > 0)
				{
					// ���� �����ִ� ��Ŷ�� �����ٸ�, ���� ���� ��Ŷ�� ����
					if (packet_size == 0)
						packet_size = p[0];

					// ��Ŷ�� ����� ���� ũ��?
					int required = packet_size - player->GetPrevSize();

					// ��Ŷ�� ���� �� �ִٸ�, (���� ���� ��Ŷ�� ũ�Ⱑ required ���� ũ��)
					if (rest_size >= required)
					{
						memcpy(player->GetPacketBuf() + player->GetPrevSize(), p, required);
						ProcessPacket((int)id, player->GetPacketBuf());

						rest_size -= required;
						p += required;
						packet_size = 0;
					}
					// ��Ŷ�� ���� �� ���ٸ�,
					else
					{
						// ���� ���� ��Ŷ�� ũ�⸸ŭ, ���� ���� ��Ŷ�� �����Ų��.?
						memcpy(player->GetPacketBuf() + player->GetPrevSize(), p, rest_size);
						rest_size = 0;
					}
				}
				Recv(static_cast<int>(id));
				break;
			}

		// Send( )�� ��,
		case EV_SEND:
			{
				delete pOverEx;
				break;
			}

		default:
			cout << "Unknown Event" << endl;
				break;
		}
	}
}

void IOCP::ReleaseWorkerThread()
{
	OverEx* pOverEx = new OverEx;
	pOverEx->eventType = EV_QUIT;
	PostQueuedCompletionStatus(m_IOCPHandle, 1, 0, &pOverEx->overlapped);

	cout << "��Ŀ ������ ����" << endl;
}

void IOCP::Accept()
{
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0)
	{
		cout << "Error - Can not load 'winsock.dll' file" << endl;
		return;
	}

	m_ListenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_ListenSocket == INVALID_SOCKET)
	{
		cout << "Error - Invalid socket" << endl;
		return;
	}

	SOCKADDR_IN serverAddr;
	memset(&serverAddr, 0, sizeof(SOCKADDR_IN));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	// 2. ���ϼ���
	if (::bind(m_ListenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Error - Fail bind" << endl;
		closesocket(m_ListenSocket);
		WSACleanup();
		return;
	}

	// 3. ���Ŵ�⿭����
	if (listen(m_ListenSocket, 5) == SOCKET_ERROR)
	{
		cout << "Error - Fail listen" << endl;
		closesocket(m_ListenSocket);
		WSACleanup();
		return;
	}

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	memset(&clientAddr, 0, addrLen);
	SOCKET clientSocket;

	while (true)
	{
		clientSocket = accept(m_ListenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Error - Fail Accept" << endl;
			break;
		}

		// �� ���̵� ��������
		//int new_id = Get_New_ID();
		int new_id = 0;
		Player* player = reinterpret_cast<Player*>(m_Characters[new_id]);

		player->SetSocket(clientSocket);

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSocket), m_IOCPHandle, new_id, 0);

		Recv(new_id);
	}
}

void IOCP::ReleaseAcceptTrhead()
{
	// ���� ��������
	closesocket(m_ListenSocket);

	// Winsock End
	WSACleanup();

	cout << "Accept ������ ����" << endl;
}

void IOCP::Recv(int id)
{
	DWORD flags = 0;
	SOCKET socket = reinterpret_cast<Player*>(m_Characters[id])->GetSocket();
	OverEx* pOverEx = &m_Characters[id]->GetOverEx();

	// WSASend(���信 ����)�� �ݹ��� ���
	pOverEx->dataBuffer.len = BUF_SIZE;
	pOverEx->dataBuffer.buf = pOverEx->messageBuffer;
	ZeroMemory(&(pOverEx->overlapped), sizeof(WSAOVERLAPPED));

	if (WSARecv(socket, &pOverEx->dataBuffer, 1, nullptr, &flags, &(pOverEx->overlapped), nullptr) == SOCKET_ERROR)
	{
		int error_no = WSAGetLastError();

		if (error_no != WSA_IO_PENDING)
			ErrorDisplay("WSARecv() Error - ", error_no);
	}
}

void IOCP::Send(int id, char* packet)
{
	SOCKET socket = reinterpret_cast<Player*>(m_Characters[id])->GetSocket();
	OverEx* pOverEx = new OverEx;

	pOverEx->dataBuffer.len = packet[0];
	pOverEx->dataBuffer.buf = pOverEx->messageBuffer;

	// ��Ŷ�� ������ ���ۿ� ����
	memcpy(pOverEx->messageBuffer, packet, packet[0]);

	ZeroMemory(&(pOverEx->overlapped), sizeof(WSAOVERLAPPED));
	pOverEx->eventType = EV_SEND;

	if (WSASend(socket, &pOverEx->dataBuffer, 1, nullptr, 0, &(pOverEx->overlapped), nullptr) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
			cout << "Error - Fail WSARecv(error_code : " << WSAGetLastError() << endl;
	}
}

void IOCP::ProcessPacket(int id, char* buf)
{
	switch (buf[1])
	{
	case CS_LOGIN:
		break;

	case CS_MOVE:
		{
			CS_Packet_Move* packet = reinterpret_cast<CS_Packet_Move*>(buf);

			float x = m_Characters[id]->GetX();
			float y = m_Characters[id]->GetY();

			m_Characters[id]->Move(packet->direction, x, y);

			// �þ� ó��
			//ProcessView(id);

			Send_Position_Packet(id, id);
			break;
		}

	default:
		Disconnect(id);
		break;
	}
}

void IOCP::Disconnect(int id)
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (reinterpret_cast<Player*>(m_Characters[i])->GetIsConnect() == false)	continue;

		m_Characters[i]->GetViewListMtx().lock();
		if (m_Characters[i]->GetViewList().count(id) != 0)
		{
			m_Characters[i]->GetViewListMtx().unlock();
			Send_Remove_Object_Packet(i, id);
		}

		else
			m_Characters[i]->GetViewListMtx().unlock();
	}

	Player* player = reinterpret_cast<Player*>(m_Characters[id]);
	//if (player->GetIsLogin() == true)
	//{
	//	 ������ ���� DB�� �÷��̾� ���� ������Ʈ
	//	g_DB.UpdatePlayerStatus(id);
	//}
	// �÷��̾� ���� �ʱ�ȭ ��Ŵ
	reinterpret_cast<Player*>(m_Characters[id])->ClearPlayerInfo();
}

void IOCP::Send_Login_Ok_Packet(int to)
{
	Player* player = reinterpret_cast<Player*>(m_Characters[to]);

	SC_Packet_Login_OK packet;
	packet.size = sizeof(SC_Packet_Login_OK);
	packet.type = SC_LOGIN_OK;
	packet.id = to;
	packet.x = player->GetX();
	packet.y = player->GetY();
	packet.HP = player->GetHp();
	packet.LEVEL = player->GetLevel();
	packet.EXP = player->GetExp();
	packet.AttackPower = player->GetAttackPower();

	Send(to, reinterpret_cast<char*>(&packet));
}

void IOCP::Send_Login_Fail_Packet(int to)
{
	SC_Packet_Login_Fail packet;
	packet.size = sizeof(SC_Packet_Login_Fail);
	packet.type = SC_LOGIN_FAIL;

	Send(to, reinterpret_cast<char*>(&packet));
}

void IOCP::Send_Add_Object_Packet(int to, int obj)
{
	SC_Packet_Add_Object packet;

	packet.id = obj;
	packet.size = sizeof(SC_Packet_Add_Object);
	packet.type = SC_ADD_OBJECT;
	packet.x = m_Characters[obj]->GetX();
	packet.y = m_Characters[obj]->GetY();
	packet.HP = m_Characters[obj]->GetHp();
	packet.EXP = m_Characters[obj]->GetExp();

	Send(to, reinterpret_cast<char*>(&packet));
}

void IOCP::Send_Position_Packet(int to, int from)
{
	SC_Packet_Position packet;

	packet.id = from;
	packet.size = sizeof(SC_Packet_Position);
	packet.type = SC_POSITION;
	packet.x = m_Characters[from]->GetX();
	packet.y = m_Characters[from]->GetY();

	Send(to, reinterpret_cast<char*>(&packet));
}

void IOCP::Send_Remove_Object_Packet(int to, int id)
{
	SC_Packet_Remove_Object packet;
	packet.size = sizeof(SC_Packet_Remove_Object);
	packet.type = SC_REMOVE_OBJECT;
	packet.id = id;

	Send(to, reinterpret_cast<char*>(&packet));
}



