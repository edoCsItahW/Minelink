// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file registry.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 19:13
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module world;

import :registry.decl;
import :block_table;

namespace minelink::world {

    namespace {

        constexpr BlockInfo unknown_block_v{"unknown", 0, 0};

    }  // namespace

    const BlockInfo& blockInfo(std::uint32_t stateId) noexcept {
        // 找到最后一个 minState <= stateId 的区间
        const auto it = std::upper_bound(
            block_table.begin(), block_table.end(), stateId,
            [](std::uint32_t value, const BlockInfo& block) { return value < block.minState; }
        );

        if (it == block_table.begin()) return unknown_block_v;

        const BlockInfo& block = *(it - 1);

        return stateId <= block.maxState ? block : unknown_block_v;
    }

    std::string_view blockName(std::uint32_t stateId) noexcept { return blockInfo(stateId).name; }

    bool validBlockState(std::uint32_t stateId) noexcept {
        return &blockInfo(stateId) != &unknown_block_v;
    }

}  // namespace minelink::world
