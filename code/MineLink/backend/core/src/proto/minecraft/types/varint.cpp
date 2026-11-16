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
 * @date 2026/11/15 20:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module protocol;

import :varint.decl;
import std;

namespace minelink::protocol {

    // read

    std::expected<std::int32_t, WireError> readVarInt(
        const std::byte*& current, const std::byte* end
    ) noexcept {
        using enum WireError;

        // 成功才写回
        const std::byte* pointer = current;

        // 单字节快速路径
        if (pointer != end && std::to_integer<std::uint8_t>(*pointer) < CONTINUE_BITS) {
            current = pointer + 1;
            return static_cast<std::int32_t>(std::to_integer<std::uint8_t>(*pointer));
        }

        std::uint32_t result = 0;
        for (int i = 0; i < 4; ++i) {
            if (pointer == end) return std::unexpected(Truncated);

            const auto byte = std::to_integer<std::uint8_t>(*pointer++);

            result |= static_cast<std::uint32_t>(byte & SEGMENT_BITS) << (7 * i);

            if (byte < CONTINUE_BITS) {
                current = pointer;
                return static_cast<std::int32_t>(result);
            }
        }

        if (pointer == end) return std::unexpected(Truncated);

        if (const auto byte5 = std::to_integer<std::uint8_t>(*pointer);
            (byte5 & CONTINUE_BITS) != 0u)
            return std::unexpected(VarIntTooLong);

        else if (byte5 > 0x0Fu)
            return std::unexpected(VarIntOverflow);

        else {
            result |= static_cast<std::uint32_t>(byte5) << 28;
            current = pointer + 1;
        }

        return static_cast<std::int32_t>(result);
    }

    std::expected<std::int32_t, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<std::int32_t>, const std::byte*& current, const std::byte* end
    ) noexcept {
        return readVarInt(current, end);
    }

    // write

    bool writeVarInt(std::byte*& current, std::byte* end, const std::int32_t& value) noexcept {
        using enum WireError;

        auto unsign = static_cast<std::uint32_t>(value);

        const std::size_t num = varIntSize(unsign);

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
        wire_write_t, wire_type_tag_t<std::int32_t>, std::byte*& current, std::byte* end,
        const std::int32_t& value
    ) noexcept {
        return writeVarInt(current, end, value);
    }

}  // namespace minelink::protocol
