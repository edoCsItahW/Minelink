// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file login.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/17 21:30
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module protocol;

import :ids;

namespace minelink::protocol {

    constexpr auto keep_alive_timeout = std::chrono::seconds{30};

    template<typename... Ts>
    [[nodiscard]] std::size_t writeFields(std::span<std::byte> out, const Ts&... values) {
        std::byte* current = out.data();

        std::byte* const end = current + out.size();

        if (!(wire_write(current, end, values) && ...)) return 0;

        return static_cast<std::size_t>(current - out.data());
    }

    [[nodiscard]] bool readKeepAliveId(
        const std::byte* payload, const std::byte* end, std::int64_t& out
    ) {
        Reader reader{
            std::span{payload, static_cast<std::size_t>(end - payload)}
        };

        const auto id = readLong(payload, end);

        if (!id) return false;

        out = *id;

        return true;
    }

    LoginSession::LoginSession(net::IConnection& connection, LoginOptions options)
        : connection_(connection)
        , options_(std::move(options))
        , in_(options_.readBufferBytes)
        , out_(64 * 1024) {}

    LoginSession::~LoginSession() = default;

    const PlayerState& LoginSession::player() const noexcept { return player_; }

    LoginState LoginSession::state() const noexcept { return state_; }

    std::string_view LoginSession::closeReason() const noexcept { return closeReason_; }

    std::uint64_t LoginSession::ignoredPackets() const noexcept { return ignoredPackets_; }

    bool LoginSession::start() {
        if (state_ != LoginState::Idle) return fail("会话已启动");

        std::array<std::byte, 512> handshake{};

        const std::size_t handshakeSize = writeFields(
            handshake, options_.protocol, std::string_view{options_.host}, options_.port,
            std::int32_t{2}
        );
        if (handshakeSize == 0) return fail("Handshake编码失败");

        if (!sendPacket(
                v1_20_4::handshake_c2s_set_protocol, std::span{handshake.data(), handshakeSize}
            ))
            return fail("Handshake发送失败");

        std::array<std::byte, 128> loginStart{};
        std::size_t loginSize = writeFields(loginStart, std::string_view{options_.username});
        if (loginSize == 0) return fail("Login Start编码失败");

        for (int i = 0; i < 16; ++i) loginStart[loginSize++] = std::byte{0x00};

        if (!sendPacket(v1_20_4::login_c2s_login_start, std::span{loginStart.data(), loginSize}))
            return fail("Login Start发送失败");

        state_ = LoginState::LoggingIn;
        return true;
    }

    bool LoginSession::pump() {
        if (!drainFrames()) return false;

        if (inLength_ == in_.size()) return fail("读缓冲区已满且没有完整帧");

        const auto got =
            connection_.readSome(std::span{in_.data() + inLength_, in_.size() - inLength_});
        if (!got) {
            if (got.error() == NetError::Timeout) return true;
            return fail(got.error() == NetError::Closed ? "对端关闭" : "读取失败");
        }
        if (*got == 0) return fail("对端关闭");

        inLength_ += *got;

        return drainFrames();
    }

    bool LoginSession::drainFrames() {
        for (;;) {
            std::size_t consumed = 0;

            auto frame = frame_.decode(std::span<const std::byte>{in_.data(), inLength_}, consumed);
            if (!frame) {
                if (frame.error() == FrameError::Truncated) return true;
                return fail("帧解析失败");
            }

            const std::byte* payload    = frame->payload.data();
            const std::byte* const pEnd = payload + frame->payload.size();

            if (!dispatch(frame->id, payload, pEnd)) return false;

            std::memmove(in_.data(), in_.data() + consumed, inLength_ - consumed);

            inLength_ -= consumed;
        }
    }

    namespace {

        [[nodiscard]] std::size_t writeLongField(std::span<std::byte> out, std::int64_t value) {
            std::byte* current   = out.data();
            std::byte* const end = current + out.size();

            if (!writeLong(current, end, value)) return 0;

            return static_cast<std::size_t>(current - out.data());
        }

    }  // namespace

