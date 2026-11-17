// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file json.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/18 20:48
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module utils;

namespace minelink {

    Json::Json(std::nullptr_t) {}

    Json::Json(bool b)
        : value(b) {}

    Json::Json(std::int64_t i)
        : value(i) {}

    Json::Json(double d)
        : value(d) {}

    Json::Json(std::string s)
        : value(s) {}

    Json::Json(const char* s)
        : value(std::string(s)) {}

    Json Json::object() {
        Json j;

        j.value = Object{};

        return j;
    }

    Json Json::array() {
        Json j;

        j.value = Array{};

        return j;
    }

    bool Json::isNull() const { return std::holds_alternative<std::nullopt_t>(value); }

    bool Json::isObject() const { return std::holds_alternative<Object>(value); }

    bool Json::isArray() const { return std::holds_alternative<Array>(value); }

    bool Json::isBool() const { return std::holds_alternative<bool>(value); }

    bool Json::isInt() const { return std::holds_alternative<std::int64_t>(value); }

    bool Json::isDouble() const { return std::holds_alternative<double>(value); }

    bool Json::isString() const { return std::holds_alternative<std::string>(value); }

    const std::string& Json::asString() const { return std::get<std::string>(value); }

    bool Json::asBool() const { return std::get<bool>(value); }

    std::int64_t Json::asInt() const { return std::get<std::int64_t>(value); }

    double Json::asDouble() const {
        if (auto p = std::get_if<double>(&value)) return *p;

        if (auto p = std::get_if<std::int64_t>(&value)) return static_cast<double>(*p);

        throw std::runtime_error("not a number");
    }

    namespace {

        constexpr std::string_view hexDigits = "0123456789abcdef";

        void writeEscaped(std::string& out, std::string_view s) {
            out += '"';
            for (char c : s) {
                switch (c) {
                    case '"': out += "\\\""; break;
                    case '\\': out += "\\\\"; break;
                    case '\n': out += "\\n"; break;
                    case '\r': out += "\\r"; break;
                    case '\t': out += "\\t"; break;
                    case '\b': out += "\\b"; break;
                    case '\f': out += "\\f"; break;
                    default:
                        if (static_cast<unsigned char>(c) < 0x20) {
                            out += "\\u00";
                            out += hexDigits[static_cast<unsigned char>(c) >> 4];
                            out += hexDigits[static_cast<unsigned char>(c) & 0xF];
                        } else {
                            out += c;
                        }
                }
            }
            out += '"';
        }

        void writeNumber(std::string& out, double d) {
            if (!std::isfinite(d)) throw std::runtime_error("json: non-finite double");
            char buf[64];
            auto r = std::to_chars(buf, buf + sizeof buf, d);  // 最短往返表示
            out.append(buf, r.ptr);
        }

        void newline(std::string& out, int indent, int depth) {
            if (indent <= 0) return;
            out += '\n';
            out.append(static_cast<std::size_t>(indent) * static_cast<std::size_t>(depth), ' ');
        }

        void writeValue(const Json& j, std::string& out, int indent, int depth) {
            if (auto p = std::get_if<bool>(&j.value)) {
                out += *p ? "true" : "false";
                return;
            }
            if (auto p = std::get_if<std::int64_t>(&j.value)) {
                out += std::to_string(*p);
                return;
            }
            if (auto p = std::get_if<double>(&j.value)) {
                writeNumber(out, *p);
                return;
            }
            if (auto p = std::get_if<std::string>(&j.value)) {
                writeEscaped(out, *p);
                return;
            }
            if (auto p = std::get_if<Json::Array>(&j.value)) {
                if (p->empty()) {
                    out += "[]";
                    return;
                }
                out += '[';
                for (std::size_t i = 0; i < p->size(); ++i) {
                    if (i) out += ',';
                    newline(out, indent, depth + 1);
                    writeValue((*p)[i], out, indent, depth + 1);
                }
                newline(out, indent, depth);
                out += ']';
                return;
            }
            if (auto p = std::get_if<Json::Object>(&j.value)) {
                if (p->empty()) {
                    out += "{}";
                    return;
                }
                out += '{';
                bool first = true;
                for (const auto& [k, v] : *p) {
                    if (!first) out += ',';
                    first = false;
                    newline(out, indent, depth + 1);
                    writeEscaped(out, k);
                    out += indent > 0 ? ": " : ":";
                    writeValue(v, out, indent, depth + 1);
                }
                newline(out, indent, depth);
                out += '}';
                return;
            }
            out += "null";  // 兜底：std::nullopt_t
        }

    }  // namespace

    std::string Json::toString(std::size_t indent) const {
        std::string out;
        writeValue(*this, out, static_cast<int>(indent), 0);
        return out;
    }

}  // namespace minelink
