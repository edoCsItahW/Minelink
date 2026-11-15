// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file reader.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/14 21:26
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:reader.impl;

import :reader.decl;
import :wire.decl;

export namespace minelink::protocol {

    // Reader

    constexpr Reader::Reader(std::span<const std::byte> data) noexcept
        : current_(data.data())
        , end_(data.data() + data.size()) {}

    constexpr bool Reader::empty() const noexcept { return current_ == end_; }

    template<typename T>
    auto Reader::read() noexcept -> std::expected<T, WireError> {
        const std::byte* current = current_;

        auto out = wire_read<T>(current, end_);
        if (out) current_ = current;

        return out;
    }

}  // namespace minelink::protocol
