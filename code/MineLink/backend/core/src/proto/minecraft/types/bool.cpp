// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file bool.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 22:26
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module protocol;

namespace minelink::protocol {

    // read

    std::expected<bool, WireError> readBool(
        const std::byte*& current, const std::byte* end
    ) noexcept {
        using enum WireError;

        if (current == end) return std::unexpected(Truncated);

        return std::to_integer<std::uint8_t>(*current++) != 0u;
    }

    std::expected<bool, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<bool>, const std::byte*& current, const std::byte* end
    ) noexcept {
        return readBool(current, end);
    }

    // write

    bool writeBool(std::byte*& current, std::byte* end, bool value) noexcept {
        if (current == end) return false;

        *current++ = static_cast<std::byte>(value ? 1u : 0u);

        return true;
    }

    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<bool>, std::byte*& current, std::byte* end, bool value
    ) noexcept {
        return writeBool(current, end, value);
    }

}  // namespace minelink::protocol