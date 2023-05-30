// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

//
// Created by Tamika Nomara on 28.05.2023.
//
//
// Manteined by Elle Solomina since 29.05.2023.
//

#include "stdafx.h"

#include "issue_manager.h"

std::ostringstream& IssueManager::report(Issue&& issue) {
    _has_errors |= issue.severity == Issue::Severity::ERR;
    _errors.emplace_back(std::move(issue));
    return _errors.back().message;
}

std::ostringstream& IssueManager::report(Issue::Severity severity, size_t pos) {
    return report(Issue{{}, severity, pos});
}

std::ostringstream& IssueManager::info(size_t pos) {
    return report(Issue::Severity::INF, pos);
}

std::ostringstream& IssueManager::warning(size_t pos) {
    return report(Issue::Severity::WARN, pos);
}

std::ostringstream& IssueManager::error(size_t pos) {
    return report(Issue::Severity::ERR, pos);
}

std::ostream& operator<<(std::ostream& os, const Issue::Severity severity) {
    switch (severity) {
        case Issue::Severity::INF:
            return os << "Info";
        case Issue::Severity::WARN:
            return os << "Warning";
        case Issue::Severity::ERR:
            return os << "Error";
    }
    __assume(false); // C++23 unreachable();
}

std::ostream& operator<<(std::ostream& os, const Issue& issue) {
    os << issue.severity << " at pos:" << issue.pos << ": " << issue.message.str(); //-V128
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::span<const Issue>& errors) {
    os << "Completed with " //-V128
       << errors.size()
       << " message"
       << (errors.size() == 1 ? "" : "s")
       << (errors.empty() ? "." : ":")
       << "\n";

    for (auto& error: errors) {
        os << "  " << error << "\n";
    }

    return os;
}

const std::string& IssueManager::to_string() const {
    static std::string out;
    out.clear();
    out += "Completed with "
        + std::to_string(errors().size())
        + " message"
        + (errors().size() == 1 ? "" : "s")
        + (errors().empty() ? "." : ":")
        + "\n";

    for (auto& error : errors()) {
        out += "  " + error.message.str() + "\n";
    }

    return out;
}
