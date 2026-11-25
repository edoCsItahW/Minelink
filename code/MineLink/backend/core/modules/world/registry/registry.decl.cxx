// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file registry.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 19:12
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module world:registry.decl;

import std;

export namespace minelink::world {

    struct BlockInfo {
        std::string_view name;
        std::uint32_t minState{0};
        std::uint32_t maxState{0};
    };

    [[nodiscard]] const BlockInfo& blockInfo(std::uint32_t stateId) noexcept;

    [[nodiscard]] std::string_view blockName(std::uint32_t stateId) noexcept;

    [[nodiscard]] bool validBlockState(std::uint32_t stateId) noexcept;

}  // namespace minelink::world
