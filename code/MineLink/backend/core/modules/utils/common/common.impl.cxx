// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file common.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 15:05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:common.impl;

import :common.decl;

export namespace minelink {

    template<IsEnum E>
    consteval auto makeEnumTable() {
        constexpr static auto entries = std::define_static_array(std::meta::enumerators_of(^^E));

        std::array<std::string_view, entries.size()> table;
        std::size_t i                                        = 0;
        template for (constexpr auto e : entries) table[i++] = std::meta::identifier_of(e);
        return table;
    }

    template<IsEnum E>
    constexpr auto enumName(E value) {
        return enum_table_v<E>[static_cast<std::size_t>(value)];
    }

}  // namespace minelink
