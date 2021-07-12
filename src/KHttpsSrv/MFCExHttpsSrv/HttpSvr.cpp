/*!
	\file http_server.cpp
	\brief HTTP server example
	\author Ivan Shynkarenka
	\date 30.04.2019
	\copyright MIT License
*/

/// ���� include������ �־����� ����� ����鼭 �׸� �Ű� ����.

#include "pch.h"

#include <iostream>
#include <map>
#include <mutex>
#include <tuple>

#include "HttpSvr.h" // ���� ����� ��������, �и� �Ͽ� �����.
#include "HttpsSvr.h" // ���� �κ�















#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//shared_ptr<CMyHttp> CMyHttp::s_body;

//CKTrace std_cout;



void CMyHttp::SampleData(int port, const char* vpath, const char* path)// error :"../www/api")
{
	_port = port;
	if(path)
		_cachedPath = path;
	if(vpath)
		_cachedUrl = vpath;
}
















CMyHttp::~CMyHttp()
{
	stop_server();// CMFCServerView::OnDestroy() ���� �̹� �ҷ�����, ������ ���
	std_cout << "~CMyHttp() destroyed." << std_endl;
}


int CMyHttp::start_server()
{
// 	if(_service && _service->IsStarted() && _server && _server->IsStarted())
// 		return 1;
	// Create a new Asio service
	try 
	{
		///TODO: �ϴ� service�� �Ź� û��
		if(_service)
		{/// ������ û��
			auto service = _service;
			if(service->IsStarted())
				service->Stop();

			_service.reset();
		}

		ASSERT(!_service);
		
		int nThread = 4;
		bool bPool = true;
		_service = std::make_shared<CAsioService>(nThread, bPool);

		std_cout << "Asio service starting...";
		if(_service->Start())
			std_cout << "Done!" << std_endl;
		else
			std_cout << "Asio service is failed!" << std_endl;
/*
		if(!_service)
			_service = std::make_shared<CAsioService>();
		///_service = CAsioSvcInst::getAsioService(1)->_service;
		/// site�� ��Ʈ ���� ���� _service�� ����� ����. �׷��� ���� ������ �־�� �ҵ�.

		if (_service) // operator bool ����. .get())
			std_cout << "Asio service starting..." << std_endl;
		else
			return 2;
		// Start the Asio service
		if(!_service->IsStarted())
		{
			if(_service->Start())
				std_cout << "Done!" << std_endl;
			else
				std_cout << "Asio service is failed!" << std_endl;
		}
		else
			std_cout << "Asio service is already started!!" << std_endl;
*/

		///[https �� �ٸ� �κ� ���� �κ��� ������.
		/// 
		/// 
		/// 
		/// 
		/// 
		/// 
		/// 
		/// 
		///] 

		if(_server)
		{/// ������ û��
			auto server1 = _server;
			if(server1->IsStarted())
				server1->Stop();

			_server.reset();
		}
		ASSERT(!_server);

		// Create a new HTTP server
		_server = std::make_shared<HTTPCacheServer>(_service, _port);
		// ������ ���� �Ǹ� 
		if(!_server->_fncOnTrace)
			_server->_fncOnTrace = _fncOnTrace;//?ExTrace 4 CMyHttps -> _server (���ʿ�)
		
		if(_bStaticCache && _cachedPath.size() > 0 && _cachedUrl.size() > 0)
		{
			string cachedPath = (PAS)KwReplaceStr(CStringA(_cachedPath.c_str()), "\\", "/");
			_server->AddStaticContent(cachedPath, _cachedUrl);//, "/api");
			/// ���⼭ ���� ����, Dropbox ����Ʈ����ȭ�� ���÷� �ȵȰ�� ������ ����� ���о �׷���.
		}

		// Start the server
		std_cout << "Server starting..." << std_endl;
		if(!_server->IsStarted())
		{
			if(_server->Start())
			{
				//std_cout << "Done! (No SSL)" << std_endl;
			}
			else
			{
				std_cout << "Server starting ERROR!!!!!!!! (No SSL)" << std_endl;
				return -1;
			}
		}
		else
			std_cout << "Server is already started!!" << std_endl;
	}
	catch(TException ex)
	{
		TErrCode ec = ex.code();
		std_cout << "Error " << ec.message() << std_endl;
		return -3;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(std::exception& e)
	{
		std_cout << "Error " << e.what() << std_endl;
		return -4;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(std::string& es)
	{
		std_cout << "Error " << es << std_endl;
		return -5;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(int es)
	{
		std_cout << "Error " << es << std_endl;
		return -6;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(PAS es)
	{
		std_cout << "Error " << es << std_endl;
		return -6;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	catch(...)
	{
		return -7;//https�� ������ȣ ����. 2�� https���� SSL�ʱ�ȭ ����
	}
	return 0;
}

/// CMyHttps�� �Ͱ� ����
void CMyHttp::restart_server()
{
	if(_service)
	{
		if(_server)
			HttpCmn::restart_server(_service, _server);
	}
}


/// CMyHttps�� �Ͱ� ����
void CMyHttp::stop_server()
{
	if(_service)
	{
		if(_server)
			HttpCmn::stop_server(_service, _server);
	}
}

void CMyHttp::shutdown_server()
{
	if(_service)
	{
		if(_server)
			HttpCmn::shutdown_server(_service, _server);
	}
}

/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onError(int error, const string& category, const string& message)
{
	std_cout << "HTTPS server caught an error with code " << error << " and category '" << category << "': " << message << std_endl;
	if(_fncOnError.get())
		(*_fncOnError.get())(error, category, message);
}


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onStarted()
{
	std_cout << "HTTPCacheServer::onStarted " << std_endl;
	if(_fncOnStarted.get())
		(*_fncOnStarted.get())();
}


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onStopped()
{
	std_cout << "HTTPCacheServer::onStopped " << std_endl;
	if(_fncOnStopped.get())
		(*_fncOnStopped.get())();//?destroy 4

// 	string esm = _eStopMode;// {"none"};// "stop", "shutdown", "restart",
// 	if(esm == "shutdown")
// 	{
// 		///TODO: ��� �˴ٿ� ����?
// 	}

}


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onConnected(shared_ptr<TCPSession>& session)
{
	std_coutD << "HTTPCacheServer::onConnected " << std_endl;
	if(_fncOnConnected.get())
		(*_fncOnConnected.get())(*(HTTPCacheSession*)session.get());
}


/// CMyHttps::onHandshaked �� SSL�� �ʿ� ������ ���⼭ ������.


/// CMyHttps�� �Ͱ� ����
void HTTPCacheServer::onDisconnected(shared_ptr<TCPSession>& session)
{
	std_coutD << "HTTPCacheServer::onHandshaked " << std_endl;
	if(_fncOnDisconnected.get())
		(*_fncOnDisconnected.get())(*(HTTPCacheSession*)session.get());
}


/// ///////////////////////// Session ////////////////////////////////////////////
void HTTPCacheSession::onReceivedRequestHeader(const HTTPRequest& request)
{// base�� �� �Լ��� __super�� �θ� �ʿ� ����.
	//std_cout << "HTTPCacheSession::onReceivedRequestHeader- len: " << std_endl;//request.body_length <<  
}
void HTTPCacheSession::onReceivedRequest(const HTTPRequest& request)
{
	//std_coutD << "HTTPCacheSession::onReceivedRequest" << std_endl;
	HTTPRequest& req1 = (HTTPRequest&)request; // this->request(); kdw�� �߰�
	auto svr = server();	//+svr	shared_ptr<SSLServer>
	auto svr2 = dynamic_cast<HTTPCacheServer*>(svr.get());
	HttpCmn::onReceivedRequest(svr2, this, req1);
}


//?kdw virtual�� �ƴѰ� virtual�� ���� override�Ѵ�.
void HTTPCacheSession::onReceivedRequestInternal(const HTTPRequest& request)
{
	//std_coutD << "HTTPCacheSession::onReceivedRequestInternal" << std_endl;
	if(_fncOnReceivedRequestInternal)
		(*_fncOnReceivedRequestInternal)(this, (HTTPRequest&)request);
	__super::onReceivedRequestInternal(request);
}

void HTTPCacheSession::onReceived(const void* buffer, size_t size)
{
	//std_coutD << "HTTPCacheSession::onReceived " << size << std_endl;
	__super::onReceived(buffer, size);// �̰� �Ⱥθ��� onReceivedRequest�� �Ⱥҷ�
	if (_fncOnReceived.get())
		(*_fncOnReceived.get())(this, buffer, size);
}

void HTTPCacheSession::onSent(size_t sent, size_t pending)
{
	//std_coutD << "HTTPCacheSession::onSent " << sent << ", " << pending << std_endl;
	if (_fncOnSent.get())
		(*_fncOnSent.get())(this, sent, pending);
}
void HTTPCacheSession::onSentKw(uint8_t* data, size_t szAll)
{
	if (_fncOnSentKw.get())
		(*_fncOnSentKw.get())(this, data, szAll);
	_sinfo.Clear();
}

void HTTPCacheSession::onReceivedRequestError(const CppServer::HTTP::HTTPRequest& request, const std::string& error)
{
	std_cout << "Session Request error: " << error << std_endl;
}






void HTTPCacheSession::onError(int error, const std::string& category, const std::string& message)
{
	std_cout << "HTTP session caught an error with code " << error << " and category '" << category << "': " << message << std_endl;
}

void HTTPCacheSession::onConnected()
{
	std_coutD << "HTTPCacheSession::onConnected " << std_endl;
}

void HTTPCacheSession::onDisconnected()
{
	std_coutD << "HTTPCacheSession::onDisconnected >> super" << std_endl << std_endl;
	__super::onDisconnected();
}

void HTTPCacheSession::onEmpty()
{
	//std_cout << "HTTPCacheSession::onEmpty >> super" << std_endl << std_endl;
}

