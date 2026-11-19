// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file platform.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/17 17:59
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <cstdint>
#include <string>

#ifdef _WIN32

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #include <winsock2.h>
    #include <ws2tcpip.h>

#else

    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <unistd.h>

#endif


namespace minelink::net {

#ifdef _WIN32

    using NativeSocket                             = SOCKET;
    inline constexpr NativeSocket invalid_handle_v = INVALID_SOCKET;

#else

    using NativeSocket                           = int;
    inline constexpr NativeSocket invalid_socket = -1;

#endif

    namespace {

        void closeSocket(NativeSocket socket) noexcept {
#ifdef _WIN32

            closesocket(socket);

#else

            close(socket);

#endif
        }

        bool setBlocking(NativeSocket socket, bool blocking) noexcept {
#ifdef _WIN32

            u_long mode = blocking ? 0ul : 1ul;
            return ioctlsocket(socket, FIONBIO, &mode) == 0;

#else

            const int flags = fcntl(socket, F_GETFL, 0);

            if (flags < 0) return false;

            const int next = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

            return fcntl(socket, F_SETFL, next) == 0;

#endif
        }

        bool setIoTimeout(NativeSocket socket, int ioTimeoutMs) noexcept {
#ifdef _WIN32

            const auto ms = static_cast<DWORD>(ioTimeoutMs);
            return setsockopt(
                       socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&ms),
                       sizeof(ms)
                   ) == 0
                && setsockopt(
                       socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&ms),
                       sizeof(ms)
                   ) == 0;

#else

            timeval tv{.tv_sec = ioTimeoutMs / 1000, .tv_usec = (ioTimeoutMs % 1000) * 1000};

            return setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) == 0
                && setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) == 0;

#endif
        }

        bool connectWithTimeout(
            NativeSocket socket, const sockaddr* address, int addressLength, int connectTimeoutMs
        ) noexcept {
            if (!setBlocking(socket, false)) return false;

            if (connect(socket, address, addressLength) != 0) {
#ifdef _WIN32

                const bool inProgress = WSAGetLastError() == WSAEWOULDBLOCK;

#else

                const bool inProgress = errno == EINPROGRESS;

#endif
                if (!inProgress) return false;

                fd_set writable;
                FD_ZERO(&writable);
                FD_SET(socket, &writable);

                timeval tv{
                    .tv_sec  = static_cast<long>(connectTimeoutMs / 1000),
                    .tv_usec = static_cast<long>((connectTimeoutMs % 1000) * 1000)
                };

#ifdef _WIN32

                const int ready = select(0, nullptr, &writable, nullptr, &tv);

#else

                const int ready = select(socket + 1, nullptr, &writable, nullptr, &tv);

#endif
                if (ready <= 0) return false;

                int error             = 0;
                socklen_t errorLength = sizeof(error);
                if (getsockopt(
                        socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &errorLength
                    ) != 0
                    || error != 0)
                    return false;
            }

            return setBlocking(socket, true);
        }

        bool lastErrorIsTimeout() noexcept {
#ifdef _WIN32
            const int code = WSAGetLastError();
            return code == WSAETIMEDOUT || code == WSAEWOULDBLOCK;
#else
            return errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR;
#endif
        }

        struct SocketRuntime {
            bool ready = false;

            SocketRuntime() noexcept;

            ~SocketRuntime();
        };

        bool runtimeReady() noexcept {
            static SocketRuntime instance;

            return instance.ready;
        }

    }  // namespace

    extern "C" {

        std::intptr_t mlnSocketConnect(
            const char* host, unsigned short port, int connectTimeoutMs, int ioTimeoutMs
        ) {
            if (host == nullptr || !runtimeReady()) return -1;

            addrinfo hints{
                .ai_family = hints.ai_family = AF_UNSPEC,
                .ai_socktype = hints.ai_socktype = SOCK_STREAM,
                .ai_protocol = hints.ai_protocol = IPPROTO_TCP
            };
            const std::string service = std::to_string(port);

            addrinfo* resolved = nullptr;
            if (getaddrinfo(host, service.c_str(), &hints, &resolved) != 0 || resolved == nullptr)
                return -1;

            for (addrinfo* it = resolved; it != nullptr; it = it->ai_next) {
                const NativeSocket native = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
                if (native == invalid_handle_v) continue;

                int no_delay = 1;
                setsockopt(
                    native, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&no_delay),
                    sizeof(no_delay)
                );

                if (connectWithTimeout(
                        native, it->ai_addr, static_cast<int>(it->ai_addrlen), connectTimeoutMs
                    )
                    && setIoTimeout(native, ioTimeoutMs)) {
                    freeaddrinfo(resolved);
                    return static_cast<std::intptr_t>(native);
                }

                closeSocket(native);
            }

            freeaddrinfo(resolved);
            return -1;
        }

        std::ptrdiff_t mlnSocketRead(std::intptr_t handle, void* buffer, std::size_t size) {
            if (handle < 0 || buffer == nullptr || size == 0) return -2;

            const auto socket = static_cast<NativeSocket>(handle);
            const int want =
                size > static_cast<std::size_t>(INT_MAX) ? INT_MAX : static_cast<int>(size);
            const int received = ::recv(socket, static_cast<char*>(buffer), want, 0);

            if (received > 0) return received;
            if (received == 0) return 0;
            return lastErrorIsTimeout() ? -1 : -2;
        }

        std::ptrdiff_t mlnSocketWrite(std::intptr_t handle, const void* buffer, std::size_t size) {
            if (handle < 0 || buffer == nullptr || size == 0) return 0;

            const auto socket = static_cast<NativeSocket>(handle);
            const int want =
                size > static_cast<std::size_t>(INT_MAX) ? INT_MAX : static_cast<int>(size);
            const int sent = send(socket, static_cast<const char*>(buffer), want, 0);
            return sent > 0 ? sent : 0;
        }

        void mlnSocketClose(std::intptr_t handle) {
            if (handle >= 0) closeSocket(static_cast<NativeSocket>(handle));
        }

        bool mlnSocketRuntimeInit() {
#ifdef _WIN32
            WSADATA data{};
            return WSAStartup(MAKEWORD(2, 2), &data) == 0;
#else
            return true;
#endif
        }

        void mlnSocketRuntimeShutdown() {
#ifdef _WIN32
            WSACleanup();
#endif
        }
    }

    SocketRuntime::SocketRuntime() noexcept
        : ready(mlnSocketRuntimeInit()) {}

    SocketRuntime::~SocketRuntime() {
        if (ready) mlnSocketRuntimeShutdown();
    }

}  // namespace minelink::net
