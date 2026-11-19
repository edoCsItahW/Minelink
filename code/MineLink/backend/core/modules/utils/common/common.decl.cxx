// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file common.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 12:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:common.decl;

import std;

export namespace minelink {

    template<typename>
    inline constexpr bool always_false_v = false;

    template<typename T>
    concept IsEnum = std::is_enum_v<T>;

    template<IsEnum E>
    inline constexpr auto size_of_enum_v = std::meta::enumerators_of(^^E).size();

    template<IsEnum E>
    consteval auto makeEnumTable();

    template<IsEnum E>
    inline constexpr auto enum_table_v = makeEnumTable<E>();

    template<IsEnum E>
    constexpr auto enumName(E value) pre(static_cast<std::size_t>(value) < enum_table_v<E>.size());

    template<typename T>
    inline constexpr bool is_string_like = std::is_same_v<std::remove_cvref_t<T>, std::string>
                                        || std::is_same_v<std::remove_cvref_t<T>, std::string_view>
                                        || std::is_same_v<std::remove_cvref_t<T>, const char*>
                                        || std::is_same_v<std::remove_cvref_t<T>, char*>;

    template<typename>
    struct is_variant : std::false_type {};

    template<typename... Ts>
    struct is_variant<std::variant<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr auto is_variant_v = is_variant<T>::value;

    template<typename T>
    concept IsVariant = is_variant_v<T>;

    template<typename>
    struct is_vector : std::false_type {};

    template<typename T>
    struct is_vector<std::vector<T>> : std::true_type {};

    template<typename T>
    inline constexpr auto is_vector_v = is_vector<T>::value;

    template<typename T>
    concept IsVector = is_vector_v<T>;

    template<typename>
    struct is_optional : std::false_type {};

    template<typename T>
    struct is_optional<std::optional<T>> : std::true_type {};

    template<typename T>
    inline constexpr auto is_optional_v = is_optional<T>::value;

    template<typename T>
    concept IsOptional = is_optional_v<T>;

    template<typename>
    struct is_sequence : std::false_type {};

    template<typename T, typename A>
    struct is_sequence<std::vector<T, A>> : std::true_type {};

    template<typename T>
    inline constexpr auto is_sequence_v = is_sequence<T>::value;

    template<typename T>
    concept IsSequence = is_sequence_v<T>;

    template<typename>
    struct is_map : std::false_type {};

    template<typename K, typename V, typename C, typename A>
    struct is_map<std::map<K, V, C, A>> : std::true_type {};

    template<typename T>
    inline constexpr auto is_map_v = is_map<T>::value;

    template<typename T>
    concept IsMap = is_map_v<T>;

}  // namespace minelink
