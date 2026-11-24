// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file login.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 22:38
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:login.decl;

import std;
import base;
import net;
import :frame;

export namespace minelink::protocol {

    enum class LoginState : std::uint8_t { Idle, LoggingIn, Configuring, Play, Closed };

    struct PlayerState {
        std::int32_t entityId{-1};
        double x{};
        double y{};
        double z{};
        float yaw{};
        float pitch{};
        std::int32_t health{20};
    };

    //

    struct PingResult {
        std::int32_t protocol{};
        std::string version;
        std::string json;
    };

    [[nodiscard]] std::expected<PingResult, NetError> ping(
        const net::Endpoint& endpoint, std::chrono::milliseconds timeout
    );

    struct LoginOptions {
        std::string host;
        std::uint16_t port{25565};
        std::string username{"minelink"};
        std::int32_t protocol{765};
        std::uint8_t viewDistance{8};
        std::size_t readBufferBytes{4u * 1024u * 1024u};
        bool dumpPackets{false};
    };

    class LoginSession {
    public:
        LoginSession(net::IConnection& connection, LoginOptions options);

        ~LoginSession();

        LoginSession(const LoginSession&)            = delete ("");
        LoginSession& operator=(const LoginSession&) = delete ("");

        [[nodiscard]] bool start();

        [[nodiscard]] bool pump();

        [[nodiscard]] bool checkAlive(std::chrono::steady_clock::time_point now);

        [[nodiscard]] LoginState state() const noexcept;

        [[nodiscard]] const PlayerState& player() const noexcept;

        [[nodiscard]] std::string_view closeReason() const noexcept;

        [[nodiscard]] std::uint64_t ignoredPackets() const noexcept;

        using PacketHandler = std::function<void(std::int32_t id, std::span<const std::byte> body)>;

        void setPacketHandler(PacketHandler handler);

        [[nodiscard]] const std::array<std::uint64_t, 256>& packetCounts() const noexcept;

        [[nodiscard]] std::uint64_t dispatchedPackets() const noexcept;

    private:
        [[nodiscard]] bool drainFrames();

        [[nodiscard]] bool dispatch(
            std::int32_t id, const std::byte* payload, const std::byte* end
        );

        [[nodiscard]] bool sendPacket(std::int32_t id, std::span<const std::byte> body);

        [[nodiscard]] bool onPlayPacket(
            std::int32_t id, const std::byte* payload, const std::byte* end
        );

        [[nodiscard]] bool sendClientInformation();

        [[nodiscard]] bool readDisconnect(const std::byte* payload, const std::byte* end);

        [[nodiscard]] bool fail(std::string_view reason);

        net::IConnection& connection_;

        LoginOptions options_;

        FrameCodec frame_;

        LoginState state_{LoginState::Idle};

        PlayerState player_{};

        std::string closeReason_;

        std::uint64_t ignoredPackets_{0};
        std::uint64_t dispatchedPackets_{0};

        std::vector<std::byte> in_;

        std::size_t inLength_{0};

        std::vector<std::byte> out_;

        std::chrono::steady_clock::time_point lastKeepAlive_{};

        PacketHandler packetHandler_{};

        std::array<std::uint64_t, 256> packetCounts_{};
    };

}  // namespace minelink::protocol
