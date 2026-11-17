// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file transport.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 13:37
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module net:transport.decl;

import std;
import base;

export namespace minelink::net {

    struct Endpoint {
        std::string host;
        std::uint16_t port{};
    };

    class IConnection {
    public:
        virtual ~IConnection() = default;

        [[nodiscard]] virtual std::expected<std::size_t, NetError> readSome(
            std::span<std::byte> out
        ) = 0;

        [[nodiscard]] virtual bool readExact(std::span<std::byte> out);

        [[nodiscard]] virtual bool writeAll(std::span<const std::byte> data) = 0;

        [[nodiscard]] virtual bool discard(std::size_t n) = 0;

        virtual void close() noexcept = 0;
    };

    [[nodiscard]] std::expected<std::unique_ptr<IConnection>, NetError> connectTcp(
        const Endpoint& endpoint, std::chrono::milliseconds connectTimeout,
        std::chrono::milliseconds ioTimeout = std::chrono::milliseconds{30000}
    );

}  // namespace minelink::net
