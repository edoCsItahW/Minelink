// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file protocol.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 17:44
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

import std;
import protocol;
import base;

using namespace minelink;
using namespace protocol;

namespace {

    [[nodiscard]] std::vector<std::byte> Bytes(std::initializer_list<unsigned> init) {
        std::vector<std::byte> out;
        out.reserve(init.size());

        for (auto b : init) out.push_back(static_cast<std::byte>(b));

        return out;
    }

    struct VarIntCase {
        std::vector<std::byte> bytes;
        std::int32_t value;
        std::optional<WireError> error;
    };

    class VarIntDecode : public testing::TestWithParam<VarIntCase> {};

    TEST_P(VarIntDecode, DecodesOrRejects) {
        const auto& param = GetParam();

        Reader reader(param.bytes);

        const auto got = reader.read<std::int32_t>();

        if (param.error == std::nullopt) {
            ASSERT_TRUE(got.has_value());
            EXPECT_EQ(*got, param.value);
            EXPECT_TRUE(reader.empty());
        } else {
            ASSERT_FALSE(got.has_value());
            EXPECT_EQ(got.error(), param.error);
        }
    }

    INSTANTIATE_TEST_SUITE_P(
        Protocol, VarIntDecode,
        testing::Values(
            VarIntCase{Bytes({0x00}), 0, std::nullopt},
            VarIntCase{Bytes({0x7F}), 127, std::nullopt},
            VarIntCase{Bytes({0x80, 0x01}), 128, std::nullopt},
            VarIntCase{Bytes({0xFF, 0x7F}), 16383, std::nullopt},
            VarIntCase{Bytes({0xFF, 0xFF, 0xFF, 0xFF, 0x0F}), -1, std::nullopt},
            VarIntCase{Bytes({0x80, 0x80, 0x80, 0x80, 0x10}), 0, WireError::VarIntOverflow},
            VarIntCase{Bytes({}), 0, WireError::Truncated},  // 空输入
            VarIntCase{Bytes({0x80}), 0, WireError::Truncated},
            VarIntCase{Bytes({0x80, 0x80, 0x80, 0x80, 0x80, 0x01}), 0, WireError::VarIntTooLong}
        )
    );

}  // namespace
