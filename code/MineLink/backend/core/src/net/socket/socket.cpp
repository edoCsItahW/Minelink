// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file socket.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/17 13:02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module net;

import std;
import :transport.decl;
import :socket.decl;

namespace minelink::net {

    TcpConnection::TcpConnection(std::intptr_t socket) noexcept
        : handle_(socket) {}

    TcpConnection::~TcpConnection() { close(); }

    std::expected<std::size_t, NetError> TcpConnection::readSome(std::span<std::byte> out) {
        using enum NetError;

        if (handle_ < 0 || out.empty()) return std::unexpected(Closed);

        const auto rc = mlnSocketRead(handle_, out.data(), out.size());

        if (rc > 0) return static_cast<std::size_t>(rc);

        if (rc == 0) return std::unexpected(Closed);

        return std::unexpected(rc == -1 ? Timeout : Io);
    }

    bool TcpConnection::writeAll(std::span<const std::byte> data) {
        if (handle_ < 0) return false;

        while (!data.empty()) {
            const auto rc = mlnSocketWrite(handle_, data.data(), data.size());
            if (rc <= 0) return false;
            data = data.subspan(static_cast<std::size_t>(rc));
        }

        return true;
    }

    bool TcpConnection::discard(std::size_t bytes) {
        std::array<std::byte, 4096> scratch{};

        while (bytes != 0) {
            const std::size_t chunk = bytes < scratch.size() ? bytes : scratch.size();

            const auto got = readSome(std::span{scratch.data(), chunk});

            if (!got) return false;

            bytes -= *got;
        }

        return true;
    }

    void TcpConnection::close() noexcept {
        if (handle_ >= 0) {
            mlnSocketClose(handle_);
            handle_ = -1;
        }
    }

    [[nodiscard]] std::expected<std::unique_ptr<IConnection>, NetError> connectTcp(
        const Endpoint& endpoint, std::chrono::milliseconds connectTimeout,
        std::chrono::milliseconds ioTimeout
    ) {
        const std::intptr_t handle = mlnSocketConnect(
            endpoint.host.c_str(), endpoint.port, static_cast<int>(connectTimeout.count()),
            static_cast<int>(ioTimeout.count())
        );

        if (handle < 0) return std::unexpected(NetError::ConnectFailed);

        return std::unique_ptr<IConnection>(new TcpConnection{handle});
    }


}  // namespace minelink::net
