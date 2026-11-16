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
 * @date 2026/11/15 20:39
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module protocol;

import :varlong.decl;

namespace minelink::protocol {

    namespace {

        [[nodiscard]] constexpr std::size_t varLongSize(std::uint64_t v) noexcept {
            return (static_cast<std::size_t>(std::bit_width(v | 1ull)) + 6u) / 7u;
        }

    }  // namespace

    // read

    std::expected<std::int64_t, WireError> readVarLong(
        const std::byte*& current, const std::byte* end
    ) noexcept {
        using enum WireError;

        const std::byte* pointer = current;
        std::uint64_t result     = 0;

        for (int i = 0; i < 10; ++i) {
            if (pointer == end) return std::unexpected(Truncated);

            const auto byte = std::to_integer<std::uint8_t>(*pointer++);

            if (i == 9) {
                if ((byte & CONTINUE_BITS) != 0u) return std::unexpected(VarIntTooLong);
                if (byte > 0x01u) return std::unexpected(VarIntOverflow);
            }

            result |= static_cast<std::uint64_t>(byte & SEGMENT_BITS) << (7 * i);

            if ((byte & CONTINUE_BITS) == 0u) {
                current = pointer;  // 成功才前移
                return static_cast<std::int64_t>(result);
            }
        }

        return std::unexpected(VarIntTooLong);
    }

    auto globalTagInvoke(
        wire_read_t, wire_type_tag_t<std::int64_t>, const std::byte*& current, const std::byte* end
    ) noexcept -> std::expected<std::int64_t, WireError> {
        return readVarLong(current, end);
    }

    // write

    bool writeVarLong(std::byte*& current, std::byte* end, const std::int64_t& value) noexcept {
        auto unsign           = static_cast<std::uint64_t>(value);
        const std::size_t num = varLongSize(unsign);

        if (static_cast<std::size_t>(end - current) < num) return false;

        std::byte* pointer = current;
        for (std::size_t i = 0; i < num; ++i) {
            const bool last = i + 1 == num;

            pointer[i] =
                static_cast<std::byte>((unsign & SEGMENT_BITS) | (last ? 0u : CONTINUE_BITS));
            unsign >>= 7;
        }

        current = pointer + num;

        return true;
    }

    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<std::int64_t>, std::byte*& current, std::byte* end,
        const std::int64_t& value
    ) noexcept {
        return writeVarLong(current, end, value);
    }

}  // namespace minelink::protocol
