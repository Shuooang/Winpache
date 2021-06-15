/*!
    \file tcp_client.cpp
    \brief TCP client implementation
    \author Ivan Shynkarenka
    \date 15.12.2016
    \copyright MIT License
*/
#include "server/kwadjust.h"

#include "server/asio/tcp_client.h"

namespace CppServer {
namespace Asio {

TCPClient::TCPClient(const std::shared_ptr<Service>& service, const std::string& address, int port)
    : _id(CppCommon::UUID::Sequential()),
      _service(service),
      _io_service(_service->GetAsioService()),
      _strand(*_io_service),
      _strand_required(_service->IsStrandRequired()),
      _address(address),
      _port(port),
      _socket(*_io_service),
      _resolving(false),
      _connecting(false),
      _connected(false),
      _bytes_pending(0),
      _bytes_sending(0),
      _bytes_sent(0),
      _bytes_received(0),
      _receiving(false),
      _sending(false),
      _send_buffer_flush_offset(0),
      _option_keep_alive(false),
      _option_no_delay(false)
{
    assert((service != nullptr) && "Asio service is invalid!");
    if (service == nullptr)
        throw CppCommon::ArgumentException("Asio service is invalid!");
}

TCPClient::TCPClient(const std::shared_ptr<Service>& service, const std::string& address, const std::string& scheme)
    : _id(CppCommon::UUID::Sequential()),
      _service(service),
      _io_service(_service->GetAsioService()),
      _strand(*_io_service),
      _strand_required(_service->IsStrandRequired()),
      _address(address),
      _scheme(scheme),
      _port(0),
      _socket(*_io_service),
      _resolving(false),
      _connecting(false),
      _connected(false),
      _bytes_pending(0),
      _bytes_sending(0),
      _bytes_sent(0),
      _bytes_received(0),
      _receiving(false),
      _sending(false),
      _send_buffer_flush_offset(0),
      _option_keep_alive(false),
      _option_no_delay(false)
{
    assert((service != nullptr) && "Asio service is invalid!");
    if (service == nullptr)
        throw CppCommon::ArgumentException("Asio service is invalid!");
}

TCPClient::TCPClient(const std::shared_ptr<Service>& service, const BAsio::ip::tcp::endpoint& endpoint)
    : _id(CppCommon::UUID::Sequential()),
      _service(service),
      _io_service(_service->GetAsioService()),
      _strand(*_io_service),
      _strand_required(_service->IsStrandRequired()),
      _address(endpoint.address().to_string()),
      _port(endpoint.port()),
      _endpoint(endpoint),
      _socket(*_io_service),
      _resolving(false),
      _connecting(false),
      _connected(false),
      _bytes_pending(0),
      _bytes_sending(0),
      _bytes_sent(0),
      _bytes_received(0),
      _receiving(false),
      _sending(false),
      _send_buffer_flush_offset(0),
      _option_keep_alive(false),
      _option_no_delay(false)
{
    assert((service != nullptr) && "Asio service is invalid!");
    if (service == nullptr)
        throw CppCommon::ArgumentException("Asio service is invalid!");
}

size_t TCPClient::option_receive_buffer_size() const
{
    BAsio::socket_base::receive_buffer_size option;
    _socket.get_option(option);
    return option.value();
}

size_t TCPClient::option_send_buffer_size() const
{
    BAsio::socket_base::send_buffer_size option;
    _socket.get_option(option);
    return option.value();
}

void TCPClient::SetupReceiveBufferSize(size_t size)
{
    BAsio::socket_base::receive_buffer_size option((int)size);
    _socket.set_option(option);
}

void TCPClient::SetupSendBufferSize(size_t size)
{
    BAsio::socket_base::send_buffer_size option((int)size);
    _socket.set_option(option);
}

bool TCPClient::Connect()
{
    if (IsConnected())
        return false;

    TErrCode ec;

    // Create the server endpoint
    _endpoint = BAsio::ip::tcp::endpoint(BAsio::ip::make_address(_address), (unsigned short)_port);

    // Connect to the server
    _socket.connect(_endpoint, ec);

    // Disconnect on error
    if (ec)
    {
        SendError(ec);

        // Call the client disconnected handler
        onDisconnected();
        return false;
    }

    // Apply the option: keep alive
    if (option_keep_alive())
        _socket.set_option(BAsio::ip::tcp::socket::keep_alive(true));
    // Apply the option: no delay
    if (option_no_delay())
        _socket.set_option(BAsio::ip::tcp::no_delay(true));

    // Prepare receive & send buffers
    _receive_buffer.resize(option_receive_buffer_size());
    _send_buffer_main.reserve(option_send_buffer_size());
    _send_buffer_flush.reserve(option_send_buffer_size());

    // Reset statistic
    _bytes_pending = 0;
    _bytes_sending = 0;
    _bytes_sent = 0;
    _bytes_received = 0;

    // Update the connected flag
    _connected = true;

    // Call the client connected handler
    onConnected();

    // Call the empty send buffer handler
    if (_send_buffer_main.empty())
        onEmpty();

    return true;
}

bool TCPClient::Connect(const std::shared_ptr<TCPResolver>& resolver)
{
    if (IsConnected())
        return false;

    TErrCode ec;

    // Resolve the server endpoint
    BAsio::ip::tcp::resolver::query query(_address, (_scheme.empty() ? std::to_string(_port) : _scheme));
    auto endpoints = resolver->resolver().resolve(query, ec);

    // Disconnect on error
    if (ec)
    {
        SendError(ec);

        // Call the client disconnected handler
        onDisconnected();
        return false;
    }

    //  Connect to the server
    _endpoint = BAsio::connect(_socket, endpoints, ec);

    // Disconnect on error
    if (ec)
    {
        SendError(ec);

        // Call the client disconnected handler
        onDisconnected();
        return false;
    }

    // Apply the option: keep alive
    if (option_keep_alive())
        _socket.set_option(BAsio::ip::tcp::socket::keep_alive(true));
    // Apply the option: no delay
    if (option_no_delay())
        _socket.set_option(BAsio::ip::tcp::no_delay(true));

    // Prepare receive & send buffers
    _receive_buffer.resize(option_receive_buffer_size());
    _send_buffer_main.reserve(option_send_buffer_size());
    _send_buffer_flush.reserve(option_send_buffer_size());

    // Reset statistic
    _bytes_pending = 0;
    _bytes_sending = 0;
    _bytes_sent = 0;
    _bytes_received = 0;

    // Update the connected flag
    _connected = true;

    // Call the client connected handler
    onConnected();

    // Call the empty send buffer handler
    if (_send_buffer_main.empty())
        onEmpty();

    return true;
}

bool TCPClient::Disconnect()
{
    if (!IsConnected())
        return false;

    // Close the client socket
    _socket.close();

    // Update the connected flag
    _resolving = false;
    _connecting = false;
    _connected = false;

    // Update sending/receiving flags
    _receiving = false;
    _sending = false;

    // Clear send/receive buffers
    ClearBuffers();

    // Call the client disconnected handler
    onDisconnected();

    return true;
}

bool TCPClient::Reconnect()
{
    if (!Disconnect())
        return false;

    return Connect();
}

bool TCPClient::ConnectAsync()
{
    if (IsConnected() || _resolving || _connecting)
        return false;

    // Post the connect handler
    auto self(this->shared_from_this());
    auto connect_handler = [this, self]()
    {
        if (IsConnected() || _resolving || _connecting)
            return;

        // Async connect with the connect handler
        _connecting = true;
        auto async_connect_handler = [this, self](TErrCode ec)
        {
            _connecting = false;

            if (IsConnected() || _resolving || _connecting)
                return;

            if (!ec)
            {
                // Apply the option: keep alive
                if (option_keep_alive())
                    _socket.set_option(BAsio::ip::tcp::socket::keep_alive(true));
                // Apply the option: no delay
                if (option_no_delay())
                    _socket.set_option(BAsio::ip::tcp::no_delay(true));

                // Prepare receive & send buffers
                _receive_buffer.resize(option_receive_buffer_size());
                _send_buffer_main.reserve(option_send_buffer_size());
                _send_buffer_flush.reserve(option_send_buffer_size());

                // Reset statistic
                _bytes_pending = 0;
                _bytes_sending = 0;
                _bytes_sent = 0;
                _bytes_received = 0;

                // Update the connected flag
                _connected = true;

                // Call the client connected handler
                onConnected();

                // Call the empty send buffer handler
                if (_send_buffer_main.empty())
                    onEmpty();

                // Try to receive something from the server
                TryReceive();
            }
            else
            {
                SendError(ec);

                // Call the client disconnected handler
                onDisconnected();
            }
        };

        // Create the server endpoint
        _endpoint = BAsio::ip::tcp::endpoint(BAsio::ip::make_address(_address), (unsigned short)_port);

        if (_strand_required)
            _socket.async_connect(_endpoint, bind_executor(_strand, async_connect_handler));
        else
            _socket.async_connect(_endpoint, async_connect_handler);
    };
    if (_strand_required)
        _strand.post(connect_handler);
    else
        _io_service->post(connect_handler);

    return true;
}

bool TCPClient::ConnectAsync(const std::shared_ptr<TCPResolver>& resolver)
{
    if (IsConnected() || _resolving || _connecting)
        return false;

    // Post the connect handler
    auto self(this->shared_from_this());
    auto connect_handler = [this, self, resolver]()
    {
        if (IsConnected() || _resolving || _connecting)
            return;

        // Async resolve with the connect handler
        _resolving = true;
        auto async_resolve_handler = [this, self](TErrCode ec1, BAsio::ip::tcp::resolver::results_type endpoints)
        {
            _resolving = false;

            if (IsConnected() || _resolving || _connecting)
                return;

            if (!ec1)
            {
                // Async connect with the connect handler
                _connecting = true;
                auto async_connect_handler = [this, self](TErrCode ec2, const BAsio::ip::tcp::endpoint& endpoint)
                {
                    _connecting = false;

                    if (IsConnected() || _resolving || _connecting)
                        return;

                    if (!ec2)
                    {
                        //  Connect to the server
                        _endpoint = endpoint;

                        // Apply the option: keep alive
                        if (option_keep_alive())
                            _socket.set_option(BAsio::ip::tcp::socket::keep_alive(true));
                        // Apply the option: no delay
                        if (option_no_delay())
                            _socket.set_option(BAsio::ip::tcp::no_delay(true));

                        // Prepare receive & send buffers
                        _receive_buffer.resize(option_receive_buffer_size());
                        _send_buffer_main.reserve(option_send_buffer_size());
                        _send_buffer_flush.reserve(option_send_buffer_size());

                        // Reset statistic
                        _bytes_pending = 0;
                        _bytes_sending = 0;
                        _bytes_sent = 0;
                        _bytes_received = 0;

                        // Update the connected flag
                        _connected = true;

                        // Call the client connected handler
                        onConnected();

                        // Call the empty send buffer handler
                        if (_send_buffer_main.empty())
                            onEmpty();

                        // Try to receive something from the server
                        TryReceive();
                    }
                    else
                    {
                        SendError(ec2);

                        // Call the client disconnected handler
                        onDisconnected();
                    }
                };
                if (_strand_required)
                    BAsio::async_connect(_socket, endpoints, bind_executor(_strand, async_connect_handler));
                else
                    BAsio::async_connect(_socket, endpoints, async_connect_handler);
            }
            else
            {
                SendError(ec1);

                // Call the client disconnected handler
                onDisconnected();
            }
        };

        // Resolve the server endpoint
        BAsio::ip::tcp::resolver::query query(_address, (_scheme.empty() ? std::to_string(_port) : _scheme));
        if (_strand_required)
            resolver->resolver().async_resolve(query, bind_executor(_strand, async_resolve_handler));
        else
            resolver->resolver().async_resolve(query, async_resolve_handler);
    };
    if (_strand_required)
        _strand.post(connect_handler);
    else
        _io_service->post(connect_handler);

    return true;
}

bool TCPClient::DisconnectAsync(bool dispatch)
{
    if (!IsConnected() || _resolving || _connecting)
        return false;

    // Dispatch or post the disconnect handler
    auto self(this->shared_from_this());
    auto disconnect_handler = [this, self]() { Disconnect(); };
    if (_strand_required)
    {
        if (dispatch)
            _strand.dispatch(disconnect_handler);
        else
            _strand.post(disconnect_handler);
    }
    else
    {
        if (dispatch)
            _io_service->dispatch(disconnect_handler);
        else
            _io_service->post(disconnect_handler);
    }

    return true;
}

bool TCPClient::ReconnectAsync()
{
    if (!DisconnectAsync())
        return false;

    while (IsConnected())
        CppCommon::Thread::Yield();

    return ConnectAsync();
}

size_t TCPClient::Send(const void* buffer, size_t size)
{
    if (!IsConnected())
        return 0;

    if (size == 0)
        return 0;

    assert((buffer != nullptr) && "Pointer to the buffer should not be null!");
    if (buffer == nullptr)
        return 0;

    TErrCode ec;

    // Send data to the server
    size_t sent = BAsio::write(_socket, BAsio::buffer(buffer, size), ec);
    if (sent > 0)
    {
        // Update statistic
        _bytes_sent += sent;

        // Call the buffer sent handler
        onSent(sent, bytes_pending());
    }

    // Disconnect on error
    if (ec)
    {
        SendError(ec);
        Disconnect();
    }

    return sent;
}

size_t TCPClient::Send(const void* buffer, size_t size, const CppCommon::Timespan& timeout)
{
    if (!IsConnected())
        return 0;

    if (size == 0)
        return 0;

    assert((buffer != nullptr) && "Pointer to the buffer should not be null!");
    if (buffer == nullptr)
        return 0;

    int done = 0;
    std::mutex mtx;
    std::condition_variable cv;
    TErrCode error;
    BAsio::system_timer timer(_socket.get_executor());

    // Prepare done handler
    auto async_done_handler = [&](TErrCode ec)
    {
        std::unique_lock<std::mutex> lck(mtx);
        if (done++ == 0)
        {
            error = ec;
            _socket.cancel();
            timer.cancel();
        }
        cv.notify_one();
    };

    // Async wait for timeout
    timer.expires_from_now(timeout.chrono());
    timer.async_wait([&](const TErrCode& ec) { async_done_handler(ec ? ec : BAsio::error::timed_out); });

    // Async write some data to the server
    size_t sent = 0;
    _socket.async_write_some(BAsio::buffer(buffer, size), [&](TErrCode ec, size_t write) { async_done_handler(ec); sent = write; });

    // Wait for complete or timeout
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [&]() { return done == 2; });

