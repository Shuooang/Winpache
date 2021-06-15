/*!
    \file ssl_session.cpp
    \brief SSL session implementation
    \author Ivan Shynkarenka
    \date 30.12.2016
    \copyright MIT License
*/
#include "server/kwadjust.h"

#include "server/asio/ssl_session.h"
#include "server/asio/ssl_server.h"

namespace CppServer {
namespace Asio {

SSLSession::SSLSession(const std::shared_ptr<SSLServer>& server)
    : _id(CppCommon::UUID::Sequential()),
      _server(server),
      _io_service(server->service()->GetAsioService()),
      _strand(*_io_service),
      _strand_required(_server->_strand_required),
      _stream(*_io_service, *server->context()),
      _connected(false),
      _handshaked(false),
      _bytes_pending(0),
      _bytes_sending(0),
      _bytes_sent(0),
      _bytes_received(0),
      _receiving(false),
      _sending(false),
      _send_buffer_flush_offset(0)
{
}

size_t SSLSession::option_receive_buffer_size() const
{
    BAsio::socket_base::receive_buffer_size option;
    _stream.next_layer().get_option(option);
    return option.value();
}

size_t SSLSession::option_send_buffer_size() const
{
    BAsio::socket_base::send_buffer_size option;
    _stream.next_layer().get_option(option);
    return option.value();
}

void SSLSession::SetupReceiveBufferSize(size_t size)
{
    BAsio::socket_base::receive_buffer_size option((int)size);
    _stream.next_layer().set_option(option);
}

void SSLSession::SetupSendBufferSize(size_t size)
{
    BAsio::socket_base::send_buffer_size option((int)size);
    _stream.next_layer().set_option(option);
}

void SSLSession::Connect()
{
    // Apply the option: keep alive
    if (_server->option_keep_alive())
        socket().set_option(BAsio::ip::tcp::socket::keep_alive(true));
    // Apply the option: no delay
    if (_server->option_no_delay())
        socket().set_option(BAsio::ip::tcp::no_delay(true));

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

    // Call the session connected handler
    onConnected();

    // Call the session connected handler in the server
    auto connected_session(this->shared_from_this());
    _server->onConnected(connected_session);

    // Async SSL handshake with the handshake handler
    auto self(this->shared_from_this());
    auto async_handshake_handler = [this, self](TErrCode ec)
    {
        if (IsHandshaked())
            return;

        if (!ec)
        {
            // Update the handshaked flag
            _handshaked = true;

            // Call the session handshaked handler
            onHandshaked();

            // Call the session handshaked handler in the server
            auto handshaked_session(this->shared_from_this());
            _server->onHandshaked(handshaked_session);

            // Call the empty send buffer handler
            if (_send_buffer_main.empty())
                onEmpty();

            // Try to receive something from the client
            TryReceive();
        }
        else
        {
            // Disconnect in case of the bad handshake
            SendError(ec);
            Disconnect(ec);
        }
    };
    if (_strand_required)
        _stream.async_handshake(BAsio::ssl::stream_base::server, bind_executor(_strand, async_handshake_handler));
    else
        _stream.async_handshake(BAsio::ssl::stream_base::server, async_handshake_handler);
}

void SSLSession::Disconnect(TErrCode ec)
{
    if (!IsConnected())
        return;

    // Close the session socket
    socket().close();

    // Update the handshaked flag
    _handshaked = false;

    // Update the connected flag
    _connected = false;

    // Update sending/receiving flags
    _receiving = false;
    _sending = false;

    // Clear send/receive buffers
    ClearBuffers();

    // Call the session disconnected handler
    onDisconnected();

    // Call the session disconnected handler in the server
    auto disconnected_session(this->shared_from_this());
    _server->onDisconnected(disconnected_session);

    // Dispatch the unregister session handler
    auto self(this->shared_from_this());
    auto unregister_session_handler = [this, self]()
    {
        _server->UnregisterSession(id());
    };
    if (_server->_strand_required)
        _server->_strand.dispatch(unregister_session_handler);
    else
        _server->_io_service->dispatch(unregister_session_handler);
}

bool SSLSession::DisconnectAsync(bool dispatch)
{
    if (!IsConnected())
        return false;

    // Dispatch or post the disconnect handler
    auto self(this->shared_from_this());
    auto disconnect_handler = [this, self]()
    {
        if (!IsConnected())
            return;

        // Cancel the session socket
        TErrCode error;
        socket().cancel(error);
        if (error)
        {
            Disconnect(error);
            return;
        }

        // Async SSL shutdown with the shutdown handler
        auto async_shutdown_handler = [this, self](TErrCode ec) { Disconnect(ec); };
        if (_strand_required)
            _stream.async_shutdown(bind_executor(_strand, async_shutdown_handler));
        else
            _stream.async_shutdown(async_shutdown_handler);
    };
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

size_t SSLSession::Send(const void* buffer, size_t size)
{
    if (!IsHandshaked())
        return 0;

    if (size == 0)
        return 0;

    assert((buffer != nullptr) && "Pointer to the buffer should not be null!");
    if (buffer == nullptr)
        return 0;

    TErrCode ec;

    // Send data to the client
    size_t sent = BAsio::write(_stream, BAsio::buffer(buffer, size), ec);
    if (sent > 0)
    {
        // Update statistic
        _bytes_sent += sent;
        _server->_bytes_sent += sent;

        // Call the buffer sent handler
        onSent(sent, bytes_pending());
    }

    // Disconnect on error
    if (ec)
    {
        SendError(ec);
        Disconnect(ec);
    }

    return sent;
}

size_t SSLSession::Send(const void* buffer, size_t size, const CppCommon::Timespan& timeout)
{
    if (!IsHandshaked())
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
    BAsio::system_timer timer(_stream.get_executor());

    // Prepare done handler
    auto async_done_handler = [&](TErrCode ec)
    {
        std::unique_lock<std::mutex> lck(mtx);
        if (done++ == 0)
        {
            error = ec;
            _stream.next_layer().cancel();
            timer.cancel();
        }
        cv.notify_one();
    };

    // Async wait for timeout
    timer.expires_from_now(timeout.chrono());
    timer.async_wait([&](const TErrCode& ec) { async_done_handler(ec ? ec : BAsio::error::timed_out); });

    // Async write some data to the client
    size_t sent = 0;
    _stream.async_write_some(BAsio::buffer(buffer, size), [&](TErrCode ec, size_t write) { async_done_handler(ec); sent = write; });

    // Wait for complete or timeout
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [&]() { return done == 2; });

    // Send data to the client
    if (sent > 0)
    {
        // Update statistic
        _bytes_sent += sent;
        _server->_bytes_sent += sent;

        // Call the buffer sent handler
        onSent(sent, bytes_pending());
    }

    // Disconnect on error
    if (error && (error != BAsio::error::timed_out))
    {
        SendError(error);
        Disconnect(error);
    }

    return sent;
}

bool SSLSession::SendAsync(const void* buffer, size_t size)
{
    if (!IsHandshaked())
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

size_t SSLSession::Receive(void* buffer, size_t size)
{
    if (!IsHandshaked())
        return 0;

    if (size == 0)
        return 0;

    assert((buffer != nullptr) && "Pointer to the buffer should not be null!");
    if (buffer == nullptr)
        return 0;

    TErrCode ec;

    // Receive data from the client
    size_t received = _stream.read_some(BAsio::buffer(buffer, size), ec);
    if (received > 0)
    {
        // Update statistic
        _bytes_received += received;
        _server->_bytes_received += received;

        // Call the buffer received handler
        onReceived(buffer, received);
    }

    // Disconnect on error
    if (ec)
    {
        SendError(ec);
        Disconnect(ec);
    }

    return received;
}

std::string SSLSession::Receive(size_t size)
{
    std::string text(size, 0);
    text.resize(Receive(text.data(), text.size()));
    return text;
}

size_t SSLSession::Receive(void* buffer, size_t size, const CppCommon::Timespan& timeout)
{
    if (!IsHandshaked())
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
    BAsio::system_timer timer(_stream.get_executor());

    // Prepare done handler
    auto async_done_handler = [&](TErrCode ec)
    {
        std::unique_lock<std::mutex> lck(mtx);
        if (done++ == 0)
        {
            error = ec;
            _stream.next_layer().cancel();
            timer.cancel();
        }
        cv.notify_one();
    };

    // Async wait for timeout
    timer.expires_from_now(timeout.chrono());
    timer.async_wait([&](const TErrCode& ec) { async_done_handler(ec ? ec : BAsio::error::timed_out); });

    // Async read some data from the client
    size_t received = 0;
    _stream.async_read_some(BAsio::buffer(buffer, size), [&](TErrCode ec, size_t read) { async_done_handler(ec); received = read; });

    // Wait for complete or timeout
    std::unique_lock<std::mutex> lck(mtx);
    cv.wait(lck, [&]() { return done == 2; });

    // Received some data from the client
    if (received > 0)
    {
        // Update statistic
        _bytes_received += received;
        _server->_bytes_received += received;

        // Call the buffer received handler
        onReceived(buffer, received);
    }

    // Disconnect on error
    if (error && (error != BAsio::error::timed_out))
    {
        SendError(error);
        Disconnect(error);
    }

    return received;
}

std::string SSLSession::Receive(size_t size, const CppCommon::Timespan& timeout)
{
    std::string text(size, 0);
    text.resize(Receive(text.data(), text.size(), timeout));
    return text;
}

void SSLSession::ReceiveAsync()
{
    // Try to receive data from the client
    TryReceive();
}

void SSLSession::TryReceive()
{
    if (_receiving)
        return;

    if (!IsHandshaked())
        return;

    // Async receive with the receive handler
    _receiving = true;
    auto self(this->shared_from_this());
    auto async_receive_handler = make_alloc_handler(_receive_storage, [this, self](TErrCode ec, size_t size)
    {
        _receiving = false;

        if (!IsHandshaked())
            return;

        // Received some data from the client
        if (size > 0)
        {
            // Update statistic
            _bytes_received += size;
            _server->_bytes_received += size;

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
            Disconnect(ec);
        }
    });
    if (_strand_required)
        _stream.async_read_some(BAsio::buffer(_receive_buffer.data(), _receive_buffer.size()), bind_executor(_strand, async_receive_handler));
    else
        _stream.async_read_some(BAsio::buffer(_receive_buffer.data(), _receive_buffer.size()), async_receive_handler);
}

void SSLSession::TrySend()
{
    if (_sending)
        return;

    if (!IsHandshaked())
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

        if (!IsHandshaked())
            return;

        // Send some data to the client
        if (size > 0)
        {
            // Update statistic
            _bytes_sending -= size;
            _bytes_sent += size;
            _server->_bytes_sent += size;

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
            onSentKw(_send_buffer_flush.data(), _send_buffer_flush.size());
        }

        // Try to send again if the session is valid
        if (!ec)
            TrySend();
        else
        {
            SendError(ec);
            Disconnect(ec);
        }
    });
    if (_strand_required)
        _stream.async_write_some(BAsio::buffer(_send_buffer_flush.data() + _send_buffer_flush_offset, _send_buffer_flush.size() - _send_buffer_flush_offset), bind_executor(_strand, async_write_handler));
    else
        _stream.async_write_some(BAsio::buffer(_send_buffer_flush.data() + _send_buffer_flush_offset, _send_buffer_flush.size() - _send_buffer_flush_offset), async_write_handler);
}

void SSLSession::ClearBuffers()
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

void SSLSession::ResetServer()
{
    // Reset cycle-reference to the server
    _server.reset();
}

void SSLSession::SendError(TErrCode ec)
{
    // Skip Asio disconnect errors
    if ((ec == BAsio::error::connection_aborted) ||
        (ec == BAsio::error::connection_refused) ||
        (ec == BAsio::error::connection_reset) ||
        (ec == BAsio::error::eof) ||
        (ec == BAsio::error::operation_aborted))
        return;

    // Skip OpenSSL annoying errors
    if (ec == BAsio::ssl::error::stream_truncated)
        return;
    if (ec.category() == BAsio::error::get_ssl_category())
    {
        if ((ERR_GET_REASON(ec.value()) == SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC) ||
            (ERR_GET_REASON(ec.value()) == SSL_R_PROTOCOL_IS_SHUTDOWN) ||
            (ERR_GET_REASON(ec.value()) == SSL_R_WRONG_VERSION_NUMBER))
            return;
    }

    onError(ec.value(), ec.category().name(), ec.message());
}

} // namespace Asio
} // namespace CppServer
