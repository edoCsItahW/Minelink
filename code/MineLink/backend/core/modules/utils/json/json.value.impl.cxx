// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.value.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 12:56
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:json.value.impl;

import :json.value.decl;

export namespace minelink {

    // template<typename Self>
    // auto& Json::asObject(this Self& self) {
    //     return self.template expect<Object>("object");
    // }

    Json::Object& Json::asObject() { return expect<Object>("object"); }

    const Json::Object& Json::asObject() const { return expect<Object>("object"); }

    // template<typename Self>
    // auto& Json::asArray(this Self& self) {
    //     return self.template expect<Array>("array");
    // }

    Json::Array& Json::asArray() { return expect<Array>("array"); }

    const Json::Array& Json::asArray() const { return expect<Array>("array"); }

    // template<typename T, typename Self>
    // auto& Json::expect(this Self& self, const char* what) {
    //     if (auto p = std::get_if<T>(&self.value)) return *p;
    //
    //     throw std::runtime_error(std::string("json: expected ") + what);
    // }

    template<typename T>
    T& Json::expect(const char* what) {
        if (auto p = std::get_if<T>(&value)) return *p;
        throw std::runtime_error(std::string("json: expected ") + what);
    }

    template<typename T>
    const T& Json::expect(const char* what) const {
        if (auto p = std::get_if<T>(&value)) return *p;
        throw std::runtime_error(std::string("json: expected ") + what);
    }

}  // namespace minelink
