// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file writer.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 16:17
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:writer.impl;

import :writer.decl;
import :wire;

export namespace minelink::protocol {

    constexpr Writer::Writer(std::span<std::byte> buffer) noexcept
        : buffer_(buffer) {}

    template<typename T>
    bool Writer::write(const T& value) noexcept {
        std::byte* current = buffer_.data() + pos_;

        std::byte* const end = buffer_.data() + buffer_.size();
        if (!wire_write<T>(current, end, value)) return false;

        pos_ = static_cast<std::size_t>(end - buffer_.data());

        return true;
    }

}  // namespace minelink::protocol
