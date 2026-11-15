// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file boolean.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15
 * @brief bool 编解码单元测试（vanilla 语义：零为 false、非零为 true）。
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

namespace {

/// @brief 读一个 bool 并断言成功与消费长度。
[[nodiscard]] bool expectBool(const std::vector<std::byte>& input) {
    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readBool(current, end);
    EXPECT_TRUE(got.has_value());
    if (!got) return false;

    EXPECT_EQ(current, end);
    return *got;
}

TEST(BooleanDecode, ZeroIsFalse) {
    EXPECT_FALSE(expectBool(bytes({0x00})));
}

/// @brief 非零即真：协议只约定 0/1，但实现按 vanilla 语义接受任意非零值。
TEST(BooleanDecode, NonZeroIsTrue) {
    EXPECT_TRUE(expectBool(bytes({0x01})));
    EXPECT_TRUE(expectBool(bytes({0x02})));
    EXPECT_TRUE(expectBool(bytes({0xFF})));
}

TEST(BooleanDecode, EmptyInputIsTruncated) {
    const auto input = bytes({});

    const std::byte* current   = input.data();
    const std::byte* const end = current + input.size();

    const auto got = readBool(current, end);
    ASSERT_FALSE(got.has_value());
    EXPECT_EQ(got.error(), WireError::Truncated);
    EXPECT_EQ(current, input.data());
}

TEST(BooleanWrite, WritesCanonicalByte) {
    std::array<std::byte, 2> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    ASSERT_TRUE(writeBool(current, end, true));
    ASSERT_TRUE(writeBool(current, end, false));

    EXPECT_EQ(std::to_integer<int>(buffer[0]), 1);
    EXPECT_EQ(std::to_integer<int>(buffer[1]), 0);
    EXPECT_EQ(static_cast<std::size_t>(current - buffer.data()), 2);
}

TEST(BooleanWrite, RejectsFullBuffer) {
    std::span<std::byte> nothing{};
    std::byte* current   = nothing.data();
    std::byte* const end = current + nothing.size();

    EXPECT_FALSE(writeBool(current, end, true));
    EXPECT_EQ(current, nothing.data());
}

}  // namespace
