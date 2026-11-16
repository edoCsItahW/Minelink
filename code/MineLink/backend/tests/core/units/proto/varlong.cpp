// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file varlong.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15
 * @brief VarLong 编解码单元测试（10 字节上限、有符号 64 位边界）。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

#include "support.hpp"

import std;
import protocol;
import base;

using namespace minelink;
using namespace minelink::protocol;
using minelink::test::bytes;

namespace {

/// @brief VarLong 解码用例。
struct DecodeCase {
    std::vector<std::byte> input;    ///< 输入字节
    std::int64_t value{0};           ///< 期望值，`error` 为空时有效
    std::optional<WireError> error;  ///< 期望错误；为空表示期望成功
};

class VarLongDecode : public testing::TestWithParam<DecodeCase> {};

TEST_P(VarLongDecode, DecodesOrRejects) {
    const auto& param = GetParam();

    const std::byte* current   = param.input.data();
    const std::byte* const end = current + param.input.size();

    const auto got = wire_read<std::int64_t>(current, end);

    if (!param.error) {
        ASSERT_TRUE(got.has_value());
        EXPECT_EQ(*got, param.value);
        EXPECT_EQ(current, end);
    } else {
        ASSERT_FALSE(got.has_value());
        EXPECT_EQ(got.error(), *param.error);
        EXPECT_EQ(current, param.input.data());
    }
}

INSTANTIATE_TEST_SUITE_P(
    Protocol, VarLongDecode,
    testing::Values(
        // 单/双字节等价类
        DecodeCase{bytes({0x00}), 0, std::nullopt},
        DecodeCase{bytes({0x7F}), 127, std::nullopt},
        DecodeCase{bytes({0x80, 0x01}), 128, std::nullopt},
        // 六字节：2^35，跨越 32 位边界
        DecodeCase{bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x01}), 34359738368LL, std::nullopt},
        // 九字节上限：INT64_MAX（63 位有效）
        DecodeCase{
            bytes({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F}),
            9223372036854775807LL, std::nullopt},
        // 十字节上限：全 64 位有效
        DecodeCase{
            bytes({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01}),
            -1, std::nullopt},
        DecodeCase{
            bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01}),
            -9223372036854775807LL - 1, std::nullopt},
        // 非最小编码
        DecodeCase{bytes({0x80, 0x00}), 0, std::nullopt},
        // 截断
        DecodeCase{bytes({}), 0, WireError::Truncated},
        DecodeCase{bytes({0x80}), 0, WireError::Truncated},
        // 溢出：第 10 字节仅低 1 位有效
        DecodeCase{
            bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x02}),
            0, WireError::VarIntOverflow},
        // 超长：第 10 字节仍带续位
        DecodeCase{
            bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01}),
            0, WireError::VarIntTooLong}
    )
);

/// @brief VarLong 编码用例。
struct EncodeCase {
    std::int64_t value;              ///< 待编码值
    std::vector<std::byte> expected; ///< 期望字节
};

class VarLongEncode : public testing::TestWithParam<EncodeCase> {};

TEST_P(VarLongEncode, WritesBytesAndRespectsCapacity) {
    const auto& param = GetParam();

    std::array<std::byte, 10> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    ASSERT_TRUE(writeVarLong(current, end, param.value));
    EXPECT_EQ(static_cast<std::size_t>(current - buffer.data()), param.expected.size());
    EXPECT_TRUE(std::equal(param.expected.begin(), param.expected.end(), buffer.begin()));

    // 容量边界：正好放下成功，少 1 字节失败且不推进游标
    std::array<std::byte, 10> exact{};
    std::byte* c1 = exact.data();
    ASSERT_TRUE(writeVarLong(c1, c1 + param.expected.size(), param.value));

    std::byte* c2 = exact.data();
    EXPECT_FALSE(writeVarLong(c2, c2 + param.expected.size() - 1, param.value));
    EXPECT_EQ(c2, exact.data());
}

INSTANTIATE_TEST_SUITE_P(
    Protocol, VarLongEncode,
    testing::Values(
        EncodeCase{0, bytes({0x00})},
        EncodeCase{127, bytes({0x7F})},
        EncodeCase{128, bytes({0x80, 0x01})},
        EncodeCase{34359738368LL, bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x01})},
        EncodeCase{
            9223372036854775807LL,
            bytes({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F})},
        EncodeCase{-1, bytes({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01})},
        EncodeCase{
            -9223372036854775807LL - 1,
            bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x01})}
    )
);

}  // namespace
