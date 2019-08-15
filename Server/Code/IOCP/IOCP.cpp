#include "IOCP.h"
#include "../Character/Character.h"
#include <crtdbg.h>

INIT_INSTACNE(IOCP)

IOCP::IOCP()
	: m_IOCPHandle(nullptr)
{
	// 에러발생시 한글로 출력되도록 명령
	_wsetlocale(LC_ALL, L"korean");
	wcout.imbue(locale("korean"));

	m_WorkerThread.reserve(MAX_WORKER_THREAD);

	m_IsRelease = false;
	m_ListenSocket = 0;

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// 출력된 번호를 넣어주면 그 지점으로 바로 이동시켜준다.
	// [ 예시 ]
	// Detected memory leaks!
	//	Dumping objects ->
	// {233} normal block at 0x000001469D91A680, 24 bytes long.
	// 233 이라는 지점에서 릭이 생김	
	//_CrtSetBreakAlloc(265);
}

IOCP::~IOCP()
{
	m_WorkerThread.clear();

	CloseHandle(m_IOCPHandle);
	m_IOCPHandle = nullptr;

	SAFE_DELETE_LIST(m_Characters);

	cout << "IOCP Server 종료" << endl;
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
	wcout << L"설명 : " << lpMsgBuf << endl;

	// 에러발생시 무한루프로 멈추게함
	while (true);
	LocalFree(lpMsgBuf);
}

bool IOCP::Initialize()
{
	// IOCP 객체 생성
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
	// 워커 스레드 생성
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

			// 워커 스레드 종료 대기
			for (auto& worker : m_WorkerThread)
				worker.join();

			ReleaseAcceptTrhead();
			// accept 스레드 종료 대기
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
		// lpover에 recv인지 send인지 정보를 넣어야 됨.
		int is_error = GetQueuedCompletionStatus(m_IOCPHandle, &io_byte, &id, reinterpret_cast<LPWSAOVERLAPPED*>(&pOverEx), INFINITE);

		//  GetQueuedCompletionStatus( )가 에러인지 아닌지 확인한다
		if (is_error == false)
		{
			int error_num = WSAGetLastError();
			if (error_num != 64)
				ErrorDisplay(" GetQueuedCompletionStatus()", error_num);
			else
			{
				cout << id << "번 클라이언트 나감" << endl;
				Disconnect((int)id);
				continue;
			}
		}

		// 클라와 끊어졌다면 (클라가 나갔을 때)
		if (io_byte == 0)
		{
			cout << id << "번 클라이언트 나감" << endl;
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
				// 남은 패킷 크기(들어온 패킷 크기)
				int rest_size = io_byte;
				char* p = pOverEx->messageBuffer;
				char packet_size = 0;

				if (player->GetPrevSize() > 0)
					packet_size = player->GetPacketBuf()[0];

				while (rest_size > 0)
				{
					// 전에 남아있던 패킷이 없었다면, 현재 들어온 패킷을 저장
					if (packet_size == 0)
						packet_size = p[0];

					// 패킷을 만들기 위한 크기?
					int required = packet_size - player->GetPrevSize();

					// 패킷을 만들 수 있다면, (현재 들어온 패킷의 크기가 required 보다 크면)
					if (rest_size >= required)
					{
						memcpy(player->GetPacketBuf() + player->GetPrevSize(), p, required);
						ProcessPacket((int)id, player->GetPacketBuf());

						rest_size -= required;
						p += required;
						packet_size = 0;
					}
					// 패킷을 만들 수 없다면,
					else
					{
						// 현재 들어온 패킷의 크기만큼, 현재 들어온 패킷을 저장시킨다.?
						memcpy(player->GetPacketBuf() + player->GetPrevSize(), p, rest_size);
						rest_size = 0;
					}
				}
				Recv(static_cast<int>(id));
				break;
			}

		// Send( )일 때,
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

	cout << "워커 스레드 종료" << endl;
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

	// 2. 소켓설정
	if (::bind(m_ListenSocket, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		cout << "Error - Fail bind" << endl;
		closesocket(m_ListenSocket);
		WSACleanup();
		return;
	}

	// 3. 수신대기열생성
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

		// 빈 아이디를 생성해줌
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
	// 리슨 소켓종료
	closesocket(m_ListenSocket);

	// Winsock End
	WSACleanup();

	cout << "Accept 스레드 종료" << endl;
}

void IOCP::Recv(int id)
{
	DWORD flags = 0;
	SOCKET socket = reinterpret_cast<Player*>(m_Characters[id])->GetSocket();
	OverEx* pOverEx = &m_Characters[id]->GetOverEx();

	// WSASend(응답에 대한)의 콜백일 경우
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

	// 패킷의 내용을 버퍼에 복사
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

			// 시야 처리
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
	//	 나가기 전에 DB에 플레이어 정보 업데이트
	//	g_DB.UpdatePlayerStatus(id);
	//}
	// 플레이어 정보 초기화 시킴
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



