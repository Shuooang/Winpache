#pragma once
#include <assert.h>

#include <string>
#include <memory>
#include <set>

#include <winsock2.h> // ���� ��� ���� 
#include <ws2tcpip.h>
#include "stdecl.h"
//[��ó] C++ Winsock UDP | �ۼ��� ��������
// http://blog.naver.com/PostView.nhn?blogId=windowsub0406&logNo=220537565086

class KUdpSvr
{
public:

	WSADATA _wsaData{ 0 }; // ���� ������ ����ü.(WSAStartup() ����Ҳ�!)
	SOCKET _sockSvr{ 0 }; // ���� ����
	SOCKET _sockBCast{ 0 }; // ���� ����
	bool _bWsaInit{ false };
	CString _hostSvr;
	int _portSvr{ 0 };//61007; // ���� ��Ʈ��ȣ
	int _portBCast{ 0 };//61007; // ���� ��Ʈ��ȣ

	SOCKADDR_IN _addrSvrInfo{ 0, }; // ���� �ּ����� ����ü
	SOCKADDR_IN _addrBrodcast{ 0, }; // ���� �ּ����� ����ü
	//SOCKADDR_IN _addrFromCli{ 0, }; // Ŭ���̾�Ʈ���� �޴� �ּ����� ����ü

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
		if(WSAStartup(0x202, &_wsaData) == SOCKET_ERROR) // WSAStartup �������� ���� �߻��ϸ�
			throw "WSAStartup error";
		_bWsaInit = true;
	}

	~KUdpSvr()
	{
		if(_sockSvr)
			closesocket(_sockSvr); // ������ �ݽ��ϴ�.

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

	/// ���Ʈĳ��Ʈ ���� ��� �Ҽ� �ִ�.
	void RegisterBoadcastClient(SOCKADDR_IN& addrFromCli);

	/// �����ϱ� ���� �ҷ����� ����
	//SOCKADDR_IN& addrFromCli, shared_ptr<char> bufRecv, int szRecv, shared_ptr<char> bufSend, int szSend)
	shared_ptr<function<int(SOCKADDR_IN&, shared_ptr<char>, int, shared_ptr<char>&, int&)>> _fncResponse;
	
	/// ��ε�ĳ��Ʈ�� �ҷ����� ����.
	shared_ptr<function<int(shared_ptr<char>&, int&)>> _fncBraodcast;

	template<typename TFNC> 
	void SetResponse(TFNC fnc)
	{
		//_fncResponse = std::make_shared<TFNC>(fnc);//TCreateFuncValue(_fncResponse, fnc);
		//_fncResponse = shared_ptr<TFNC>(new function<std::make_shared<TFNC>(fnc);
		TSetLambdaToSharedFunction(_fncResponse, fnc);
	}
	/* usage: ��û ���� ���� ��Ŷ�� ����, �ٷ� ������ ��� ���⿡ �ִ´�.
	udpSvr.SetResponse([&](SOCKADDR_IN& addrCli, shared_ptr<char> bufRecv, int szRecv, shared_ptr<char>& bufSend, int& szSend)
		{
			bufSend = shared_ptr<char>(3);
			strcpy(bufSend.get(), "OK");
			szSend = strlen("OK");
			return 0; // else 1,2,3

			//or ���Ʈĳ��Ʈ ���� ��� �Ҽ� �ִ�.

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

