// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file locales.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 14:46
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module base:locales.decl;

import std;
import utils;

export namespace minelink {

    struct Arity {
        int value = 0;

        constexpr Arity(int val);
    };

    template<IsEnum E, template<typename, E> class M>
    struct MsgPack {
        template<typename T, E I>
        using msg_map_type = M<T, I>;

        using msg_id_type = E;
    };

    template<typename>
    struct is_msg_pack : std::false_type {};

    template<IsEnum E, template<typename, E> class M>
    struct is_msg_pack<MsgPack<E, M>> : std::true_type {};

    template<typename T>
    inline constexpr auto is_msg_pack_v = is_msg_pack<T>::value;

    template<typename T>
    concept IsMsgPack = is_msg_pack_v<T>;

    template<IsEnum E>
    class LanguageBase {
    public:
        template<E I>
        [[nodiscard]] std::string_view getMsg() const;

        virtual ~LanguageBase() = default;

    protected:
        [[nodiscard]] virtual std::string_view getMsgImpl(E id) const = 0;
    };

    template<IsEnum E, E I>
    consteval int arityOf();

    consteval std::size_t countPlaceholders(std::string_view fmt);

    template<typename T, IsMsgPack P>
    class Language final : public LanguageBase<typename P::msg_id_type> {
    public:
        using E = P::msg_id_type;

    protected:
        [[nodiscard]] std::string_view getMsgImpl(E id) const override;

    private:
        static constexpr auto table_ = []() consteval {
            constexpr auto es = std::meta::enumerators_of(^^E);

            std::array<std::string_view, es.size()> arr{};

            template for (constexpr auto e : es) {
                constexpr auto id = std::meta::extract<E>(e);
                arr[static_cast<std::size_t>(id)] = P::template msg_map_type<T, id>::msg;
            }

            return arr;
        };
    };

    template<IsMsgPack P>
    class Locale {
    public:
        using E = P::msg_id_type;

        template<typename T>
        static void registerLanguage(std::string name);

        static void setLanguage(const std::string& name);

        template<typename P::msg_id_type I>
            requires(arityOf<typename P::msg_id_type, I>() == 0)
        [[nodiscard]] static auto msg() pre(language_ != nullptr);

        template<typename P::msg_id_type I>
            requires(arityOf<typename P::msg_id_type, I>() == 0)
        [[nodiscard]] static std::string msgStr();

        template<typename P::msg_id_type I, typename... Args>
            requires(arityOf<typename P::msg_id_type, I>() == sizeof...(Args))
        [[nodiscard]] static auto msgFormat(Args&&... args);

    private:
        using Creator = std::unique_ptr<LanguageBase<E>> (*)();

        inline static std::unordered_map<std::string, Creator> registry_ = {};
        inline static std::unique_ptr<LanguageBase<E>> language_         = nullptr;
    };

}  // namespace minelink
