// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file cli.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/11/16 19:17
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module cli;

namespace minelink::cli {

    void printUsage() {
        std::println("Usage: minelink <command> [options] [args...]");
        std::println("Commands: ");

        for (auto name : detail::command_names_v)
            std::println(" {}", name);
    }

    template<typename Config>
    void printHelp() {
        constexpr auto& table = field_table_v<Config>;

        std::println("Usage: {} [options] [args...]", detail::toCommandName(^^Config));

        for (const auto& field : table) {
            std::string names;

            if (!field.shortName.empty()) {
                names += field.shortName;
                names += ", ";
            }

            names += field.longName;

            if (field.positionalIndex >= 0) {
                names += " (pos ";
                names += std::to_string(field.positionalIndex);
                names += ')';
            }

            auto help = std::string{field.help};

            if (!field.defaultRepr.empty()) {
                help += " (default: ";
                help += field.defaultRepr;
                help += ')';
            }

            std::println(" {:<32} {}", names, help);
        }
    }

    int runCommand(std::span<const std::string_view> tokens) {
        if (tokens.empty()) {
            printUsage();
            return 2;
        }

        const std::string_view rawCommand = tokens[0];
        // const std::string_view command = resolveAlias(rawCommand);
        const auto rest = tokens.subspan(1);

        if (auto result = dispatch<CommandVariant>(rawCommand, rest)) return *result;

        std::println("unknown command: {}", rawCommand);

        printUsage();

        return 2;
    }

}
