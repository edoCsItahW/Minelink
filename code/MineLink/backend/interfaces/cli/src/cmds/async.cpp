// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file async.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/17 12:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module cli;

import :args;
import net;
import protocol;
import :cli.commands;

namespace minelink::cli {

    constexpr auto TIMEOUT = std::chrono::milliseconds(5000);

    namespace {

        bool advanceToPlay(protocol::LoginSession& session, std::string& reason) {
            while (session.state() != protocol::LoginState::Play) {
                if (!session.pump()) {
                    reason = session.closeReason();
                    return false;
                }

                if (!session.checkAlive(std::chrono::steady_clock::now())) {
                    reason = "推进阶段心跳超时";
                    return false;
                }
            }

            return true;
        }

    }  // namespace

    int runCmd(const AsyncCmd& cmd) {
        const net::Endpoint endpoint{cmd.host, cmd.port};

        const auto info = protocol::ping(endpoint, TIMEOUT);
        if (!info) {
            std::println("ping失败，错误码{}", static_cast<int>(info.error()));
            return 1;
        }

        auto connection = net::connectTcp(endpoint, TIMEOUT, TIMEOUT);
        if (!connection) {
            std::println("连接失败，错误码{}", static_cast<int>(connection.error()));
            return 1;
        }

        protocol::LoginSession session{
            *connection.value(),
            {.host        = cmd.host,
                                .port        = cmd.port,
                                .username    = cmd.username,
                                .protocol    = info->protocol,
                                .dumpPackets = cmd.dumpPackets}
        };

        if (!session.start()) {
            std::println("登录失败：{}", session.closeReason());
            return 1;
        }

        std::string reason;
        if (!advanceToPlay(session, reason)) {
            std::println("登录失败{}", reason);
            return 1;
        }

        const auto& player = session.player();
        if (!cmd.json) std::println("进入Play");

        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{cmd.seconds};
        while (std::chrono::steady_clock::now() < deadline) {
            if (!session.pump()) {
                std::println("断开{}", session.closeReason());
                return 1;
            }

            if (!session.checkAlive(std::chrono::steady_clock::now())) {
                std::println("Play阶段心跳超时");
                return 1;
            }
        }

        const auto ignored = session.ignoredPackets();
        if (cmd.json)
            std::println("{}", toJson(cmd).toString());
        else
            std::println("保持在线{}成功，忽略{}个未解析包", cmd.seconds, ignored);

        return 0;
    }

}  // namespace minelink::cli
