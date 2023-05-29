// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//

#include "stdafx.h"

#include "lexer.h"

Token Lexer::next() {
    bool reported_errors = false;
    while (!_view.empty()) {
        if (std::isspace(_view.front())) {
            advance();
            continue;
        }
#ifdef CALC_VER2_PATCHES
        else if (isdigit(_view.front()) || _view.front() == '.')
        {
            return read_number();
        }
        else if (isalpha(_view.front()) || _view.front() == '_')
        {
            return read_ident();
        }
        else
#endif
        switch (_view.front()) {
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
#ifndef CALC_VER2_PATCHES
            case '0'...'9':
            case '.':
                return read_number();
            case 'a'...'z':
            case 'A'...'Z':
            case '_':
                return read_ident();
#endif
            default:
                if (!reported_errors) {
                    _im.error(_line, _pos) << "unknown character " << std::quoted(_view.substr(0, 1));
                    reported_errors = true;
                }
                advance();
        }
    }
    return emit(Token::Type::END);
}

void Lexer::advance() {
    if (_view.empty()) {
        return;
    }

    if (_view.starts_with('\n')) {
        ++_line;
        _pos = 0;
    } else {
        ++_pos;
    }

    _view.remove_prefix(1);
}

Token Lexer::emit(Token::Type type, size_t n, long double val) {
    return Token{
        type,
        _line,
        _pos,
        _view.substr(0, n),
        val};
}

Token Lexer::emit_and_advance(Token::Type type, size_t n, long double val) {
    n = std::min(n, _view.size());

    auto token = emit(type, n, val);
    _view.remove_prefix(n);
    _pos += n;
    return token;
}

Token Lexer::read_number() {

#ifdef CALC_VER2_PATCHES
    char* end;
    const auto val = std::strtold(_view.data(), &end);
    const auto size = end - _view.data();

    if (!size
#else
    char* end = nullptr;

    auto val = std::strtold(_view.begin(), &end);

    if (end == _view.begin() || end == nullptr
#endif
        ) {
        _im.error(_line, _pos) << "unable to parse number";
        advance();
        return emit(Token::Type::NUM, 0, NAN);
    }
    if (errno == ERANGE) {
        _im.warning(_line, _pos) << "long double value is out of range";
    }

#ifdef CALC_VER2_PATCHES
    return emit_and_advance(Token::Type::NUM, size, val);
#else
    return emit_and_advance(Token::Type::NUM, end - _view.begin(), val);
#endif
}

Token Lexer::read_ident() {
    size_t n = 0;
    for (; n < _view.size(); n++) {
        if (!(isalnum(_view[n]) || _view[n] == '_')) {
            break;
        }
    }

    return emit_and_advance(Token::Type::IDENT, n);
}
