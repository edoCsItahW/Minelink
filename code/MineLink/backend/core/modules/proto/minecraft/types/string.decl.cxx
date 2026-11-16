// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file string.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 20:48
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:string.decl;

import :wire;

export namespace minelink::protocol {

    inline constexpr std::size_t MAX_STRING_BYTES = 32767;

    // read

    std::expected<std::string_view, WireError> readString(
        const std::byte*& current, const std::byte* end, bool validateUtf8 = true
    ) noexcept;

    std::expected<std::string_view, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<std::string_view>, const std::byte*& current,
        const std::byte* end
    ) noexcept;

    // write

    bool writeString(std::byte*& current, std::byte* end, const std::string_view& value) noexcept;

    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<std::string_view>, std::byte*& current, std::byte* end,
        const std::string_view& value
    ) noexcept;

}  // namespace minelink::protocol
