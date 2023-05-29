// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//

#include "stdafx.h"

#include "parser.h"

#ifdef CALC_VER2_PATCHES
#include <functional>

#define _USE_MATH_DEFINES
#include <math.h>

#endif

namespace {
    // Template magic! Do not touch.

    using Fn = std::function<long double(IssueManager&, Token functionName, std::span<const long double>)>;

    template<typename... Ts, size_t... Is>
    Fn make_fn_impl(long double(* inner)(Ts...), std::index_sequence<Is...>) {
        return [=](IssueManager& im, Token functionName, std::span<const long double> params) -> long double {
            if (params.size() != sizeof...(Ts)) {
                im.error(functionName)
                    << "function "
                    << functionName.span
                    << " requires "
                    << sizeof...(Ts)
                    << " argument"
                    << (sizeof...(Ts) == 1 ? "" : "s")
                    << ", got "
                    << params.size();
                return NAN;
            } else {
                return inner(params[Is]...);
            }
        };
    }

    template<typename... Ts>
    Fn make_fn(long double(* inner)(Ts...)) {
        return make_fn_impl(inner, std::index_sequence_for<Ts...>{});
    }
}

namespace {
    const std::unordered_map<std::string_view, long double> CONSTANTS = {
        {"pi", M_PI},
        {"e",  M_E},
        {"inf", INFINITY},
    };

    long double min(IssueManager&, Token, std::span<const long double> args) { return *std::min_element(args.begin(), args.end()); }
    long double max(IssueManager&, Token, std::span<const long double> args) { return *std::max_element(args.begin(), args.end()); }
    long double rad(long double x) { return x * M_PI / 180; }
    long double deg(long double x) { return x * 180 / M_PI; }

    const std::unordered_map<std::string_view, Fn> FUNCTIONS = {
        {"sin", make_fn(&std::sinl)},
        {"cos", make_fn(&std::cosl)},
        {"pow", make_fn(&std::powl)},
        {"sqrt", make_fn(&std::sqrtl)},
        {"min", Fn(min)},
        {"max", Fn(max)},
        {"rad", make_fn(rad)},
        {"deg", make_fn(deg)},
    };
}

long double Parser::parse() {
    auto result = parse_expr_4();
    if (_current.type != Token::Type::END) {
        _im.error(_current) << "extraneous input at the end of expression: " << _current;
        result = NAN;
    }
    return result;
}

void Parser::advance() {
    _current = _lex.next();
}

long double Parser::parse_expr_4() {
    auto result = parse_expr_3();
    while (true) {
        switch (_current.type) {
            case Token::Type::ADD:
                advance();
                result += parse_expr_3();
                break;
            case Token::Type::SUB:
                advance();
                result -= parse_expr_3();
                break;
            default:
                return result;
        }
    }
}

long double Parser::parse_expr_3() {
    auto result = parse_expr_2();
    while (true) {
        switch (_current.type) {
            case Token::Type::MUL:
                advance();
                result *= parse_expr_2();
                break;
            case Token::Type::DIV:
                advance();
                result /= parse_expr_2();
                break;
            case Token::Type::REM:
                advance();
                result = std::fmod(result, parse_expr_2());
                break;
            default:
                return result;
        }
    }
}

long double Parser::parse_expr_2() {
    std::vector<long double> values;

    while (true) {
        values.push_back(parse_expr_1());

        if (_current.type == Token::Type::POW) {
            advance();
        } else {
            break;
        }
    }

    long double result = values.back();
    for (auto value: std::ranges::reverse_view(values) | std::views::drop(1)) {
        result = std::pow(value, result);
    }
    return result;
}

long double Parser::parse_expr_1() {
    switch (_current.type) {
        case Token::Type::ADD:
            advance();
            return +parse_expr_0();
        case Token::Type::SUB:
            advance();
            return -parse_expr_0();
        default:
            return parse_expr_0();
    }
}

long double Parser::parse_expr_0() {
    switch (_current.type) {
        case Token::Type::LPAREN: {
            advance();
            auto result = parse_expr_4();
            if (_current.type != Token::Type::RPAREN) {
                _im.error(_current) << "expected closing paren, got " << _current;
                return NAN;
            } else {
                advance();
                return result;
            }
        }
        case Token::Type::NUM: {
            auto num = _current;
            advance();
            return num.val;
        }
        case Token::Type::IDENT: {
            auto ident = _current;
            advance();
            if (_current.type == Token::Type::LPAREN) {
                advance();
                return process_function(ident, parse_function_params());
            } else {
                return process_const(ident);
            }
        }
        default:
            _im.error(_current) << "unexpected " << _current;
            return NAN;
    }
}

std::vector<long double> Parser::parse_function_params() {
    std::vector<long double> params;

    while (true) {
        params.push_back(parse_expr_4());

        if (_current.type == Token::Type::RPAREN) {
            advance();
            break;
        } else if (_current.type == Token::Type::COMA) {
            advance();
            continue;
        } else {
            _im.error(_current) << "expected closing paren or coma, got " << _current;
            break;
        }
    }
    return params;
}

long double Parser::process_function(Token functionName, const std::vector<long double>& params) {
    if (params.empty()) {
        _im.error(functionName) << functionName.span << " requires at least one param" << functionName.span;
        return NAN;
    } else if (auto function = FUNCTIONS.find(functionName.span); function != FUNCTIONS.end()) {
        return function->second(_im, functionName, params);
    } else {
        _im.error(functionName) << "unknown function " << functionName.span;
        return NAN;
    }
}

long double Parser::process_const(Token constName) {
    if (auto constant = CONSTANTS.find(constName.span); constant != CONSTANTS.end()) {
        return constant->second;
    } else {
        _im.error(constName) << "unknown constant " << constName.span;
        return NAN;
    }
}
