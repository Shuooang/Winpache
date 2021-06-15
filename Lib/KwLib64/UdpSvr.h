#pragma once
#include <assert.h>

#include <string>
#include <memory>
#include <set>

#include <winsock2.h> // 윈속 헤더 포함 
#include <ws2tcpip.h>
#include "stdecl.h"
//[출처] C++ Winsock UDP | 작성자 끄적끄적
// http://blog.naver.com/PostView.nhn?blogId=windowsub0406&logNo=220537565086

class KUdpSvr
{
public:

	WSADATA _wsaData{ 0 }; // 윈속 데이터 구조체.(WSAStartup() 사용할꺼!)
	SOCKET _sockSvr{ 0 }; // 소켓 선언
	SOCKET _sockBCast{ 0 }; // 소켓 선언
	bool _bWsaInit{ false };
	CString _hostSvr;
	int _portSvr{ 0 };//61007; // 서버 포트번호
	int _portBCast{ 0 };//61007; // 서버 포트번호

	SOCKADDR_IN _addrSvrInfo{ 0, }; // 서버 주소정보 구조체
	SOCKADDR_IN _addrBrodcast{ 0, }; // 서버 주소정보 구조체
	//SOCKADDR_IN _addrFromCli{ 0, }; // 클라이언트에서 받는 주소정보 구조체

	//std::set<std::string> _ipClient;
	std::map<string, SOCKADDR_IN> _ipClient;
	//std::map<string, shared_ptr<SOCKADDR_IN>> _ipClient;
	enum
	{
		eBufSize = 10240,
	};

	shared_ptr<char> _sbufRecv{ std::make_shared<char>(eBufSize) };
	shared_ptr<char> _sbufSend{ std::make_shared<char>(eBufSize) };
	char* _bufRecv{ _sbufRecv.get() };
	char* _bufSend{ _sbufSend.get() };
	//char _bufRecv[eBufSize];
	//char _bufSend[eBufSize];


	void Open();
	void Init()
	{
		//MAKEWORD(2, 2) == 0x202
		if(WSAStartup(0x202, &_wsaData) == SOCKET_ERROR) // WSAStartup 설정에서 문제 발생하면
			throw "WSAStartup error";
		_bWsaInit = true;
	}

	~KUdpSvr()
	{
		if(_sockSvr)
			closesocket(_sockSvr); // 소켓을 닫습니다.

		if(_bWsaInit)
			WSACleanup();
	}

	int SendTo(SOCKET sockSvr, const char* bufSend, int szSendTo, SOCKADDR_IN& addrCli)
	{
		int szSend = sendto(sockSvr, bufSend, szSendTo, 0, (struct sockaddr*)&addrCli, sizeof(addrCli));
		return szSend;
	}
	

	int AsyncStartUdpSvrGen();

	HANDLE _eventBCast{ nullptr };

	/// 브로트캐스트 수신 등록 할수 있다.
	void RegisterBoadcastClient(SOCKADDR_IN& addrFromCli);

	/// 응답하기 전에 불려지는 람다
	//SOCKADDR_IN& addrFromCli, shared_ptr<char> bufRecv, int szRecv, shared_ptr<char> bufSend, int szSend)
	shared_ptr<function<int(SOCKADDR_IN&, shared_ptr<char>, int, shared_ptr<char>&, int&)>> _fncResponse;
	
	/// 브로드캐스트때 불려지는 람다.
	shared_ptr<function<int(shared_ptr<char>&, int&)>> _fncBraodcast;

	template<typename TFNC> 
	void SetResponse(TFNC fnc)
	{
		//_fncResponse = std::make_shared<TFNC>(fnc);//TCreateFuncValue(_fncResponse, fnc);
		//_fncResponse = shared_ptr<TFNC>(new function<std::make_shared<TFNC>(fnc);
		TSetLambdaToSharedFunction(_fncResponse, fnc);
	}
	/* usage: 요청 직후 받은 패킷을 보고, 바로 응답할 경우 여기에 넣는다.
	udpSvr.SetResponse([&](SOCKADDR_IN& addrCli, shared_ptr<char> bufRecv, int szRecv, shared_ptr<char>& bufSend, int& szSend)
		{
			bufSend = shared_ptr<char>(3);
			strcpy(bufSend.get(), "OK");
			szSend = strlen("OK");
			return 0; // else 1,2,3

			//or 브로트캐스트 수신 등록 할수 있다.

			udpSvr.RegisterBoadcastClient(addrCli);
		});
	*/
	template<typename TFNC>
	void SetBoradcast(TFNC fnc)
	{
		_fncBraodcast = std::make_shared<TFNC>(fnc);//TCreateFuncValue(_fncResponse, fnc);
	}

	bool _bBroadcasting{ false };
	void AsyncStartBroadcast();

	void SetBoradcastEvent()
	{
		ASSERT(_eventBCast);
		SetEvent(_eventBCast);
	}

};

