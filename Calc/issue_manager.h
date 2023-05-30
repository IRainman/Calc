//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#pragma once

#include "Flags.h"

#ifndef CALC_ISSUE_MANAGER_H
#define CALC_ISSUE_MANAGER_H

#include "token.h"

/**
 * Represents a message from the compiler.
 */
struct Issue {
    /**
     * How severe is this issue?
     */
    enum class Severity:int {

        // Errors that do stop processing.
        ERR,

        // Informational messages and comments to previous issues.
        INF,

        // Warnings that don't stop processing.
        WARN,
    };

    // Message describing the issue.
    std::ostringstream message;

    // Issue severity.
    Severity severity;

    // Position withing the line at which the issue has occurred.
    size_t pos; //-V122
};

/**
 * Reports and prints issues.
 */
class IssueManager {
public:
    /**
     * Report a new issue.
     */
    std::ostringstream& report(Issue&& issue);
    std::ostringstream& report(Issue::Severity severity, size_t pos);

    /**
     * Report a new info message.
     */
    std::ostringstream& info(size_t pos);

    /**
     * Report a new warning.
     */
    std::ostringstream& warning(size_t pos);

    /**
     * Report a new error.
     */
    std::ostringstream& error(size_t pos);

    /**
     * Indicate whether any errors have been reported so far.
     */
    bool has_errors() const noexcept {
        return _has_errors;
    }

    /**
     * Return reference to the vector of all errors have been reported so far.
     */
    std::span<const Issue> errors() const {
        return _errors;
    }

    /**
     * Return full report of expression processing.
     */
    const std::string& to_string() const;

private:
    bool _has_errors = false;
    std::vector<Issue> _errors;
};

std::ostream& operator<<(std::ostream& os, const Issue::Severity severity);
std::ostream& operator<<(std::ostream& os, const Issue& issue);
std::ostream& operator<<(std::ostream& os, const std::span<const Issue>& manager);

#endif //CALC_ISSUE_MANAGER_H
