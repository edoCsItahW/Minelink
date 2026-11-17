// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file locales.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 15:13
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module base:locales.impl;

import std;
import :locales.decl;

export namespace minelink {

    constexpr Arity::Arity(int val)
        : value(val) {}

    template<IsEnum E>
    template<E I>
    std::string_view LanguageBase<E>::getMsg() const {
        return getMsgImpl(I);
    }

    template<IsEnum E>
    inline constexpr auto arityTable = []() consteval {
        constexpr auto es = std::meta::enumerators_of(^^E);

        std::array<int, es.size()> arr{};

        template for (constexpr auto e :
                      es) template for (constexpr auto a :
                                        std::meta::annotations_of(
                                            e
                                        )) if constexpr (std::meta::type_of(a) == ^^Arity)
            arr[static_cast<std::size_t>(std::meta::extract<E>(e))] =
                std::meta::extract<Arity>(a).value;

        return arr;
    };

    template<IsEnum E, E I>
    consteval int arityOf() {
        return arityTable<E>[static_cast<std::size_t>(std::to_underlying(I))];
    }

    consteval std::size_t countPlaceholders(std::string_view fmt) {
        std::size_t count = 0;
        for (std::size_t i = 0; i < fmt.size(); ++i)
            if (fmt[i] == '{') {
                if (i + 1 < fmt.size() && fmt[i + 1] == '{') {
                    ++i;
                    continue;
                }

                ++count;
            } else if (fmt[i] == '}' && i + 1 < fmt.size() && fmt[i + 1] == '}')
                ++i;

        return count;
    }

    template<IsMsgPack P>
    template<typename T>
    void Locale<P>::registerLanguage(std::string name) {
        template for (constexpr auto e : std::meta::enumerators_of(^^E)) {
            using Map = P::template msg_map_type<T, std::meta::extract<E>(e)>;

            if constexpr (requires { Map::msg; })
                static_assert(
                    countPlaceholders(Map::msg) == arityOf<E, std::meta::extract<E>(e)>(), ""
                );
            else
                static_assert(requires { Map::msg; }, "");
        }

        registry_[std::move(name)] = [] { return std::make_unique<Language<T, P>>(); };
    }

    template<IsMsgPack P>
    void Locale<P>::setLanguage(const std::string& name) {
        if (auto it = registry_.find(name); it != registry_.end()) {
            language_ = it->second();
            return;
        }

        throw std::runtime_error("Unknown language: " + name);
    }

    template<IsMsgPack P>
    template<typename P::msg_id_type I>
        requires(arityOf<typename P::msg_id_type, I>() == 0)
    auto Locale<P>::msg() {
        return language_->template getMsg<I>();
    }

    template<IsMsgPack P>
    template<typename P::msg_id_type I>
        requires(arityOf<typename P::msg_id_type, I>() == 0)
    std::string Locale<P>::msgStr() {
        return std::string(msg<I>());
    }

    template<IsMsgPack P>
    template<typename P::msg_id_type I, typename... Args>
        requires(arityOf<typename P::msg_id_type, I>() == sizeof...(Args))
    auto Locale<P>::msgFormat(Args&&... args) {
        return std::vformat(msg<I>(), std::make_format_args(args...));
    }

}  // namespace minelink
