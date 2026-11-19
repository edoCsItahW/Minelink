// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.codec.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 13:02
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:json.codec.decl;

import :json.value.decl;
import :json.reflect.decl;

export namespace minelink {

    struct to_json_tag_t {};

    inline constexpr to_json_tag_t to_json_tag{};

    struct from_json_tag_t {};

    inline constexpr from_json_tag_t from_json_tag{};

    template<typename T>
    concept HasCustomTo = requires(Json& j, const T& v) { globalJsonTagInvoke(to_json_tag, j, v); };

    template<typename T>
    concept HasCustomFrom =
        requires(const Json& j, T& v) { globalJsonTagInvoke(from_json_tag, j, v); };

    template<typename T>
    Json toJson(const T& v);

    template<typename T>
    T fromJson(const Json& j);

    // ---------- 分发器（显式编排：自定义优先，否则按类型分发） ----------
    template<typename T>
    void encodeTo(const T& v, Json& out);

    template<typename T>
    void decodeFrom(const Json& j, T& out);

    // ---------- 默认分发：每个分支一行，调用具名 codec ----------
    template<typename T>
    void encodeDefault(const T& v, Json& out);

    template<typename T>
    void decodeDefault(const Json& j, T& out);

    // ---------- 基础 codec：单一业务动作，可单测 ----------
    inline void encodeNull(Json& out);

    inline void encodeBool(bool v, Json& out);

    inline void encodeInt(std::int64_t v, Json& out);

    inline void encodeDouble(double v, Json& out);

    inline void encodeString(std::string_view v, Json& out);

    inline void decodeBool(const Json& j, bool& out);

    inline void decodeInt(const Json& j, std::int64_t& out);

    inline void decodeDouble(const Json& j, double& out);

    inline void decodeString(const Json& j, std::string& out);

    // ---------- enum codec：显式区分 string / int，修复 fallback 矛盾 ----------
    template<typename E>
    void encodeEnum(E v, Json& out);

    template<typename E>
    void decodeEnum(const Json& j, E& out);

    // ---------- 容器 codec ----------
    template<typename T>
    void encodeOptional(const T& v, Json& out);

    template<typename T>
    void decodeOptional(const Json& j, T& out);

    template<typename Seq>
    void encodeSequence(const Seq& v, Json& out);

    template<typename Seq>
    void decodeSequence(const Json& j, Seq& out);

    template<typename Map>
    void encodeMap(const Map& v, Json& out);

    template<typename Map>
    void decodeMap(const Json& j, Map& out);

    // ---------- struct codec：只做 struct 领域，细节下沉到 reflect ----------
    template<typename T>
    void encodeStruct(const T& v, Json& out);

    template<typename T>
    void decodeStruct(const Json& j, T& out);

}  // namespace minelink
