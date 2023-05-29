//
// Created by Tamika Nomara on 28.05.2023.
//

#pragma once

#include "Flags.h"

#ifndef CALC_TOKEN_H
#define CALC_TOKEN_H

#include <string_view>
#include <optional>
#include <ostream>

struct Token {
    enum class Type {
        END,
        ADD,
        SUB,
        MUL,
        DIV,
        POW,
        REM,
        LPAREN,
        RPAREN,
        COMA,
        NUM,
        IDENT,
    };

    // Type of this token.
    Type type;

    // Line number at which this token was parsed.
    std::size_t line;

    // Position of the token's first symbol within the line.
    std::size_t pos;

    // Text of the token.
    std::string_view span;

    // If token is a number, this is the parsed value of it.
    long double val;

};

std::ostream& operator<<(std::ostream& os, const Token::Type& token);
std::ostream& operator<<(std::ostream& os, const Token& token);

#endif //CALC_TOKEN_H
