// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file string.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15
 * @brief 字符串编解码单元测试（长度上限、UTF-8 校验、容量边界）。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include "support.hpp"
#include <gtest/gtest.h>

import std;
import protocol;
import base;

using namespace minelink;
using namespace minelink::protocol;
using minelink::test::bytes;
using minelink::test::framed;

namespace {

/// @brief 读一个字符串并断言成功，返回视图。
[[nodiscard]] std::string_view expectString(const std::vector<std::byte>& input) {
    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readString(current, end);
    EXPECT_TRUE(got.has_value());
    if (!got) return {};

    EXPECT_EQ(current, end);  // 成功必须精确消费
    return *got;
}

TEST(StringDecode, EmptyPayloadIsAccepted) {
    EXPECT_TRUE(expectString(bytes({0x00})).empty());
}

TEST(StringDecode, AsciiPayloadIsAccepted) {
    EXPECT_EQ(expectString(bytes({0x03, 'a', 'b', 'c'})), "abc");
}

/// @brief 长度上限 32767 字节：合法上边界。
TEST(StringDecode, MaxLengthIsAccepted) {
    std::vector<std::byte> input;
    input.reserve(3 + MAX_STRING_BYTES);
    for (auto b : {0xFFu, 0xFFu, 0x01u}) input.push_back(static_cast<std::byte>(b));  // VarInt 32767
    input.resize(3 + MAX_STRING_BYTES, std::byte{'a'});

    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readString(current, end);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(got->size(), MAX_STRING_BYTES);
    EXPECT_EQ(current, end);
}

/// @brief 上限 +1：声明长度即被拒绝，无需真的提供内容。
TEST(StringDecode, LengthAboveLimitIsRejected) {
    const auto input = bytes({0x80, 0x80, 0x02});  // VarInt 32768

    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readString(current, end);
    ASSERT_FALSE(got.has_value());
    EXPECT_EQ(got.error(), WireError::StringTooLong);
}

/// @brief 负长度（VarInt -1）：同样按超限处理。
TEST(StringDecode, NegativeLengthIsRejected) {
    const auto input = bytes({0xFF, 0xFF, 0xFF, 0xFF, 0x0F});

    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readString(current, end);
    ASSERT_FALSE(got.has_value());
    EXPECT_EQ(got.error(), WireError::StringTooLong);
}

/// @brief 声明长度大于剩余字节。
TEST(StringDecode, TruncatedBodyIsRejected) {
    const auto input = bytes({0x05, 'a', 'b'});

    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readString(current, end);
    ASSERT_FALSE(got.has_value());
    EXPECT_EQ(got.error(), WireError::Truncated);
}

/// @brief 4 类非法 UTF-8：孤立续字节、超长编码、代理区、超出 U+10FFFF、截断多字节序列。
class StringInvalidUtf8 : public testing::TestWithParam<std::vector<std::byte>> {};

TEST_P(StringInvalidUtf8, IsRejected) {
    const auto& input = GetParam();

    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readString(current, end);
    ASSERT_FALSE(got.has_value());
    EXPECT_EQ(got.error(), WireError::InvalidUtf8);
}

INSTANTIATE_TEST_SUITE_P(
    Protocol, StringInvalidUtf8,
    testing::Values(
        framed({0x01}, {0x80}),                   // 孤立续字节
        framed({0x02}, {0xC0, 0x80}),             // 超长编码 U+0000
        framed({0x03}, {0xED, 0xA0, 0x80}),       // 代理区 U+D800
        framed({0x04}, {0xF4, 0x90, 0x80, 0x80}), // 超出 U+10FFFF
        framed({0x02}, {0xE4, 0xB8})              // 三字节序列被截断
    )
);

/// @brief 合法多字节：2 字节（中）与 4 字节（U+1F600）。
TEST(StringDecode, ValidMultibyteIsAccepted) {
    EXPECT_EQ(expectString(framed({0x03}, {0xE4, 0xB8, 0xAD})), "\xE4\xB8\xAD");
    EXPECT_EQ(expectString(framed({0x04}, {0xF0, 0x9F, 0x98, 0x80})), "\xF0\x9F\x98\x80");
}

/// @brief 关闭校验后同一负载应被接受（性能路径保留给可信来源）。
TEST(StringDecode, ValidationCanBeDisabled) {
    const auto input = framed({0x01}, {0x80});

    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readString(current, end, /*validateUtf8=*/false);
    ASSERT_TRUE(got.has_value());
}

TEST(StringWrite, WritesLengthPrefixAndBody) {
    std::array<std::byte, 4> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    ASSERT_TRUE(writeString(current, end, std::string_view{"ab"}));
    EXPECT_EQ(static_cast<std::size_t>(current - buffer.data()), 3);
    EXPECT_EQ(std::to_integer<int>(buffer[0]), 2);
    EXPECT_EQ(std::to_integer<int>(buffer[1]), 'a');
    EXPECT_EQ(std::to_integer<int>(buffer[2]), 'b');
}

TEST(StringWrite, EmptyStringWritesZeroLength) {
    std::array<std::byte, 1> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    ASSERT_TRUE(writeString(current, end, std::string_view{}));
    EXPECT_EQ(std::to_integer<int>(buffer[0]), 0);
    EXPECT_EQ(static_cast<std::size_t>(current - buffer.data()), 1);
}

/// @brief 容量少 1 字节：失败且不写入任何字节。
TEST(StringWrite, InsufficientCapacityIsRejected) {
    std::array<std::byte, 2> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    EXPECT_FALSE(writeString(current, end, std::string_view{"ab"}));
    EXPECT_EQ(current, buffer.data());
}

/// @brief 超过 32767 字节：即使缓冲足够也必须拒绝。
TEST(StringWrite, OversizedValueIsRejected) {
    const std::string value(MAX_STRING_BYTES + 1, 'a');
    std::vector<std::byte> buffer(MAX_STRING_BYTES + 8);

    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    EXPECT_FALSE(writeString(current, end, std::string_view{value}));
}

}  // namespace
