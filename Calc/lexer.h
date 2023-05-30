//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
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
    Lexer(const std::string& data, IssueManager& im) noexcept
        : _data{data}, _view{_data}, _im{im} {
    }

public:
    /**
     * Return the next token.
     */
    Token next();

    /**
     * Return the current position.
     */
    size_t get_position() { return _view.data() - _data.data(); }

private:
    /**
     * Move current position one symbol further.
     */
    void advance() noexcept;

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

    const std::string& _data;
    std::string_view _view;
};

#endif //CALC_LEXER_H