    bool LoginSession::dispatch(std::int32_t id, const std::byte* payload, const std::byte* end) {
        const auto span = std::span{payload, static_cast<std::size_t>(end - payload)};

        switch (state_) {
            using enum LoginState;
            case LoggingIn: {
                if (id == v1_20_4::login_s2c_compress) {
                    Reader reader{span};

                    const auto threshold = reader.read<std::int32_t>();
                    if (!threshold) return fail("Set Compression解析失败");

                    frame_.enableCompression(*threshold);

                    return true;
                }

                if (id == v1_20_4::login_s2c_success) {
                    if (!sendPacket(v1_20_4::login_c2s_login_acknowledged, {}))
                        return fail("Login Acknowledged发送失败");

                    state_ = Configuring;

                    return sendClientInformation();
                }

                if (id == v1_20_4::login_s2c_disconnect) return readDisconnect(payload, end);

                if (options_.dumpPackets)
                    std::println(
                        "[ignored] state={} id=0x{:02x} size={}", static_cast<int>(state_), id,
                        span.size()
                    );

                ++ignoredPackets_;

                return true;
            }

            case Configuring: {
                if (id == v1_20_4::config_s2c_keep_alive) {
                    std::int64_t keepAlive = 0;
                    if (!readKeepAliveId(payload, end, keepAlive))
                        return fail("Keep Alive解析失败");

                    std::array<std::byte, 16> body{};
                    const std::size_t size = writeLongField(body, keepAlive);
                    if (size == 0) return fail("Keep Alive编码失败");

                    if (!sendPacket(v1_20_4::config_c2s_keep_alive, std::span{body.data(), size}))
                        return fail("Keep Alive回显失败");

                    return true;
                }

                if (id == v1_20_4::config_s2c_finish_configuration) {
                    if (!sendPacket(v1_20_4::config_c2s_finish_configuration, {}))
                        return fail("Finish Configuration发送失败");

                    state_         = Play;
                    lastKeepAlive_ = std::chrono::steady_clock::now();

                    return true;
                }

                if (id == v1_20_4::config_s2c_disconnect) return readDisconnect(payload, end);

                if (options_.dumpPackets)
                    std::println(
                        "[ignored] state={} id=0x{:02x} size={}", static_cast<int>(state_), id,
                        span.size()
                    );

                ++ignoredPackets_;

                return true;
            }

            case Play: return onPlayPacket(id, payload, end);

            default: return true;
        }
    }

    bool LoginSession::onPlayPacket(
        std::int32_t id, const std::byte* payload, const std::byte* end
    ) {
        const auto span = std::span{payload, static_cast<std::size_t>(end - payload)};

        if (id == v1_20_4::play_s2c_keep_alive) {
            std::int64_t keepAlive = 0;
            if (!readKeepAliveId(payload, end, keepAlive)) return fail("Keep Alive解析失败");

            lastKeepAlive_ = std::chrono::steady_clock::now();

            std::array<std::byte, 8> body{};
            const std::size_t size = writeLongField(body, keepAlive);
            if (size == 0) return fail("Keep Alive编码失败");

            if (!sendPacket(v1_20_4::play_c2s_keep_alive, std::span{body.data(), size}))
                return fail("Keep Alive回显失败");

            return true;
        }

        if (id == v1_20_4::play_s2c_position) {
            Reader reader{span};

            const auto x          = reader.read<double>();
            const auto y          = reader.read<double>();
            const auto z          = reader.read<double>();
            const auto yaw        = reader.read<float>();
            const auto pitch      = reader.read<float>();
            const auto flags      = reader.read<std::uint8_t>();
            const auto teleportId = reader.read<std::int32_t>();
            if (!x || !y || !z || !yaw || !pitch || !flags || !teleportId)
                return fail("Synchronize Player Position解析失败");

            player_.x     = *x;
            player_.y     = *y;
            player_.z     = *z;
            player_.yaw   = *yaw;
            player_.pitch = *pitch;

            std::array<std::byte, 16> confirm{};
            const std::size_t confirmSize = writeFields(confirm, *teleportId);
            if (confirmSize == 0
                || !sendPacket(
                    v1_20_4::play_c2s_teleport_confirm, std::span{confirm.data(), confirmSize}
                ))
                return fail("Accept Teleportation发送失败");

            std::array<std::byte, 64> position{};
            const std::size_t positionSize = writeFields(
                position, player_.x, player_.y, player_.z, player_.yaw, player_.pitch, true
            );
            if (positionSize == 0
                || !sendPacket(
                    v1_20_4::play_c2s_position_look, std::span{position.data(), positionSize}
                ))
                return fail("Player Position发送失败");

            return true;
        }

        if (id == v1_20_4::play_s2c_login) {
            Reader reader{span};
            const auto entityId = reader.read<std::int32_t>();
            if (!entityId) return fail("Join Game解析失败");

            player_.entityId = *entityId;

            return true;
        }

        if (id == v1_20_4::play_s2c_update_health) {
            Reader reader{span};
            const auto health = reader.read<float>();
            if (!health) return fail("Update Health解析失败");

            player_.health = static_cast<std::int32_t>(*health);

            return true;
        }

        if (id == v1_20_4::play_s2c_kick_disconnect) return readDisconnect(payload, end);

        if (options_.dumpPackets)
            std::println(
                "[ignored] state={} id=0x{:02x} size={}", static_cast<int>(state_), id, span.size()
            );

        ++ignoredPackets_;

        return true;
    }

