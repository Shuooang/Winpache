/*!
    \file service.cpp
    \brief Asio service implementation
    \author Ivan Shynkarenka
    \date 16.12.2016
    \copyright MIT License
*/

#include "server/kwadjust.h"
#include "server/asio/service.h"

#include "errors/fatal.h"


//#define _STL_OPTIMIZE_SYSTEM_ERROR_OPERATORS //kdw <system_error> 160 warning C4005: '_STL_OPTIMIZE_SYSTEM_ERROR_OPERATORS': 매크로 재정의

namespace CppServer {
namespace Asio {

Service::Service(int threads, bool pool)
    : _strand_required(false),
      _polling(false),
      _started(false),
      _round_robin_index(0)
{
    assert((threads >= 0) && "Working threads counter must not be negative!");

    if (threads == 0)
    {
        // Single Asio IO service without thread pool
        _services.emplace_back(std::make_shared<BAsio::io_service>());
    }
    else if (!pool)
    {
        // Io-service-per-thread design
        for (int thread = 0; thread < threads; ++thread)
        {
            _services.emplace_back(std::make_shared<BAsio::io_service>());
            _threads.emplace_back(std::thread());
        }
    }
    else
    {
        // Thread-pool design
        _services.emplace_back(std::make_shared<BAsio::io_service>());
        for (int thread = 0; thread < threads; ++thread)
            _threads.emplace_back(std::thread());
        _strand = std::make_shared<BAsio::io_service::strand>(*_services[0]);
        _strand_required = true;
    }
}

Service::Service(const std::shared_ptr<BAsio::io_service>& service, bool strands)
    : _strand_required(strands),
      _polling(false),
      _started(false),
      _round_robin_index(0)
{
    assert((service != nullptr) && "Asio IO service is invalid!");
    if (service == nullptr)
        throw CppCommon::ArgumentException("Asio IO service is invalid!");

    _services.emplace_back(service);
    if (_strand_required)
        _strand = std::make_shared<BAsio::io_service::strand>(*_services[0]);
}

bool Service::Start(bool polling)
{
    assert(!IsStarted() && "Asio service is already started!");
    if (IsStarted())
        return false;

    // Update polling loop mode flag
    _polling = polling;

    // Reset round robin index
    _round_robin_index = 0;

    // Post the started handler
    auto self(this->shared_from_this());
    auto start_handler = [this, self]()
    {
        if (IsStarted())
            return;

         // Update the started flag
        _started = true;

        // Call the service started handler
        onStarted();
    };
    if (_strand_required)
        _strand->post(start_handler);
    else
        _services[0]->post(start_handler);

    // Start service working threads
    for (size_t thread = 0; thread < _threads.size(); ++thread)
        _threads[thread] = CppCommon::Thread::Start([this, self, thread]() 
            {
                ServiceThread(self, _services[thread % _services.size()]); 
            });

    return true;
}

bool Service::Stop()
{
    assert(IsStarted() && "Asio service is not started!");
    if (!IsStarted())
        return false;

    // Post the stop routine
    auto self(this->shared_from_this());
    auto stop_handler = [this, self]()
    {
        if (!IsStarted())
            return;

        // Stop Asio services
        for (auto& service : _services)
            service->stop();

        // Update the started flag
        _started = false;

        // Call the service stopped handler
        onStopped();
    };
    if (_strand_required)
        _strand->post(stop_handler);
    else
        _services[0]->post(stop_handler);

    // Wait for all service working threads
    for (auto& thread : _threads)
        thread.join();

    // Update polling loop mode flag
    _polling = false;

    return true;
}

bool Service::Restart()
{
    bool polling = IsPolling();

    if (!Stop())
        return false;

    // Reinitialize new Asio IO services
    for (size_t service = 0; service < _services.size(); ++service)
        _services[service] = std::make_shared<BAsio::io_service>();
    if (_strand_required)
        _strand = std::make_shared<BAsio::io_service::strand>(*_services[0]);

    return Start(polling);
}

void Service::ServiceThread(const std::shared_ptr<Service>& service, const std::shared_ptr<BAsio::io_service>& io_service)
{
    bool polling = service->IsPolling();

    // Call the initialize thread handler
    service->onThreadInitialize();

    try
    {
        // Attach the current working thread to the Asio service
        BAsio::io_service::work work(*io_service);

        // Service loop...
        do
        {
            // ...with handling some specific Asio errors
            try
            {
                if (polling)
                {
                    // Poll all pending handlers
                    io_service->poll();

                    // Call the idle handler
                    service->onIdle();
                }
                else
                {
                    // Run all pending handlers
                    io_service->run();
                    break;
                }
            }
            catch (const boost::system::system_error& ex)//C2039: 'system_error': 'BAsio::error'의 멤버가 아닙니다.
            {
                TErrCode ec = ex.code();// asio -> std

                // Skip Asio disconnect errors
                //if (ec.value == BAsio::error::not_connected) //kdw: .value
                //    continue;

                throw;
            }
        } while (service->IsStarted());
    }
    catch (const boost::system::system_error& ex)//error C2039: 'system_error': 'boost::asio'의 멤버가 아닙니다.
    {
        service->SendError(ex.code());
    }
    catch (const std::exception& ex)
    {
        fatality(ex);
    }
    catch (...)
    {
        fatality("Asio service thread terminated!");
    }

    // Call the cleanup thread handler
    service->onThreadCleanup();

#if (OPENSSL_VERSION_NUMBER >= 0x10100000L)
    // Delete OpenSSL thread state
    OPENSSL_thread_stop();
#endif
}

void Service::SendError(TErrCode ec)
{
    onError(ec.value(), ec.category().name(), ec.message());
}

} // namespace Asio
} // namespace CppServer
