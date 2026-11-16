// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file varint.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15
 * @brief VarInt 编解码单元测试（等价类 + 边界值 + 游标语义）。
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

/// @brief VarInt 解码用例。
struct DecodeCase {
    std::vector<std::byte> input;    ///< 输入字节
    std::int32_t value{0};           ///< 期望值，`error` 为空时有效
    std::optional<WireError> error;  ///< 期望错误；为空表示期望成功
};

/// @brief 覆盖 1..5 字节各等价类、非最小编码、截断、超长与溢出。
class VarIntDecode : public testing::TestWithParam<DecodeCase> {};

TEST_P(VarIntDecode, DecodesOrRejects) {
    const auto& param = GetParam();

    const std::byte* current      = param.input.data();
    const std::byte* const end    = current + param.input.size();

    const auto got = wire_read<std::int32_t>(current, end);

    if (!param.error) {
        ASSERT_TRUE(got.has_value());
        EXPECT_EQ(*got, param.value);
        EXPECT_EQ(current, end);  // 成功必须精确消费
    } else {
        ASSERT_FALSE(got.has_value());
        EXPECT_EQ(got.error(), *param.error);
        EXPECT_EQ(current, param.input.data());  // 失败不得推进游标
    }
}

INSTANTIATE_TEST_SUITE_P(
    Protocol, VarIntDecode,
    testing::Values(
        // 等价类 1：单字节 [0, 127]
        DecodeCase{bytes({0x00}), 0, std::nullopt},
        DecodeCase{bytes({0x7F}), 127, std::nullopt},
        // 等价类 2：双字节 [128, 16383]
        DecodeCase{bytes({0x80, 0x01}), 128, std::nullopt},
        DecodeCase{bytes({0xFF, 0x7F}), 16383, std::nullopt},
        // 等价类 3：三字节 [16384, 2097151]
        DecodeCase{bytes({0x80, 0x80, 0x01}), 16384, std::nullopt},
        DecodeCase{bytes({0xFF, 0xFF, 0x7F}), 2097151, std::nullopt},
        // 等价类 4：四字节 [2097152, 268435455]
        DecodeCase{bytes({0x80, 0x80, 0x80, 0x01}), 2097152, std::nullopt},
        DecodeCase{bytes({0xFF, 0xFF, 0xFF, 0x7F}), 268435455, std::nullopt},
        // 等价类 5：五字节，末字节仅低 4 位有效
        DecodeCase{bytes({0x80, 0x80, 0x80, 0x80, 0x01}), 268435456, std::nullopt},
        DecodeCase{bytes({0xFF, 0xFF, 0xFF, 0xFF, 0x07}), 2147483647, std::nullopt},
        DecodeCase{bytes({0x80, 0x80, 0x80, 0x80, 0x08}), -2147483647 - 1, std::nullopt},
        DecodeCase{bytes({0xFF, 0xFF, 0xFF, 0xFF, 0x0F}), -1, std::nullopt},
        // 非最小编码：vanilla 宽容接受
        DecodeCase{bytes({0x80, 0x00}), 0, std::nullopt},
        // 截断：按上下文长度分 1 例，避免逐长度重复
        DecodeCase{bytes({}), 0, WireError::Truncated},
        DecodeCase{bytes({0x80}), 0, WireError::Truncated},
        DecodeCase{bytes({0x80, 0x80, 0x80, 0x80}), 0, WireError::Truncated},
        // 溢出：末字节 0x10 超出 32 位
        DecodeCase{bytes({0x80, 0x80, 0x80, 0x80, 0x10}), 0, WireError::VarIntOverflow},
        // 超长：第 6 字节仍带续位
        DecodeCase{bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x01}), 0, WireError::VarIntTooLong}
    )
);

/// @brief VarInt 编码用例。
struct EncodeCase {
    std::int32_t value;              ///< 待编码值
    std::vector<std::byte> expected; ///< 期望字节
};

class VarIntEncode : public testing::TestWithParam<EncodeCase> {};

TEST_P(VarIntEncode, WritesBytesAndRespectsCapacity) {
    const auto& param = GetParam();

    std::array<std::byte, 5> buffer{};
    std::byte* current      = buffer.data();
    std::byte* const end    = current + buffer.size();

    ASSERT_TRUE(writeVarInt(current, end, param.value));
    EXPECT_EQ(static_cast<std::size_t>(current - buffer.data()), param.expected.size());
    EXPECT_EQ(varIntSize(static_cast<std::uint32_t>(param.value)), param.expected.size());
    EXPECT_TRUE(std::equal(param.expected.begin(), param.expected.end(), buffer.begin()));

    // 容量边界：正好放下时成功
    std::array<std::byte, 5> exact{};
    std::byte* c1 = exact.data();
    ASSERT_TRUE(writeVarInt(c1, c1 + param.expected.size(), param.value));

    // 容量边界：少 1 字节必须失败且游标不变
    if (param.expected.size() > 1) {
        std::byte* c2 = exact.data();
        EXPECT_FALSE(writeVarInt(c2, c2 + param.expected.size() - 1, param.value));
        EXPECT_EQ(c2, exact.data());
    }
}

INSTANTIATE_TEST_SUITE_P(
    Protocol, VarIntEncode,
    testing::Values(
        EncodeCase{0, bytes({0x00})},
        EncodeCase{127, bytes({0x7F})},
        EncodeCase{128, bytes({0x80, 0x01})},
        EncodeCase{16383, bytes({0xFF, 0x7F})},
        EncodeCase{16384, bytes({0x80, 0x80, 0x01})},
        EncodeCase{2097151, bytes({0xFF, 0xFF, 0x7F})},
        EncodeCase{2097152, bytes({0x80, 0x80, 0x80, 0x01})},
        EncodeCase{268435455, bytes({0xFF, 0xFF, 0xFF, 0x7F})},
        EncodeCase{268435456, bytes({0x80, 0x80, 0x80, 0x80, 0x01})},
        EncodeCase{2147483647, bytes({0xFF, 0xFF, 0xFF, 0xFF, 0x07})},
        EncodeCase{-2147483647 - 1, bytes({0x80, 0x80, 0x80, 0x80, 0x08})},
        EncodeCase{-1, bytes({0xFF, 0xFF, 0xFF, 0xFF, 0x0F})}
    )
);

/// @brief 编码长度与解码消费长度必须一致（防止两条路径各自漂移）。
TEST(VarInt, SizeMatchesDecodedConsumption) {
    for (const auto value : {0, 127, 128, 16383, 16384, 2097151, 2097152, -1}) {
        std::array<std::byte, 5> buffer{};
        std::byte* out   = buffer.data();
        std::byte* const outEnd = out + buffer.size();
        ASSERT_TRUE(writeVarInt(out, outEnd, value));

        const std::byte* in  = buffer.data();
        const std::byte* const inEnd = in + buffer.size();
        const auto got = wire_read<std::int32_t>(in, inEnd);

        ASSERT_TRUE(got.has_value());
        EXPECT_EQ(*got, value);
        EXPECT_EQ(static_cast<std::size_t>(in - buffer.data()), varIntSize(static_cast<std::uint32_t>(value)));
    }
}

}  // namespace
