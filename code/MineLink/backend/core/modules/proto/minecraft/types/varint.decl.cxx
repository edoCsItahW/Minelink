// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file varint.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 15:34
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:varint.decl;

import :wire;

export namespace minelink::protocol {

    [[nodiscard]] constexpr std::size_t varIntSize(std::uint32_t value) noexcept {
        return (static_cast<std::size_t>(std::bit_width(value | 1u)) + 6u) / 7u;
    }

    // read

    std::expected<std::int32_t, WireError> readVarInt(
        const std::byte*& current, const std::byte* end
    ) noexcept;

    std::expected<std::int32_t, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<std::int32_t>, const std::byte*& current, const std::byte* end
    ) noexcept;

    // write

    bool writeVarInt(std::byte*& current, std::byte* end, const std::int32_t& value) noexcept;

    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<std::int32_t>, std::byte*& current, std::byte* end,
        const std::int32_t& value
    ) noexcept;

}  // namespace minelink::protocol
