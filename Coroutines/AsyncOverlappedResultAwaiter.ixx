module;
#include <Windows.h>
#include <coroutine>
#include <wil\resource.h>

export module AsyncOverlappedResultAwaiter;
import AsyncOverlappedResult;
import Exceptions;

using unique_wait_handle = wil::unique_any<HANDLE, decltype(&::UnregisterWait), ::UnregisterWait>;

struct AsyncOverlappedResultAwaiter {
    AsyncOverlappedResult& m_result;
    mutable unique_wait_handle m_waitHandle;

    bool await_ready() const {
        return m_result.hasResult();
    }

    void await_suspend(std::coroutine_handle<> coroHandle) const {
        auto callback = [](void* parameter, BOOLEAN isTimedOut) {
            std::coroutine_handle<>::from_address(parameter).resume();
        };

        if (RegisterWaitForSingleObject(m_waitHandle.put(), m_result.getOverlapped().hEvent, callback, coroHandle.address(), INFINITE, WT_EXECUTEONLYONCE) == 0) {
            throw Win32Exception("RegisterWaitForSingleObject");
        }
    }

    unsigned long await_resume() { return m_result.getOverlapped().InternalHigh; }
};

export auto operator co_await(AsyncOverlappedResult& result) {
    return AsyncOverlappedResultAwaiter{ result };
}

export auto operator co_await(AsyncOverlappedResult&& result) {
    return AsyncOverlappedResultAwaiter{ result };
}