    // Send data to the server
    if (sent > 0)
    {
        // Update statistic
        _bytes_sent += sent;

        // Call the buffer sent handler
        onSent(sent, bytes_pending());
    }

    // Disconnect on error
    if (error && (error != BAsio::error::timed_out))
    {
        SendError(error);
        Disconnect();
    }

    return sent;
}

bool TCPClient::SendAsync(const void* buffer, size_t size)
{
    if (!IsConnected())
        return false;

    if (size == 0)
        return true;

    assert((buffer != nullptr) && "Pointer to the buffer should not be null!");
    if (buffer == nullptr)
        return false;

    {
        std::scoped_lock locker(_send_lock);

        // Detect multiple send handlers
        bool send_required = _send_buffer_main.empty() || _send_buffer_flush.empty();

        // Fill the main send buffer
        const uint8_t* bytes = (const uint8_t*)buffer;
        _send_buffer_main.insert(_send_buffer_main.end(), bytes, bytes + size);

        // Update statistic
        _bytes_pending = _send_buffer_main.size();

        // Avoid multiple send handlers
        if (!send_required)
            return true;
    }

    // Dispatch the send handler
    auto self(this->shared_from_this());
    auto send_handler = [this, self]()
    {
        // Try to send the main buffer
        TrySend();
    };
    if (_strand_required)
        _strand.dispatch(send_handler);
    else
        _io_service->dispatch(send_handler);

    return true;
}

size_t TCPClient::Receive(void* buffer, size_t size)
{
    if (!IsConnected())
        return 0;

    if (size == 0)
        return 0;

    assert((buffer != nullptr) && "Pointer to the buffer should not be null!");
    if (buffer == nullptr)
        return 0;

    TErrCode ec;

    // Receive data from the server
    size_t received = _socket.read_some(BAsio::buffer(buffer, size), ec);
    if (received > 0)
    {
        // Update statistic
        _bytes_received += received;

        // Call the buffer received handler
        onReceived(buffer, received);
    }

    // Disconnect on error
    if (ec)
    {
        SendError(ec);
        Disconnect();
    }

    return received;
}

std::string TCPClient::Receive(size_t size)
{
    std::string text(size, 0);
    text.resize(Receive(text.data(), text.size()));
    return text;
}

size_t TCPClient::Receive(void* buffer, size_t size, const CppCommon::Timespan& timeout)
{
    if (!IsConnected())
        return 0;

    if (size == 0)
        return 0;

    assert((buffer != nullptr) && "Pointer to the buffer should not be null!");
    if (buffer == nullptr)
        return 0;

    int done = 0;
    std::mutex mtx;
    std::condition_variable cv;
    TErrCode error;
    BAsio::system_timer timer(_socket.get_executor());

    // Prepare done handler
    auto async_done_handler = [&](TErrCode ec)
    {
        std::unique_lock<std::mutex> lck(mtx);
        if (done++ == 0)
        {
            error = ec;
            _socket.cancel();
            timer.cancel();
        }
        cv.notify_one();
    };

    // Async wait for timeout
    timer.expires_from_now(timeout.chrono());
    timer.async_wait([&](const TErrCode& ec) { async_done_handler(ec ? ec : BAsio::error::timed_out); });

    // Async read some data from the server
    size_t received = 0;
    _socket.async_read_some(BAsio::buffer(buffer, size), [&](TErrCode ec, size_t read) { async_done_handler(ec); received = read; });

    // Wait for complete or timeout
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [&]() { return done == 2; });

