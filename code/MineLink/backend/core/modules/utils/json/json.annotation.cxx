// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.annotation.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/18 19:39
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:json.annotation;

import std;

export namespace minelink {

    struct rename {
        std::string_view name;
    };

    struct ignore {};

    struct flatten {};

    struct require {};

    struct enum_rename {
        std::string_view name;
    };

    struct via_codec {
        std::meta::info codecType;
    };

}  // namespace minelink
