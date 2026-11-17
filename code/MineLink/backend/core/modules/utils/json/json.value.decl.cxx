// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.value.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 12:50
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:json.value.decl;

import std;

export namespace minelink {

    struct Json {
        using Object = std::map<std::string, Json, std::less<>>;
        using Array  = std::vector<Json>;

        std::variant<std::nullopt_t, bool, std::int64_t, double, std::string, Array, Object> value{
            std::nullopt
        };

        Json() = default;

        Json(std::nullptr_t);

        Json(bool b);

        Json(std::int64_t i);

        Json(double d);

        Json(std::string s);

        Json(const char* s);

        static Json object();

        static Json array();

        bool isNull() const;
        bool isBool() const;
        bool isInt() const;
        bool isDouble() const;
        bool isString() const;
        bool isObject() const;
        bool isArray() const;

        // template<typename Self>
        // auto& asObject(this Self& self);

        Object& asObject();
        const Object& asObject() const;

        // template<typename Self>
        // auto& asArray(this Self& self);

        Array& asArray();

        const Array& asArray() const;

        const std::string& asString() const;

        bool asBool() const;

        std::int64_t asInt() const;

        double asDouble() const;

        std::string toString(std::size_t indent = 4) const;

    private:
        // template<typename T, typename Self>
        // auto& expect(this Self& self, const char* what);

        template<typename T>
        T& expect(const char* what);

        template<typename T>
        const T& expect(const char* what) const;
    };

}  // namespace minelink
