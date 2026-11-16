// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file support.hpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15
 * @brief 协议原语单元测试共用的小工具。
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#pragma once

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace minelink::test {

/// @brief 把字节字面量列表构造为 std::vector<std::byte>。
/// @param init 每个元素以 unsigned 书写，函数内做一次显式转换。
/// @return 长度等于 init 元素个数的字节序列。
/// @note 不能直接写 std::vector<std::byte>{0x00}：int 无法隐式转换为 std::byte，
///       列表初始化会退化为 (size_type) 构造函数，得到 0 个元素。
[[nodiscard]] inline std::vector<std::byte> bytes(std::initializer_list<unsigned> init) {
    std::vector<std::byte> out;
    out.reserve(init.size());
    for (auto b : init) out.push_back(static_cast<std::byte>(b));
    return out;
}

/// @brief 构造 "VarInt 长度前缀 + 内容" 形式的字节序列。
/// @param prefix 长度前缀的原始字节
/// @param body 内容字节
[[nodiscard]] inline std::vector<std::byte> framed(
    std::initializer_list<unsigned> prefix, std::initializer_list<unsigned> body
) {
    std::vector<std::byte> out;
    out.reserve(prefix.size() + body.size());
    for (auto b : prefix) out.push_back(static_cast<std::byte>(b));
    for (auto b : body) out.push_back(static_cast<std::byte>(b));
    return out;
}

}  // namespace minelink::test
