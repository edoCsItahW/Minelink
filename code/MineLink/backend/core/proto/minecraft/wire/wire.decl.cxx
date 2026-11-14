// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file wire.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/14 21:44
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:wire.decl;

import std;
import base;

export namespace minelink::protocol {

    template<typename>
    struct wire_type_tag_t {};

    template<typename T>
    inline constexpr wire_type_tag_t<T> wire_type_tag_v{};

    // Reader

    struct wire_read_t {};

    template<typename T>
    auto wire_read(const std::byte*& current, const std::byte* end) noexcept
        -> std::expected<T, WireError>;

    // Writer

    struct wire_write_t {};

    template<typename T>
    bool wire_write(std::byte*& current, std::byte* end, const T& value) noexcept;

    // globalTagInvoke


    template<typename T>
    auto globalTagInvoke(
        wire_read_t, wire_type_tag_t<T>, const std::byte*& current, const std::byte* end
    ) -> std::expected<T, WireError>;

    template<typename T>
    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<T>, std::byte*& current, std::byte* end, const T& value
    );

}  // namespace minelink::protocol
