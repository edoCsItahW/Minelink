// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.codec.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 13:05
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module utils:json.codec.impl;

import :json.value.decl;
import :json.codec.decl;
import :common.decl;

export namespace minelink {

    template<typename T>
    Json toJson(const T& v) {
        Json j;
        encodeTo(v, j);
        return j;
    }

    template<typename T>
    T fromJson(const Json& j) {
        T t{};
        decodeFrom(j, t);
        return t;
    }

    template<typename T>
    void encodeTo(const T& v, Json& out) {
        using U = std::remove_cvref_t<T>;
        if constexpr (HasCustomTo<U>)
            globalJsonTagInvoke(to_json_tag, out, v);
        else
            encodeDefault(v, out);
    }

    template<typename T>
    void decodeFrom(const Json& j, T& out) {
        using U = std::remove_cvref_t<T>;
        if constexpr (HasCustomFrom<U>)
            globalJsonTagInvoke(from_json_tag, j, out);  // 修复：补 j
        else
            decodeDefault(j, out);
    }

    template<typename T>
    void encodeDefault(const T& v, Json& out) {
        using U = std::remove_cvref_t<T>;
        if constexpr (std::is_same_v<U, std::nullopt_t>)
            encodeNull(out);
        else if constexpr (std::is_same_v<U, bool>)
            encodeBool(v, out);
        else if constexpr (std::is_integral_v<U>)
            encodeInt(v, out);
        else if constexpr (std::is_floating_point_v<U>)
            encodeDouble(v, out);
        else if constexpr (is_string_like<U>)
            encodeString(v, out);
        else if constexpr (std::is_enum_v<U>)
            encodeEnum(v, out);
        else if constexpr (IsOptional<U>)
            encodeOptional(v, out);
        else if constexpr (IsSequence<U>)
            encodeSequence(v, out);
        else if constexpr (IsMap<U>)
            encodeMap(v, out);
        else if constexpr (isStructLike<U>())
            encodeStruct(v, out);
        else
            static_assert(always_false_v<U>, "toJson: unsupported type");
    }

    template<typename T>
    void decodeDefault(const Json& j, T& out) {
        using U = std::remove_cvref_t<T>;
        if constexpr (std::is_same_v<U, bool>)
            decodeBool(j, out);
        else if constexpr (std::is_integral_v<U>)
            decodeInt(j, out);
        else if constexpr (std::is_floating_point_v<U>)
            decodeDouble(j, out);
        else if constexpr (std::is_same_v<U, std::string>)
            decodeString(j, out);
        else if constexpr (std::is_enum_v<U>)
            decodeEnum(j, out);
        else if constexpr (IsOptional<U>)
            decodeOptional(j, out);
        else if constexpr (IsSequence<U>)
            decodeSequence(j, out);
        else if constexpr (IsMap<U>)
            decodeMap(j, out);
        else if constexpr (isStructLike<U>())
            decodeStruct(j, out);
        else
            static_assert(always_false_v<U>, "fromJson: unsupported type");
    }

    inline void encodeNull(Json& out) { out = Json(nullptr); }

    inline void encodeBool(bool v, Json& out) { out = Json(v); }

    inline void encodeInt(std::int64_t v, Json& out) { out = Json(v); }

    inline void encodeDouble(double v, Json& out) { out = Json(v); }

    inline void encodeString(std::string_view v, Json& out) { out = Json(std::string(v)); }

    inline void decodeBool(const Json& j, bool& out) { out = j.asBool(); }

    inline void decodeInt(const Json& j, std::int64_t& out) { out = j.asInt(); }

    inline void decodeDouble(const Json& j, double& out) { out = j.asDouble(); }

    inline void decodeString(const Json& j, std::string& out) { out = j.asString(); }

    template<typename E>
    void encodeEnum(E v, Json& out) {
        template for (constexpr auto e : std::meta::enumerators_of(^^E)) {
            constexpr auto name = enumeratorName(e);
            if (v == [:e:]) {
                out = Json(std::string(name));
                return;
            }
        }
        out = Json(static_cast<std::int64_t>(v));  // 未知枚举值输出整数，语义明确
    }

    template<typename E>
    void decodeEnum(const Json& j, E& out) {
        if (j.isString()) {
            const auto& s = j.asString();
            template for (constexpr auto e : std::meta::enumerators_of(^^E)) {
                constexpr auto name = enumeratorName(e);
                if (s == name) {
                    out = [:e:];
                    return;
                }
            }
            throw std::runtime_error("fromJson: unknown enum value: " + s);
        }
        if (j.isInt()) {
            out = static_cast<E>(j.asInt());
            return;
        }
        throw std::runtime_error("fromJson: enum expects string or integer");
    }

    template<typename T>
    void encodeOptional(const T& v, Json& out) {
        if (v)
            encodeTo(*v, out);
        else
            out = Json(nullptr);
    }

    template<typename T>
    void decodeOptional(const Json& j, T& out) {
        if (j.isNull()) {
            out.reset();
            return;
        }
        typename T::value_type tmp{};
        decodeFrom(j, tmp);
        out = std::move(tmp);
    }

    template<typename Seq>
    void encodeSequence(const Seq& v, Json& out) {
        out       = Json::array();
        auto& arr = out.asArray();
        arr.reserve(v.size());
        for (const auto& e : v) {
            Json child;
            encodeTo(e, child);
            arr.push_back(std::move(child));
        }
    }

