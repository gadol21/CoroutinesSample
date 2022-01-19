module;
#include <WinSock2.h>
#include <stdexcept>
#include <wil\resource.h>
#include <ppltasks.h>
#include <pplawait.h>

export module TcpSocket;
import AsyncOverlappedResult;
import AsyncOverlappedResultAwaiter;
import Exceptions;

using concurrency::task;

export class TcpSocket {
public:
    TcpSocket() {
        m_socket.reset(WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED));
        if (!m_socket.is_valid()) {
            throw SocketException("WSASocket");
        }
    }

    task<void> connect(const std::string& ip, unsigned short port) {
        AsyncOverlappedResult result;

        if (WSAEventSelect(m_socket.get(), result.getOverlapped().hEvent, FD_CONNECT) == SOCKET_ERROR) {
            throw SocketException("WSAEventSelect");
        }

        sockaddr_in target{
            .sin_family = AF_INET,
            .sin_port = htons(port),
            .sin_addr {.S_un {.S_addr = inet_addr(ip.c_str()) }},
        };

        if (WSAConnect(m_socket.get(), (sockaddr*)&target, sizeof(target), nullptr, nullptr, nullptr, nullptr) == SOCKET_ERROR &&
            WSAGetLastError() != WSAEWOULDBLOCK) {
            throw SocketException("WSAConnect");
        }

        co_await result;

        WSANETWORKEVENTS networkEvents{};
        if (WSAEnumNetworkEvents(m_socket.get(), nullptr, &networkEvents) == SOCKET_ERROR) {
            throw SocketException("WSAEnumNetworkEvents");
        }

        if (networkEvents.iErrorCode[FD_CONNECT_BIT] != 0) {
            throw SocketException("WSAConnect-2");
        }
    }

    task<void> send(const std::vector<char>& data) {
        AsyncOverlappedResult result;

        WSABUF buffer{
            .len = (ULONG)data.size(),
            .buf = (char*)data.data()
        };

        if (WSASend(m_socket.get(), &buffer, 1, nullptr, 0, &result.getOverlapped(), nullptr) == SOCKET_ERROR &&
            WSAGetLastError() != WSA_IO_PENDING) {
            throw SocketException("WSASend");
        }

        co_await result;
    }

    task<std::vector<char>> recv(unsigned long size) {
        AsyncOverlappedResult result;
        std::vector<char> data(size);

        WSABUF buffer{
            .len = size,
            .buf = data.data()
        };

        DWORD flags = 0;
        if (WSARecv(m_socket.get(), &buffer, 1, nullptr, &flags, &result.getOverlapped(), nullptr) == SOCKET_ERROR &&
            WSAGetLastError() != WSA_IO_PENDING) {
            throw SocketException("WSARecv");
        }

        unsigned long bytesRecvd = co_await result;
        data.resize(bytesRecvd);
        co_return data;
    }

private:
    wil::unique_socket m_socket;
};