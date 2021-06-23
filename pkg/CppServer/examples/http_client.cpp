/*!
	\file http_client.cpp
	\brief HTTP client example
	\author Ivan Shynkarenka
	\date 08.02.2019
	\copyright MIT License
*/
#include "server/kwadjust.h"

#include "asio_service.h"

#include "server/http/http_client.h"
#include "string/string_utils.h"

#include <iostream>

CKTrace std_cout;

int main(int argc, char** argv)
{
	// HTTP server address
	std::string address = "127.0.0.1";
	if(argc > 1)
		address = argv[1];
	// HTTP server port
	int port = 8080;
	if(argc > 2)
		port = std::atoi(argv[2]);

	std_cout << "HTTP server address: " << address << std_endl;
	std_cout << "HTTP server port: " << port << std_endl;

	std_cout << std_endl;

	// Create a new Asio service
	auto service = std::make_shared<AsioService>();

	// Start the Asio service
	std_cout << "Asio service starting...";
	service->Start();
	std_cout << "Done!" << std_endl;

	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// ///////////////// SSL AREA   /////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////
	/// //////////////////////////////////////////////////////////////////

	// Create a new HTTP client
	auto client = std::make_shared<CppServer::HTTP::HTTPClientEx>(service, address, port);

	std_cout << "Press Enter to stop the client or '!' to reconnect the client..." << std_endl;

	try
	{
		// Perform text input
		std::string line = "GET yyy";
		int n = 1;
		while(n--)//getline(std::cin, line))
		{
			if(line.empty())
				break;

			// Reconnect the client
			if(line == "!")
			{
				std_cout << "Client reconnecting...";
				client->IsConnected() ? client->ReconnectAsync() : client->ConnectAsync();
				std_cout << "Done!" << std_endl;
				continue;
			}

			auto commands = CppCommon::StringUtils::Split(line, ' ', true);
			if(commands.size() < 2)
			{
				std_cout << "HTTP method and URL must be entered!" << std_endl;
				continue;
			}

			std::string method = commands[0];
			std::string methodU = CppCommon::StringUtils::ToUpper(method);
			std::string url = commands[1];
			std::string content = commands[2];
			if(methodU == "HEAD")
			{
				auto response = client->SendHeadRequest(url).get();
				std_cout << response << std_endl;
			}
			else if(methodU == "GET")
			{
				auto response = client->SendGetRequest(url).get();
				std_cout << response << std_endl;







			}
			else if(methodU == "POST")
			{
				if(commands.size() < 3)
				{
					std_cout << "HTTP method, URL and body must be entered!" << std_endl;
					continue;
				}
				auto response = client->SendPostRequest(url, content).get();
				std_cout << response << std_endl;
			}
			else if(methodU == "PUT")
			{
				if(commands.size() < 3)
				{
					std_cout << "HTTP method, URL and body must be entered!" << std_endl;
					continue;
				}
				auto response = client->SendPutRequest(url, content).get();
				std_cout << response << std_endl;
			}
			else if(methodU == "DELETE")
			{
				auto response = client->SendDeleteRequest(url).get();
				std_cout << response << std_endl;
			}
			else if(methodU == "OPTIONS")
			{
				auto response = client->SendOptionsRequest(url).get();
				std_cout << response << std_endl;
			}
			else if(methodU == "TRACE")
			{
				auto response = client->SendTraceRequest(url).get();
				std_cout << response << std_endl;
			}
			else
				std_cout << "Unknown HTTP method: " << commands[0] << std_endl;
		}
	}
	catch(const std::exception& ex)
	{
		std::cerr << ex.what() << std_endl;
	}

	// Stop the Asio service
	std_cout << "Asio service stopping...";
	service->Stop();
	std_cout << "Done!" << std_endl;

	return 0;
}
