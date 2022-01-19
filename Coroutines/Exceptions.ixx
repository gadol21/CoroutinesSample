module;
#include <WinSock2.h>
#include <Windows.h>
#include <stdexcept>
#include <format>

export module Exceptions;

using std::format;

export class Exception : public std::runtime_error {
public:
    Exception(const std::string& message) : std::runtime_error(message) {
        // Left blank
    }
};

export class Win32Exception : public Exception {
public:
    Win32Exception(const char* failedOperation)
        : Exception(format("Win32 operation {} failed with error {}", failedOperation, GetLastError())) {
        // Left blank
    }
};

export class SocketException : public Exception {
public:
    SocketException(const char* failedOperation)
        : Exception(format("Socket operation {} failed with error {}", failedOperation, WSAGetLastError())) {
        // Left blank
    }
};
