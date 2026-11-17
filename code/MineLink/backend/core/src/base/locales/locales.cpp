// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file locales.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 15:32
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module base;

import :locales.decl;

namespace minelink {

    template<typename T, IsMsgPack P>
    std::string_view Language<T, P>::getMsgImpl(E id) const {
        return table_[static_cast<std::size_t>(std::to_underlying(id))];
    }

}  // namespace minelink