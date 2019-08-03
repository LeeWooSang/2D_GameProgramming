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
	// �����߻��� �ѱ۷� ��µǵ��� ���
	wcout.imbue(locale("korean"));

	// IOCP ��ü ����
	m_IOCPHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	if (m_IOCPHandle == nullptr)
		return false;

	return true;
}

void IOCP::Run()
{
	// ��Ŀ ������ ����
	for (int i = 0; i < MAX_WORKER_THREAD; ++i)
		m_WorkerThread.emplace_back(thread{ &IOCP::ThreadPool, this });

	// ������ ���� ���
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
				//Disconnect((int)id);
				continue;
			}
		}

		// Ŭ��� �������ٸ� (Ŭ�� ������ ��)
		if (io_byte == 0)
		{
			cout << id << "�� Ŭ���̾�Ʈ ����" << endl;
			//Disconnect((int)id);
			continue;
		}

		switch (pOverEx->eventType)
		{
		case EV_RECV:
			{
				//Player* player = reinterpret_cast<Player*>(g_Characters[id]);
				// ���� ��Ŷ ũ��(���� ��Ŷ ũ��)
				int rest_size = io_byte;
				char* p = pOverEx->messageBuffer;
				char packet_size = 0;

				//if (player->GetPrevSize() > 0)
				//	packet_size = player->GetPacketBuf()[0];

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
						//ProcessPacket((int)id, player->GetPacketBuf());

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
	wcout << L"���� : " << lpMsgBuf << endl;

	// �����߻��� ���ѷ����� ���߰���
	while (true);
	LocalFree(lpMsgBuf);
}

void IOCP::Recv(int id)
{
	DWORD flags = 0;
	//SOCKET socket = reinterpret_cast<Player*>(g_Characters[id])->GetSocket();
	OverEx* pOverEx = &g_Characters[id]->GetOverEx();

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
	SOCKET socket = reinterpret_cast<Player*>(g_Characters[id])->GetSocket();
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
