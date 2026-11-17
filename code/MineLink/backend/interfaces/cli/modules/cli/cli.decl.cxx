// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file cli.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 19:14
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module cli:cli.decl;

import std;
import utils;

export namespace minelink::cli {

    namespace detail {

        consteval std::string_view toCommandName(std::meta::info type);

        consteval auto makeCommandNames();

    }  // namespace detail

    void printUsage();

    template<typename Config>
    void printHelp();

    template<typename Command>
    [[nodiscard]] std::optional<int> parseAndRun(
        std::string_view command, std::span<const std::string_view> rest
    );

    template<IsVariant Variant, std::size_t... Is>
    [[nodiscard]] std::optional<int> dispatchImpl(
        std::string_view command, std::span<const std::string_view> rest, std::index_sequence<Is...>
    );

    template<IsVariant Variant>
    [[nodiscard]] std::optional<int> dispatch(
        std::string_view command, std::span<const std::string_view> rest
    );

    int runCommand(std::span<const std::string_view> tokens);

}  // namespace minelink::cli
