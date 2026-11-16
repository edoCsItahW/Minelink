// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file boolean.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 22:22
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:boolean.decl;

import :wire;

export namespace minelink::protocol {

    // read

    std::expected<bool, WireError> readBool(
        const std::byte*& current, const std::byte* end
    ) noexcept;

    std::expected<bool, WireError> globalTagInvoke(
        wire_read_t, wire_type_tag_t<bool>, const std::byte*& current, const std::byte* end
    ) noexcept;

    // write

    bool writeBool(std::byte*& current, std::byte* end, bool value) noexcept;

    bool globalTagInvoke(
        wire_write_t, wire_type_tag_t<bool>, std::byte*& current, std::byte* end, bool value
    ) noexcept;

}  // namespace minelink::protocol
