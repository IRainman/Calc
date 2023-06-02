// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#include "stdafx.h"

#include "token.h"

#if 0

std::ostream& operator<<(std::ostream& os, const Token::Type token_type) {
    switch (token_type) {
        case Token::Type::END:
            return os << "END";
        case Token::Type::ADD:
            return os << "ADD";
        case Token::Type::SUB:
            return os << "SUB";
        case Token::Type::MUL:
            return os << "MUL";
        case Token::Type::DIV:
            return os << "DIV";
        case Token::Type::POW:
            return os << "POW";
        case Token::Type::REM:
            return os << "REM";
        case Token::Type::LPAREN:
            return os << "LPAREN";
        case Token::Type::RPAREN:
            return os << "RPAREN";
        case Token::Type::COMA:
            return os << "COMA";
        case Token::Type::NUM:
            return os << "NUM";
        case Token::Type::IDENT:
            return os << "IDENT";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    switch (token.type) {
        case Token::Type::END:
            return os << "end of expresion";
        default:
            return os << std::quoted(token.text);
    }
}

#endif