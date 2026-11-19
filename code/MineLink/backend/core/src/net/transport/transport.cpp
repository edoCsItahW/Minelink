// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file transport.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/18 14:37
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module net;

import :transport.decl;

namespace minelink::net {

    bool IConnection::readExact(std::span<std::byte> out) {
        while (!out.empty()) {
            const auto got = readSome(out);

            if (!got || *got == 0) return false;  // 0 视为对端关闭，避免死循环

            out = out.subspan(*got);
        }

        return true;
    }

}  // namespace minelink::net
