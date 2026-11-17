// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file frame.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/18 14:16
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module;

#include <zlib.h>

module protocol;

namespace minelink::protocol {

    FrameCodec::FrameCodec(std::size_t maxFrameBytes)
        : maxFrame_(maxFrameBytes) {}

    void FrameCodec::enableCompression(std::int32_t threshold) noexcept {
        threshold_ = threshold < 0 ? -1 : threshold;
    }

    bool FrameCodec::compressionEnabled() const noexcept { return threshold_ >= 0; }

    std::int32_t FrameCodec::threshold() const noexcept { return threshold_; }

    std::size_t FrameCodec::encode(
        std::int32_t id, std::span<const std::byte> body, std::span<std::byte> out
    ) {
        const std::size_t rawSize = varIntSize(static_cast<std::uint32_t>(id)) + body.size();
        if (rawSize > maxFrame_) return 0;

        // 1) 组原始包：id + body
        scratch_.resize(rawSize);
        {
            std::byte* current   = scratch_.data();
            std::byte* const end = current + scratch_.size();
            if (!wire_write(current, end, id)) return 0;
            if (!body.empty()) std::memcpy(current, body.data(), body.size());
        }

        // 2) 决定数据段
        std::span<const std::byte> data = scratch_;
        std::int32_t dataLength         = 0;  // 0 表示未压缩

        if (compressionEnabled() && rawSize >= static_cast<std::size_t>(threshold_)) {
            compressed_.resize(compressBound(rawSize));
            uLongf produced = compressed_.size();
            if (compress2(
                    reinterpret_cast<Bytef*>(compressed_.data()), &produced,
                    reinterpret_cast<const Bytef*>(scratch_.data()), rawSize, Z_DEFAULT_COMPRESSION
                )
                != Z_OK) {
                return 0;
            }
            data       = {compressed_.data(), static_cast<std::size_t>(produced)};
            dataLength = static_cast<std::int32_t>(rawSize);
        }

        // 3) 写帧
        std::byte* current   = out.data();
        std::byte* const end = current + out.size();

        const std::size_t total =
            (compressionEnabled() ? varIntSize(static_cast<std::uint32_t>(dataLength)) : 0u)
            + data.size();
        if (total > maxFrame_) return 0;

        if (!wire_write(current, end, static_cast<std::int32_t>(total))) return 0;
        if (compressionEnabled() && !wire_write(current, end, dataLength)) return 0;

        if (static_cast<std::size_t>(end - current) < data.size()) return 0;
        if (!data.empty()) std::memcpy(current, data.data(), data.size());
        current += data.size();

        return static_cast<std::size_t>(current - out.data());
    }

    std::expected<Frame, FrameError> FrameCodec::decode(
        std::span<const std::byte> buffer, std::size_t& consumed
    ) {
        using enum FrameError;
        consumed = 0;

        const std::byte* current   = buffer.data();
        const std::byte* const end = current + buffer.size();

        const auto length = wire_read<std::int32_t>(current, end);
        if (!length)
            return std::unexpected(
                length.error() == WireError::Truncated ? Truncated : VarIntTooLong
            );

        if (*length <= 0 || static_cast<std::size_t>(*length) > maxFrame_)
            return std::unexpected(TooLarge);

        if (static_cast<std::size_t>(end - current) < static_cast<std::size_t>(*length))
            return std::unexpected(Truncated);


        const std::byte* const frameEnd = current + *length;
        const std::size_t frameBytes    = static_cast<std::size_t>(frameEnd - buffer.data());

        if (compressionEnabled()) {
            const auto dataLength = wire_read<std::int32_t>(current, frameEnd);
            if (!dataLength) return std::unexpected(InvalidCompressed);

            if (*dataLength != 0) {
                if (*dataLength < 0 || static_cast<std::size_t>(*dataLength) > maxFrame_)
                    return std::unexpected(TooLarge);

                inflate_.resize(static_cast<std::size_t>(*dataLength));
                uLongf produced = inflate_.size();
                if (uncompress(
                        reinterpret_cast<Bytef*>(inflate_.data()), &produced,
                        reinterpret_cast<const Bytef*>(current), frameEnd - current
                    ) != Z_OK
                    || produced != inflate_.size())
                    return std::unexpected(InvalidCompressed);

                const std::byte* p           = inflate_.data();
                const std::byte* const pEnd = p + inflate_.size();

                const auto id                = wire_read<std::int32_t>(p, pEnd);
                if (!id) return std::unexpected(InvalidCompressed);

                consumed = frameBytes;

                return Frame{
                    *id, {p, static_cast<std::size_t>(pEnd - p)}
                };
            }
            // dataLength == 0：未压缩
        }

        const auto id = wire_read<std::int32_t>(current, frameEnd);
        if (!id) return std::unexpected(Truncated);

        consumed = frameBytes;
        return Frame{
            *id, {current, static_cast<std::size_t>(frameEnd - current)}
        };
    }

}  // namespace minelink::protocol
