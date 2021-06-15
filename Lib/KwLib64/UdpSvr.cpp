#include "pch.h"
#include "UdpSvr.h"
#include "KTemple.h"

void KUdpSvr::Open()
{
	//IN_ADDR addr{ 0 };
	_addrSvrInfo.sin_family = AF_INET; // IPv4 �ּ�ü�� ��� 
	//_addrSvrInfo.sin_addr.s_addr = inet_addr((const char*)_hostSvr);//"127.0.0.1"); // ������ IP. �� ȥ�ڳ��̿�..
	InetPton(AF_INET, _hostSvr, &_addrSvrInfo.sin_addr.s_addr);
	_addrSvrInfo.sin_port = htons(_portSvr); // ��Ʈ��ȣ

	_sockSvr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // udp�� ���� ����. SOCK_DGRAM : UDP ���
	if(_sockSvr == INVALID_SOCKET) // ���� �߻���
		throw "socket error";

}

int KUdpSvr::AsyncStartUdpSvrGen()
{
	//Init(); �̰� �� �Լ� �θ��� ���� �ҷ��� �Ѵ�.

	// UDP Server ���� ����
	Open();
	KAtEnd d_svr([&]() { 
		closesocket(_sockSvr);
		_sockSvr = 0;
	});

	// bind() - ���� ���� Ŭ���̾�Ʈ�� ���� welcome ����
	// ���޸� �Ҳ��� �ʿ����
	int ir = 0;
	//���ε� ���Ͽ� �������� �ο�
	ir = bind(_sockSvr, (struct sockaddr*)&_addrSvrInfo, sizeof(_addrSvrInfo));
	if(ir == SOCKET_ERROR)
		throw "bind error";//GetLastError()

	_ipClient.clear();

	for(int inw = 0;; inw++)
	{
		SOCKADDR_IN addrFromCli{ 0, }; // Ŭ���̾�Ʈ���� �޴� �ּ����� ����ü

		int szFromCli = sizeof(addrFromCli);
		// recvfrom : UDP��� �񿬰���.  ��Ŷ����
		int szRecv = recvfrom(_sockSvr, _sbufRecv.get(), eBufSize, 0, (struct sockaddr*)&addrFromCli, &szFromCli);
		if(szRecv < 0)
		{
			TRACE("recvfrom() error!\r\n");
			continue; // return -3;
		}

		///���� ��� ��û/���� ������� ��� �� ��� sendto�� �Ѵ�.
		int szSend{ 0 };
		if(_fncResponse.get())
		{
			shared_ptr<char> bufSend;
			int szSendTo{ 0 };
			// ������ �۾��� �̸� ������ ���� �Լ��� ���� �´�.
			if((*_fncResponse.get())(addrFromCli, _sbufRecv, szRecv, bufSend, szSendTo) == 0)
			{
				/// 0 ��� 1 ���� �ϰ�, �񵿱�� ���� ó���� SendTo���� �θ� �� �ִ�. ����� ����� �����̴�.
				/// ���� ���� ������ ���� ��ε�ĳ��Ʈ ��� �� ���� �ִ�.
				szSend = SendTo(_sockSvr, bufSend.get(), szSendTo, addrFromCli);//sendto(_sockSvr, bufSend.get(),szSendTo, 0, (struct sockaddr*)&_addrFromCli, sizeof(_addrFromCli));
			}
		}

			//case 54:// client ip 0x36
			//	if(!_bBroadcasting)
			//	{
			//		QueueFUNC(([&]() -> void // [&] ���� ����
			//			{
			//				AsyncStartBroadcast();//_hostCl ������ ì����.
			//			}));
			//	}

//			PostMessage(WM_USER_UPDATEDATA, 0, 0);
	}//while (1)

    return 0;
}

/// ���Ʈĳ��Ʈ ���� ��� �Ҽ� �ִ�.

void KUdpSvr::RegisterBoadcastClient(SOCKADDR_IN& addrFromCli)
{
	CStringA ip;
	InetNtopA(AF_INET, &addrFromCli.sin_addr, ip.GetBuffer(512), 500);
	ip.ReleaseBuffer();
	std::string ipClient = (LPCSTR)ip;// inet_ntoa(addrFromCli.sin_addr);
	//��Ŷ�� ���� Ŭ���̾�Ʈ �ּҸ� ��ε�ĳ��Ʈ �Ϸ��� ����.
	_ipClient.insert({ ipClient, addrFromCli });
}

