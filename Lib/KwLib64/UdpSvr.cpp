#include "pch.h"
#include "UdpSvr.h"
#include "KTemple.h"

void KUdpSvr::Open()
{
	//IN_ADDR addr{ 0 };
	_addrSvrInfo.sin_family = AF_INET; // IPv4 주소체계 사용 
	//_addrSvrInfo.sin_addr.s_addr = inet_addr((const char*)_hostSvr);//"127.0.0.1"); // 루프백 IP. 즉 혼자놀이용..
	InetPton(AF_INET, _hostSvr, &_addrSvrInfo.sin_addr.s_addr);
	_addrSvrInfo.sin_port = htons(_portSvr); // 포트번호

	_sockSvr = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // udp용 소켓 생성. SOCK_DGRAM : UDP 사용
	if(_sockSvr == INVALID_SOCKET) // 에러 발생시
		throw "socket error";

}

int KUdpSvr::AsyncStartUdpSvrGen()
{
	//Init(); 이건 이 함수 부르기 전에 불러야 한다.

	// UDP Server 소켓 생성
	Open();
	KAtEnd d_svr([&]() { 
		closesocket(_sockSvr);
		_sockSvr = 0;
	});

	// bind() - 새로 오는 클라이언트를 받을 welcome 소켓
	// 전달만 할꺼면 필요없음
	int ir = 0;
	//바인드 소켓에 서버정보 부여
	ir = bind(_sockSvr, (struct sockaddr*)&_addrSvrInfo, sizeof(_addrSvrInfo));
	if(ir == SOCKET_ERROR)
		throw "bind error";//GetLastError()

	_ipClient.clear();

	for(int inw = 0;; inw++)
	{
		SOCKADDR_IN addrFromCli{ 0, }; // 클라이언트에서 받는 주소정보 구조체

		int szFromCli = sizeof(addrFromCli);
		// recvfrom : UDP통신 비연결형.  패킷수신
		int szRecv = recvfrom(_sockSvr, _sbufRecv.get(), eBufSize, 0, (struct sockaddr*)&addrFromCli, &szFromCli);
		if(szRecv < 0)
		{
			TRACE("recvfrom() error!\r\n");
			continue; // return -3;
		}

		///받은 즉시 요청/응답 방식으로 대답 할 경우 sendto를 한다.
		int szSend{ 0 };
		if(_fncResponse.get())
		{
			shared_ptr<char> bufSend;
			int szSendTo{ 0 };
			// 응답전 작업은 미리 셋팅한 람다 함수로 갔다 온다.
			if((*_fncResponse.get())(addrFromCli, _sbufRecv, szRecv, bufSend, szSendTo) == 0)
			{
				/// 0 대신 1 리턴 하고, 비동기로 로직 처리후 SendTo까지 부를 수 있다. 여기는 동기식 응답이다.
				/// 또한 받은 데이터 보고 브로드캐스트 등록 할 수도 있다.
				szSend = SendTo(_sockSvr, bufSend.get(), szSendTo, addrFromCli);//sendto(_sockSvr, bufSend.get(),szSendTo, 0, (struct sockaddr*)&_addrFromCli, sizeof(_addrFromCli));
			}
		}

			//case 54:// client ip 0x36
			//	if(!_bBroadcasting)
			//	{
			//		QueueFUNC(([&]() -> void // [&] 에러 나네
			//			{
			//				AsyncStartBroadcast();//_hostCl 위에서 챙겼지.
			//			}));
			//	}

//			PostMessage(WM_USER_UPDATEDATA, 0, 0);
	}//while (1)

    return 0;
}

/// 브로트캐스트 수신 등록 할수 있다.

void KUdpSvr::RegisterBoadcastClient(SOCKADDR_IN& addrFromCli)
{
	CStringA ip;
	InetNtopA(AF_INET, &addrFromCli.sin_addr, ip.GetBuffer(512), 500);
	ip.ReleaseBuffer();
	std::string ipClient = (LPCSTR)ip;// inet_ntoa(addrFromCli.sin_addr);
	//패킷을 보낸 클라이언트 주소를 브로드캐스트 하려고 보관.
	_ipClient.insert({ ipClient, addrFromCli });
}

//#define PORT 65429
//https://tapito.tistory.com/556
/* 비동기로 부른다.
QueueFUNC(([&]() -> void // [&] 에러 나네
	{
		AsyncStartBroadcast();//_hostCl 위에서 챙겼지.
	}));
*/
void KUdpSvr::AsyncStartBroadcast()
{
	ASSERT(_bWsaInit);
	// 소켓을 생성한다. socket([IPv4 인터넷 사용], [데이터그램 방식의 패킷 사용], [데이터그램이므로 UDP 프로토콜 사용]);
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

	// 소켓에 옵션을 지정한다. setsockopt([소켓], [옵션 지정 대상], [옵션 값], [옵션 값의 크기]); 
	//TRACE("trying: setsockopt...\n");
	int fBroadcast = 1;
	/// /////////////////////////////////////////////////////////////////////
	int rv = setsockopt(_sockBCast, SOL_SOCKET, SO_BROADCAST, (const char*)&fBroadcast, sizeof(fBroadcast));
	if(rv == SOCKET_ERROR)
		throw "setsockopt Error";

	_bBroadcasting = true;

	while(1)
	{
		/// 여기서 시그널을 기다린다.
		// 아무 편집이나 발생 하면 보낸다. 편집중인 user가 테이블에 있어야 한다.
	/// /////////////////////////////////////////////////////////////////////
		DWORD ret = WaitForSingleObject(_eventBCast, INFINITE);

		if(ret == WAIT_FAILED) //HANDLE이 Invalid 할 경우
		{
			return;
		}
		else if(ret == WAIT_TIMEOUT) //TIMEOUT시 명령
		{
			continue;
		}
		else
		{
			//Event Object: Signaled 상태
			//프로그램 코드
			if(!_fncBraodcast)//브로드캐스트 람다가 있으면.
				continue;

			for(auto& [ipCli, sckin] : _ipClient)
			{
				shared_ptr<char> bufSend;
				int szSendTo{ 0 };
				// 응답전 작업은 미리 셋팅한 람다 함수로 갔다 온다.
				if((*_fncBraodcast.get())(bufSend, szSendTo) == 0)
				{
					/* UDP 패킷을 전송할 대상을 지정한다. { 인터넷 사용, 255.255.255.255를 대상으로 함, 포트 번호 } */
					SOCKADDR_IN addrCli{ 0, }; // 이거 sckin 와 같아져야 하는거 아냐?
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
			
			ResetEvent(_eventBCast); //Event Object Nonsignaled 상태로 변환
		}
	}

}