    // Received some data from the server
    if (received > 0)
    {
        // Update statistic
        _bytes_received += received;

        // Call the buffer received handler
        onReceived(buffer, received);
    }

    // Disconnect on error
    if (error && (error != BAsio::error::timed_out))
    {
        SendError(error);
        Disconnect();
    }

    return received;
}

std::string TCPClient::Receive(size_t size, const CppCommon::Timespan& timeout)
{
    std::string text(size, 0);
    text.resize(Receive(text.data(), text.size(), timeout));
    return text;
}

void TCPClient::ReceiveAsync()
{
    // Try to receive data from the server
    TryReceive();
}

void TCPClient::TryReceive()
{
    if (_receiving)
        return;

    if (!IsConnected())
        return;

    // Async receive with the receive handler
    _receiving = true;
    auto self(this->shared_from_this());
    auto async_receive_handler = make_alloc_handler(_receive_storage, [this, self](TErrCode ec, size_t size)
    {
        _receiving = false;

        if (!IsConnected())
            return;

        // Received some data from the server
        if (size > 0)
        {
            // Update statistic
            _bytes_received += size;

            // Call the buffer received handler
            onReceived(_receive_buffer.data(), size);

            // If the receive buffer is full increase its size
            if (_receive_buffer.size() == size)
                _receive_buffer.resize(2 * size);
        }

        // Try to receive again if the session is valid
        if (!ec)
            TryReceive();
        else
        {
            SendError(ec);
            DisconnectAsync(true);
        }
    });
    if (_strand_required)
        _socket.async_read_some(BAsio::buffer(_receive_buffer.data(), _receive_buffer.size()), bind_executor(_strand, async_receive_handler));
    else
        _socket.async_read_some(BAsio::buffer(_receive_buffer.data(), _receive_buffer.size()), async_receive_handler);
}

