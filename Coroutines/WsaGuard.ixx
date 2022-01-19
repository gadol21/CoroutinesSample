module;
#include <WinSock2.h>
#include <format>

export module WsaGuard;
import Exceptions;

using std::format;

export class WsaGuard {
public:
    WsaGuard() {
        int result = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
        if (result != 0) {
            throw Exception(format("Failed initializing WsaGuard with error {}", result));
        }
    }

    ~WsaGuard() {
        WSACleanup();
    }

    WsaGuard(const WsaGuard&) = delete;
    WsaGuard& operator=(const WsaGuard&) = delete;

private:
    WSADATA m_wsaData;
};