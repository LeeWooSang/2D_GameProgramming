#include "IOCP.h"

INIT_INSTACNE(IOCP)

IOCP::IOCP()
	: m_IOCPHandle(nullptr)
{
	m_WorkerThread.reserve(MAX_WORKER_THREAD);
}

IOCP::~IOCP()
{
	m_WorkerThread.clear();

	CloseHandle(m_IOCPHandle);
	m_IOCPHandle = nullptr;
}

bool IOCP::Initialize()
{
	_wsetlocale(LC_ALL, L"korean");
	// 에러발생시 한글로 출력되도록 명령
	wcout.imbue(locale("korean"));

	// IOCP 객체 생성
	m_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (m_IOCPHandle == nullptr)
		return false;

	return true;
}

void IOCP::Run()
{
	// 워커 스레드 생성
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
		m_WorkerThread.emplace_back(thread{ &IOCP::ThreadPool, this });

	// 스레드 종료 대기
	for (auto& worker : m_WorkerThread)
		worker.join();
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
				//Disconnect((int)id);
				continue;
			}
		}

		// 클라와 끊어졌다면 (클라가 나갔을 때)
		if (io_byte == 0)
		{
			cout << id << "번 클라이언트 나감" << endl;
			//Disconnect((int)id);
			continue;
		}

		switch (pOverEx->eventType)
		{
		case EV_RECV:
			{
				//Player* player = reinterpret_cast<Player*>(g_Characters[id]);
				// 남은 패킷 크기(들어온 패킷 크기)
				int rest_size = io_byte;
				char* p = pOverEx->messageBuffer;
				char packet_size = 0;

				//if (player->GetPrevSize() > 0)
				//	packet_size = player->GetPacketBuf()[0];

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
						//ProcessPacket((int)id, player->GetPacketBuf());

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
				break;
		}
	}
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

void IOCP::Recv(int id)
{
	DWORD flags = 0;
	//SOCKET socket = reinterpret_cast<Player*>(g_Characters[id])->GetSocket();
	OverEx* pOverEx = &g_Characters[id]->GetOverEx();

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
	SOCKET socket = reinterpret_cast<Player*>(g_Characters[id])->GetSocket();
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

void IOCP::ProcessPacket(int id, char* packet)
{
	switch (packet[1])
	{
	default:
		break;
	}
}

void IOCP::Disconnect(int)
{
}