void TCPClient::TrySend()
{
    if (_sending)
        return;

    if (!IsConnected())
        return;

    // Swap send buffers
    if (_send_buffer_flush.empty())
    {
        std::scoped_lock locker(_send_lock);

        // Swap flush and main buffers
        _send_buffer_flush.swap(_send_buffer_main);
        _send_buffer_flush_offset = 0;

        // Update statistic
        _bytes_pending = 0;
        _bytes_sending += _send_buffer_flush.size();
    }

    // Check if the flush buffer is empty
    if (_send_buffer_flush.empty())
    {
        // Call the empty send buffer handler
        onEmpty();
        return;
    }

    // Async write with the write handler
    _sending = true;
    auto self(this->shared_from_this());
    auto async_write_handler = make_alloc_handler(_send_storage, [this, self](TErrCode ec, size_t size)
    {
        _sending = false;

        if (!IsConnected())
            return;

        // Send some data to the server
        if (size > 0)
        {
            // Update statistic
            _bytes_sending -= size;
            _bytes_sent += size;

            // Increase the flush buffer offset
            _send_buffer_flush_offset += size;

            // Successfully send the whole flush buffer
            if (_send_buffer_flush_offset == _send_buffer_flush.size())
            {
                // Clear the flush buffer
                _send_buffer_flush.clear();
                _send_buffer_flush_offset = 0;
            }

            // Call the buffer sent handler
            onSent(size, bytes_pending());
        }

        // Try to send again if the session is valid
        if (!ec)
            TrySend();
        else
        {
            SendError(ec);
            DisconnectAsync(true);
        }
    });
    if (_strand_required)
        _socket.async_write_some(BAsio::buffer(_send_buffer_flush.data() + _send_buffer_flush_offset, _send_buffer_flush.size() - _send_buffer_flush_offset), bind_executor(_strand, async_write_handler));
    else
        _socket.async_write_some(BAsio::buffer(_send_buffer_flush.data() + _send_buffer_flush_offset, _send_buffer_flush.size() - _send_buffer_flush_offset), async_write_handler);
}

void TCPClient::ClearBuffers()
{
    {
        std::scoped_lock locker(_send_lock);

        // Clear send buffers
        _send_buffer_main.clear();
        _send_buffer_flush.clear();
        _send_buffer_flush_offset = 0;

        // Update statistic
        _bytes_pending = 0;
        _bytes_sending = 0;
    }
}

void TCPClient::SendError(TErrCode ec)
{
    // Skip Asio disconnect errors
    if ((ec == BAsio::error::connection_aborted) ||
        (ec == BAsio::error::connection_refused) ||
        (ec == BAsio::error::connection_reset) ||
        (ec == BAsio::error::eof) ||
        (ec == BAsio::error::operation_aborted))
        return;

    onError(ec.value(), ec.category().name(), ec.message());
}

} // namespace Asio
} // namespace CppServer
