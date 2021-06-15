/*!
    \file barrier.cpp
    \brief Barrier synchronization primitive implementation
    \author Ivan Shynkarenka
    \date 16.03.2016
    \copyright MIT License
*/
//#include "pch.h"

#include "threads/barrier.h"

#include <cassert>

#if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(__APPLE__)
#include "errors/fatal.h"
#include <pthread.h>
#elif defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__)
#include <condition_variable>
#include <mutex>
#elif defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

namespace CppCommon {

//! @cond INTERNALS

class Barrier::Impl
{
public:
    explicit Impl(int threads) : _threads(threads)
    {
        assert((threads > 0) && "Barrier threads counter must be greater than zero!");

#if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(__APPLE__)
        int result = pthread_barrier_init(&_barrier, nullptr, threads);
        if (result != 0)
            throwex SystemException("Failed to initialize a synchronization barrier!", result);
#elif defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__)
        _counter = threads;
        _generation = 0;
#elif defined(_WIN32) || defined(_WIN64)
        if (!InitializeSynchronizationBarrier(&_barrier, threads, -1))
            throwex SystemException("Failed to initialize a synchronization barrier!");
#endif
    }

    ~Impl()
    {
#if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(__APPLE__)
        int result = pthread_barrier_destroy(&_barrier);
        if (result != 0)
            fatality(SystemException("Failed to destroy a synchronization barrier!", result));
#elif defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__)
        // Do nothing here...
#elif defined(_WIN32) || defined(_WIN64)
        DeleteSynchronizationBarrier(&_barrier);
#endif
    }

    int threads() const noexcept
    {
        return _threads;
    }

    bool Wait()
    {
#if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(__APPLE__)
        int result = pthread_barrier_wait(&_barrier);
        if ((result != PTHREAD_BARRIER_SERIAL_THREAD) && (result != 0))
            throwex SystemException("Failed to wait at a synchronization barrier!", result);
        return (result == PTHREAD_BARRIER_SERIAL_THREAD);
#elif defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__)
        std::unique_lock<std::mutex> lock(_mutex);

        // Remember the current barrier generation
        int generation = _generation;

        // Decrease the count of waiting threads
        if (--_counter == 0)
        {
            // Increase the current barrier generation
            ++_generation;

            // Reset waiting threads counter
            _counter = _threads;

            // Notify all waiting threads
            _cond.notify_all();

            // Notify the last thread that reached the barrier
            return true;
        }

        // Wait for the next barrier generation
        _cond.wait(lock, [&, this]() { return generation != _generation; });

        // Notify each of remaining threads
        return false;
#elif defined(_WIN32) || defined(_WIN64)
        return (EnterSynchronizationBarrier(&_barrier, 0) == TRUE);
#endif
    }

private:
    int _threads;
#if (defined(unix) || defined(__unix) || defined(__unix__)) && !defined(__APPLE__)
    pthread_barrier_t _barrier;
#elif defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__)
    std::mutex _mutex;
    std::condition_variable _cond;
    int _counter;
    int _generation;
#elif defined(_WIN32) || defined(_WIN64)
    SYNCHRONIZATION_BARRIER _barrier;
#endif
};

//! @endcond

Barrier::Barrier(int threads) : _pimpl(std::make_unique<Impl>(threads))
{
}

Barrier::~Barrier()
{
}

int Barrier::threads() const noexcept
{
    return _pimpl->threads();
}

bool Barrier::Wait()
{
    return _pimpl->Wait();
}

} // namespace CppCommon
