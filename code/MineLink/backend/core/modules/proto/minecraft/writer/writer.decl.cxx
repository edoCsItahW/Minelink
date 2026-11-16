// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file writer.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/15 16:16
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:writer.decl;

import std;

export namespace minelink::protocol {

    class Writer {
    public:
        constexpr explicit Writer(std::span<std::byte> buffer) noexcept;

        [[nodiscard]] constexpr std::size_t size() const noexcept;

        [[nodiscard]] constexpr std::span<const std::byte> bytes() const noexcept;

        template<typename T>
        bool write(const T& value) noexcept;

    private:
        std::span<std::byte> buffer_;
        std::size_t pos_{0};
    };

}  // namespace minelink::protocol
