// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file cli.commands.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 22:11
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module cli:cli.commands;

import std;
import :args;

export namespace minelink::cli {

    struct CommonArgs {
        [[= positional{0}, = help{"服务器地址"}]] std::string host                   = "127.0.0.1";
        [[= positional{1}, = help{"端口"}]] std::uint16_t port                       = 25565;
        [[= positional{2}, = help{"协议版本"}]] std::optional<std::int32_t> protocol = std::nullopt;
        [[= positional{3}, = help{"用户名"}]] std::string username = "MineLinkBot";
        [[= positional{4}, = help{"会话时长（秒）"}]] int seconds  = 10;
        [[= short_name{'j'}, = help{"输出 JSON 摘要"}]] bool json  = false;
    };

    struct AsyncCmd : CommonArgs {
        [[= positional{5}, = help{"空闲超时"}]] int idleSeconds = 90;
        [[= help{"打印未处理包"}]] bool dumpPackets             = false;
    };

    using CommandVariant = std::variant<AsyncCmd>;

    int runCmd(const AsyncCmd&);

}  // namespace minelink::cli
