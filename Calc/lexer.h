//
// Created by Tamika Nomara on 28.05.2023.
//

#pragma once

#include "Flags.h"

#ifndef CALC_LEXER_H
#define CALC_LEXER_H

#include "issue_manager.h"
#include "token.h"

#include <string_view>
#include <charconv>
#include <iomanip>
#include <sstream>
#include <format>

class Lexer {
public:
    Lexer(std::string data, IssueManager& im)
        : _data{std::move(data)}, _view{_data}, _im{im} {
    }

public:
    /**
     * Return the next token.
     */
    Token next();

private:
    /**
     * Move current position one symbol further.
     */
    void advance();

    /**
     * Create a new token at the beginning of the input view.
     */
    Token emit(Token::Type type, size_t n = 0, long double val = NAN);

    /**
     * Create a new token at the beginning of the input view, and move current position to the end of the token.
     */
    Token emit_and_advance(Token::Type type, size_t n, long double val = NAN);

    /**
     * Read number at the beginning of the input view.
     */
    Token read_number();

    /**
     * Read identifier at the beginning of the input view.
     */
    Token read_ident();

private:
    IssueManager& _im;

    const std::string _data;
    std::string_view _view;

    std::size_t _line = 1;
    std::size_t _pos = 0;
};

#endif //CALC_LEXER_H
