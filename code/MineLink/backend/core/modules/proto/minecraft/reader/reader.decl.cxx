// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file reader.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/14 21:24
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:reader.decl;

import std;
import base;

export namespace minelink::protocol {

    class Reader {
    public:
        constexpr explicit Reader(std::span<const std::byte> data) noexcept;

        [[nodiscard]] constexpr bool empty() const noexcept;

        [[nodiscard]] constexpr std::size_t remaining() const noexcept;

        template<typename T>
        auto read() noexcept -> std::expected<T, WireError>;

    private:
        const std::byte* current_;
        const std::byte* end_;
    };

}  // namespace minelink::protocol
