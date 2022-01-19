#include <string>
#include <vector>
#include <iostream>
#include <ppltasks.h>
#include <pplawait.h>

import AsyncOverlappedResult;
import AsyncOverlappedResultAwaiter;
import TcpSocket;
import WsaGuard;

using concurrency::task;

task<void> logic() {
    WsaGuard wsaGuard;

    TcpSocket sock;
    co_await sock.connect("127.0.0.1", 13370);

    while (true) {
        std::vector<char> data = co_await sock.recv(1024);
        co_await sock.send(data);
    }
}

int main() {
    auto res = logic();

    std::cout << "Logic started" << std::endl;

    res.wait();

    return 0;
}