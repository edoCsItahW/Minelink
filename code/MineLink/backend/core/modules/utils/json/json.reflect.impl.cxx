// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.reflect.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 14:05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:json.reflect.impl;

import :common.decl;
import :json.reflect.decl;

export namespace minelink {

    template<typename Annotation>
    consteval bool hasAnnotation(std::meta::info r) {
        for (auto annotation : std::define_static_array(std::meta::annotations_of(r)))
            if (std::meta::type_of(annotation) == ^^Annotation) return true;

        return false;
    }

    template<typename Annotation>
    consteval Annotation getAnnotation(std::meta::info r) {
        for (auto annotation : std::meta::annotations_of(r))
            if (std::meta::type_of(annotation) == ^^Annotation)
                return std::meta::extract<Annotation>(annotation);

        return {};
    }

    consteval std::string_view fieldName(std::meta::info member) {
        if (hasAnnotation<rename>(member)) return getAnnotation<rename>(member).name;

        return std::meta::identifier_of(member);
    }

    consteval std::string_view enumeratorName(std::meta::info e) {
        if (hasAnnotation<enum_rename>(e)) return getAnnotation<enum_rename>(e).name;

        return std::meta::identifier_of(e);
    }

    template<typename T>
    consteval bool isStructLike() {
        if constexpr (
            std::is_same_v<T, Json> || std::is_same_v<T, std::string>
            || std::is_same_v<T, std::string_view> || IsOptional<T> || IsSequence<T> || IsMap<T>
        )
            return false;
        else if constexpr (std::is_class_v<T>)
            return std::meta::is_class_type(^^T) && std::meta::is_complete_type(^^T);
        else
            return false;
    }

}  // namespace minelink
