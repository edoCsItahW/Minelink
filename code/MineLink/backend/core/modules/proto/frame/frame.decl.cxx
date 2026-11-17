// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file frame.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 13:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:frame.decl;

import std;
import base;

export namespace minelink::protocol {

    inline constexpr std::size_t MAX_FRAME_BYTES = 8u * 1024u * 1024u;

    struct Frame {
        std::int32_t id{};
        std::span<const std::byte> payload{};
    };

    class FrameCodec {
    public:
        explicit FrameCodec(std::size_t maxFrameBytes = MAX_FRAME_BYTES);

        void enableCompression(std::int32_t threshold) noexcept;

        [[nodiscard]] bool compressionEnabled() const noexcept;

        [[nodiscard]] std::int32_t threshold() const noexcept;

        [[nodiscard]] std::size_t encode(std::int32_t id, std::span<const std::byte> body, std::span<std::byte> out);

        [[nodiscard]] std::expected<Frame, FrameError> decode(std::span<const std::byte> buffer, std::size_t& consumed);

    private:
        std::vector<std::byte> scratch_;
        std::vector<std::byte> compressed_;
        std::vector<std::byte> inflate_;
        std::size_t maxFrame_{MAX_FRAME_BYTES};
        std::int32_t threshold_{-1};
    };

}  // namespace minelink::protocol
