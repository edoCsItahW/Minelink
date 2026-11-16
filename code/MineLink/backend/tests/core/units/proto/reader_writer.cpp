// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file reader_writer.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15
 * @brief Reader / Writer 的游标与容量契约测试（不重复各类型的编解码细节）。
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

TEST(ReaderBehaviour, EmptinessReflectsCursor) {
    std::array<std::byte, 1> raw{std::byte{0x01}};

    std::span<const std::byte> nothing{};
    Reader empty{nothing};
    EXPECT_TRUE(empty.empty());

    Reader one{raw};
    EXPECT_FALSE(one.empty());

    const auto got = one.read<std::int32_t>();
    ASSERT_TRUE(got.has_value());
    EXPECT_EQ(*got, 1);
    EXPECT_TRUE(one.empty());
}

TEST(ReaderBehaviour, ConsumesValuesInOrder) {
    const auto data = bytes({0x01, 0x02});
    Reader reader{data};

    const auto first = reader.read<std::int32_t>();
    ASSERT_TRUE(first.has_value());
    EXPECT_EQ(*first, 1);

    const auto second = reader.read<std::int32_t>();
    ASSERT_TRUE(second.has_value());
    EXPECT_EQ(*second, 2);

    EXPECT_TRUE(reader.empty());
}

/// @brief 失败读取不得移动游标：同一字节应被反复看到。
TEST(ReaderBehaviour, DoesNotAdvanceWhenReadFails) {
    const auto data = bytes({0x80});
    Reader reader{data};

    EXPECT_FALSE(reader.read<std::int32_t>().has_value());
    EXPECT_FALSE(reader.empty());

    EXPECT_FALSE(reader.read<std::int32_t>().has_value());
    EXPECT_FALSE(reader.empty());
}

TEST(ReaderBehaviour, MixesStringAndIntegral) {
    const auto data = bytes({0x02, 'h', 'i', 0x2A});
    Reader reader{data};

    const auto text = reader.read<std::string_view>();
    ASSERT_TRUE(text.has_value());
    EXPECT_EQ(*text, "hi");

    // 字符串视图必须别名输入缓冲，而不是拷贝
    EXPECT_EQ(reinterpret_cast<const std::byte*>(text->data()), data.data() + 1);

    const auto number = reader.read<std::int32_t>();
    ASSERT_TRUE(number.has_value());
    EXPECT_EQ(*number, 42);

    EXPECT_TRUE(reader.empty());
}

TEST(WriterBehaviour, WritesVarIntIntoBuffer) {
    std::array<std::byte, 4> buffer{};
    Writer writer{buffer};

    ASSERT_TRUE(writer.write(std::int32_t{300}));
    EXPECT_EQ(std::to_integer<int>(buffer[0]), 0xAC);
    EXPECT_EQ(std::to_integer<int>(buffer[1]), 0x02);
}

TEST(WriterBehaviour, SequentialWritesAdvanceCursor) {
    std::array<std::byte, 4> buffer{};
    Writer writer{buffer};

    ASSERT_TRUE(writer.write(std::int32_t{1}));
    ASSERT_TRUE(writer.write(std::int32_t{2}));

    EXPECT_EQ(std::to_integer<int>(buffer[0]), 1);
    EXPECT_EQ(std::to_integer<int>(buffer[1]), 2);
}

TEST(WriterBehaviour, RejectsWhenBufferTooSmall) {
    std::array<std::byte, 1> buffer{};
    Writer writer{buffer};

    EXPECT_FALSE(writer.write(std::int32_t{300}));  // 需要 2 字节
}

TEST(WriterBehaviour, RejectsEmptyBuffer) {
    std::span<std::byte> nothing{};
    Writer writer{nothing};

    EXPECT_FALSE(writer.write(std::int32_t{0}));
}

TEST(WriterBehaviour, DispatchesNonIntegralTypes) {
    std::array<std::byte, 4> buffer{};
    Writer writer{buffer};

    ASSERT_TRUE(writer.write(true));
    EXPECT_EQ(std::to_integer<int>(buffer[0]), 1);
}

}  // namespace
