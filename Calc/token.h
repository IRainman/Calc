//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#pragma once

#include "Flags.h"

#ifndef CALC_TOKEN_H
#define CALC_TOKEN_H

#include <string_view>
#include <optional>
#include <ostream>

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
    std::string_view span;

    // If token is a number, this is the parsed value of it.
    [[deprecated("architectural error")]]
    long double val;
};

[[deprecated("it's issue_manager artefact")]]
std::ostream& operator<<(std::ostream& os, const Token::Type token);

[[deprecated("it's issue_manager artefact")]]
std::ostream& operator<<(std::ostream& os, const Token& token);

#endif //CALC_TOKEN_H