    template<typename Seq>
    void decodeSequence(const Json& j, Seq& out) {
        const auto& arr = j.asArray();
        out.clear();
        out.reserve(arr.size());
        for (const auto& e : arr) {
            typename Seq::value_type tmp{};
            decodeFrom(e, tmp);
            out.push_back(std::move(tmp));
        }
    }

    template<typename Map>
    void encodeMap(const Map& v, Json& out) {
        out       = Json::object();
        auto& obj = out.asObject();
        for (const auto& [k, val] : v) {
            Json child;
            encodeTo(val, child);
            obj.emplace(std::string(k), std::move(child));
        }
    }

    template<typename Map>
    void decodeMap(const Json& j, Map& out) {
        out.clear();
        for (const auto& [k, val] : j.asObject()) {
            typename Map::mapped_type tmp{};
            decodeFrom(val, tmp);
            out.emplace(k, std::move(tmp));
        }
    }

    namespace detail {

        template<auto M, typename T>
        void setField(T& obj, const Json& j) {
            decodeFrom(j, obj.[:M:]);
        }

        template<auto M, class Codec, typename T>
        void setFieldCodec(T& obj, const Json& j) {
            obj.[:M:] = Codec::decode(j);
        }

        template<class T, std::meta::info Type>
        consteval void collectFields(struct_meta_t<T>& r) {
            constexpr auto ctx = std::meta::access_context::unchecked();

            template for (constexpr auto b : std::meta::bases_of(Type, ctx))
                collectFields<T, std::meta::type_of(b)>(r);

            template for (constexpr auto m : std::meta::nonstatic_data_members_of(Type, ctx)) {
                if constexpr (!hasAnnotation<ignore>(m)) {
                    if constexpr (hasAnnotation<flatten>(m)) {
                        r.flattens.push_back(&setField<m, T>);
                    } else {
                        constexpr auto name = fieldName(m);
                        if constexpr (hasAnnotation<via_codec>(m)) {
                            using Codec = typename[:getAnnotation<via_codec>(m).codecType:];
                            r.fields.push_back({name, &setFieldCodec<m, Codec, T>});
                        } else {
                            r.fields.push_back({name, &setField<m, T>});
                        }
                        if constexpr (hasAnnotation<require>(m)) r.required.push_back(name);
                    }
                }
            }
        }

    }  // namespace detail

    template<typename T>
    consteval auto makeStructMeta() {
        struct_meta_t<T> r;
        detail::collectFields<T, ^^T>(r);
        std::ranges::sort(r.fields, {}, &FieldEntry<T>::name);
        return r;
    }

    template<typename T>
    const FieldEntry<T>* findField(std::string_view key) {
        const auto& f = struct_meta_v<T>.fields;
        auto it       = std::ranges::lower_bound(f, key, [](auto& e, std::string_view k) {
            return e.name < k;
        });
        return (it != f.end() && it->name == key) ? &*it : nullptr;
    }

    namespace detail {

        template<typename T>
        void encodeFlatten(const T& v, Json::Object& obj) {
            Json sub;
            encodeTo(v, sub);
            if (!sub.isObject()) throw std::runtime_error("flatten: expected object");
            for (auto& [k, val] : sub.asObject()) obj.emplace(k, std::move(val));
        }

        template<typename T>
        void encodeStructFields(Json::Object& obj, const T& v) {
            using U            = std::remove_cvref_t<T>;
            constexpr auto ctx = std::meta::access_context::unchecked();

            // GCC 的 bases_of 返回基类说明符（tree_binfo），不能直接 splice，必须先 type_of 取到类型
            template for (constexpr auto b : std::define_static_array(std::meta::bases_of(^^U, ctx)))
                encodeStructFields(obj, static_cast<const typename[: std::meta::type_of(b) :]&>(v));

            template for (constexpr auto m : std::define_static_array(std::meta::nonstatic_data_members_of(^^U, ctx))) {
                if constexpr (!hasAnnotation<ignore>(m)) {
                    constexpr auto name = fieldName(m);
                    const auto& member  = v.[:m:];

                    if constexpr (hasAnnotation<flatten>(m)) {
                        if constexpr (IsOptional<std::remove_cvref_t<decltype(member)>>) {
                            if (member) encodeFlatten(*member, obj);
                        } else
                            encodeFlatten(member, obj);

                    } else if constexpr (hasAnnotation<via_codec>(m)) {
                        using Codec = typename[:getAnnotation<via_codec>(m).codecType:];
                        obj.emplace(name, Codec::encode(member));
                    } else if constexpr (IsOptional<std::remove_cvref_t<decltype(member)>>) {
                        if (member) {
                            Json sub;
                            encodeTo(*member, sub);
                            obj.emplace(name, std::move(sub));
                        }
                    } else {
                        Json sub;
                        encodeTo(member, sub);
                        obj.emplace(name, std::move(sub));
                    }
                }
            }
        }

    }  // namespace detail

    template<typename T>
    void encodeStruct(const T& v, Json& out) {
        out = Json::object();
        detail::encodeStructFields(out.asObject(), v);
    }

    template<typename T>
    void decodeStruct(const Json& j, T& out) {
        if (!j.isObject()) throw std::runtime_error("fromJson: expected object");
        const auto& obj  = j.asObject();
        const auto& meta = struct_meta_v<T>;

        // 顺序语义：先 flatten（子结构抓自己的字段），再本层字段（本层优先）
        for (auto f : meta.flattens) f(out, j);
        for (const auto& [key, val] : obj)
            if (auto* e = findField<T>(key)) e->set(out, val);

        for (auto name : meta.required)
            if (!obj.contains(name))
                throw std::runtime_error("fromJson: missing required field: " + std::string(name));
    }

}  // namespace minelink
