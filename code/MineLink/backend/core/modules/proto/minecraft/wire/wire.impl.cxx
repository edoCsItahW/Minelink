// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file wire.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/14 21:44
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:wire.impl;

import :wire.decl;
import utils;

export namespace minelink::protocol {

    // Reader

    template<typename T>
    std::expected<T, WireError> wire_read(
        const std::byte*& current, const std::byte* end
    ) noexcept {
        return globalTagInvoke(wire_read_t{}, wire_type_tag_v<T>, current, end);
    }

    template<typename T>
    std::expected<T, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<T>, const std::byte*&, const std::byte*
    ) {
        static_assert(always_false_v<T>, "Minelink::protocol: 该类型未实现wire_read");

        std::unreachable();
    }

    // Writer

    template<typename T>
    bool wire_write(std::byte*& current, std::byte* end, const T& value) noexcept {
        return globalTagInvoke(wire_write_t{}, wire_type_tag_v<T>, current, end, value);
    }

    template<typename T>
    bool globalTagInvoke(wire_write_t, wire_type_tag_t<T>, std::byte*&, std::byte*, const T&) {
        static_assert(always_false_v<T>, "Minelink::protocol: 该类型未实现wire_write");

        std::unreachable();
    }

}  // namespace minelink::protocol
