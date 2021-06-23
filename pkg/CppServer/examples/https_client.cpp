/*!
	\file https_client.cpp
	\brief HTTPS client example
	\author Ivan Shynkarenka
	\date 12.02.2019
	\copyright MIT License
*/
#include "server/kwadjust.h"

#include "asio_service.h"

#include "server/http/https_client.h"
#include "string/string_utils.h"

#include <iostream>

CKTrace std_cout;

int main_https_client(int argc, char** argv)
{
	// HTTP server address
	std::string address = "114.201.33.80";//(광명홈)"127.0.0.1";
	if (argc > 1)
		address = argv[1];
	// HTTP server port
	int port = 443;// 19480;// 8443;443;//
	if (argc > 2)
		port = std::atoi(argv[2]);

	std_cout << "HTTPS server address: " << address << std_endl;
	std_cout << "HTTPS server port: " << port << std_endl;

	std_cout << std_endl;

	// Create a new Asio service
	auto service = std::make_shared<AsioService>();

	// Start the Asio service
	std_cout << "Asio service starting...";
	service->Start();
	std_cout << "Done!" << std_endl;

	// Create and prepare a new SSL client context
	auto context = std::make_shared<CppServer::Asio::SSLContext>(BAsio::ssl::context::tlsv12);
	try {
		context->set_default_verify_paths();//?cert_client 1
		context->set_root_certs();//?cert_client 2
		context->set_verify_mode(BAsio::ssl::verify_peer | BAsio::ssl::verify_fail_if_no_peer_cert);//?cert_client 3
		std::string keyFolder = "C:/Dropbox/Proj/STUDY/boostEx/CppServer/CppServer-master/tools/certificates/";
		std::string capem = "ca.pem";//CA self-signed certificate  이건 ZerrSSL 에서 받아온거. 반대인가?
//		std::string keyFolderIIS = "C:/Dropbox/Proj/ODISO/Src/IIS/.well-known/pki-validation/";
//		std::string capemIIS = "ca_bundle.pem";//server certificate  이건 샘플에 따라온거
	
		context->load_verify_file(keyFolder + capem);//?cert_client 4  ///"../tools/certificates/ca.pem");
		//https://github.com/chronoxor/CppServer#openssl-certificates
		//C:\Dropbox\Proj\STUDY\SSL\openssl\홈서버 구축기.docx
	} catch(TException ex)
	{
		TErrCode ec = ex.code();
		std_cout << "Error " << ec.message() << std_endl;
		return 1;
	}

	// Create a new HTTP client
	auto client = std::make_shared<CppServer::HTTP::HTTPSClientEx>(service, context, address, port);//?cert_client 5

	std_cout << "Press Enter to stop the client or '!' to reconnect the client..." << std_endl;

	try
	{
		// Perform text input
		std::string line = "GET yyy";
		int n=1;
		while (n--)//getline(std::cin, line))
		{
			if (line.empty())
				break;

			// Reconnect the client
			if (line == "!")
			{
				std_cout << "Client reconnecting...";
				client->IsConnected() ? client->ReconnectAsync() : client->ConnectAsync();
				std_cout << "Done!" << std_endl;
				continue;
			}

			auto commands = CppCommon::StringUtils::Split(line, ' ', true);
			if (commands.size() < 2)
			{
				std_cout << "HTTP method and URL must be entered!" << std_endl;
				continue;
			}
			
			std::string method = commands[0];
			std::string methodU = CppCommon::StringUtils::ToUpper(method);
			std::string url = commands[1];
			std::string content = commands[2];
			if (method == "HEAD")
			{
				auto response = client->SendHeadRequest(url).get();
				std_cout << response.string() << std_endl;
			}
			else if (method == "GET")
			{
				auto response = client->SendGetRequest(url).get();
				std_cout << response.string() << std_endl;
			//  Status: 404
			//	Status phrase: Not Found
			//	Protocol: HTTP/1.1
			//	Headers: 1
			//	Content-Length: 51
			//	Body:51
			//	Required cache value was not found for the key: yyy
			}
			else if (method == "POST")
			{
				if (commands.size() < 3)
				{
					std_cout << "HTTP method, URL and body must be entered!" << std_endl;
					continue;
				}
				auto response = client->SendPostRequest(url, content).get();//?cert_client 6
				std_cout << response.string() << std_endl;
			}
			else if (method == "PUT")
			{
				if (commands.size() < 3)
				{
					std_cout << "HTTP method, URL and body must be entered!" << std_endl;
					continue;
				}
				auto response = client->SendPutRequest(url, content).get();
				std_cout << response.string() << std_endl;
			}
			else if (method == "DELETE")
			{
				auto response = client->SendDeleteRequest(url).get();
				std_cout << response.string() << std_endl;
			}
			else if (method == "OPTIONS")
			{
				auto response = client->SendOptionsRequest(url).get();
				std_cout << response.string() << std_endl;
			}
			else if (method == "TRACE")
			{
				auto response = client->SendTraceRequest(url).get();
				std_cout << response.string() << std_endl;
			}
			else
				std_cout << "Unknown HTTP method: " << commands[0] << std_endl;
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std_endl;
	}

	// Stop the Asio service
	std_cout << "Asio service stopping...";
	service->Stop();
	std_cout << "Done!" << std_endl;

	return 0;
}
