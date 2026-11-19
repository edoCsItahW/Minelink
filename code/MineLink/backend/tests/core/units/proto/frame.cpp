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
 * @date 2026/11/19 15:26
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

import std;
import protocol;
import base;

using namespace minelink;
using namespace minelink::protocol;

namespace {

    /// @brief 构造内容可控的包体。
    [[nodiscard]] std::vector<std::byte> makeBody(std::size_t size) {
        std::vector<std::byte> body(size);
        for (std::size_t i = 0; i < size; ++i) body[i] = static_cast<std::byte>(i & 0xFFu);
        return body;
    }

    /// @brief 解出编码结果里的 dataLength 字段（仅压缩启用时有意义）。
    /// @return dataLength；解析失败返回 -1
    [[nodiscard]] std::int32_t encodedDataLength(std::span<const std::byte> frame) {
        Reader reader{frame};
        const auto total = reader.read<std::int32_t>();
        if (!total) return -1;
        const auto data_length = reader.read<std::int32_t>();
        if (!data_length) return -1;
        return *data_length;
    }

    /// @brief 编解码往返：相同阈值下 id 与 body 必须原样还原。
    void expectRoundTrip(std::int32_t threshold, std::size_t body_size) {
        FrameCodec encoder{64 * 1024};
        FrameCodec decoder{64 * 1024};
        if (threshold >= 0) {
            encoder.enableCompression(threshold);
            decoder.enableCompression(threshold);
        }

        const auto body = makeBody(body_size);
        std::vector<std::byte> encoded(64 * 1024);

        const std::size_t size = encoder.encode(0x2A, body, encoded);
        ASSERT_NE(size, 0u);
        encoded.resize(size);

        std::size_t consumed = 0;
        const auto frame     = decoder.decode(encoded, consumed);

        ASSERT_TRUE(frame.has_value());
        EXPECT_EQ(frame->id, 0x2A);
        EXPECT_EQ(frame->payload.size(), body.size());
        EXPECT_TRUE(std::equal(frame->payload.begin(), frame->payload.end(), body.begin()));
        EXPECT_EQ(consumed, size);
    }

}  // namespace

TEST(FrameRoundTrip, NoCompression) {
    expectRoundTrip(-1, 0);  // 只有 id
    expectRoundTrip(-1, 1);
    expectRoundTrip(-1, 300);
}

TEST(FrameRoundTrip, CompressedAndUncompressed) {
    expectRoundTrip(64, 300);  // 超过阈值：压缩
    expectRoundTrip(64, 1);    // 低于阈值：dataLength = 0
}

// 阈值边界：rawSize = idLen(1) + bodySize
// bodySize 62 => raw 63 < 64 不压缩；63 => raw 64 == 阈值压缩；64 => raw 65 压缩
TEST(FrameCompression, ThresholdBoundary) {
    constexpr std::int32_t threshold = 64;

    FrameCodec encoder{64 * 1024};
    encoder.enableCompression(threshold);

    std::vector<std::byte> encoded(64 * 1024);

    for (const std::size_t body_size : {std::size_t{62}, std::size_t{63}, std::size_t{64}}) {
        const auto body        = makeBody(body_size);
        const std::size_t size = encoder.encode(0x00, body, encoded);
        ASSERT_NE(size, 0u);

        const auto data_length =
            encodedDataLength(std::span<const std::byte>{encoded.data(), size});
        ASSERT_GE(data_length, 0);

        if (body_size < 63)
            EXPECT_EQ(data_length, 0);  // 未压缩标记
        else
            EXPECT_EQ(data_length, static_cast<std::int32_t>(1 + body_size));  // 原始包长度
    }
}

TEST(FrameDecode, HalfFrameIsTruncated) {
    FrameCodec encoder{64 * 1024};
    std::vector<std::byte> encoded(1024);

    const auto body        = makeBody(200);
    const std::size_t size = encoder.encode(0x2A, body, encoded);
    ASSERT_GT(size, 2u);

    FrameCodec decoder{64 * 1024};
    std::size_t consumed = 123;  // 故意写成非 0，确认失败时被清零

    const auto frame =
        decoder.decode(std::span<const std::byte>{encoded.data(), size / 2}, consumed);
    ASSERT_FALSE(frame.has_value());
    EXPECT_EQ(frame.error(), FrameError::Truncated);
    EXPECT_EQ(consumed, 0u);
}

TEST(FrameDecode, LengthAboveLimitIsRejected) {
    // VarInt(1000) = 0xE8 0x07，超过 64 字节上限
    std::array header{std::byte{0xE8}, std::byte{0x07}};

    FrameCodec decoder{64};
    std::size_t consumed = 0;

    const auto frame = decoder.decode(header, consumed);
    ASSERT_FALSE(frame.has_value());
    EXPECT_EQ(frame.error(), FrameError::TooLarge);
    EXPECT_EQ(consumed, 0u);
}

TEST(FrameDecode, CorruptedCompressedStreamIsRejected) {
    FrameCodec encoder{64 * 1024};
    encoder.enableCompression(8);
    std::vector<std::byte> encoded(64 * 1024);

    const auto body        = makeBody(512);
    const std::size_t size = encoder.encode(0x2A, body, encoded);
    ASSERT_GT(size, 4u);

    // 破坏压缩段的中间字节（zlib 的 adler32 会失败）
    encoded[size / 2] = static_cast<std::byte>(std::to_integer<std::uint8_t>(encoded[size / 2]) ^ 0xFFu);

    FrameCodec decoder{64 * 1024};
    decoder.enableCompression(8);
    std::size_t consumed = 0;

    const auto frame = decoder.decode(std::span<const std::byte>{encoded.data(), size}, consumed);
    ASSERT_FALSE(frame.has_value());
    EXPECT_EQ(frame.error(), FrameError::InvalidCompressed);
}