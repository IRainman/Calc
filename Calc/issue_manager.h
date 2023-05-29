//
// Created by Tamika Nomara on 28.05.2023.
//

#pragma once

#include "Flags.h"

#ifndef CALC_ISSUE_MANAGER_H
#define CALC_ISSUE_MANAGER_H

#include "token.h"

#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <ostream>
#include <sstream>

/**
 * Represents a message from the compiler.
 */
struct Issue {
    /**
     * How severe is this issue?
     */
    enum class Severity:int {

        // Errors that do stop processing.
        E,

        // Informational messages and comments to previous issues.
        I,

        // Warnings that don't stop processing.
        W,
    };

    // Message describing the issue.
    std::ostringstream message;

    // Issue severity.
    Severity severity;

    // Line number at which the issue has occurred.
    size_t line;

    // Position withing the line at which the issue has occurred.
    size_t pos;

};

/**
 * Reports and prints issues.
 */
class IssueManager {
public:
    /**
     * Report a new issue.
     */
    std::ostringstream& report(Issue issue);
    std::ostringstream& report(Issue::Severity severity, size_t line, size_t pos);
    std::ostringstream& report(Issue::Severity severity, Token position);

    /**
     * Report a new info message.
     */
    std::ostringstream& info(size_t line, size_t pos);
    std::ostringstream& info(Token position);

    /**
     * Report a new warning.
     */
    std::ostringstream& warning(size_t line, size_t pos);
    std::ostringstream& warning(Token position);

    /**
     * Report a new error.
     */
    std::ostringstream& error(size_t line, size_t pos);
    std::ostringstream& error(Token position);

    /**
     * Indicate whether any errors have been reported so far.
     */
    bool has_errors() const {
        return _has_errors;
    }

    /**
     * Return reference to the vector of all errors have been reported so far.
     */
    std::span<const Issue> errors() const {
        return _errors;
    }

#ifdef CALC_VER2_PATCHES
    const std::string& to_string() const;
#endif

private:
    bool _has_errors = false;
    std::vector<Issue> _errors;
};

std::ostream& operator<<(std::ostream& os, const Issue::Severity& severity);
std::ostream& operator<<(std::ostream& os, const Issue& issue);
std::ostream& operator<<(std::ostream& os, const IssueManager& manager);

#endif //CALC_ISSUE_MANAGER_H
