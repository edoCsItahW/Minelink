// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file numeric.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 22:30
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:numeric.decl;

import std;
import :wire;

export namespace minelink::protocol {

    // read

    /// uint

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    std::expected<T, WireError> readUInt(const std::byte*& current, const std::byte* end) noexcept;

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    std::expected<T, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<T>, const std::byte*& current, const std::byte* end
    ) noexcept;

    /// float

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    std::expected<T, WireError> readFloat(const std::byte*& current, const std::byte* end) noexcept;

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    std::expected<T, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<T>, const std::byte*& current, const std::byte* end
    ) noexcept;

    /// long
    [[nodiscard]] std::expected<std::int64_t, WireError> readLong(const std::byte*& current, const std::byte* end) noexcept;

    // write

    /// uint

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    bool writeUInt(std::byte*& current, std::byte* end, T value) noexcept;

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<T>, std::byte*& current, std::byte* end, const T& v
    ) noexcept;

    /// float

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    bool writeFloat(std::byte*& current, std::byte* end, T v) noexcept;

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<T>, std::byte*& current, std::byte* end, const T& v
    ) noexcept;

    /// long
    bool writeLong(std::byte*& current, std::byte* end, std::int64_t value) noexcept;

}  // namespace minelink::protocol