    bool LoginSession::checkAlive(std::chrono::steady_clock::time_point now) {
        if (state_ != LoginState::Play) return true;

        if (now - lastKeepAlive_ > keep_alive_timeout) return fail("Keep Alive超时");

        return true;
    }

    bool LoginSession::sendClientInformation() {
        std::array<std::byte, 128> body{};
        const std::size_t size = writeFields(
            body, std::string_view{"zh_cn"}, options_.viewDistance, std::int32_t{0}, true,
            std::uint8_t{0x7F}, std::int32_t{1}, false, true
        );
        if (size == 0) return fail("Client Information编码失败");

        if (!sendPacket(v1_20_4::config_c2s_settings, std::span{body.data(), size}))
            return fail("Client Information发送失败");

        return true;
    }

    bool LoginSession::sendPacket(std::int32_t id, std::span<const std::byte> body) {
        const std::size_t size = frame_.encode(id, body, out_);
        if (size == 0) return false;

        return connection_.writeAll(std::span<const std::byte>{out_.data(), size});
    }

    bool LoginSession::readDisconnect(const std::byte* payload, const std::byte* end) {
        Reader reader{
            std::span{payload, static_cast<std::size_t>(end - payload)}
        };

        const auto reason = reader.read<std::string_view>();
        closeReason_      = reason ? std::string{*reason} : std::string{"<无法解析的断开原因>"};

        state_ = LoginState::Closed;

        return false;
    }

    bool LoginSession::fail(std::string_view reason) {
        if (closeReason_.empty()) closeReason_ = std::string{reason};

        state_ = LoginState::Closed;

        connection_.close();

        return false;
    }

    std::expected<PingResult, NetError> ping(
        const net::Endpoint& endpoint, std::chrono::milliseconds timeout
    ) {
        using enum NetError;

        auto connection = net::connectTcp(endpoint, timeout);
        if (!connection) return std::unexpected(connection.error());

        FrameCodec frame;  // status 阶段没有压缩
        std::array<std::byte, 1024> out{};

        // Handshake(nextState = 1 = status)
        std::array<std::byte, 512> handshake{};
        const std::size_t handshakeSize = writeFields(
            handshake, v1_20_4::protocol_version, std::string_view{endpoint.host}, endpoint.port,
            std::int32_t{1}
        );
        if (handshakeSize == 0) return std::unexpected(Io);

        std::size_t frameSize = frame.encode(
            v1_20_4::handshake_c2s_set_protocol, std::span{handshake.data(), handshakeSize}, out
        );
        if (frameSize == 0
            || !connection.value()->writeAll(std::span<const std::byte>{out.data(), frameSize})) {
            return std::unexpected(Io);
        }

        frameSize = frame.encode(v1_20_4::status_c2s_ping_start, {}, out);
        if (frameSize == 0
            || !connection.value()->writeAll(std::span<const std::byte>{out.data(), frameSize})) {
            return std::unexpected(Io);
        }

        std::vector<std::byte> in(64 * 1024);
        std::size_t inLength = 0;

        for (;;) {
            std::size_t consumed = 0;
            auto decoded = frame.decode(std::span<const std::byte>{in.data(), inLength}, consumed);
            if (decoded) {
                Reader reader{decoded->payload};
                const auto json = reader.read<std::string_view>();
                if (!json) return std::unexpected(Io);

                PingResult result;
                result.json = std::string{*json};

                // 只取两个字段，避免为一次探测引入 JSON 依赖
                const std::regex protocolPattern{R"("protocol"\s*:\s*(-?[0-9]+))"};
                const std::regex versionPattern{R"json("name"\s*:\s*"([^"]*)")json"};

                std::cmatch match;
                if (std::regex_search(result.json.c_str(), match, protocolPattern))
                    result.protocol = std::stoi(match[1].str());
                if (std::regex_search(result.json.c_str(), match, versionPattern))
                    result.version = match[1].str();
                return result;
            }

            if (decoded.error() != FrameError::Truncated || inLength == in.size())
                return std::unexpected(Io);

            const auto got =
                connection.value()->readSome(std::span{in.data() + inLength, in.size() - inLength});
            if (!got) return std::unexpected(got.error());
            inLength += *got;
        }
    }

}  // namespace minelink::protocol
