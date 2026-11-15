// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file numeric.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15
 * @brief 定长大端整数与浮点编解码单元测试。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * @note 本文件实例化 readUInt / writeUInt / readFloat / writeFloat 模板，
 *       需要 protocol 导出 :numeric.impl（模板定义必须对实例化点可达），
 *       即 types.cxx 中应有 `export import :numeric.impl;`。
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

/// @brief 读无符号整数并断言值；游标语义由 AdvancesCursor 用例单独覆盖。
template<std::unsigned_integral T>
void expectUIntDecode(std::initializer_list<unsigned> raw, T expected) {
    const auto data = bytes(raw);

    const std::byte* current   = data.data();
    const std::byte* const end = current + data.size();

    const auto got = readUInt<T>(current, end);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(*got, expected);
}

/// @brief 少 1 字节：必须报 Truncated 且不推进游标。
template<std::unsigned_integral T>
void expectUIntTruncated() {
    const std::vector<std::byte> data(sizeof(T) - 1, std::byte{0});

    const std::byte* current   = data.data();
    const std::byte* const end = current + data.size();

    const auto got = readUInt<T>(current, end);
    ASSERT_FALSE(got.has_value());
    EXPECT_EQ(got.error(), WireError::Truncated);
    EXPECT_EQ(current, data.data());
}

/// @brief 读浮点并按位比较（可覆盖 -0.0 / NaN）。
template<std::floating_point T>
void expectFloatDecode(std::initializer_list<unsigned> raw, T expected) {
    const auto data = bytes(raw);

    const std::byte* current   = data.data();
    const std::byte* const end = current + data.size();

    const auto got = readFloat<T>(current, end);
    ASSERT_TRUE(got.has_value());

    // 逐位比较：-0.0 与 NaN 用数值比较会得到错误结论
    using Bits = std::array<std::byte, sizeof(T)>;

    const auto actual = std::bit_cast<Bits>(*got);
    const auto wanted = std::bit_cast<Bits>(expected);

    EXPECT_EQ(actual, wanted);
}

}  // namespace

TEST(UIntDecode, WidthsAndBoundaries) {
    expectUIntDecode<std::uint8_t>({0x00}, 0);
    expectUIntDecode<std::uint8_t>({0xFF}, 0xFF);

    expectUIntDecode<std::uint16_t>({0x00, 0x00}, 0);
    expectUIntDecode<std::uint16_t>({0x12, 0x34}, 0x1234);
    expectUIntDecode<std::uint16_t>({0xFF, 0xFF}, 0xFFFF);

    expectUIntDecode<std::uint32_t>({0xDE, 0xAD, 0xBE, 0xEF}, 0xDEADBEEFu);
    expectUIntDecode<std::uint32_t>({0xFF, 0xFF, 0xFF, 0xFF}, 0xFFFFFFFFu);

    expectUIntDecode<std::uint64_t>(
        {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}, 0x0123456789ABCDEFull
    );
    expectUIntDecode<std::uint64_t>(
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}, 0xFFFFFFFFFFFFFFFFull
    );
}

TEST(UIntDecode, TruncatedInputIsRejected) {
    expectUIntTruncated<std::uint8_t>();
    expectUIntTruncated<std::uint16_t>();
    expectUIntTruncated<std::uint32_t>();
    expectUIntTruncated<std::uint64_t>();
}

TEST(UIntDecode, AdvancesCursor) {
    const auto data = bytes({0x12, 0x34});

    const std::byte* current   = data.data();
    const std::byte* const end = current + data.size();

    const auto got = readUInt<std::uint16_t>(current, end);
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(current - data.data(), 2);
}

TEST(UIntWrite, WritesBigEndian) {
    std::array<std::byte, 2> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    ASSERT_TRUE(writeUInt<std::uint16_t>(current, end, 0x1234));
    EXPECT_EQ(std::to_integer<int>(buffer[0]), 0x12);
    EXPECT_EQ(std::to_integer<int>(buffer[1]), 0x34);
    EXPECT_EQ(current - buffer.data(), 2);
}

TEST(UIntWrite, InsufficientCapacityIsRejected) {
    std::array<std::byte, 3> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    EXPECT_FALSE(writeUInt<std::uint32_t>(current, end, 0xDEADBEEFu));
    EXPECT_EQ(current, buffer.data());
}

