// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file numeric.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 22:41
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:numeric.impl;

import :numeric.decl;

export namespace minelink::protocol {

    // read

    /// uint

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    std::expected<T, WireError> readUInt(const std::byte*& current, const std::byte* end) noexcept {
        constexpr auto N = sizeof(T);

        if (static_cast<std::size_t>(end - current) < N)
            return std::unexpected(WireError::Truncated);

        T v{};
        for (std::size_t i = 0; i < N; ++i) {
            v = static_cast<T>(v << 8);
            v = static_cast<T>(v | std::to_integer<std::uint8_t>(current[i]));
        }

        current += N;
        return v;
    }

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    std::expected<T, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<T>, const std::byte*& current, const std::byte* end
    ) noexcept {
        return readUInt<T>(current, end);
    }

    /// float

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    std::expected<T, WireError> readFloat(
        const std::byte*& current, const std::byte* end
    ) noexcept {
        using U = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;

        auto raw = readUInt<U>(current, end);

        if (!raw) return std::unexpected(raw.error());

        return std::bit_cast<T>(*raw);
    }

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    std::expected<T, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<T>, const std::byte*& current, const std::byte* end
    ) noexcept {
        return readFloat<T>(current, end);
    }

    /// long

    [[nodiscard]] std::expected<std::int64_t, WireError> readLong(
        const std::byte*& current, const std::byte* end
    ) noexcept {
        return readUInt<std::uint64_t>(current, end);
    }

    // write

    /// uint

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    bool writeUInt(std::byte*& current, std::byte* end, T value) noexcept {
        constexpr auto N = sizeof(T);

        if (static_cast<std::size_t>(end - current) < N) return false;

        for (std::size_t i = 0; i < N; ++i)
            current[i] =
                static_cast<std::byte>(static_cast<std::uint8_t>(value >> (8 * (N - 1 - i))));

        current += N;

        return true;
    }

    template<std::unsigned_integral T>
        requires(sizeof(T) >= 1 && sizeof(T) <= 8)
    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<T>, std::byte*& current, std::byte* end, const T& v
    ) noexcept {
        return writeUInt<T>(current, end, v);
    }

    /// float

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    bool writeFloat(std::byte*& current, std::byte* end, T v) noexcept {
        using U = std::conditional_t<sizeof(T) == 4, std::uint32_t, std::uint64_t>;

        return writeUInt<U>(current, end, std::bit_cast<U>(v));
    }

    template<std::floating_point T>
        requires(sizeof(T) == 4 || sizeof(T) == 8)
    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<T>, std::byte*& current, std::byte* end, const T& v
    ) noexcept {
        return writeFloat<T>(current, end, v);
    }

    /// long

    bool writeLong(std::byte*& current, std::byte* end, std::int64_t value) noexcept {
        return writeUInt<std::uint64_t>(current, end, std::bit_cast<std::uint64_t>(value));
    }

}  // namespace minelink::protocol
