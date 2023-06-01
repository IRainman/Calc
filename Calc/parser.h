//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#pragma once

#include "Flags.h"

#include "lexer.h"

class Parser {
    /*
     * This a recursive descent parser with the following grammar:
     *
     * expr   = expr_4 <END>
     *
     * expr_4 = expr_3 ( ('+' | '-' ) expr_3 )*;
     *
     * expr_3 = expr_2 ( ('*' | '/' | '%') expr_2 )*;
     *
     * expr_2 = expr_1 ( '^' expr_1 )*;
     *
     * expr_1 = expr_0 | '-' expr_0 | '+' expr_0;
     *
     * expr_0 = '(' expr_4 ')'
     *        | NUM
     *        | ID function_params?
     *        ;
     *
     * function_params = '(' expr_4 ( ',' expr_4 )* ')'
     *
     */

public:
    Parser(IssueManager& im, Lexer& lex) : _im(im), _lex(lex) {
        advance();
    }

public:
    /**
     * Parse and eval an expression. Return nullopt in case of an error.
     */
    [[nodiscard]] long double parse();

private:
    /**
     * Move current position one token further.
     */
    void advance();

    long double parse_expr_4();

    long double parse_expr_3();

    long double parse_expr_2();

    long double parse_expr_1();

    long double parse_expr_0();

    long double parse_function();

private:
    IssueManager& _im;
    Lexer& _lex;
    Token _current;
};
