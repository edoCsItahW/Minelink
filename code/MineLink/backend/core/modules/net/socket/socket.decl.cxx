// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file socket.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/17 16:23
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module net:socket.decl;

import :transport.decl;

export namespace minelink::net {

    class TcpConnection final : public IConnection {
    public:
        explicit TcpConnection(std::intptr_t handle) noexcept;

        ~TcpConnection() override;

        TcpConnection(const TcpConnection&)            = delete ("");
        TcpConnection& operator=(const TcpConnection&) = delete ("");

        std::expected<std::size_t, NetError> readSome(std::span<std::byte> out) override;

        bool writeAll(std::span<const std::byte> data) override;

        bool discard(std::size_t n) override;

        void close() noexcept override;

    private:
        std::intptr_t handle_{-1};
    };

    std::expected<std::unique_ptr<IConnection>, NetError> connectTcp(
        const Endpoint& endpoint, std::chrono::milliseconds connectTimeout,
        std::chrono::milliseconds ioTimeout
    );

    extern "C" {

        std::intptr_t mlnSocketConnect(
            const char* host, unsigned short port, int connectTimeoutMs, int ioTimeoutMs
        );

        std::ptrdiff_t mlnSocketRead(std::intptr_t handle, void* buffer, std::size_t size);

        std::ptrdiff_t mlnSocketWrite(std::intptr_t handle, const void* buffer, std::size_t size);

        void mlnSocketClose(std::intptr_t handle);

        bool mlnSocketRuntimeInit();

        void mlnSocketRuntimeShutdown();
    }

}  // namespace minelink::net
