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
                if (std::isspace(cur)) {
                    // skip
                }
                else {
                    _im.error(get_position()) << "unknown character \"" << cur << '\"';
                    return emit(Token::Type::END); // stop processing
                }
                advance();
        }
    }
    return emit(Token::Type::END);
}

void Lexer::advance() noexcept {
    _view.remove_prefix(1);
}

Token Lexer::emit(Token::Type type, size_t n, long double val) {
    return Token{
        type,
        _view.substr(0, n),
        val};
}

Token Lexer::emit_and_advance(Token::Type type, size_t n, long double val) {
    auto token = emit(type, n, val);
    _view.remove_prefix(n);
    return token;
}

Token Lexer::read_number() {
    long double val;
    const auto res = std::from_chars(_view.data(), _view.data() + _view.size(), val);
    const auto size = res.ptr - _view.data();

    if (res.ec != std::errc{}) {
        _im.error(get_position()) << "unable to parse number";
        return emit(Token::Type::NUM, 0, NAN);
    }

    return emit_and_advance(Token::Type::NUM, size, val);
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