//#define PORT 65429
//https://tapito.tistory.com/556
/* �񵿱�� �θ���.
QueueFUNC(([&]() -> void // [&] ���� ����
	{
		AsyncStartBroadcast();//_hostCl ������ ì����.
	}));
*/
void KUdpSvr::AsyncStartBroadcast()
{
	ASSERT(_bWsaInit);
	// ������ �����Ѵ�. socket([IPv4 ���ͳ� ���], [�����ͱ׷� ����� ��Ŷ ���], [�����ͱ׷��̹Ƿ� UDP �������� ���]);
	//TRACE("trying: socket...\n");
	ASSERT(_eventBCast == 0);
	ASSERT(_sockBCast == 0);
	_eventBCast = CreateEvent(NULL, TRUE, FALSE, NULL);
	/// /////////////////////////////////////////////////////////////////////
	_sockBCast = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(_sockBCast == INVALID_SOCKET)
		throw "Socket Error";
	//rv = WSAGetLastError();
	KAtEnd d_svr([&]()
		{
			closesocket(_sockBCast);
			_sockBCast = 0;
		});

	// ���Ͽ� �ɼ��� �����Ѵ�. setsockopt([����], [�ɼ� ���� ���], [�ɼ� ��], [�ɼ� ���� ũ��]); 
	//TRACE("trying: setsockopt...\n");
	int fBroadcast = 1;
	/// /////////////////////////////////////////////////////////////////////
	int rv = setsockopt(_sockBCast, SOL_SOCKET, SO_BROADCAST, (const char*)&fBroadcast, sizeof(fBroadcast));
	if(rv == SOCKET_ERROR)
		throw "setsockopt Error";

	_bBroadcasting = true;

	while(1)
	{
		/// ���⼭ �ñ׳��� ��ٸ���.
		// �ƹ� �����̳� �߻� �ϸ� ������. �������� user�� ���̺� �־�� �Ѵ�.
	/// /////////////////////////////////////////////////////////////////////
		DWORD ret = WaitForSingleObject(_eventBCast, INFINITE);

		if(ret == WAIT_FAILED) //HANDLE�� Invalid �� ���
		{
			return;
		}
		else if(ret == WAIT_TIMEOUT) //TIMEOUT�� ���
		{
			continue;
		}
		else
		{
			//Event Object: Signaled ����
			//���α׷� �ڵ�
			if(!_fncBraodcast)//��ε�ĳ��Ʈ ���ٰ� ������.
				continue;

			for(auto& [ipCli, sckin] : _ipClient)
			{
				shared_ptr<char> bufSend;
				int szSendTo{ 0 };
				// ������ �۾��� �̸� ������ ���� �Լ��� ���� �´�.
				if((*_fncBraodcast.get())(bufSend, szSendTo) == 0)
				{
					/* UDP ��Ŷ�� ������ ����� �����Ѵ�. { ���ͳ� ���, 255.255.255.255�� ������� ��, ��Ʈ ��ȣ } */
					SOCKADDR_IN addrCli{ 0, }; // �̰� sckin �� �������� �ϴ°� �Ƴ�?
					addrCli.sin_family = AF_INET;
					//addrCli.sin_addr.S_un.S_addr = inet_addr(ipCli.c_str());//"255.255.255.255");
					InetPton(AF_INET, _hostSvr, &addrCli.sin_addr.s_addr);
					addrCli.sin_port = htons(_portBCast);

					int szSend = SendTo(_sockBCast, bufSend.get(), szSendTo, addrCli);//sendto(_sockSvr, bufSend.get(),szSendTo, 0, (struct sockaddr*)&_addrFromCli, sizeof(_addrFromCli));
					if(szSend == SOCKET_ERROR)
					{
						TRACE("SendTo Error!\n");
						continue;
					}
				}
			}
			
			ResetEvent(_eventBCast); //Event Object Nonsignaled ���·� ��ȯ
		}
	}

}
