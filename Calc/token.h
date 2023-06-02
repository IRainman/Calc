//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#pragma once

#include "Flags.h"

struct Token {
    enum class Type : unsigned int {
        END = 0,
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

    // Text of the token.
    std::string_view text;

    // If token is a number, this is the parsed value of it.
    long double val;
};

#if 0

std::ostream& operator<<(std::ostream& os, const Token::Type token);

std::ostream& operator<<(std::ostream& os, const Token& token);

#endif
