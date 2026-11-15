// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file string.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 20:51
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module protocol;

import :string.decl;
import :varint.decl;

namespace minelink::protocol {

    namespace {

        [[nodiscard]] constexpr bool isValidUtf8(std::string_view s) noexcept {
            const auto* p       = reinterpret_cast<const std::uint8_t*>(s.data());
            const auto* const e = p + s.size();
            while (p != e) {
                const std::uint8_t c = *p;
                if (c < 0x80) {
                    ++p;
                    continue;
                }
                std::size_t trailing = 0;
                std::uint32_t cp     = 0;
                if ((c & 0xE0u) == 0xC0u) {
                    trailing = 1;
                    cp       = c & 0x1Fu;
                } else if ((c & 0xF0u) == 0xE0u) {
                    trailing = 2;
                    cp       = c & 0x0Fu;
                } else if ((c & 0xF8u) == 0xF0u) {
                    trailing = 3;
                    cp       = c & 0x07u;
                } else
                    return false;
                if (static_cast<std::size_t>(e - p) <= trailing) return false;
                for (std::size_t i = 1; i <= trailing; ++i) {
                    const std::uint8_t cc = p[i];
                    if ((cc & 0xC0u) != 0x80u) return false;
                    cp = (cp << 6) | (cc & 0x3Fu);
                }
                if ((trailing == 1 && cp < 0x80u) || (trailing == 2 && cp < 0x800u)
                    || (trailing == 3 && cp < 0x10000u))
                    return false;
                if (cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu)) return false;
                p += trailing + 1;
            }
            return true;
        }

    }  // namespace

    // read

    std::expected<std::string_view, WireError> readString(
        const std::byte*& current, const std::byte* end, bool validateUtf8
    ) noexcept {
        using enum WireError;

        const std::byte* cursor = current;

        auto len = readVarInt(cursor, end);
        if (!len) return std::unexpected(len.error());
        if (*len < 0) return std::unexpected(StringTooLong);

        const auto num = static_cast<std::size_t>(*len);
        if (num > MAX_STRING_BYTES) return std::unexpected(StringTooLong);
        if (static_cast<std::size_t>(end - cursor) < num) return std::unexpected(Truncated);

        const std::string_view view{reinterpret_cast<const char*>(cursor), num};
        if (validateUtf8 && !isValidUtf8(view)) return std::unexpected(InvalidUtf8);

        cursor += num;
        current = cursor;

        return view;
    }

    std::expected<std::string_view, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<std::string_view>, const std::byte*& current,
        const std::byte* end
    ) noexcept {
        return readString(current, end);
    }

    // write

    bool writeString(std::byte*& current, std::byte* end, const std::string_view& value) noexcept {
        if (value.size() > MAX_STRING_BYTES) return false;

        const std::size_t num = varIntSize(static_cast<std::uint32_t>(value.size()));

        if (static_cast<std::size_t>(end - current) < num + value.size()) return false;

        // 容量已确认，后续两步不会失败
        (void)writeVarInt(current, end, static_cast<std::int32_t>(value.size()));

        if (!value.empty()) std::memcpy(current, value.data(), value.size());

        current += value.size();

        return true;
    }

    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<std::string_view>, std::byte*& current, std::byte* end,
        const std::string_view& value
    ) noexcept {
        return writeString(current, end, value);
    }

}  // namespace minelink::protocol
