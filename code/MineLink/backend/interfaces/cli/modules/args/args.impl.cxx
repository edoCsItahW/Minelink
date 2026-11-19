// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file args.impl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/16 22:18
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module cli:args.impl;

import :args.annotation;
import :args.decl;
import utils;

export namespace minelink::cli {

    namespace detail {

        template<typename Config, std::size_t I>
        consteval std::meta::info memberAt() {
            constexpr auto members = std::define_static_array(collectMembersRecursive(^^Config));
            return members[I];
        }

        template<typename T>
        consteval std::string_view defaultReprOf(const T& v) {
            if constexpr (std::is_same_v<T, bool>)
                return std::define_static_string(std::string_view{v ? "true" : "false"});
            else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
                char buf[32]{};
                auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), v);
                if (ec != std::errc{}) return {};
                return std::define_static_string(
                    std::string_view{buf, static_cast<std::size_t>(p - buf)}
                );
            } else if constexpr (std::is_same_v<T, std::string>)
                return std::define_static_string(std::string_view{v});
            else
                return {};
        }

        template<typename Config, std::size_t I>
        consteval FieldInfo makeField() {
            constexpr auto members = std::define_static_array(collectMembersRecursive(^^Config));
            constexpr auto m       = members[I];

            FieldInfo out{};

            constexpr std::string_view id = std::meta::identifier_of(m);
            out.ident                     = std::define_static_string(id);
            {
                std::string ln = "--";
                ln += id;
                out.longName = std::define_static_string(std::string_view{ln});
            }

            using MT = typename[:std::meta::type_of(m):];
            if constexpr (std::is_same_v<MT, bool>) out.kind = Kind::Flag;

            if constexpr (std::is_default_constructible_v<Config>) {
                out.defaultRepr = []() consteval {
                    Config prototype{};  // 非 constexpr 变量，仅本求值内有效
                    return defaultReprOf(prototype.[:m:]);
                }();
            }

            static constexpr auto annotations =
                std::define_static_array(std::meta::annotations_of(m));

            template for (constexpr auto a : annotations) {
                if constexpr (std::meta::type_of(a) == ^^positional)
                    out.positionalIndex = std::meta::extract<positional>(a).index;
                else if constexpr (std::meta::type_of(a) == ^^short_name)
                    out.shortName = std::define_static_string(
                        std::string{'-', std::meta::extract<short_name>(a).character}
                    );
                else if constexpr (std::meta::type_of(a) == ^^help)
                    out.help = std::string_view{std::meta::extract<help>(a).text};
            }

            return out;
        }

        consteval std::vector<std::meta::info> collectMembersRecursive(std::meta::info type) {
            std::vector<std::meta::info> result;

            for (auto base : std::meta::bases_of(type, std::meta::access_context::current())) {
                auto sub = collectMembersRecursive(std::meta::type_of(base));
                result.insert(result.end(), sub.begin(), sub.end());
            }

            for (auto m :
                 std::meta::nonstatic_data_members_of(type, std::meta::access_context::current()))
                result.push_back(m);

            return result;
        }

        template<typename Config>
        consteval auto buildTable() {
            constexpr auto members  = std::define_static_array(collectMembersRecursive(^^Config));
            constexpr std::size_t N = members.size();

            auto table = [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return std::array<FieldInfo, N>{makeField<Config, Is>()...};
            }(std::make_index_sequence<N>{});

            for (std::size_t i = 0; i < N; ++i)
                for (std::size_t j = i + 1; j < N; ++j) {
                    if (table[i].positionalIndex >= 0
                        && table[j].positionalIndex == table[i].positionalIndex)
                        throw std::runtime_error("minelink::cli: duplicate positional index");
                    if (table[i].longName == table[j].longName)
                        throw std::runtime_error("minelink::cli: duplicate long option");
                    if (!table[i].shortName.empty() && table[i].shortName == table[j].shortName)
                        throw std::runtime_error("minelink::cli: duplicate short option");
                }

            return table;
        }

    }  // namespace detail

    template<typename T>
    [[nodiscard]] std::expected<T, std::string> fromString(std::string_view str) {
        if constexpr (std::is_same_v<T, std::string>)
            return std::string{str};

        else if constexpr (std::is_same_v<T, bool>) {
            if (str == "true" || str == "1" || str == "yes") return true;
            if (str == "false" || str == "0" || str == "no") return false;
            return std::unexpected("invalid bool: " + std::string{str});
        } else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            T v{};
            auto [p, ec] = std::from_chars(str.data(), str.data() + str.size(), v);

            if (ec != std::errc{} || p != str.data() + str.size())
                return std::unexpected("cannot parse '" + std::string{str} + "' as number");

            return v;
        } else
            // C++26 CWG 2518：模板里允许无条件 static_assert(false)
            static_assert(false, "minelink::cli::from_string: unsupported type");
    }

    namespace detail {

        template<typename Config, std::size_t I>
        [[nodiscard]] std::optional<std::string> setValue(Config& config, std::string_view val) {
            constexpr auto member = memberAt<Config, I>();

            using MT = typename[:std::meta::type_of(member):];

            if constexpr (IsVector<MT>) {
                auto r = fromString<typename MT::value_type>(val);
                if (!r) return std::move(r.error());
                config.[:member:].push_back(std::move(*r));
            } else if constexpr (IsOptional<MT>) {
                auto r = fromString<typename MT::value_type>(val);
                if (!r) return std::move(r.error());
                config.[:member:].emplace(std::move(*r));
            } else {
                auto r = fromString<MT>(val);
                if (!r) return std::move(r.error());
                config.[:member:] = std::move(*r);
            }
            return std::nullopt;
        }

        template<typename Config, std::size_t I>
        void setFlag(Config& config, bool on) {
            constexpr auto member = memberAt<Config, I>();

            config.[:member:] = on;
        }

        template<typename Config, std::size_t... Is>
        [[nodiscard]] std::optional<std::string> dispatchValue(
            Config& config, std::size_t idx, std::string_view val, std::index_sequence<Is...>
        ) {
            std::optional<std::string> err;

            ((idx == Is ? (err = setValue<Config, Is>(config, val), void()) : void()), ...);

            return err;
        }

        template<typename Config, std::size_t... Is>
        void dispatchFlag(Config& config, std::size_t idx, bool on, std::index_sequence<Is...>) {
            ((idx == Is ? (setFlag<Config, Is>(config, on), void()) : void()), ...);
        }

        inline bool isNegativeNumber(std::string_view tok) {
            return tok.size() >= 2 && tok[0] == '-'
                && std::isdigit(static_cast<unsigned char>(tok[1]));
        }

        template<typename Config>
        [[nodiscard]] std::optional<ParseErrorInfo> parseInto(
            Config& config, std::span<const std::string_view> tokens
        ) {
            using enum ParseError;

            constexpr auto& table   = field_table_v<Config>;
            constexpr std::size_t N = table.size();
            constexpr auto seq      = std::make_index_sequence<N>{};

            std::bitset<N> setByOption;
            std::vector<std::string_view> positional;
            positional.reserve(tokens.size());
            bool rest = false;

            for (std::size_t i = 0; i < tokens.size(); ++i) {
                const std::string_view tok = tokens[i];

                if (rest || tok.empty() || tok[0] != '-') {
                    positional.push_back(tok);
                    continue;
                }
                if (tok == "--") {
                    rest = true;
                    continue;
                }
                if (tok.size() == 1) {
                    positional.push_back(tok);
                    continue;
                }
                if (isNegativeNumber(tok)) {
                    positional.push_back(tok);
                    continue;
                }

                const auto eq = tok.find('=');
                const std::string_view name =
                    (eq == std::string_view::npos) ? tok : tok.substr(0, eq);
                const std::string_view ival =
                    (eq == std::string_view::npos) ? std::string_view{} : tok.substr(eq + 1);

                std::size_t idx = N;
                bool neg        = false;

                for (std::size_t j = 0; j < N; ++j) {
                    if (table[j].longName == name) {
                        idx = j;
                        break;
                    }
                    if (!table[j].shortName.empty() && table[j].shortName == name) {
                        idx = j;
                        break;
                    }
                    // --no-xxx 仅对 flag 生效；substr 零分配比较
                    if (name.starts_with("--no-") && table[j].kind == Kind::Flag
                        && table[j].longName.substr(2) == name.substr(5)) {
                        idx = j;
                        neg = true;
                        break;
                    }
                }
                if (idx == N) return ParseErrorInfo{UnknownOption, name, "unknown option"};

                if (table[idx].kind == Kind::Flag) {
                    dispatchFlag<Config>(config, idx, !neg, seq);
                    continue;
                }

                std::string_view val;
                if (eq != std::string_view::npos)
                    val = ival;
                else if (i + 1 < tokens.size())
                    val = tokens[++i];
                else
                    return ParseErrorInfo{MissingValue, name, "missing value"};


                if (auto err = dispatchValue<Config>(config, idx, val, seq))
                    return ParseErrorInfo{InvalidValue, name, std::move(*err)};

                setByOption.set(idx);
            }

            for (std::size_t j = 0; j < N; ++j) {
                const int pos = table[j].positionalIndex;
                if (pos < 0 || static_cast<std::size_t>(pos) >= positional.size()) continue;
                if (table[j].kind == Kind::Flag) continue;
                if (setByOption.test(j)) continue;
                if (auto err = dispatchValue<Config>(config, j, positional[pos], seq))
                    return ParseErrorInfo{InvalidValue, table[j].longName, std::move(*err)};
            }

            return std::nullopt;
        }

    }  // namespace detail

    template<typename Config>
    [[nodiscard]] std::expected<Config, ParseErrorInfo> parse(
        std::span<const std::string_view> tokens
    ) {
        Config config{};

        if (auto error = detail::parseInto(config, tokens))
            return std::unexpected(std::move(*error));

        return config;
    }

}  // namespace minelink::cli