TEST(FloatDecode, KnownBitPatterns) {
    expectFloatDecode<float>({0x3F, 0x80, 0x00, 0x00}, 1.0f);
    expectFloatDecode<float>({0x80, 0x00, 0x00, 0x00}, -0.0f);

    expectFloatDecode<double>({0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, 1.0);
    expectFloatDecode<double>({0xC0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, -2.5);
}

TEST(FloatDecode, TruncatedInputIsRejected) {
    const auto f32 = bytes({0x3F, 0x80, 0x00});
    const std::byte* fc = f32.data();
    const auto f32Got   = readFloat<float>(fc, fc + f32.size());
    ASSERT_FALSE(f32Got.has_value());
    EXPECT_EQ(f32Got.error(), WireError::Truncated);

    const auto f64 = bytes({0x3F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00});
    const std::byte* dc = f64.data();
    const auto f64Got   = readFloat<double>(dc, dc + f64.size());
    ASSERT_FALSE(f64Got.has_value());
    EXPECT_EQ(f64Got.error(), WireError::Truncated);
}

TEST(FloatWrite, WritesBigEndian) {
    std::array<std::byte, 12> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    ASSERT_TRUE(writeFloat(current, end, 1.0f));
    EXPECT_EQ(current - buffer.data(), 4);
    EXPECT_EQ(std::to_integer<int>(buffer[0]), 0x3F);
    EXPECT_EQ(std::to_integer<int>(buffer[1]), 0x80);

    ASSERT_TRUE(writeFloat(current, end, -2.5));
    EXPECT_EQ(current - buffer.data(), 12);
    EXPECT_EQ(std::to_integer<int>(buffer[4]), 0xC0);
    EXPECT_EQ(std::to_integer<int>(buffer[5]), 0x04);
}

TEST(FloatWrite, InsufficientCapacityIsRejected) {
    std::array<std::byte, 3> buffer{};
    std::byte* current   = buffer.data();
    std::byte* const end = current + buffer.size();

    EXPECT_FALSE(writeFloat(current, end, 1.0f));
    EXPECT_EQ(current, buffer.data());
}

TEST(NumericDispatch, WriterWritesUnsignedIntegers) {
    std::array<std::byte, 15> buffer{};
    Writer writer{buffer};

    ASSERT_TRUE(writer.write(std::uint8_t{0xAB}));
    ASSERT_TRUE(writer.write(std::uint16_t{0x1234}));
    ASSERT_TRUE(writer.write(std::uint32_t{0xDEADBEEFu}));
    ASSERT_TRUE(writer.write(std::uint64_t{0x0123456789ABCDEFull}));

    const auto expected = bytes(
        {0xAB, 0x12, 0x34, 0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}
    );

    const std::vector<std::byte> written(buffer.begin(), buffer.begin() + expected.size());
    EXPECT_EQ(written, expected);
}

TEST(NumericDispatch, WriterWritesFloatingPoint) {
    std::array<std::byte, 12> buffer{};
    Writer writer{buffer};

    ASSERT_TRUE(writer.write(1.0f));
    ASSERT_TRUE(writer.write(-2.5));

    const auto expected = bytes(
        {0x3F, 0x80, 0x00, 0x00, 0xC0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    );

    const std::vector<std::byte> written(buffer.begin(), buffer.begin() + expected.size());
    EXPECT_EQ(written, expected);
}

TEST(NumericDispatch, ReaderReadsUnsignedIntegers) {
    const auto input = bytes({0xAB, 0x12, 0x34, 0xDE, 0xAD, 0xBE, 0xEF});
    Reader reader{input};

    const auto u8 = reader.read<std::uint8_t>();
    ASSERT_TRUE(u8.has_value());
    EXPECT_EQ(*u8, 0xAB);

    const auto u16 = reader.read<std::uint16_t>();
    ASSERT_TRUE(u16.has_value());
    EXPECT_EQ(*u16, 0x1234);

    const auto u32 = reader.read<std::uint32_t>();
    ASSERT_TRUE(u32.has_value());
    EXPECT_EQ(*u32, 0xDEADBEEFu);

    EXPECT_TRUE(reader.empty());
}

TEST(NumericDispatch, ReaderReadsFloatingPoint) {
    const auto input = bytes({0x3F, 0x80, 0x00, 0x00});
    Reader reader{input};

    const auto value = reader.read<float>();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(std::bit_cast<std::uint32_t>(*value), 0x3F800000u);
    EXPECT_TRUE(reader.empty());
}
