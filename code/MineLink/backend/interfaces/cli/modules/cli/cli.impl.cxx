// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file cli.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 19:25
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module cli:cli.impl;

import :cli.decl;
import :cli.commands;
import :args;

export namespace minelink::cli {

    namespace detail {

        consteval std::string_view toCommandName(std::meta::info type) {
            std::string_view name = std::meta::identifier_of(type);

            if (name.size() > 3 && name.ends_with("Cmd")) name = name.substr(0, name.size() - 3);

            char buf[64];
            std::size_t n = 0;
            for (char c : name) buf[n++] = (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
            return std::define_static_string(std::string_view{buf, n});
        }

        consteval auto makeCommandNames() {
            constexpr auto variant = std::meta::dealias(^^CommandVariant);  // ← 解别名
            constexpr auto args    = std::define_static_array(std::meta::template_arguments_of(variant));
            constexpr std::size_t N = args.size();

            static_assert(std::meta::has_template_arguments(variant),
                          "CommandVariant 必须是某个类模板的特化（如 std::variant<...>）");

            std::array<std::string_view, N> names{};

            for (std::size_t i = 0; i < N; ++i) names[i] = toCommandName(args[i]);

            return names;
        }

        inline constexpr auto command_names_v = makeCommandNames();


    }  // namespace detail

    template<typename Command>
    [[nodiscard]] std::optional<int> parseAndRun(
        std::string_view command, std::span<const std::string_view> rest
    ) {
        constexpr auto name = detail::toCommandName(^^Command);

        if (command != name) return std::nullopt;

        auto parsed = parse<Command>(rest);
        if (!parsed) {
            std::println("error: {} - {}", parsed.error().arg, parsed.error().detail);

            printHelp<Command>();

            return 2;
        }

        return runCmd(*parsed);
    }

    template<IsVariant Variant, std::size_t... Is>
    [[nodiscard]] std::optional<int> dispatchImpl(
        std::string_view command, std::span<const std::string_view> rest, std::index_sequence<Is...>
    ) {
        constexpr auto args = std::define_static_array(std::meta::template_arguments_of(^^Variant));

        std::optional<int> result;

        ((result = parseAndRun<typename[:args[Is]:]>(command, rest)) || ...);

        return result;
    }

    template<IsVariant Variant>
    [[nodiscard]] std::optional<int> dispatch(
        std::string_view command, std::span<const std::string_view> rest
    ) {
        constexpr auto N = std::meta::template_arguments_of(^^Variant).size();

        return dispatchImpl<Variant>(command, rest, std::make_index_sequence<N>{});
    }

}  // namespace minelink::cli
