// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.reflect.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 13:00
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:json.reflect.decl;

import :json.annotation;
import :json.value.decl;

export namespace minelink {

    template<typename Annotation>
    consteval bool hasAnnotation(std::meta::info r);

    template<typename Annotation>
    consteval Annotation getAnnotation(std::meta::info r);

    consteval std::string_view fieldName(std::meta::info member);

    consteval std::string_view enumeratorName(std::meta::info e);

    template<typename T>
    consteval bool isStructLike();

    template<typename T>
    struct FieldEntry {
        std::string_view name;
        void (*set)(T&, const Json&);
    };

    template<typename T>
    struct struct_meta_t {
        std::vector<FieldEntry<T>> fields;
        std::vector<void (*)(T&, const Json&)> flattens;
        std::vector<std::string_view> required;
    };

    template<typename T> consteval auto makeStructMeta();

    template<typename T>
    inline constexpr auto struct_meta_v = makeStructMeta<T>();

    template<typename T>
    const FieldEntry<T>* findField(std::string_view key);

}  // namespace minelink
