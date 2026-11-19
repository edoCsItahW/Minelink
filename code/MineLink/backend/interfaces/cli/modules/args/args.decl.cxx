// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file args.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 18:58
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module cli:args.decl;

import std;

export namespace minelink::cli {

    enum class Kind : std::uint8_t { Value, Flag, Multi };

    struct FieldInfo {
        std::string_view ident;
        std::string_view longName;   // "--host"
        std::string_view shortName;  // "-j" 或 ""
        std::string_view help;
        std::string_view defaultRepr;
        int positionalIndex = -1;  // -1 = 非位置参数
        Kind kind           = Kind::Value;
    };

    namespace detail {

        template<typename Config, std::size_t I>
        consteval std::meta::info memberAt();

        template<typename T>
        consteval std::string_view defaultReprOf(const T& v);

        template<typename Config, std::size_t I>
        consteval FieldInfo makeField();

        consteval std::vector<std::meta::info> collectMembersRecursive(std::meta::info type);

        template<typename Config>
        consteval auto buildTable();

    }  // namespace detail

    template<typename Config>
    inline constexpr auto field_table_v = detail::buildTable<Config>();

    template<typename T>
    [[nodiscard]] std::expected<T, std::string> fromString(std::string_view str);

    namespace detail {

        template<typename Config, std::size_t I>
        [[nodiscard]] std::optional<std::string> setValue(Config& config, std::string_view val);

        template<typename Config, std::size_t I>
        void setFlag(Config& config, bool on);

        template<typename Config, std::size_t... Is>
        [[nodiscard]] std::optional<std::string> dispatchValue(
            Config& config, std::size_t idx, std::string_view val, std::index_sequence<Is...>
        );

        template<typename Config, std::size_t... Is>
        void dispatchFlag(Config& config, std::size_t idx, bool on, std::index_sequence<Is...>);

    }  // namespace detail

    enum class ParseError { UnknownOption, MissingValue, InvalidValue };

    struct ParseErrorInfo {
        ParseError kind;
        std::string_view arg;
        std::string detail;
    };

    namespace detail {

        template<typename Config>
        [[nodiscard]] std::optional<ParseErrorInfo> parseInto(
            Config& config, std::span<const std::string_view> tokens
        );

    }  // namespace detail

    template<typename Config>
    [[nodiscard]] std::expected<Config, ParseErrorInfo> parse(
        std::span<const std::string_view> tokens
    );

}  // namespace minelink::cli
