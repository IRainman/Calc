// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#include "stdafx.h"

#include "lexer.h"

Token Lexer::next() {
    while (!_view.empty()) {
        const auto& cur = _view.front();
        switch (cur) {
            case '+':
                return emit_and_advance(Token::Type::ADD, 1);
            case '-':
                return emit_and_advance(Token::Type::SUB, 1);
            case '*':
                return emit_and_advance(Token::Type::MUL, 1);
            case '/':
                return emit_and_advance(Token::Type::DIV, 1);
            case '^':
                return emit_and_advance(Token::Type::POW, 1);
            case '%':
                return emit_and_advance(Token::Type::REM, 1);
            case '(':
                return emit_and_advance(Token::Type::LPAREN, 1);
            case ')':
                return emit_and_advance(Token::Type::RPAREN, 1);
            case ',':
                return emit_and_advance(Token::Type::COMA, 1);
            default:
                if (std::isdigit(cur) || cur == '.') {
                    return read_number();
                }
                else if (std::isalpha(cur) || cur == '_') {
                    return read_ident();
                }
                else if (std::isspace(cur)) {
                    advance(1);
                }
                else {
                    _im.error(get_position()) << "unknown character \"" << cur << '\"';
                    return emit(Token::Type::END, 0);
                }
        }
    }
    return emit(Token::Type::END, 0);
}

void Lexer::advance(size_t n) noexcept {
    _view.remove_prefix(n);
}

Token Lexer::emit(Token::Type type, size_t n, long double val) {
    return Token{
        type,
        _view.substr(0, n),
        val};
}

Token Lexer::emit_and_advance(Token::Type type, size_t n, long double val) {
    auto token = emit(type, n, val);
    advance(n);
    return token;
}

Token Lexer::read_number() {
    long double val;
    const auto res = std::from_chars(_view.data(), _view.data() + _view.size(), val);
    const auto n = res.ptr - _view.data();

    if (res.ec != std::errc{}) {
        _im.error(get_position()) << "unable to parse number";
        return emit(Token::Type::NUM, 0, NAN);
    }

    return emit_and_advance(Token::Type::NUM, n, val);
}

Token Lexer::read_ident() {
    size_t n = 0;
    for (; n < _view.size(); ++n) {
        if (!(std::isalnum(_view[n]) || _view[n] == '_')) {
            break;
        }
    }

    return emit_and_advance(Token::Type::IDENT, n);
}
